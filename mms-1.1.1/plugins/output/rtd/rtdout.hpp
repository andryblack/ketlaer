#ifndef SDLOUT_HPP
#define SDLOUT_HPP

#include "render_device.hpp"
#include "renderer.hpp"

#define KETLAER

class RTDdev : public RenderDevice
{
private:

  int width, height;
  int line_len;

  void draw_rectangle(unsigned *src, int x, int y, int w, int h);
  void rtd_draw();

  Render *render;

public:


  RTDdev();
  ~RTDdev();

  void init();
  void deinit();

  bool supports_wid() { return false; }

 private:
  void run();
};

#endif
