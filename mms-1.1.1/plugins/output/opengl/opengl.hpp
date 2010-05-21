#ifndef OPENGLOUT_HPP
#define OPENGLOUT_HPP

#include "render_device.hpp"
#include "renderer.hpp"

#include "SDL_GLout.h"

class OpenGLdev : public RenderDevice 
{
private:

#ifdef use_x11
  Display *display;
#endif
  SDL_Surface *screen;
  SDL_Surface *rgbsurface;
  Render *render;
  SDL_GLout * SDLGL;
  int width;
  int height;
  bool firstdraw;
  void run(){};
  double x_ratio;
  double y_ratio;
  bool defer_anim;

public:
  void get_input(){};

  OpenGLdev();
  ~OpenGLdev();
  void lock();
  void unlock();
  void init();
  void deinit();
  bool supports_wid(){return false; };
  void startup(){return;};
  void output_is_ready();
  bool supports_hw_accel(){ return true; }
  
  bool has_multiple_layers(){ return true; }

  bool supports_animations() { return true; }
  
  bool supports_shader() { return true; }

  int get_current_layer() { return currentlayer ;}

  void switch_to_layer(int new_layer);

  void swap_layers(int layer1, int layer2);

  void start_shader(int final_opa, int color_r, int color_g, int color_b, int time);

  void stop_shader();

  void animation_section_begin(bool defer = false);

  void animation_fade(float init_val, float end_val, int time, int layoutnum);

  void animation_spin(float init_angle, float end_angle, int time, int layoutnum);

  void animation_zoom(float init_xval, float init_yval, float end_xval, float end_yval, int time, int layoutnum);
  
  void animation_move(float init_xval, float init_yval, float end_xval, float end_yval, int time, int layoutnum);


  void animation_section_end(bool wait_finished);
  //void clear_animation_queue();

  void layer_active_no_wait(bool setstatus);
  void layer_active_no_wait(int layernum, bool setstatus);

  void layer_active(bool setstatus);
  void layer_active(int layernum, bool setstatus);
  void opengldraw();
  void wait_for_not_drawing(){};
  void wait_for_output_done();
  void reset_layout_attribs_nowait(int layoutnum );
  void reset_layout_attribs(int layoutnum );
  void make_actions_expire(int layoutnum);
  void set_layout_alpha(float f, int = -1);
  void refresh_screen();
  unsigned int convert_xmouse(int x);
  unsigned int convert_ymouse(int y);
  void sdlpump();


 protected:
// Config *conf;
  int currentlayer;
  
};

#endif
