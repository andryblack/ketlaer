#ifndef SDLOUT_HPP
#define SDLOUT_HPP

#include "render_device.hpp"
#include "renderer.hpp"

#include <SDL/SDL.h>
#include <SDL/SDL_syswm.h>

class SDLdev : public RenderDevice
{
private:
  int bpp;

  int sdl_window_id;

  int width, height, original_w, original_h;
  int line_len;

  volatile int initialized;
  int verbose;

  bool fullscreen;

  int window_id;
  bool window_mapped;

  int fs_window_id;
  bool fs_window_mapped;

  volatile bool needs_new_screen;
  volatile bool needs_screen_toggling;
  volatile bool needs_drawing;
  volatile bool needs_refreshing;
  volatile bool needs_pumping;

#ifdef use_x11
  Display *display;
#endif
  SDL_Surface *screen;
  SDL_Surface *rgbsurface;

  void draw_rectangle(int x, int y, int w, int h);

  void sdldev_draw();

  void sdldev_draw_32();

  Render *render;

  bool mode_switched;
  int old_v_res, old_h_res;
  pthread_mutex_t initmutex;
  pthread_mutex_t needsdrawing_mutex;
  pthread_cond_t needsdrawing_switch;
  pthread_mutex_t outputdone_mutex;
  pthread_cond_t outputdone_switch;
  volatile bool outputdone;
  void _init();
  void switch_fullscreen();
  void change_mode();
  void hold_drawing();
  void release_drawing();
public:
  void output_is_ready();
  void wait_for_output_done();
  void wait_for_not_drawing(){};
  void sdlpump();

  ost::Event get_input_event;
  std::string input_key;

  SDLdev();
  ~SDLdev();

  void init();
  void deinit();

  void lock();
  void unlock();

  bool supports_wid();

  int wid();
  int fs_wid();

  void show_window_video();
  void hide_window_video();

  void show_fs_video();
  void hide_fs_video();

  void hide_video();

  void toggle_fullscreen(bool fs);

  void switch_mode();

 private:
  void run();
};

#endif
