#include "config.h"

#include "renderer.hpp"

#include "graphics.hpp"

#include "theme.hpp"
#include "common.hpp"

#include "print.hpp"

#ifdef output_benchmark
#include "bench.hpp"
#endif

#include "touch.hpp"

#include "boost.hpp"

/* standard */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// ostringstream
#include <sstream>

/* we need to catch SIGHUP */
#include <signal.h>

// mkdir()
#include <sys/stat.h>
#include <sys/types.h>

#include <algorithm>

#include <typeinfo>

#include "gettext.hpp"

using std::vector;
using std::string;
using std::vector;

/* Various (mostly debugging) */
#include <time.h>

pthread_mutex_t Render::singleton_mutex = PTHREAD_MUTEX_INITIALIZER;

void Render::clear()
{
  imlib_image_clear_color(0,0,0,255);

  for (int i = 10; i < 20; ++i)
    current.elements[i].clear();
}

void Render::wait_and_aquire()
{
  image_mut.enterMutex();
  device->wait_for_output_done();
}

void Render::prepare_new_image()
{
  image_mut.enterMutex();

  S_Touch::get_instance()->clear();

  print_debug("Preparing new image", "RENDER");

  previous.clear();

  for (int i = 0; i < 10; ++i)
    previous.elements[i] = current.elements[i];

  device->wait_for_output_done();

  current.clear_no_delete();
}

void Render::draw_and_release(const std::string& module, bool overlay)
{
  draw(module, overlay);
  image_mut.leaveMutex();
}

bool Render::equal(GObj *lhs, GObj *rhs)
{
  if (typeid(*lhs) == typeid(*rhs)) {

    PFObj *pf1, *pf2;
    PFSObj *pfs1, *pfs2;
    PObj *p1, *p2;
    TObj *t1, *t2;
    RObj *r1, *r2;

    if ((t1 = dynamic_cast<TObj*>(lhs)) && (t2 = dynamic_cast<TObj*>(rhs)))
      return (*t1 == *t2);
    else if ((r1 = dynamic_cast<RObj*>(lhs)) && (r2 = dynamic_cast<RObj*>(rhs)))
      return (*r1 == *r2);
    else if ((pf1 = dynamic_cast<PFObj*>(lhs)) && (pf2 = dynamic_cast<PFObj*>(rhs)))
      return (*pf1 == *pf2);
    else if ((pfs1 = dynamic_cast<PFSObj*>(lhs)) && (pfs2 = dynamic_cast<PFSObj*>(rhs)))
      return (*pfs1 == *pfs2);
    else if ((p1 = dynamic_cast<PObj*>(lhs)) && (p2 = dynamic_cast<PObj*>(rhs)))
      return (*p1 == *p2);
    else
      std::cerr << "bug, not matching all graphic types" << std::endl;
  }

  return false;
}

void print_obj(GObj *g)
{
  if (PObj* p = dynamic_cast<PObj*>(g))
    std::cout << "picture: " << p->path << ":" << p->real_x << ":" << p->real_y
	      << ":" << p->real_w << ":" << p->real_h << ":"<< p->layer << std::endl;
  else if (TObj* t = dynamic_cast<TObj*>(g))
    std::cout << "text: " << t->text << ":" << t->real_x << ":" << t->real_y
	      << ":" << t->real_w << ":" << t->real_h
	      << ":" << t->font_color1 << ":" << t->font_color2 << ":" << t->font_color3
	      << ":" << t->layer << std::endl;
  else if (RObj* r = dynamic_cast<RObj*>(g))
    std::cout << "rectangle: " << r->r << ":" << r->g << ":"  << r->b << ":"
	      << r->a  << ":" << r->real_x  << ":" << r->real_y  << ":"
	      << r->real_w << ":" << r->real_h << ":" << r->layer << std::endl;
}

void Render::remove_overlapping_objects()
{
  // remove overlapping objects
  if (objects.size() > 1) {
    int pos = 0;
    while (pos < (objects.size()-1)) {
      bool next = false;
      for (graph_objs::iterator iter = objects.begin()+pos;
	   iter != objects.end() && pos < objects.size(); ++pos, ++iter) {
	for (graph_objs::iterator s_iter = objects.begin()+pos+1;
	     s_iter != objects.end(); ++s_iter) {
	  if (equal(iter->first, s_iter->first)) {
	    if (iter->second >= s_iter->second) {
	      objects.erase(s_iter);
	      next = true;
	      break;
	    } else if (intersect(iter->second, s_iter->second) && iter->first->transparent()) {

	      vector<rect> rects;

	      bool upper_used = false, lower_used = false;

	      if (iter->second.y > s_iter->second.y) {
		rect upper;
		upper.x = s_iter->second.x;
		upper.y = s_iter->second.y;
		upper.w = s_iter->second.w;
		upper.h = iter->second.y - s_iter->second.y;
		rects.push_back(upper);
		upper_used = true;
	      }

	      if (iter->second.y+iter->second.h < s_iter->second.y + s_iter->second.h) {
		rect lower;
		lower.x = s_iter->second.x;
		lower.y = iter->second.y+iter->second.h;
		lower.w = s_iter->second.w;
		lower.h = (s_iter->second.y + s_iter->second.h) - lower.y;
		rects.push_back(lower);
		lower_used = true;
	      }

	      if (!(iter->second.x <= s_iter->second.x && (iter->second.x + iter->second.w) >= (s_iter->second.x + s_iter->second.w))) {
		if (iter->second.x + iter->second.w < s_iter->second.x + s_iter->second.w) {
		  rect right;
		  right.x = iter->second.x + iter->second.w;
		  if (upper_used)
		    right.y = iter->second.y;
		  else
		    right.y = s_iter->second.y;
		  right.w = (s_iter->second.x + s_iter->second.w) - (iter->second.x + iter->second.w);
		  if (lower_used)
		    right.h = iter->second.y+iter->second.h - right.y;
		  else
		    right.h = s_iter->second.y + s_iter->second.h - right.y;
		  rects.push_back(right);
		}
		if (iter->second.x > s_iter->second.x) {
		  rect left;
		  left.x = s_iter->second.x;
		  if (upper_used)
		    left.y = iter->second.y;
		  else
		    left.y = s_iter->second.y;
		  left.w =  iter->second.x - s_iter->second.x;
		  if (lower_used)
		    left.h = iter->second.y+iter->second.h - left.y;
		  else
		    left.h = s_iter->second.y + s_iter->second.h - left.y;
		  rects.push_back(left);
		}
	      }

	      GObj *g = s_iter->first;

	      objects.erase(s_iter);

	      foreach (rect& rectangle, rects)
		objects.push_back(std::make_pair(g, rectangle));

	      next = true;
	      break;
	    }
	  }
	}
	if (next)
	  break;
      }
    }
  }
}

