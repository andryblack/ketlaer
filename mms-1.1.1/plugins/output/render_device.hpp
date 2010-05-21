#ifndef RENDER_DEVICE_HPP
#define RENDER_DEVICE_HPP

#include "config.hpp"

#define X_DISPLAY_MISSING

#define UGLY_FONT_FLICKER_HACK

#include <Imlib2.h>

#include <cc++/thread.h>
#include "common.hpp"

typedef unsigned char UINT8;

// used as superclass of output devices
class RenderDevice : public ost::Thread
{
public:
  virtual void init() = 0;
  virtual void deinit() = 0;

  virtual void wait_for_output_done();
  virtual void output_is_ready();
#ifdef use_sdl
  virtual void sdlpump(){ mmsUsleep(10*1000);  };
#endif
  // outputs supporting window id
  virtual bool supports_wid() = 0;

  virtual int wid() { return 0; }
  virtual int fs_wid() { return 0; }

  virtual void show_window_video()
  {}
  virtual void hide_window_video()
  {}

  virtual void show_fs_video()
  {}
  virtual void hide_fs_video()
  {}

  virtual void hide_video()
  {}

  virtual void wait_for_not_drawing();

  virtual void switch_mode() {}

  // dxr3 specific
  // others don't need to overwrite
  virtual void lock() {};
  virtual void unlock() {};

  virtual ~RenderDevice() {};

  RenderDevice();
  virtual unsigned int convert_xmouse(int x) {return x;}
  virtual unsigned int convert_ymouse(int y) {return y;}

  //opengl specific

  virtual bool supports_hw_accel() { return false; }
  virtual bool has_multiple_layers() { return false; }
  virtual bool supports_animations() { return false; }
  virtual bool supports_shader() { return false; }

  virtual int get_current_layer() { return 0 ;}
  virtual void switch_to_layer(int) { return; }
  virtual void swap_layers(int, int) { return; }

  virtual void start_shader(int, int, int, int, int) { return; }
  virtual void stop_shader() { return; }

  virtual void animation_fade(float, float, int, int) {};
  /* the single plugin that uses it,  should implement it as follows:
   * void animation_fade(float init_val, float end_val, int time, int layoutnum )
   */

  virtual void animation_spin(float, float, int, int) {};
  /* the single plugin that uses it,  should implement it as follows:
   * void animation_spin(float init_angle, float end_angle, int time, int layoutnum)
   */

  virtual void animation_zoom(float, float, float, float, int, int) {};
  /* the single plugin that uses it,  should implement it as follows:
   * void animation_zoom(float init_xval, float init_yval, float end_xval, float end_yval, int time, int layoutnum)
   */

  virtual void animation_move(float, float, float, float, int, int) {};
  /* the single plugin that uses it,  should implement it as follows:
   * void animation_move(float init_xval, float init_yval, float end_xval, float end_yval, int time, int layoutnum)
   */

  virtual void layer_active_no_wait(bool) {};
  virtual void layer_active_no_wait(int, bool) {};

  virtual void layer_active(bool) {};
  virtual void layer_active(int, bool) {};

  virtual void animation_section_begin(bool = false){ return; }
  virtual void animation_section_end(bool = false) { return; }
  virtual void clear_animation_queue() { return; }
  virtual void reset_layout_attribs_nowait(int = -1) {};
  virtual void reset_layout_attribs(int = -1) {};
  virtual void make_actions_expire(int) {};
  virtual void refresh_screen(){};
  virtual void set_layout_alpha(float, int = -1) {};


protected:
  Config *conf;
  ost::Event output_ready;
  ost::Event output_done;

  bool drawing;
  bool new_to_draw;

private:
  virtual void run() = 0;
};

#endif
