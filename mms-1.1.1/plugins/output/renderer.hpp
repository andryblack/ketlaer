#ifndef RENDERER_HPP
#define RENDERER_HPP

#include "config.h"

#include "render_device.hpp"
#include "image.hpp"
#include "graphic_object.hpp"
#include "common.hpp"
#include "singleton.hpp"

// mypair
#include "common-output.hpp"
#include "config.hpp"

#include <string>
#include <queue>
#include <map>
#include <iostream>

#define X_DISPLAY_MISSING

#define UGLY_FONT_FLICKER_HACK

#include <Imlib2.h>

class Render
{
private:
  Config *conf;

  int width;
  int height;

  void create_scaled_image(const std::string& in_file,
			   const std::string& out_file,
			   std::string format,
			   int max_x, int max_y);

  void create_scaled_image_upscaled(const std::string& in_file,
				    const std::string& out_file,
				    std::string format,
				    int max_x, int max_y);

  class queue_obj
  {
  public:
    queue_obj(const std::string& p, Imlib_Image i, int o, Scaling s,
	      double scale_f_w, double scale_f_h)
      : path(p), image(i), orientation(o), scaleable(s),
	scale_width(scale_f_w), scale_height(scale_f_h), count(0)
    {}

    std::string path;
    Imlib_Image image;
    int orientation;
    Scaling scaleable;
    double scale_width;
    double scale_height;
    int count;
  };

  typedef std::vector<queue_obj> image_queue;
  image_queue scaled_images;

  typedef std::map<std::string, Imlib_Font> font_map;
  font_map fonts;

  Imlib_Image current_nonblur;

  void remove_overlapping_objects();

  void clean_up_for_next_draw(bool overlay);

  Image prev_overlay;

public:

  static pthread_mutex_t singleton_mutex;

  Render();
  ~Render();

  bool set_font(const std::string& font_name);

  /* imlib context safe way of knowing the size of a string */
  void font_query_text_size(const std::string& text, const std::string& font, int *w, int *h);

  void set_theme();
  void set_audio_state();

  volatile bool show_audio_track;

  Image current;
  Image previous;

  std::string default_path;

  ost::Mutex image_mut;

  typedef std::pair<GObj*, rect> covered_obj;

  typedef std::vector<std::pair<GObj*, rect> > graph_objs;
  graph_objs objects;
  std::vector<rect> draw_rects;

  // used for overlays
  //
  // automaticly takes the image_mutex
  void wait_and_aquire();

  // used for normal draws
  //
  // automaticly takes the image_mutex
  void prepare_new_image();

  // automaticly releases the image_mutex
  void draw_and_release(const std::string& module, bool overlay = false);

  void draw(const std::string& module, bool overlay = false);
  void complete_draw();
  void complete_redraw();

  bool contains(const rect& outer, const rect& inner);
  bool equal(GObj *lhs, GObj *rhs);

  // helper function
  bool intersect_check(int x1, int w1, int y1, int h1,
		       int x2, int w2, int y2, int h2);

  bool intersect(GObj *lhs, GObj *rhs);
  bool intersect(const rect& lhs, const rect& rhs);
  bool intersect(const rect& lhs, GObj *rhs, rect& r);
  bool intersect(GObj *lhs, GObj *rhs, rect& r);

  Imlib_Image image_loader(const std::string& path, Scaling scaleable,
			   double scale_factor_h, double scale_factor_w, int orientation);

  // x,y,width,height in background rendered at x_in_d,y_in_d
  void image_rectangle(GObj *g, int x, int y,
		       int width, int height, int x_in_d, int y_in_d);
  void image_rectangle_no_bb(GObj *g, int x, int y,
			     int width, int height, int x_in_d, int y_in_d);
  void image_rectangle_bb(GObj *p, int x, int y,
			  int width, int height, int x_in_d, int y_in_d);

  RenderDevice *device;

  void blur();
  void clear();
  Imlib_Image new_imlib_image();

  std::string image_get_fullpath(const std::string& path, int max_width, int max_height);
  std::pair<int, int> image_dimensions(const std::string& path, bool *transparent);
  bool image_fullscreen_dimensions(const std::string& path, bool zoom, int orientation,
				   int zoom_level, int zoom_pos,
				   int &real_x, int &real_y, int &real_w, int &real_h,
				   int &w, int &h, bool *transparent);
  void image(const std::string& path, int x, int y, int w, int h, Scaling scaleable,
	     double scale_factor_h, double scale_factor_w, int orientation);
  void image_fullpath(const std::string& path, int x, int y, bool border, Scaling scaleable,
		      double scale_factor_h, double scale_factor_w, int orientation,
		      int bx, int by, int bw, int bh, bool border_set);
  void image_fullscreen_zoom(const std::string& path, int orientation, int zoom_level, int zoom_position,
			     int zoom_vertical_levels, int zoom_horizontal_levels, int x, int y,
			     int scaled_w, int scaled_h, Scaling scaleable, double scale_factor_h, double scale_factor_w);
  void image_fullscreen(const std::string& path, int orientation, int x, int y, int scaled_w, int scaled_h,
			Scaling scaleable, double scale_factor_h, double scale_factor_w);

  void create_scaled_image_wrapper(const std::string& path, int max_x, int max_y);
  void create_scaled_image_wrapper_upscaled(const std::string& path, int max_x, int max_y);

  void text(const std::string& text, const std::string& font, int x, int y, int r, int g, int b);
  void text_part(const std::string& text, const std::string& font, int r, int g, int b,
		 int x, int y, int total_w, int total_h, const rect& area);
  void rectangle(int x, int y, int width, int height, int r, int g, int b, int a);

  void new_resolution();
};

typedef Singleton<Render> S_Render;

#endif