void Render::draw(const string& module, bool overlay)
{
#ifdef output_benchmark
  Bench bench;
  bench.start();
#endif

  if (conf->p_blur()) {
    imlib_context_set_image(current.image_data);
    imlib_free_image();

    imlib_context_set_image(current_nonblur);
    current.image_data = imlib_clone_image();
    imlib_context_set_image(current.image_data);
  }

  // clean up
  objects.clear();

  // find out what items are the same as last time

  int layer = 0;
  foreach (Image::gelements& current_element, current.elements) {
    if (overlay) {
      if (layer < 10) {
	foreach (GObj *obj, current_element)
	  obj->dirty = true;
	Image::gelements e = vector_lookup(previous.elements, layer);
	foreach (GObj *obj, e)
	  obj->dirty = true;
      }
    }

    if (layer >= 10) {
      foreach (GObj *obj, current_element) {
	Image::gelements e = vector_lookup(prev_overlay.elements, layer);
	foreach (GObj *prev_obj, e) {
	  if (equal(obj, prev_obj)) {
	    obj->dirty = true;
	    break;
	  }
	}
      }
    }

    foreach (GObj *obj, current_element) {
      Image::gelements e = vector_lookup(previous.elements, layer);
      foreach (GObj *prev_obj, e) {
	if (equal(obj, prev_obj)) {
	  obj->dirty = true;
	  prev_obj->dirty = true;
	  break;
	}
      }
    }
    ++layer;
  }

  // currently objects that needs to be removed is marked !dirty in previous
  // and things that needs to be be drawed is marked as !dirty in current.

  Image::gelements stale_objects;
  Image::gelements rem_objects;
  Image::gelements new_objects;

  // stats

  // area redrawed and total area
  int area = 0, total_area = 0;
  int prev_elements = 0, cur_elements = 0;

  // lets split up the objects that needs to be modified into three:
  // rem_objects, stale_objects and new_objects

  foreach (Image::gelements& prev_element, previous.elements)
    foreach (GObj *obj, prev_element) {
      total_area += obj->w * obj->h;
      if (!obj->dirty) {
	area += obj->w * obj->h;
	rem_objects.push_back(obj);
      }
      ++prev_elements;
    }

  foreach (Image::gelements& current_element, current.elements)
    foreach (GObj *obj, current_element) {
      total_area += obj->w * obj->h;
      if (!obj->dirty) {
	area += obj->w * obj->h;
	new_objects.push_back(obj);
      } else
	stale_objects.push_back(obj);
      ++cur_elements;
    }

#if 0 // debug
  std::cout << "========== new =======" << std::endl;
  foreach (GObj *new_obj, new_objects)
    print_obj(new_obj);
#endif

#if 0 // debug
  std::cout << "========== removed =======" << std::endl;
  foreach (GObj *rem_obj, rem_objects)
    print_obj(rem_obj);

  std::cout << "area stat:" << (area*100)/total_area << std::endl;
#endif

  // optimization, nothing to draw
  if (rem_objects.size() == 0 && new_objects.size() == 0) {

    draw_rects.clear();
    device->output_is_ready();
    clean_up_for_next_draw(overlay);

    return;
  }

  // if two screens have almost nothing in common
  if (prev_elements - 1 == rem_objects.size() || prev_elements == rem_objects.size() ||
      (area*100)/total_area > 75) {

    print_debug("Drawing whole background", "RENDER");

    Image::gelements all_objects;

    foreach (GObj *new_obj, new_objects)
      all_objects.push_back(new_obj);

    foreach (GObj *stale_obj, stale_objects)
      all_objects.push_back(stale_obj);

    // sort according to layer
    std::sort(all_objects.begin(), all_objects.end(), SDescLayer());

    foreach (GObj *obj, all_objects)
      obj->draw();

    objects.push_back(std::make_pair(new TempGObj(0, 0, 0, conf->p_h_res(), conf->p_v_res()),
				     rect(0, 0, conf->p_h_res(), conf->p_v_res())));

  } else {

    rect overlapping_rectangle;
    // lets check if some of the new objects intersects with objects on higher
    // levels
    foreach (GObj *new_obj, new_objects)
      foreach (GObj *stale_obj, stale_objects)
	if (new_obj->layer < stale_obj->layer)
	  if (intersect(new_obj, stale_obj, overlapping_rectangle))
	    objects.push_back(std::make_pair(stale_obj, overlapping_rectangle));

    // lets check if some of the objects to be removed intersects with objects on
    // other levels
    foreach (GObj *rem_obj, rem_objects)
      foreach (GObj *stale_obj, stale_objects)
	if (intersect(rem_obj, stale_obj, overlapping_rectangle))
	  objects.push_back(std::make_pair(stale_obj, overlapping_rectangle));

    foreach (GObj *new_obj, new_objects)
      objects.push_back(std::make_pair(new_obj, rect(new_obj->real_x, new_obj->real_y,
						     new_obj->real_w, new_obj->real_h)));

    remove_overlapping_objects();

    // sort according to layer
    std::sort(objects.begin(), objects.end(), SDescLayer());

#ifdef output_benchmark
    bench.stop();
    bench.print("done calculating for module: " + module);
    bench.start();
#endif

#if 0
    std::cout << "=============" << module << "==================" << std::endl;

    foreach (covered_obj& obj, objects)
      print_obj(obj.first);

    std::cout << "===============================" << std::endl;
#endif

    foreach (covered_obj& obj, objects) {

      // new objects
      if (obj.second.x == obj.first->real_x && obj.second.y == obj.first->real_y &&
	  obj.second.w == obj.first->real_w && obj.second.h == obj.first->real_h) {
#if 0
	print_obj(obj.first);
	std::cout << "new obj at: " << obj.second.x << ":" << obj.second.y << ":"
		  << obj.second.w << ":" << obj.second.h << std::endl;
#endif
	obj.first->draw();

      } else {

#if 0
	print_obj(obj.first);
	std::cout << "overlapping at: " << obj.second.x << ":" << obj.second.y << ":"
		  << obj.second.w << ":" << obj.second.h << std::endl;
#endif

	if (dynamic_cast<PObj*>(obj.first)) {
#if 0
	  std::cout << "PObjs at:" << dynamic_cast<PObj*>(obj.first)->path << " " << std::abs(obj.second.x - obj.first->real_x) << ":" << std::abs(obj.second.y - obj.first->real_y) << ":" << obj.second.w << ":" << obj.second.h << std::endl;
#endif
	  image_rectangle(obj.first, std::abs(obj.second.x - obj.first->real_x),
			  std::abs(obj.second.y - obj.first->real_y), obj.second.w, obj.second.h, obj.second.x, obj.second.y);
	}
	else if (RObj *r = dynamic_cast<RObj*>(obj.first)) {
#if 0
	  std::cout << "RObjs colors: " << r->r << ":" << r->g << ":" << r->b << ":" << r->a << std::endl;
	  std::cout << "RObjs pos: " << obj.second.x << ":" << obj.second.y << ":" << obj.second.w << ":" << obj.second.h << std::endl;
#endif

	  rectangle(obj.second.x, obj.second.y, obj.second.w, obj.second.h, r->r, r->g, r->b, r->a);
	}
	else if (TObj *t = dynamic_cast<TObj*>(obj.first)) {
#if 0
	  std::cout << "TObjs " << t->text << std::endl;
#endif
	  t->draw_part(obj.second);
	}
      }
    }
  }

  // since everything has been drawed to the new surface, there is no need to
  // blit completely overlapping rectangles
  draw_rects.clear();

  int pos = 0;
  for (graph_objs::iterator iter = objects.begin()+pos; iter != objects.end(); ++pos, ++iter) {
    for (graph_objs::iterator s_iter = objects.begin()+pos+1; s_iter != objects.end();) {
      if (contains(iter->second, s_iter->second))
	s_iter = objects.erase(s_iter);
      else
	++s_iter;
    }
  }

  foreach (covered_obj& obj, objects)
    draw_rects.push_back(obj.second);

#ifdef output_benchmark
  bench.stop();
  bench.print("done drawing for module: " + module);

  int new_objs = new_objects.size();
  int stale_count = 0;
#endif

  imlib_context_set_image(current.image_data);

  if (draw_rects.size() == 0)
    return;

  if (conf->p_blur())
    blur();

  // update screen
  device->output_is_ready();

  clean_up_for_next_draw(overlay);

#ifdef output_benchmark
  std::cout << "############## ---> STATS <--- ###############" << std::endl;
  std::cout << "total area redrawed: " << area << " out of a total " << total_area << std::endl;
  std::cout << "##############################################" << std::endl;
  std::cout << "Before intersect test:" << std::endl;
  std::cout << "in " << new_objs << " new objects" << std::endl;
  std::cout << "in " << rem_objects.size() << " removed objects" << std::endl;
  std::cout << "##############################################" << std::endl;
  std::cout << "After intersect test:" << std::endl;
  std::cout << "in " << stale_count  << " stale objects" << std::endl;
  std::cout << "##############################################" << std::endl;
  std::cout << "out of a total " << cur_elements << " currrent objects" << std::endl;
  std::cout << "out of a total " << prev_elements << " previous objects" << std::endl;
  std::cout << "##############################################" << std::endl;
#endif
}

