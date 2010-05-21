#ifndef DXR3OUT_HPP
#define DXR3OUT_HPP

#include "render_device.hpp"
#include "mpeg.hpp"
#include "dxr3_config.hpp"
#include <linux/em8300.h>

#define DXR3_MAX_DRIVER_FILENAME_LEN 64
#define DXR3_SYSFS_ZOOM_FILE_MASK "/sys/class/em8300/em8300-%d/device/zoom"
#define DXR3_DRIVER_MV_FILE_MASK "/dev/em8300_mv-%d"
#define DXR3_DRIVER_FILE_MASK "/dev/em8300-%d"

#undef EM8300_IOCTL_SET_ZOOM

class Dxr3 : public RenderDevice
{
private:
  mpeg_frame iframe;
  int orig_scr_zoom;
  int fd_control;
  int fd_video;
  int initialized;

  bool locked;

  ost::Mutex locked_mutex;

  Dxr3Config *dxr3_conf;

public:

  Dxr3();
  ~Dxr3();

  void init();
  void deinit();

  void lock();
  void unlock();

  bool supports_wid() { return false; }

 private:
  void run();
};

#endif
