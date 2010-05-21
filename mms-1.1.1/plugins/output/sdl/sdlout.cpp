//#define DLOG
#include "config.h"

#include "sdlout.hpp"

#include "touch.hpp"

#ifdef benchmark_output
#include "bench.hpp"
#endif

#include "common-feature.hpp"
#include "busy_indicator.hpp"

#include "print.hpp"
#include "gettext.hpp"

#include <sstream>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <assert.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/kd.h>
#include <sys/types.h>
#include <stdlib.h>

#ifdef use_x11
#include "X11/extensions/Xfixes.h"
#endif

#include "themes.hpp"
#include "theme.hpp"

#ifdef use_nls
#include <langinfo.h>
#include <locale.h>
#endif


#define MAX_LOOP_INTERVAL 250

void SDLdev::deinit()
{
#ifdef use_x11
  if (!conf->p_cursor())
    XFixesShowCursor(display, sdl_window_id);
#endif

  hold_drawing();
  if (initialized == -1){
    release_drawing();
    return;
  }
  initialized = -1;
  needs_drawing = 1;
  release_drawing();
  wait_for_output_done();
  while (initialized == -1)
    mmsUsleep(20*1000);
}

void SDLdev::init(){
  start();
  while (initialized != 1){
    mmsUsleep(20*1000);
  }
}

void SDLdev::_init()
{
  if(SDL_Init(SDL_INIT_VIDEO /*| SDL_INIT_EVENTTHREAD*/) < 0 ) {
    print_critical(dgettext("mms-sdl", "Could not initialize") + std::string(" ") + SDL_GetError(), "SDL");
    std::exit(1);
  }

  SDL_EventState (SDL_ALLEVENTS, SDL_IGNORE);
  SDL_EventState (SDL_KEYDOWN, SDL_ENABLE);
  SDL_EventState (SDL_KEYUP, SDL_ENABLE);
  SDL_EventState (SDL_MOUSEBUTTONUP, SDL_ENABLE);
  SDL_EventState (SDL_MOUSEBUTTONDOWN, SDL_ENABLE);
  SDL_EventState (SDL_MOUSEMOTION, SDL_ENABLE);

  screen = SDL_SetVideoMode(width, height, 32, SDL_SWSURFACE);

  if (!screen) {
    print_critical(dgettext("mms-sdl", "Could not set video mode ") +
		      conv::itos(width) + "x" + conv::itos(height) +
		      "x32," + dgettext("mms-sdl", "error") + " " + SDL_GetError(), "SDL");
    SDL_Quit();
    std::exit(1);
  }

  rgbsurface = SDL_CreateRGBSurface(SDL_SRCCOLORKEY, width, height, 32,
				    0x00FF0000, 0x0000FF00, 0x000000FF, 0/*0xFF000000*/);
  original_w = width;
  original_h = height;

  // set the caption
  SDL_WM_SetCaption("mms", "mms");

  bpp = screen->format->BitsPerPixel;
  line_len = screen->format->BytesPerPixel * width;

#ifdef use_x11
  SDL_SysWMinfo info;
  SDL_VERSION(&info.version);

  int parent;
  if (SDL_GetWMInfo(&info) > 0) {
    info.info.x11.lock_func();
    display = info.info.x11.display;
    sdl_window_id = info.info.x11.wmwindow;
    parent = info.info.x11.window;
    int x11_width = WidthOfScreen(DefaultScreenOfDisplay(info.info.x11.display));
    int x11_height = HeightOfScreen(DefaultScreenOfDisplay(info.info.x11.display));
    info.info.x11.unlock_func();
    if (x11_width < width || x11_height < height) {
      print_critical("MMS resolution larger than X11 resolution", "SDL");
      std::cout << "Current X11 resolution is " << x11_width << " " << x11_height << std::endl;
      SDL_Quit();
      std::exit(1);
    }
  }

  window_id = XCreateSimpleWindow(display, parent, width-width/6, height-height/6, width/6, height/6, 0, 0, 0);
  XUnmapWindow(display, window_id);

  fs_window_id = XCreateSimpleWindow(display, parent, 0, 0, width, height, 0, 0, 0);
  XUnmapWindow(display, fs_window_id);

  if (!conf->p_cursor())
    XFixesHideCursor(display, sdl_window_id);
#endif

  if(conf->p_fullscreen())
    switch_fullscreen();

  render = S_Render::get_instance();

  initialized = 1;
}