void Render::clean_up_for_next_draw(bool overlay)
{
  foreach (Image::gelements& current_element, current.elements)
    foreach (GObj *obj, current_element)
      obj->dirty = false;

  for (int i = 10; i < 20; ++i)
    prev_overlay.elements[i] = current.elements[i];
}

void Render::complete_draw()
{
  draw_rects.clear();
  draw_rects.push_back(rect(0, 0, conf->p_h_res(), conf->p_v_res()));

  // update screen
  device->output_is_ready();
}

void Render::complete_redraw()
{
  device->wait_for_output_done();

  print_debug("Redrawing everything", "RENDER");

  complete_draw();
}

bool Render::contains(const rect& outer, const rect& inner)
{
  if (outer.x <= inner.x && outer.y <= inner.y &&
      outer.x + outer.w >= inner.x + inner.w &&
      outer.y + outer.h >= inner.y + inner.h)
    return true;
  else
    return false;
}

bool Render::intersect_check(int x1, int w1, int y1, int h1,
			     int x2, int w2, int y2, int h2)
{
  if (w1 <= x2 || w2 <= x1 || h1 <= y2 || h2 <= y1)
    return false;
  else
    return true;
}

bool Render::intersect(GObj *lhs, GObj *rhs)
{
  int rhs_x_r = rhs->real_x + rhs->real_w;
  int rhs_y_r = rhs->real_y + rhs->real_h;
  int lhs_x_r = lhs->real_x + lhs->real_w;
  int lhs_y_r = lhs->real_y + lhs->real_h;

  return intersect_check(lhs->real_x, lhs_x_r, lhs->real_y, lhs_y_r,
			 rhs->real_x, rhs_x_r, rhs->real_y, rhs_y_r);
}

bool Render::intersect(const rect& lhs, const rect& rhs)
{
  int rhs_x_r = rhs.x + rhs.w;
  int rhs_y_r = rhs.y + rhs.h;
  int lhs_x_r = lhs.x + lhs.w;
  int lhs_y_r = lhs.y + lhs.h;

  return intersect_check(lhs.x, lhs_x_r, lhs.y, lhs_y_r,
			 rhs.x, rhs_x_r, rhs.y, rhs_y_r);
}

bool Render::intersect(const rect& lhs, GObj *rhs, rect& r)
{
  int rhs_x_r = rhs->real_x + rhs->real_w;
  int rhs_y_r = rhs->real_y + rhs->real_h;
  int lhs_x_r = lhs.x + lhs.w;
  int lhs_y_r = lhs.y + lhs.h;

  if (intersect_check(lhs.x, lhs_x_r, lhs.y, lhs_y_r,
		      rhs->real_x, rhs_x_r, rhs->real_y, rhs_y_r)) {
    int x = std::max(lhs.x, rhs->real_x);
    int y = std::max(lhs.y, rhs->real_y);
    int w = std::min(lhs_x_r, rhs_x_r) - x;
    int h = std::min(lhs_y_r, rhs_y_r) - y;
    r = rect(x, y, w, h);

    return true;
  } else
    return false;
}

bool Render::intersect(GObj *lhs, GObj *rhs, rect& r)
{
  if (intersect(lhs, rhs)) {

    int rhs_x_r = rhs->real_x + rhs->real_w;
    int rhs_y_r = rhs->real_y + rhs->real_h;
    int lhs_x_r = lhs->real_x + lhs->real_w;
    int lhs_y_r = lhs->real_y + lhs->real_h;

    int x = std::max(lhs->real_x, rhs->real_x);
    int y = std::max(lhs->real_y, rhs->real_y);
    int w = std::min(lhs_x_r, rhs_x_r) - x;
    int h = std::min(lhs_y_r, rhs_y_r) - y;

    r = rect(x, y, w, h);

    return true;
  } else
    return false;
}

#define IMGCACHE

