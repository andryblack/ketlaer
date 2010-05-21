#include "render_device.hpp"

RenderDevice::RenderDevice()
  : drawing(false), new_to_draw(false)
{
  conf = S_Config::get_instance();
}

void RenderDevice::wait_for_output_done()
{
  output_done.wait();
  output_done.reset();
  new_to_draw = true;
}

void RenderDevice::output_is_ready()
{
  output_ready.signal();
}

void RenderDevice::wait_for_not_drawing()
{
  while (drawing || new_to_draw)
    mmsUsleep(100*1000);
}

