#ifndef DVBOUT_HPP
#define DVBOUT_HPP

#include "render_device.hpp"
#include "mpeg.hpp"
#include "dvb_config.hpp"

#include <linux/types.h>
#include <linux/dvb/video.h>
#include <linux/dvb/audio.h>
#include <poll.h>

class DvbDraw : public ost::Thread
{
public:
  volatile bool running;
  RenderDevice *device;
  DvbDraw();
  volatile bool quitted;  /* It's a hack that I don't like, this plugin should be rewritten from scratch */
private:
  void run();
  void Final() 
  { delete this; } // we instantiate dvbdraw as a pointer so this is safe
};

class Dvb : public RenderDevice
{
private:
  int fd_video;

  mpeg_frame iframe;
  pollfd pfd[1];

  int initialized;
  bool locked;

  // first frame ready
  bool ffr;

  DvbConfig *dvb_conf;

  DvbDraw *d;

  struct video_still_picture still;

  void my_write(unsigned char* data, int len);

public:
  // for synchronization with drawer
  ost::Mutex locked_mutex;

  Dvb();
  ~Dvb();

  void init();
  void deinit();
  
  void create_packet_and_write();

  bool supports_wid() { return false; }

  void lock();
  void unlock();

 private:
  void run();
};

#endif