Imlib_Image Render::image_loader(const string& path, Scaling scaleable, double scale_factor_h,
				 double scale_factor_w, int orientation)
{
  Imlib_Image new_image;

  bool found_match = false;

#ifdef IMGCACHE
  foreach (queue_obj& obj, scaled_images)
    if (obj.path == path && obj.orientation == orientation
	&& obj.scaleable == scaleable && obj.scale_width == scale_factor_w
	&& obj.scale_height == scale_factor_h) {
      print_debug("found match", "RENDER");

      found_match = true;
      new_image = obj.image;

      ++obj.count;

      if (scaleable == NOSCALING) {
	imlib_context_set_image(new_image);
	if (orientation != 0)
	  imlib_image_orientate(orientation);
      }

      break;
    }
#endif

  if (!found_match) {

    std::ostringstream out;

    if (scaleable != NOSCALING) {

      Imlib_Image img = imlib_load_image(path.c_str());

      imlib_context_set_image(img);

      if (orientation != 0)
	imlib_image_orientate(orientation);

      int w = imlib_image_get_width();
      int h = imlib_image_get_height();

      int scaled_width = round_to_int(w*scale_factor_w);
      int scaled_height = round_to_int(h*scale_factor_h);

      if (scaleable == SCALE_FULL)
	new_image = imlib_create_cropped_scaled_image(0, 0, w, h, scaled_width, scaled_height);
      else if (scaleable == SCALE_VERTICAL)
	new_image = imlib_create_cropped_scaled_image(0, 0, w, h, scaled_width, h);
      else if (scaleable == SCALE_HORIZONTAL)
	new_image = imlib_create_cropped_image(0, h-round_to_int(h*scale_factor_h), w, round_to_int(h*scale_factor_h));

      imlib_free_image();

      imlib_context_set_image(new_image);

#if 0
      out << "inserting image: " << path << ":" << round_to_int(w*scale_factor_w)
	  << ":" << round_to_int(h*scale_factor_h)
	  << ":" << orientation << ":" << scale_factor_w << ":" << scale_factor_h;
#endif

    } else {

      new_image = imlib_load_image(path.c_str());
      imlib_context_set_image(new_image);
      if (orientation != 0)
	imlib_image_orientate(orientation);

#if 0
      out << "inserting image without scaling: " << path << ":" << orientation;
#endif
    }

#if 0
    print_debug(out.str(), "RENDER");
#endif

#ifdef IMGCACHE
    if (scaled_images.size() > conf->p_image_cache()) {
      int min = scaled_images.front().count + 1;
      image_queue::iterator iter_kick;

      // find the LRU and kick it out
      for (image_queue::iterator im = scaled_images.begin(),
            im_end = scaled_images.end(); im != im_end; ++im) {
	if (im->count < min) {
	  iter_kick = im;
	  min = im->count;
	}
	--im->count;
      }

      imlib_context_set_image(iter_kick->image);
      imlib_free_image();
      scaled_images.erase(iter_kick);
    }

    scaled_images.push_back(queue_obj(path, new_image, orientation, scaleable, scale_factor_w, scale_factor_h));
#endif
  }

  if (!new_image){ /* note, the imlib image context isn't really restored after
		      calling this method, it's just set to whatever 'image'
		      parameter is */
    print_critical(gettext("The image ") + path + gettext(" could not be loaded"), "RENDER");
    new_image = imlib_create_image(1, 1);
    imlib_context_set_image(new_image);
    imlib_image_clear_color(0,0,0,255);
  } else
    imlib_context_set_image(new_image);

  return new_image;
}

void Render::image_rectangle(GObj *p, int x, int y, int w, int h,
			     int x_in_d, int y_in_d)
{
  if (PFObj *pic = dynamic_cast<PFObj*>(p)) {
    int border_size = 1;

    if (pic->border)
      border_size = 5;

    if (pic->black_bordered) {
      image_rectangle_bb(pic, x, y, w, h, x_in_d, y_in_d);
      return;
    }
  }

  image_rectangle_no_bb(p, x, y, w, h, x_in_d, y_in_d);
}

