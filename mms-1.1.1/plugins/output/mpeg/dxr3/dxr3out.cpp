#include "dxr3out.hpp"

#ifdef benchmark_output
#include "bench.hpp"
#endif

#include "gettext.hpp"
#include "renderer.hpp"
#include <sys/ioctl.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#ifdef use_nls
#include <langinfo.h>
#include <locale.h>
#endif


void Dxr3::run()
{
#ifdef benchmark_output
  Bench bench2;
  Bench bench;
#endif

  while (true) {

    drawing = false;
    new_to_draw = false;

    output_done.signal();
    output_ready.wait();
    output_ready.reset();

    locked_mutex.enterMutex();

    if (!locked) {
      locked_mutex.leaveMutex();
      continue;
    }

    drawing = true;

    UINT8 *frame = reinterpret_cast<UINT8 *>(imlib_image_get_data_for_reading_only());

#ifdef benchmark_output
    bench2.start();
    bench.start();
#endif

    iframe = mpeg_draw(frame);

    if (iframe.data[iframe.size - 1] != 0xb7) {
      /* add sequence end code */
      iframe.data[iframe.size] = 0x00;
      iframe.data[iframe.size + 1] = 0x00;
      iframe.data[iframe.size + 2] = 0x01;
      iframe.data[iframe.size + 3] = 0xb7;
      iframe.size += 4;
    }

#ifdef benchmark_output
    bench.stop();
    bench.print("libavcodec done rendering frame ( = convert + encode)");
#endif

#ifdef benchmark_output
    bench.start();
#endif

    write(fd_video, iframe.data, iframe.size);

#ifdef benchmark_output
    bench.stop();
    bench.print("done writing to mpeg1 frame to the dxr3");
#endif

#ifdef benchmark_output
    bench2.stop();
    bench2.print("dxr3 complete draw");
#endif

    locked_mutex.leaveMutex();
  }
}

void Dxr3::lock()
{
  // setup 4:3 for menu
  // FIXME:  + " -s " + conf->p_audio_output_type()
  system((dxr3_conf->p_em8300setup_path() + " " + dxr3_conf->p_tv_mode()).c_str());

  init();

  locked = true;
}

// unlock it for other processes to use, this won't actually
// kill the application
void Dxr3::unlock()
{
  deinit();
}

void Dxr3::deinit()
{
  if (initialized == -1)
    return;

  locked_mutex.enterMutex();

  locked = false;

  if(orig_scr_zoom>0){
#ifdef EM8300_IOCTL_SET_ZOOM
    ioctl(fd_control, EM8300_IOCTL_SET_ZOOM, &orig_scr_zoom);
#else
    char temp_driver_file_name[DXR3_MAX_DRIVER_FILENAME_LEN];
    sprintf(temp_driver_file_name,DXR3_SYSFS_ZOOM_FILE_MASK,dxr3_conf->p_device_index());
    FILE * fd_zoom = fopen (temp_driver_file_name, "w");
    if (fd_zoom == NULL){  //Non-fatal
      print_warning(gettext("Could not open device to set DXR3 zoom: ") + std::string(temp_driver_file_name), "DXR3");
    }else{
      fprintf(fd_zoom,"%d%",orig_scr_zoom);
      fclose(fd_zoom);
    }
#endif
  }
  ::close(fd_video);
  ::close(fd_control);

  fd_video = -1;
  initialized = -1;

  locked_mutex.leaveMutex();
}

Dxr3::Dxr3() : fd_control(-1),fd_video(-1),initialized(-1),locked(false),orig_scr_zoom(-1)
{
#ifdef use_nls
  // gettext
  setlocale(LC_ALL, ""); /* set from the environment variables */
  bindtextdomain("mms-dxr3", mms_prefix"/share/locale");
  bind_textdomain_codeset("mms-dxr3", nl_langinfo(CODESET));
#endif

  dxr3_conf = S_Dxr3Config::get_instance();
  dxr3_conf->parse_configuration_file(conf->p_homedir());

  // lets see if the dxr3 is set to ntsc
  bool ntsc = false;

  if (dxr3_conf->p_tv_mode().find("-n") != std::string::npos)
    ntsc = true;

  mpeg_init(ntsc);
}

Dxr3::~Dxr3()
{
  mpeg_deinit();
}

