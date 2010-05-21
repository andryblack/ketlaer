#ifndef MPEG_OUT_HPP
#define MPEG_OUT_HPP

#include "render_device.hpp"
#include "mpeg.hpp"

class MpegOut : public RenderDevice
{
private:
  mpeg_frame iframe;

public:

  MpegOut();

  void init();
  void deinit();

  bool supports_wid() { return false; }

 private:
  void run();

  FILE *fpek;

  int initialized;
};

#endif