void Render::image_rectangle_no_bb(GObj *p, int x, int y, int w, int h, int x_in_d, int y_in_d)
{
  Themes *themes = S_Themes::get_instance();

  string path, pic_path;
  Scaling scaleable = NOSCALING;
  double scale_factor_h = 1, scale_factor_w = 1;
  int orientation = 0;

  // borders
  int orig_x = x_in_d;
  int orig_y = y_in_d;
  int orig_w = w;
  int orig_h = h;

  if (PFSObj *pic = dynamic_cast<PFSObj*>(p)) {

    if (pic->zoom_level != 1) {
      
      Imlib_Image new_image = image_loader(pic->path, pic->scaleable, pic->scale_factor_h, pic->scale_factor_w, pic->orientation);

      int image_w = imlib_image_get_width();
      int image_h = imlib_image_get_height();
      
      imlib_context_set_image(current.image_data);
      
      int x_pos, y_pos;

      if (double(image_w)/image_h > 1 && (pic->scale_factor_w < 1 || pic->scale_factor_h < 1)) {
	
	x_pos = (pic->zoom_position-(pic->zoom_position/pic->zoom_v_levels)*pic->zoom_v_levels)*image_w/(2*pic->zoom_level);
	y_pos = round_to_int(pic->zoom_position/pic->zoom_v_levels)*image_h/(2*pic->zoom_level);
    
      } else {

	x_pos = std::min((pic->zoom_position-(pic->zoom_position/pic->zoom_v_levels)*pic->zoom_v_levels)*image_w/(2*pic->zoom_level), std::max(0, image_w-(conf->p_h_res())));
	y_pos = std::min(round_to_int(pic->zoom_position/pic->zoom_v_levels)*image_h/(2*pic->zoom_level),std::max(0, image_h-(conf->p_v_res())));

      }

      double scale_w = double(image_w)/conf->p_h_res(), scale_h = double(image_h)/conf->p_v_res();

      double x_factor, y_factor;

      if (pic->zoom_v_levels == 1) {
	x_factor = 1;
      } else if (pic->zoom_v_levels == 2) {
	x_factor = 3.0/4.0;
      } else if (pic->zoom_v_levels == 3) {
	x_factor = 1.0/2.0;
      }
      
      if (pic->zoom_h_levels == 1) {
	y_factor = 1;
      } else if (pic->zoom_h_levels == 2) {
	y_factor = 3.0/4.0;
      } else if (pic->zoom_h_levels == 3) {
	y_factor = 1.0/2.0;
      }

      int x_in_original = x_pos + x;
      int y_in_original = y_pos + y;
      int w_in_original = w;
      int h_in_original = h;

      if (scale_w <= 1 && scale_h <= 1) {
	x_in_original = x_pos + x * x_factor;
	y_in_original = y_pos + y * y_factor;
	w_in_original = w * x_factor;
	h_in_original = h * y_factor;
      }

      if (scale_w > 1 && scale_h > 1 &&
	  pic->zoom_v_levels == 2 && pic->zoom_h_levels == 3 &&
	  (pic->zoom_position == 2 || pic->zoom_position == 3))
	y_in_original += conf->p_v_res()/6;

      imlib_blend_image_onto_image(new_image, 0, x_in_original, y_in_original, 
				   w_in_original, h_in_original, x_in_d, y_in_d, w, h);
      return;

    } else {
      pic_path = pic->path;
      scaleable = pic->scaleable;
      scale_factor_h = pic->scale_factor_h;
      scale_factor_w = pic->scale_factor_w;
      orientation = pic->orientation;
    }

  } else if (PFObj *pic = dynamic_cast<PFObj*>(p)) {

    pic_path = pic->path;
    scaleable = pic->scaleable;
    scale_factor_h = pic->scale_factor_h;
    scale_factor_w = pic->scale_factor_w;
    orientation = pic->orientation;

    if (!pic->transparent()) {

      // find out if we should paint all borders

      int border_size = 1;

      if (pic->border)
	border_size = 5;

      bool left_border = false, right_border = false,
	top_border = false, bottom_border = false;

      if (x == 0)
	left_border = true;

      if (x + w == pic->w+border_size*2)
	right_border = true;

      if (y == 0)
	top_border = true;

      if (y + h == pic->h+border_size*2)
	bottom_border = true;

      x -= border_size;

      if (right_border)
	w -= border_size;

      y -= border_size;

      if (bottom_border)
	h -= border_size;

      if (top_border)
	rectangle(orig_x, pic->real_y, orig_w, border_size,
		  themes->general_marked_picture_color1,
		  themes->general_marked_picture_color2,
		  themes->general_marked_picture_color3,
		  themes->general_marked_picture_color4);

      if (left_border)
	rectangle(pic->real_x, orig_y, border_size, orig_h,
		  themes->general_marked_picture_color1,
		  themes->general_marked_picture_color2,
		  themes->general_marked_picture_color3,
		  themes->general_marked_picture_color4);

      if (bottom_border)
	rectangle(orig_x, pic->real_y+pic->real_h-border_size, orig_w, border_size,
		  themes->general_marked_picture_color1,
		  themes->general_marked_picture_color2,
		  themes->general_marked_picture_color3,
		  themes->general_marked_picture_color4);

      if (right_border)
	rectangle(pic->real_x+pic->real_w-border_size, orig_y, border_size, orig_h,
		  themes->general_marked_picture_color1,
		  themes->general_marked_picture_color2,
		  themes->general_marked_picture_color3,
		  themes->general_marked_picture_color4);
    }

  } else if (PObj *pic = dynamic_cast<PObj*>(p)) {
    pic_path = pic->path;
    scaleable = pic->scaleable;
    scale_factor_h = pic->scale_factor_h;
    scale_factor_w = pic->scale_factor_w;
    orientation = pic->orientation;
  }

  Imlib_Image img = image_loader(pic_path, scaleable, scale_factor_h, scale_factor_w, orientation);

  imlib_context_set_image(current.image_data);

  imlib_blend_image_onto_image(img, 0, x, y, w, h, x_in_d, y_in_d, w, h);
}

void Render::image_rectangle_bb(GObj *g, int x, int y, int w, int h,
				int x_in_d, int y_in_d)
{
  PFObj *pic = dynamic_cast<PFObj*>(g);

  Themes *themes = S_Themes::get_instance();

  string path, pic_path;
  Scaling scaleable = NOSCALING;
  double scale_factor_h = 1, scale_factor_w = 1;
  int orientation = 0;

  int black_w = w;
  int black_h = h;

  int black_x_in_d = x_in_d;
  int black_y_in_d = y_in_d;

  // borders
  int orig_x = x_in_d;
  int orig_y = y_in_d;
  int orig_w = w;
  int orig_h = h;

  // find out if we should paint all borders

  int border_size = 1;

  if (pic->border)
    border_size = 5;

  bool left_border = false, right_border = false,
    top_border = false, bottom_border = false;

  if (x == 0)
    left_border = true;

  if (x + w == pic->real_w)
    right_border = true;

  if (y == 0)
    top_border = true;

  if (y + h == pic->real_h)
    bottom_border = true;

  if (left_border)
    w -= border_size;

  if (right_border)
    w -= border_size;

  if (top_border)
    h -= border_size;

  if (bottom_border)
    h -= border_size;

  black_h = h;
  black_w = w;

  // the amount of black including borders
  int black_horiz_sides = pic->x - pic->real_x;

  if (black_horiz_sides > 0) {
    if (left_border) {
      black_x_in_d += border_size;
      x_in_d += black_horiz_sides;
      x += black_horiz_sides;
      w -= black_horiz_sides - border_size;
    }
    if (right_border) {
      w -= black_horiz_sides - border_size;
      x -= black_horiz_sides;
      //w -= black_horiz_sides;
    }
  }

  // the amount of black including borders
  int black_vert_sides = pic->y - pic->real_y;

  if (black_vert_sides > 0) {
    if (top_border) {
      black_y_in_d += border_size;
      y_in_d += black_vert_sides;
      //y += black_vert_sides + border_size;
      h -= black_vert_sides - border_size;
    }
    if (bottom_border) {
      //y -= black_vert_sides + border_size;
      h -= black_vert_sides - border_size;
      //      h -= border_size;
    }
  }

  pic_path = pic->path;
  scaleable = pic->scaleable;
  scale_factor_h = pic->scale_factor_h;
  scale_factor_w = pic->scale_factor_w;
  orientation = pic->orientation;

  if (top_border) {
    rectangle(orig_x, pic->real_y, orig_w, border_size,
	      themes->general_marked_picture_color1,
	      themes->general_marked_picture_color2,
	      themes->general_marked_picture_color3,
	      themes->general_marked_picture_color4);
  }

  if (left_border) {
    rectangle(pic->real_x, orig_y, border_size, orig_h,
	      themes->general_marked_picture_color1,
	      themes->general_marked_picture_color2,
	      themes->general_marked_picture_color3,
	      themes->general_marked_picture_color4);
  }

  if (bottom_border) {
    rectangle(orig_x, pic->real_y+pic->real_h-border_size, orig_w, border_size,
	      themes->general_marked_picture_color1,
	      themes->general_marked_picture_color2,
	      themes->general_marked_picture_color3,
	      themes->general_marked_picture_color4);
  }

  if (right_border) {
    rectangle(pic->real_x+pic->real_w-border_size, orig_y, border_size, orig_h,
	      themes->general_marked_picture_color1,
	      themes->general_marked_picture_color2,
	      themes->general_marked_picture_color3,
	      themes->general_marked_picture_color4);
  }

  // at this point x_in_d and y_in_d together with w and h represents the
  // rectangle of the picture that needs redrawing in cordinates relative to
  // the whole screen

  imlib_context_set_color(0, 0, 0, 255);
  imlib_image_fill_rectangle(black_x_in_d, black_y_in_d, black_w, black_h);

  Imlib_Image img = image_loader(pic_path, scaleable, scale_factor_h, scale_factor_w, orientation);

  imlib_context_set_image(current.image_data);

  imlib_blend_image_onto_image(img, 0, x, y, w, h, x_in_d, y_in_d, w, h);
}