bool SDLdev::supports_wid()
{
#ifdef use_x11
  return true;
#else
  return false;
#endif
}

int SDLdev::wid()
{
  return window_id;
}

void SDLdev::show_window_video() // TODO: ensure if this needs be in the SDL loop
{
  hold_drawing();
  if (!window_mapped) {
#ifdef use_x11
    XMapWindow(display, window_id);
#endif
    window_mapped = true;
  }
  release_drawing();
}

void SDLdev::hide_window_video() // TODO: ensure if this needs be in the SDL loop
{
  hold_drawing();
  if (window_mapped) {
#ifdef use_x11
    XUnmapWindow(display, window_id);
#endif
    window_mapped = false;
  }
  release_drawing();
}

int SDLdev::fs_wid()
{
  return fs_window_id;
}

void SDLdev::show_fs_video() // TODO: ensure if this needs be in the SDL loop
{
  hold_drawing();
  if (!fs_window_mapped) {
#ifdef use_x11
    XMapWindow(display, fs_window_id);
#endif
    fs_window_mapped = true;
  }
  release_drawing();
}

void SDLdev::hide_fs_video() // TODO: ensure if this needs be in the SDL loop
{
  if (fs_window_mapped) {
    hold_drawing();

#ifdef use_x11
    XUnmapWindow(display, fs_window_id);
#endif
    fs_window_mapped = false;
    release_drawing();
    render->complete_redraw();
    wait_for_output_done();
  }
}

void SDLdev::hide_video()
{
  hide_window_video();
  hide_fs_video();
}

void SDLdev::unlock()
{
  toggle_fullscreen( false );
}

void SDLdev::lock()
{
  if (!mode_switched && conf->p_fullscreen())
    toggle_fullscreen( true );
}   

SDLdev::SDLdev()
  : width(conf->p_h_res()), height(conf->p_v_res()),
    initialized(-1), verbose(3), fullscreen(false),
    window_mapped(false), fs_window_mapped(false), mode_switched(false)
{
#ifdef use_x11
  assert(XInitThreads());// enable thread safe mode in X
#endif
#ifdef use_nls
  // gettext
  setlocale(LC_ALL, ""); /* set from the environment variables */
  bindtextdomain("mms-sdl", mms_prefix"/share/locale");
  bind_textdomain_codeset("mms-sdl", nl_langinfo(CODESET));
#endif
  pthread_mutex_init(&needsdrawing_mutex,NULL);
  pthread_mutex_init(&outputdone_mutex,NULL);
  pthread_cond_init(&needsdrawing_switch, NULL);
  pthread_cond_init(&outputdone_switch, NULL);
}

SDLdev::~SDLdev()
{
  //terminate(); /* thread */
  /* terminate is no longer necessary, this plugin 
   * must end gracefully to prevent leaving the video
   * system in an undefined status */
}

void SDLdev::draw_rectangle(int x, int y, int w, int h)
{
  SDL_Rect rect;
  rect.x = x;
  rect.y = y;

  if (w > conf->p_h_res())
    rect.w = conf->p_h_res();
  else
    rect.w = w;

  if (h > conf->p_v_res())
    rect.h = conf->p_v_res();
  else
    rect.h = h;

#if 0
  std::ostringstream out;
  out << "blitting " << rect.x << ":" << rect.y << ":" << rect.w << ":" << rect.h;
  DebugPrint print(out.str(), Print::INFO, DebugPrint::ALL, "SDL out");
#endif

  SDL_BlitSurface(rgbsurface, &rect, screen, &rect);
  SDL_UpdateRect(screen, rect.x, rect.y, rect.w, rect.h);
}

