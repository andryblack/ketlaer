#include "dvbout.hpp"

#include "renderer.hpp"

#include "config.h"

#include "gettext.hpp"

#ifdef benchmark_output
#include "bench.hpp"
#endif

#include <sys/ioctl.h>

#ifdef use_nls
#include <langinfo.h>
#include <locale.h>
#endif

void Dvb::deinit()
{
  if (initialized == -1)
    return;

  // stop drawer
  d->running = false;
  while(!d->quitted) /* Sigh  */
    mmsUsleep(20000);

  mpeg_deinit();

  ::close(fd_video);

  initialized = -1;
}

void Dvb::init()
{
  if (initialized != -1)
    return;

  initialized = 1;

  mpeg_init(false);

  fd_video = open(dvb_conf->p_dvb_device().c_str(), O_RDWR | O_NONBLOCK);

  pfd[0].fd = fd_video;
  pfd[0].events = POLLOUT;

  if (fd_video == -1) {
    print_critical(dgettext("mms-dvb", "Could not initialize device: ") + dvb_conf->p_dvb_device(), "DVB");
    std::exit(1);
  }

  ioctl(fd_video, VIDEO_SET_BLANK, true);
  ioctl(fd_video, VIDEO_STOP, true);
  ioctl(fd_video, VIDEO_CLEAR_BUFFER);
  ioctl(fd_video, VIDEO_SELECT_SOURCE, VIDEO_SOURCE_MEMORY);
  ioctl(fd_video, VIDEO_PLAY);

  if (conf->p_h_res() != 720 || (conf->p_v_res() != 540 && conf->p_v_res() != 405)) {
    print_critical(dgettext("mms-dvb", "DVB output only supports the resolution 720x540 (4:3) or 720x405 (16:9)"),
		   "DVB");
    ::exit(1);
  }

  // start the drawer
  d = new DvbDraw();
  d->device = this;
  d->start();

  locked = true;
}

Dvb::Dvb()
  : fd_video(-1), initialized(-1), locked(false), ffr(false), d(NULL)
{
#ifdef use_nls
  // gettext
  setlocale(LC_ALL, ""); /* set from the environment variables */
  bindtextdomain("mms-dvb", mms_prefix"/share/locale");
  bind_textdomain_codeset("mms-dvb", nl_langinfo(CODESET));
#endif

  dvb_conf = S_DvbConfig::get_instance();
  dvb_conf->parse_configuration_file(conf->p_homedir());
}

Dvb::~Dvb()
{
  deinit();
}

void Dvb::lock()
{
  init();
}

void Dvb::unlock()
{
  locked_mutex.enterMutex();
  locked = false;
  deinit();
  locked_mutex.leaveMutex();
}

void Dvb::run()
{
#ifdef benchmark_output
  Bench bench;
#endif

  while (true) {

    drawing = false;
    new_to_draw = false;

    output_done.signal();
    output_ready.wait();
    output_ready.reset();

    drawing = true;

    locked_mutex.enterMutex();

    if (!locked) {
      locked_mutex.leaveMutex();
      continue;
    }

    UINT8 *frame = reinterpret_cast<UINT8 *>(imlib_image_get_data_for_reading_only());

#ifdef benchmark_output
    bench.start();
#endif

    iframe = mpeg_draw(frame);

    ffr = true;

#ifdef benchmark_output
    bench.stop();
    bench.print("libavcodec done rendering frame ( = convert + encode)");
#endif

    locked_mutex.leaveMutex();
  }
}

void Dvb::create_packet_and_write()
{
  if (!ffr)
    return;

  const unsigned char *data = (const unsigned char*)iframe.data;
  int len = iframe.size, timestamp = 1;

#define PES_MAX_SIZE 2048
    int ptslen = timestamp ? 5 : 1;
    static unsigned char pes_header[PES_MAX_SIZE];

    // startcode:
    pes_header[0] = pes_header[1] = 0;
    pes_header[2] = 1;
    pes_header[3] = 0xe0;

    while (len > 0)
    {
        int payload_size = len; // data + PTS

        if (6 + ptslen + payload_size > PES_MAX_SIZE)
            payload_size = PES_MAX_SIZE - (6 + ptslen);

        // construct PES header:  (code from ffmpeg's libav)
        // packetsize:
        pes_header[4] = (ptslen + payload_size) >> 8;
        pes_header[5] = (ptslen + payload_size) & 255;

        if (ptslen == 5)
        {
            int x;

            // presentation time stamp:
            x = (0x02 << 4) | (((timestamp >> 30) & 0x07) << 1) | 1;
            pes_header[8] = x;
            x = ((((timestamp >> 15) & 0x7fff) << 1) | 1);
            pes_header[7] = x >> 8;
            pes_header[8] = x & 255;
            x = ((((timestamp) & 0x7fff) << 1) | 1);
            pes_header[9] = x >> 8;
            pes_header[10] = x & 255;
        }
        else
        {
            // stuffing and header bits:
            pes_header[6] = 0x0f;
        }

        memcpy (&pes_header[6 + ptslen], data, payload_size);

	my_write(pes_header, 6 + ptslen + payload_size);

        len -= payload_size;
        data += payload_size;
        ptslen = 1;             // store PTS only once, at first packet!
    }
}

void Dvb::my_write(unsigned char* data, int len)
{
  while(len>0) {
    if (poll(pfd,1,1)) {
      if (pfd[0].revents & POLLOUT){
	int ret=write(fd_video,data,len);
	if(ret<=0)
	  usleep(0); /* TODO: what's this for??? */
	else
	  len-=ret; data+=ret;
      } else
	mmsUsleep(1000);
    }
  }
}

DvbDraw::DvbDraw()
: device(NULL), quitted(false)
{
}

void DvbDraw::run()
{
#ifdef benchmark_output
  Bench bench;
#endif

  running = true;

  Dvb *dvb = dynamic_cast<Dvb*>(device);
  assert(dvb);

  while (running) {

    dvb->locked_mutex.enterMutex();

    if (!running) {
      dvb->locked_mutex.leaveMutex();
      break;
    }

#ifdef benchmark_output
    bench.start();
#endif

    dvb->create_packet_and_write();

#ifdef benchmark_output
    bench.stop();
    bench.print("done writing to mpeg1 frame to the dvb");
#endif

    dvb->locked_mutex.leaveMutex();

    // lets just take it easy
    mmsUsleep(10000);
  }
  quitted = true;
}