// ----------------------------------------------- //

Render::Render()
{
  conf = S_Config::get_instance();

  width = conf->p_h_res();
  height = conf->p_v_res();

  show_audio_track = true; 

  if (conf->p_blur()) {
    current_nonblur = new_imlib_image();
  }

  // initialize the images
  current.image_data = new_imlib_image();
  imlib_context_set_image(current.image_data);

  // cache
  imlib_set_cache_size(2096 * 1024);
  imlib_set_font_cache_size(512 * 1024);

  // nicer scaling
  imlib_context_set_anti_alias(1);

  // dither for depths < 24bpp
  imlib_context_set_dither(1);

  // enable blending (should be faster)
  imlib_context_set_blend(1);

  // font
  std::ostringstream path;
  path << mms_prefix << "/share/mms/fonts/";
  imlib_add_path_to_font_path(path.str().c_str());
  clear();
}

Render::~Render()
{
  foreach (queue_obj& obj, scaled_images) {
    imlib_context_set_image(obj.image);
    imlib_free_image();
  }

  if (conf->p_blur()) {
    imlib_context_set_image(current_nonblur);
    imlib_free_image();
  }
}

void Render::new_resolution()
{
  width = conf->p_h_res();
  height = conf->p_v_res();

  // initialize the images
  current.image_data = new_imlib_image();
  imlib_context_set_image(current.image_data);
}

void Render::set_theme()
{
  Theme *theme = S_Theme::get_instance();

  std::ostringstream cur_theme;
  cur_theme << mms_prefix << "/share/mms/themes/"
	    << theme->cur_theme << "/";
  default_path = cur_theme.str();
}

Imlib_Image Render::new_imlib_image()
{
  return imlib_create_image(width, height);
}

void Render::image(const string& path, int x, int y, int w, int h,
		   Scaling scaleable, double scale_factor_h, double scale_factor_w, int orientation)
{
  Imlib_Image new_image = image_loader(path, scaleable, scale_factor_h, scale_factor_w, orientation);

  imlib_context_set_image(current.image_data);

  imlib_blend_image_onto_image(new_image, 0, 0, 0, w, h, x, y, w, h);
}

void Render::blur()
{
  imlib_context_set_image(current_nonblur);
  imlib_free_image();

  imlib_context_set_image(current.image_data);
  current_nonblur = imlib_clone_image();

  Imlib_Image new_image = image_loader(default_path + "blur.png", NOSCALING, 1, 1, 0);

  imlib_context_set_image(current.image_data);

  imlib_image_blur(1);
  imlib_blend_image_onto_image(new_image, 0, 0, 0, 1, 1, 0, 0, 1, 1);

  imlib_context_set_image(current.image_data);
  imlib_image_blur(0);
}

std::pair<int, int> Render::image_dimensions(const string& path, bool *transparent)
{
  Imlib_Image img = imlib_load_image(path.c_str());
  imlib_context_set_image(img);

  *transparent = imlib_image_has_alpha();

  int w = imlib_image_get_width();
  int h = imlib_image_get_height();
  imlib_free_image();
  imlib_context_set_image(current.image_data);
  return std::make_pair(h, w);
}

std::string Render::image_get_fullpath(const string& path, int max_width, int max_height)
{
  struct stat64 statbuf;
  stat64(path.c_str(), &statbuf);

  string::size_type pos = path.rfind(".");

  std::ostringstream filename; filename << conf->p_temp_image_loc()
					<< ((pos != string::npos) ? path.substr(0, pos) : path)
					<< "_" << statbuf.st_mtime
					<< "_" << max_width
					<< "_" << max_height
					<< ".jpg";

  return filename.str();
}

void Render::image_fullpath(const string& path, int x, int y,
			    bool use_border, Scaling scaleable, double scale_factor_h,
			    double scale_factor_w, int orientation, int bx, int by, int bw, int bh,
			    bool black_background)
{
  Themes *themes = S_Themes::get_instance();

  Imlib_Image new_image = image_loader(path, scaleable, scale_factor_h, scale_factor_w, orientation);

  int w = imlib_image_get_width();
  int h = imlib_image_get_height();

  bool has_transparency = imlib_image_has_alpha();

  imlib_context_set_image(current.image_data);

  if (!has_transparency) {
    imlib_context_set_color(themes->general_marked_picture_color1,
			    themes->general_marked_picture_color2,
			    themes->general_marked_picture_color3,
			    themes->general_marked_picture_color4);

    if (use_border || (h > 1) || (w > 1))
      imlib_image_fill_rectangle(bx, by, bw, bh);

    if (black_background && use_border) {
      imlib_context_set_color(0, 0, 0, 255);
      imlib_image_fill_rectangle(bx+5, by+5, bw-10, bh-10);
    } else if (black_background) {
      imlib_context_set_color(0, 0, 0, 255);
      imlib_image_fill_rectangle(bx+1, by+1, bw-2, bh-2);
    }
  }

  imlib_blend_image_onto_image(new_image, 0, 0, 0, w, h, x, y, w, h);
}