void SDLdev::sdldev_draw_32()
{
  rgbsurface->pixels = reinterpret_cast<UINT8 *>(imlib_image_get_data_for_reading_only());

  foreach (rect& r, render->draw_rects)
    draw_rectangle(r.x, r.y, r.w, r.h);
}

void SDLdev::sdldev_draw()
{
  if (render->draw_rects.size() == 0) // optimization
    return;

  switch (bpp) {
  case 32 : sdldev_draw_32(); break;
    //  case 24 : sdldev_draw_24(dataptr); break;
    //  case 16 : sdldev_draw_16(dataptr); break;
  default :
    {
      print_critical("Color depth not supported", "SDL");
    }
  }
}

void SDLdev::sdlpump(){
  hold_drawing();
  needs_pumping = true;
  release_drawing();
  wait_for_output_done();
}
//----------------------------------------------------------------------
void SDLdev::run()
{
  _init();
  if (initialized != 1){
    std::exit(1);
  }

#ifdef benchmark_output
  Bench bench;
#endif

  pthread_mutex_lock(&needsdrawing_mutex);
  outputdone = false;
  needs_new_screen = false;
  needs_screen_toggling = false;
  needs_drawing = false;
  needs_refreshing = true;
  needs_pumping = false;
  struct timespec ts = time_helper::compute_interval(MAX_LOOP_INTERVAL);
  while (initialized == 1){
    int cond_wait_ret = 0;
    while (!needs_new_screen &&
	!needs_screen_toggling &&
	!needs_drawing &&
	!needs_refreshing &&
	!needs_pumping &&
	cond_wait_ret != ETIMEDOUT){
      cond_wait_ret = pthread_cond_timedwait(&needsdrawing_switch, &needsdrawing_mutex, &ts);
    }
/*
    if (cond_wait_ret == ETIMEDOUT){
      dprintf("Forcing sdl events handling\n");
    }
*/
    if (needs_screen_toggling){
      switch_fullscreen();
      needs_screen_toggling = false;
    }

    if (needs_new_screen){
      change_mode();
      needs_new_screen = false;
    }

    if (needs_drawing){
      needs_drawing = false;

#ifdef benchmark_output
      bench.start();
#endif

      sdldev_draw();

#ifdef benchmark_output
      bench.stop();
      bench.print("Rendering using SDL");
#endif
    }

    /* Refresh the event queue in SDL to prevent its starvation */
    SDL_PumpEvents();
    needs_pumping = false;

    pthread_mutex_lock(&outputdone_mutex);
    outputdone = true;
    needs_refreshing = false;
    pthread_cond_broadcast(&outputdone_switch);
    pthread_mutex_unlock(&outputdone_mutex);
    ts = time_helper::compute_interval(MAX_LOOP_INTERVAL);
  }
  if (conf->p_fullscreen())
    switch_fullscreen();
  SDL_ShowCursor(SDL_ENABLE);
  SDL_PumpEvents();
  SDL_Quit();
  pthread_mutex_unlock(&needsdrawing_mutex);
  initialized = 0;
}

void SDLdev::hold_drawing(){
  pthread_mutex_lock(&needsdrawing_mutex);
  outputdone = false;
  needs_refreshing = true;
}

void SDLdev::release_drawing(){
  pthread_cond_broadcast(&needsdrawing_switch);
  pthread_mutex_unlock(&needsdrawing_mutex);
}

void SDLdev::wait_for_output_done(){
  pthread_mutex_lock(&outputdone_mutex);
  while (!outputdone)
    pthread_cond_wait(&outputdone_switch, &outputdone_mutex);
  pthread_mutex_unlock(&outputdone_mutex);
}