void Dxr3::init()
{
  if (initialized != -1)
    return;

  int ioval;
  em8300_register_t reg;

  char temp_driver_file_name[DXR3_MAX_DRIVER_FILENAME_LEN];

  sprintf(temp_driver_file_name,DXR3_DRIVER_MV_FILE_MASK,dxr3_conf->p_device_index());
  fd_video = open (temp_driver_file_name, O_WRONLY);
  if (fd_video < 1){
    print_critical(dgettext("mms-dxr3", "Could not open device: ") + std::string(temp_driver_file_name), "DXR3");
    std::exit(1);
  }

  sprintf(temp_driver_file_name,DXR3_DRIVER_FILE_MASK,dxr3_conf->p_device_index());
  fd_control = open (temp_driver_file_name, O_WRONLY);
  if (fd_control < 1){
    print_critical(dgettext("mms-dxr3", "Could not open device: ") + std::string(temp_driver_file_name), "DXR3");
    std::exit(1);
  }

  /* Set the playmode to play (just in case another app has set it to something else) */
  ioval = EM8300_PLAYMODE_PLAY;
  if (ioctl(fd_control, EM8300_IOCTL_SET_PLAYMODE, &ioval) < 0) {
    print_critical(dgettext("mms-dxr3", "Unable to set playmode"), "DXR3");
  }

  /* Start em8300 prebuffering and sync engine */
  reg.microcode_register = 1;
  reg.reg = 0;
  reg.val = MVCOMMAND_SYNC;
  ioctl(fd_control, EM8300_IOCTL_WRITEREG, &reg);

  /* Clean buffer by syncing it */
  ioval = EM8300_SUBDEVICE_VIDEO;
  ioctl(fd_control, EM8300_IOCTL_FLUSH, &ioval);
  ioval = EM8300_SUBDEVICE_AUDIO;
  ioctl(fd_control, EM8300_IOCTL_FLUSH, &ioval);

  /* Sync the video device to make sure the buffers are empty
   * and set the playback speed to normal. Also reset the
   * em8300 internal clock.
   */
  fsync(fd_video);
  ioval = 0x900;
  ioctl(fd_control, EM8300_IOCTL_SCR_SETSPEED, &ioval);

  if ( !( (conf->p_h_res() == 960 && conf->p_v_res() == 540)      // anamoprh 16:9
	  || (conf->p_h_res() == 720 && conf->p_v_res() == 540)      // normal PAL 4/3
	  || (conf->p_h_res() == 720 && conf->p_v_res() == 405) )) {   // old 16:9
    print_critical(dgettext("mms-dxr3", "DXR3 output only supports the resolutions 960x540 (16:9 anamorphic), 720x540 (4:3) and 720x405 (16:9)"),
		   "DXR3");
    ::exit(1);
  }

  if ((dxr3_conf->p_zoom() > 0) && (dxr3_conf->p_zoom() <= 100)) {
#ifdef EM8300_IOCTL_SET_ZOOM
    if(ioctl(fd_control, EM8300_IOCTL_GET_ZOOM, &ioval) >= 0) {
      orig_scr_zoom=ioval;
      ioval = dxr3_conf->p_zoom();
      if (ioctl(fd_control, EM8300_IOCTL_SET_ZOOM, &ioval) < 0)
	orig_scr_zoom=-1;
    }else{
      print_warning(gettext("Zoom option is not supported by DXR3 driver."),"DXR3");
    }
#else
    sprintf(temp_driver_file_name,DXR3_SYSFS_ZOOM_FILE_MASK,dxr3_conf->p_device_index());
    FILE *fd_zoom = fopen (temp_driver_file_name, "r+");
    if (fd_zoom == NULL) {  //Non-fatal
      print_warning(gettext("Could not open device to set DXR3 zoom: ") + std::string(temp_driver_file_name), "DXR3");
    } else {
      if (fscanf(fd_zoom,"%d",&orig_scr_zoom) == EOF)
	orig_scr_zoom = -1;
      fclose(fd_zoom); //we close it and then reopen it
      fd_zoom = fopen (temp_driver_file_name, "w");
      if (fd_zoom == NULL)   //Non-fatal
	print_warning(gettext("Could not open device to set DXR3 zoom: ") + std::string(temp_driver_file_name), "DXR3");
      else{
	print_warning(gettext("Set DXR3 zoom using device ") + std::string(temp_driver_file_name), "DXR3");
	if(fprintf(fd_zoom,"%d",dxr3_conf->p_zoom()) < 0)
	  print_warning(gettext("Failed to set zoom value in the DXR3 driver."),"DXR3");
	fclose(fd_zoom);
      }
    }
#endif
  }

  initialized = 1;

  // LOCK dxr3 so we can output things to it.
  lock();
}