bool Render::image_fullscreen_dimensions(const string& path, bool zoom, int orientation,
					 int zoom_level, int zoom_position,
					 int &real_x, int &real_y, int &real_w, int &real_h,
					 int &w, int &h, bool *transparent)
{
  int max_x = conf->p_h_res(), max_y = conf->p_v_res();

  Imlib_Image img = imlib_load_image(path.c_str());
  imlib_context_set_image(img);

  *transparent = imlib_image_has_alpha();

  // rotation
  if (orientation != 0)
    imlib_image_orientate(orientation);

  w = imlib_image_get_width();
  h = imlib_image_get_height();

  imlib_free_image();

  double ratio = double(w)/h;

  if (zoom_level > 1) {

    bool ok = false;

    if (w < conf->p_h_res() || h < conf->p_v_res()) {

      real_w = w*zoom_level;
      real_h = h*zoom_level;

      ok = true;

    } else if (ratio < 1) {

      real_w = w;
      real_h = h;

      ok = true;
    }

    if (ok) {
      double scale_x = 2.0, scale_y = 2.0;

      if (real_w > 2*max_x)
	scale_x = double(2*max_x)/w;

      if (real_h > 2*max_y)
	scale_y = double(2*max_y)/h;

      double min_scale = std::min(scale_x, scale_y);

      real_w = round_to_int(w*min_scale);
      real_h = round_to_int(h*min_scale);

      real_x = 0;
      real_y = 0;

      if (real_w < conf->p_h_res())
	real_x = (conf->p_h_res() - real_w)/2;

      if (real_h < conf->p_v_res())
	real_y = (conf->p_v_res() - real_h)/2;

      imlib_context_set_image(current.image_data);
      return true;
    }
  }

  if (ratio > double(max_x)/max_y) {
    if (w > max_x) {
      real_w = max_x;
      real_h = round_to_int(max_x/ratio+0.5);
      real_x = 0;
      real_y = (max_y-real_h)/2;
    } else {
      if (!zoom) {
	real_w = w;
	real_h = h;
	real_x = (max_x-real_w)/2;
	real_y = (max_y-real_h)/2;
      } else {
	if (w > h) {
	  real_w = max_x;
	  real_h = round_to_int(max_x/ratio);
	  real_x = 0;
	  real_y = (max_y-real_h)/2;
	} else {
	  real_w = round_to_int(max_y*ratio);
	  real_h = max_y;
	  real_x = (max_x-real_w)/2;
	  real_y = 0;
	}
      }
    }
  } else if (h > max_y) {
    real_w = round_to_int(max_y*ratio);
    real_h = max_y;
    real_x = (max_x-real_w)/2;
    real_y = 0;
  } else if (!zoom) {
    real_w = w;
    real_h = h;
    real_x = (max_x-real_w)/2;
    real_y = (max_y-real_h)/2;
  } else if (w > h) {
    if (max_x/ratio > max_y) {
      real_w = round_to_int(max_y*ratio);
      real_h = max_y;
      real_x = (max_x - real_w)/2;
    } else {
      real_w = max_x;
      real_h = round_to_int(max_x/ratio);
      real_x = 0;
    }
    real_y = (max_y-real_h)/2;
  } else {
    real_w = round_to_int(max_y*ratio);
    real_h = max_y;
    real_x = (max_x-real_w)/2;
    real_y = 0;
  }

  bool scaleable = false;

  if (w > max_x || h > max_y || zoom)
    scaleable = true;

  imlib_context_set_image(current.image_data);

  return scaleable;
}

void Render::image_fullscreen_zoom(const string& path, int orientation, int zoom_level, int zoom_position,
				   int zoom_vertical_levels, int zoom_horizontal_levels,
				   int x, int y, int scaled_w, int scaled_h,
				   Scaling scaleable, double scale_factor_h, double scale_factor_w)
{
  Imlib_Image new_image = image_loader(path, scaleable, scale_factor_h, scale_factor_w, orientation);

  int w = imlib_image_get_width();
  int h = imlib_image_get_height();

  imlib_context_set_image(current.image_data);

  int x_pos, y_pos;
  int vertical_print, horizontal_print;

  if (double(w)/h > 1 && (scale_factor_w < 1 || scale_factor_h < 1)) {

    x_pos = (zoom_position-(zoom_position/zoom_vertical_levels)*zoom_vertical_levels)*w/(2*zoom_level);
    y_pos = round_to_int(zoom_position/zoom_vertical_levels)*h/(2*zoom_level);

    vertical_print = scaled_w/2;
    horizontal_print = scaled_h/2;

  } else {

    x_pos = std::min((zoom_position-(zoom_position/zoom_vertical_levels)*zoom_vertical_levels)*w/(zoom_vertical_levels),
		     std::max(0, w-(conf->p_h_res())));
    y_pos = std::min(round_to_int(zoom_position/zoom_vertical_levels)*h/(zoom_horizontal_levels),
		     std::max(0, h-(conf->p_v_res())));

    vertical_print = std::min(scaled_w, conf->p_h_res());
    horizontal_print = std::min(scaled_h, conf->p_v_res());

    scaled_w = vertical_print;
    scaled_h = horizontal_print;
  }

  imlib_blend_image_onto_image(new_image, 0, x_pos, y_pos, vertical_print, horizontal_print, x, y, scaled_w, scaled_h);
}

void Render::image_fullscreen(const string& path, int orientation, int x, int y, int scaled_w, int scaled_h,
			      Scaling scaleable, double scale_factor_h, double scale_factor_w)
{
  Imlib_Image new_image = image_loader(path, scaleable, scale_factor_h, scale_factor_w, orientation);

  imlib_context_set_image(current.image_data);

  imlib_blend_image_onto_image(new_image, 0, 0, 0, scaled_w, scaled_h, x, y, scaled_w, scaled_h);
}

void Render::font_query_text_size(const string& text, const string& font_name, int *w, int *h){
  if (w)
    *w = 0;
  if (h)
    *h = 0;

  if(text.empty() || font_name.empty())
    return;
  Imlib_Font ffont = NULL;

  image_mut.enterMutex();
  font_map::iterator font_iter = fonts.find(font_name);
  if (font_iter != fonts.end())
    ffont = font_iter->second;
  else {
    ffont = imlib_load_font(font_name.c_str());
    if (!ffont){
      image_mut.leaveMutex();
      print_critical(gettext("The font ") + font_name + gettext(" could not be loaded"), "RENDER");
      return;
    }
    fonts.insert(std::make_pair(font_name, ffont));
  }

  assert(ffont);
  int text_w;
  int text_h;

  Imlib_Context ic = imlib_context_new();

  imlib_context_push(ic);
  imlib_context_set_font(ffont);
  imlib_get_text_size((text + " ").c_str(), &text_w, &text_h);
  imlib_free_font();
  imlib_context_pop();
  imlib_context_free(ic);
  image_mut.leaveMutex();

  if(w)
    *w = text_w;
  if(h)
    *h = text_h;
  return;

}

bool Render::set_font(const string& font_name)
{
  font_map::iterator font_iter = fonts.find(font_name);
  if (font_iter != fonts.end())
    imlib_context_set_font(font_iter->second);
  else {
    Imlib_Font font = imlib_load_font(font_name.c_str());
    if (!font) {
      print_critical(gettext("The font ") + font_name + gettext(" could not be loaded"), "RENDER");
      return false;
    } else
      fonts.insert(std::make_pair(font_name, font));
    imlib_context_set_font(font);
  }
  return true;
}