void SDLdev::output_is_ready(){
  hold_drawing();
  needs_drawing = true;
  release_drawing();
  wait_for_output_done();
}

void SDLdev::toggle_fullscreen(bool fs){
  hold_drawing();
  if (fullscreen == fs){
    release_drawing();
    return;
  }
  needs_screen_toggling = true;
  release_drawing();
  wait_for_output_done();
  render->complete_redraw();
  output_is_ready();
}

void SDLdev::switch_fullscreen()
{
   
  screen = SDL_GetVideoSurface(); // you never know, I am paranoid
  assert(screen);/*
  int w = screen->w;
  int h = screen->h;
  int bits = screen->format->BitsPerPixel; */

  screen = SDL_SetVideoMode(
      SDL_GetVideoSurface()->w, SDL_GetVideoSurface()->h, 0, SDL_SWSURFACE | (screen->flags & SDL_FULLSCREEN) ? 0 : SDL_FULLSCREEN);

  if (screen == NULL) {
    std::cerr << "Unable to toggle fullscreen mode, SDL returned: " << SDL_GetError() << std::endl;
    std::exit(1);
  }
  fullscreen = ((screen->flags & SDL_FULLSCREEN) != 0);

  SDL_ShowCursor(fullscreen > 0 ? SDL_DISABLE : SDL_ENABLE);
  SDL_UpdateRect(screen, 0, 0, SDL_GetVideoSurface()->w, SDL_GetVideoSurface()->h);

}

void SDLdev::switch_mode()
{
  S_BusyIndicator::get_instance()->disable();
  run::aquire_exclusive_access();

  hold_drawing();
  needs_new_screen = true;
  release_drawing();
  wait_for_output_done();
  render->complete_redraw();
  output_is_ready();
  run::release_exclusive_access();
  S_BusyIndicator::get_instance()->enable();
}

void SDLdev::change_mode()
{
  if (original_w  == conf->p_alternative_h_res() && original_h  == conf->p_alternative_v_res()){
    switch_fullscreen();
    mode_switched = !mode_switched;
    return;
  }

  if (mode_switched) {
    width = old_h_res;
    height = old_v_res;
    mode_switched = false;
  } else {

    old_v_res = height;
    old_h_res = width;

    width = conf->p_alternative_h_res();
    height = conf->p_alternative_v_res();

    mode_switched = true;
  }

  conf->set_res(height, width);

  S_Themes::get_instance()->parser(S_Theme::get_instance()->cur_theme, false);

  if  (fullscreen || !conf->p_fullscreen()){
    screen = SDL_SetVideoMode(width, height, 32, SDL_SWSURFACE);
  } else{
    screen = SDL_SetVideoMode(width, height, 32, SDL_SWSURFACE | SDL_FULLSCREEN);
  }

  if(!screen) {
    print_critical(dgettext("mms-sdl", "Could not set video mode ") +
	     conv::itos(width) + "x" + conv::itos(height) +
  	     "x32," + dgettext("mms-sdl", "error") + " " + SDL_GetError(), "SDL");
    SDL_Quit();
    std::exit(1);
  }
  fullscreen = ((screen->flags & SDL_FULLSCREEN) != 0);

  rgbsurface = SDL_CreateRGBSurface(SDL_SRCCOLORKEY, width, height, 32,
				    0x00FF0000, 0x0000FF00, 0x000000FF, 0/*0xFF000000*/);
  if (fullscreen)
    SDL_ShowCursor(SDL_DISABLE );
  else
    SDL_ShowCursor(SDL_ENABLE);

  bpp = screen->format->BitsPerPixel;
  line_len = screen->format->BytesPerPixel * width;
  SDL_UpdateRect(screen, 0, 0, SDL_GetVideoSurface()->w, SDL_GetVideoSurface()->h);
  render->new_resolution();

}