void Render::text(const string& message, const string& font_name, int x, int y, int r, int g, int b)
{
  if (set_font(font_name)) {

    string fix_imlib = message;
    fix_imlib.append(" ");

#ifdef UGLY_FONT_FLICKER_HACK
    // Don't use shadows for black text
    if (r != 0 || g != 0 || b != 0) {
      imlib_context_set_color(0, 0, 0, 255);
      imlib_text_draw(x+2, y+2, fix_imlib.c_str());
    }
#endif
    imlib_context_set_color(r, g, b, 255);

    imlib_text_draw(x, y, fix_imlib.c_str());
  }
}

void Render::text_part(const string& message, const string& font_name, int r, int g, int b,
		       int x, int y, int total_w, int total_h, const rect& area)
{
  if (set_font(font_name)) {
    Imlib_Image temp2= imlib_create_cropped_image(area.x, area.y, area.w, area.h);

    Imlib_Image temp = imlib_create_image(total_w, total_h);

    imlib_context_set_image(temp);

    imlib_blend_image_onto_image(temp2, 0, 0, 0, area.w, area.h, area.x-x, area.y-y, area.w, area.h);

    imlib_context_set_image(temp2);
    imlib_free_image();

    imlib_context_set_image(temp);

    string fix_imlib = message;
    fix_imlib.append(" ");

#ifdef UGLY_FONT_FLICKER_HACK
    // Don't use shadows for black text
    if (r != 0 || g != 0 || b != 0) {
      imlib_context_set_color(0, 0, 0, 255);
      imlib_text_draw(2, 2, fix_imlib.c_str());
    }
#endif
    imlib_context_set_color(r, g, b, 255);

    imlib_text_draw(0, 0, fix_imlib.c_str());

    imlib_context_set_image(current.image_data);

    imlib_blend_image_onto_image(temp, 0, area.x-x, area.y-y, area.w, area.h, area.x, area.y, area.w, area.h);

    imlib_context_set_image(temp);
    imlib_free_image();

    imlib_context_set_image(current.image_data);
  }
}

void Render::rectangle(int x, int y, int w, int h, int r, int g, int b, int a)
{
  imlib_context_set_color(r, g, b, a);
  imlib_image_fill_rectangle(x, y, w, h);
}

void Render::create_scaled_image_wrapper_upscaled(const string& path, int max_width, int max_height)
{
  string filename = image_get_fullpath(path, max_width, max_height);

  // check if local directory structure exist, else create it.
  if (!file_exists(filename)) {

    // check that the directories actually exists.
    string::size_type pos = path.rfind("/");
    create_if_not_exists(conf->p_temp_image_loc() + path.substr(0,pos+1));

    create_scaled_image_upscaled(path, filename, "jpg", max_width, max_height);
  }
}

void Render::create_scaled_image_wrapper(const string& path, int max_width, int max_height)
{
  string filename = image_get_fullpath(path, max_width, max_height);

  // check if local directory structure exist, else create it.
  if (!file_exists(filename)) {

    // check that the directories actually exists.
    string::size_type pos = path.rfind("/");
    create_if_not_exists(conf->p_temp_image_loc() + path.substr(0,pos+1));

    create_scaled_image(path, filename, "jpg", max_width, max_height);
  }
}

void Render::create_scaled_image(const string& in_file, const string& out_file,
				 string format, int max_x, int max_y)
{
  Imlib_Image img = imlib_load_image_without_cache(in_file.c_str());

  if (!img){ /* note, the imlib image context isn't really restored after
                     calling this method, it's just set to whatever 'img'
                     parameter is */
    print_critical(gettext("The image ") + in_file + gettext(" could not be loaded"), "RENDER");
    img = imlib_create_image(max_x, max_y);
    imlib_context_set_image(img);
    imlib_image_clear_color(0,0,0,255);
  } else
    imlib_context_set_image(img);

  char had_alpha = imlib_image_has_alpha();

  int w = imlib_image_get_width();
  int h = imlib_image_get_height();

  if (w > max_x || h > max_y) {

    int scaled_w, scaled_h;

    int ratio = (w*100)/h;

    if (ratio > ((max_x*100)/max_y)) {
      if (w > max_x) {
	scaled_w = max_x;
	scaled_h = (max_x * 100) / ratio;
      } else {
	scaled_w = w;
	scaled_h = h;
      }
    } else {
      if (h > max_y) {
	scaled_w = (max_y * ratio) / 100;
	scaled_h = max_y;
      } else {
	scaled_w = w;
	scaled_h = h;
      }
    }

    Imlib_Image new_image = imlib_create_cropped_scaled_image(0, 0, w, h, scaled_w, scaled_h);

    // delete old, we don't need it anymore
    imlib_free_image();

    imlib_context_set_image(new_image);
  }

  if (had_alpha) {
    format = "png";
    imlib_image_set_has_alpha(had_alpha);
  }

  imlib_image_set_format(format.c_str());

  /* save the image */
  imlib_save_image(out_file.c_str());

  imlib_free_image();

  imlib_context_set_image(current.image_data);
}

void Render::create_scaled_image_upscaled(const string& in_file, const string& out_file,
					  string format, int max_x, int max_y)
{
  Imlib_Image img = imlib_load_image_without_cache(in_file.c_str());

  if (!img){ /* note, the imlib image context isn't really restored after
		calling this method, it's just set to whatever 'img'
		parameter is */
    print_critical(gettext("The image ") + in_file + gettext(" could not be loaded"), "RENDER");
    img = imlib_create_image(max_x, max_y);
    imlib_context_set_image(img);
    imlib_image_clear_color(0,0,0,255);
  } else
    imlib_context_set_image(img);

  char had_alpha = imlib_image_has_alpha();

  int w = imlib_image_get_width();
  int h = imlib_image_get_height();

  int scaled_w, scaled_h;

  int ratio = (w*100)/h;

  if (ratio > ((max_x*100)/max_y)) {
    scaled_w = max_x;
    scaled_h = (max_x * 100) / ratio;
  } else {
    scaled_w = (max_y * ratio) / 100;
    scaled_h = max_y;
  }

  Imlib_Image new_image = imlib_create_cropped_scaled_image(0, 0, w, h, scaled_w, scaled_h);

  // delete old, we don't need it anymore
  imlib_free_image();

  imlib_context_set_image(new_image);

  if (had_alpha) {
    format = "png";
    imlib_image_set_has_alpha(had_alpha);
  }

  imlib_image_set_format(format.c_str());

  /* save the image */
  imlib_save_image(out_file.c_str());

  imlib_free_image();

  imlib_context_set_image(current.image_data);
}
