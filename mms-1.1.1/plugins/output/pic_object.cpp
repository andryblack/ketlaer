#include "pic_object.hpp"

#include "renderer.hpp"

PObj::PObj(const std::string& p, int x_pos, int y_pos, int l, Scaling s, bool theme)
  : GObj(x_pos, y_pos, l), scaleable(s), orientation(0), is_transparent(false)
{
  render = S_Render::get_instance();

  if (theme)
    path = render->default_path + p;
  else
    path = p;

  std::pair<int, int> dimensions = render->image_dimensions(path, &is_transparent);
  h = dimensions.first;
  w = dimensions.second;
  real_h = h;
  real_w = w;

  Config *conf = S_Config::get_instance();

  if (scaleable == SCALE_FULL) {
    scale_factor_h = double(conf->p_v_res())/h;
    scale_factor_w = double(conf->p_h_res())/w;
    real_h = h = int(h*scale_factor_h);
    real_w = w = int(w*scale_factor_w);
  } else if (scaleable == SCALE_VERTICAL) {
    scale_factor_w = double(conf->p_h_res())/w;
    scale_factor_h = 1.0;
    real_w = w = int(w*scale_factor_w);
  } else if (scaleable == SCALE_HORIZONTAL) {
    scale_factor_h = double(conf->p_v_res())/h;
    scale_factor_w = 1.0;
    real_h = h = int(h*scale_factor_h);
  } else
    scale_factor_w = scale_factor_h = 1.0;
}

PObj::PObj(const std::string& p, int x_pos, int y_pos, int width,
	   int height, bool theme, int l)
  : GObj(x_pos, y_pos, l), scaleable(SCALE_FULL), orientation(0),
    is_transparent(false)
{
  render = S_Render::get_instance();

  if (theme)
    path = render->default_path + p;
  else
    path = p;

  std::pair<int, int> dimensions = render->image_dimensions(path, &is_transparent);

  real_h = h = height;
  real_w = w = width;

  scale_factor_w = double(width)/dimensions.second;
  scale_factor_h = double(height)/dimensions.first;
}

PObj::PObj(const std::string& p, int x_pos, int y_pos, int width, int l)
  : GObj(x_pos, y_pos, l), scaleable(SCALE_VERTICAL), orientation(0), is_transparent(false)
{
  render = S_Render::get_instance();
  path = render->default_path + p;
  std::pair<int, int> dimensions = render->image_dimensions(path, &is_transparent);
  real_h = h = dimensions.first;
  real_w = w = width;

  scale_factor_w = double(width)/dimensions.first;
  scale_factor_h = 1;
}

PObj::PObj(const std::string& p, int x_pos, int y_pos, double height_percentage, int l)
  : GObj(x_pos, y_pos, l), scaleable(SCALE_HORIZONTAL), orientation(0), is_transparent(false)
{
  render = S_Render::get_instance();
  path = render->default_path + p;
  std::pair<int, int> dimensions = render->image_dimensions(path, &is_transparent);
  real_h = h = round_to_int(dimensions.first*height_percentage);
  real_w = w = dimensions.second;

  scale_factor_w = 1;
  scale_factor_h = height_percentage;
}

PObj::PObj(int x_pos, int y_pos, const std::string& p, int l, Scaling s, int o)
  : GObj(x_pos, y_pos, l), path(p), scaleable(s), orientation(o), is_transparent(false)
{
  render = S_Render::get_instance();
}

PFObj::PFObj(const std::string& p, int x_pos, int y_pos, int scaled_x, int scaled_y, int layer, bool theme)
  : PObj(x_pos, y_pos, p, layer, NOSCALING, 0), border(false), black_bordered(false)
{
  if (theme)
    path = render->default_path + p;
  else
    path = p;

  std::pair<int, int> dimensions = render->image_dimensions(path, &is_transparent);

  scaleable = SCALE_FULL;

  scale_factor_w = scaled_x/(1.0*dimensions.second);
  scale_factor_h = scaled_y/(1.0*dimensions.first);

  h = round_to_int(dimensions.first * scale_factor_h);
  w = round_to_int(dimensions.second * scale_factor_w);
  real_h = h;
  real_w = w;

  if (!is_transparent) {
    // compensate for borders
    real_x -= 1;
    real_y -= 1;
    real_w += 2;
    real_h += 2;
  }
}

PFObj::PFObj(const std::string& p, int x_pos, int y_pos,
	     int max_width, int max_height, int scaled_x, int scaled_y, int layer)
  : PObj(x_pos, y_pos, p, layer, NOSCALING, 0), border(false), black_bordered(false)
{
  path = render->image_get_fullpath(p, max_width, max_height);
  std::pair<int, int> dimensions = render->image_dimensions(path, &is_transparent);

  scaleable = SCALE_FULL;

  scale_factor_w = scaled_x/(1.0*max_width);
  scale_factor_h = scaled_y/(1.0*max_height);

  if (scale_factor_w > scale_factor_h)
    scale_factor_w = scale_factor_h;
  else
    scale_factor_h = scale_factor_w;

  h = round_to_int(dimensions.first * scale_factor_h);
  w = round_to_int(dimensions.second * scale_factor_w);
  real_h = h;
  real_w = w;

  // compensate for borders
  real_x -= 1;
  real_y -= 1;
  real_w += 2;
  real_h += 2;
}

PFObj::PFObj(const std::string& p, int x_pos, int y_pos, int max_width, int max_height, bool b, int l)
  : PObj(x_pos, y_pos, p, l, NOSCALING, 0), border(b), black_bordered(false)
{
  path = render->image_get_fullpath(p, max_width, max_height);
  std::pair<int, int> dimensions = render->image_dimensions(path, &is_transparent);

  scale_factor_w = scale_factor_h = 1;

  h = dimensions.first;
  w = dimensions.second;
  real_h = h;
  real_w = w;

  // compensate for borders
  if (border) {
    real_x -= 5;
    real_y -= 5;
    real_w += 10;
    real_h += 10;
  } else {
    real_x -= 1;
    real_y -= 1;
    real_w += 2;
    real_h += 2;
  }
}

PFObj::PFObj(const std::string& p, int x_pos, int y_pos, bool b, int l)
  : PObj(x_pos, y_pos, p, l, NOSCALING, 0), border(b), black_bordered(false)
{
  path = p;
  std::pair<int, int> dimensions = render->image_dimensions(path, &is_transparent);

  scale_factor_w = scale_factor_h = 1;

  h = dimensions.first;
  w = dimensions.second;
  real_h = h;
  real_w = w;

  // compensate for borders
  if (border) {
    real_x -= 5;
    real_y -= 5;
    real_w += 10;
    real_h += 10;
  } else {
    real_x -= 1;
    real_y -= 1;
    real_w += 2;
    real_h += 2;
  }
}

PFSObj::PFSObj(const std::string& p, bool z, int o, int zl, int zp, int zvl, int zhl, int l)
  : PObj(0, 0, p, l, z?SCALE_FULL:NOSCALING, o), zoom(z), zoom_level(zl),
    zoom_position(zp), zoom_v_levels(zvl), zoom_h_levels(zhl)
{
  scaleable = render->image_fullscreen_dimensions(path, zoom, orientation, zoom_level, zoom_position,
						  real_x, real_y, real_w, real_h,
						  w, h, &is_transparent)?SCALE_FULL:NOSCALING;
  scale_factor_w = double(real_w)/w;
  scale_factor_h = double(real_h)/h;

  x = real_x;
  y = real_y;
}

bool PObj::operator==(const PObj& p)
{
  return (op_eq(p) && path == p.path && scaleable == p.scaleable &&
	  scale_factor_w == p.scale_factor_w && scale_factor_h == p.scale_factor_h &&
	  orientation == p.orientation);
}

void PObj::draw() const
{
  render->image(path, x, y, w, h, scaleable, scale_factor_h, scale_factor_w, orientation);
}

bool PObj::transparent() const
{
  return is_transparent;
}

bool PFObj::operator==(const PFObj& p)
{
  return (op_eq(p) && path == p.path && border == p.border && scaleable == p.scaleable &&
	  scale_factor_w == p.scale_factor_w && scale_factor_h == p.scale_factor_h &&
	  orientation == p.orientation);
}

void PFObj::draw() const
{
  render->image_fullpath(path, x, y, border, scaleable, scale_factor_h, scale_factor_w, orientation,
			 real_x, real_y, real_w, real_h, black_bordered);
}

void PFObj::set_border_position(int bx, int by, int bw, int bh)
{
  black_bordered = true;
  real_x = bx - 5;
  real_y = by - 5;
  real_w = bw + 10;
  real_h = bh + 10;
}

bool PFSObj::operator==(const PFSObj& p)
{
  return (op_eq(p) && path == p.path && zoom == p.zoom && zoom_level == p.zoom_level &&
	  zoom_position == p.zoom_position && orientation == p.orientation &&
	  scale_factor_w == p.scale_factor_w && scale_factor_h == p.scale_factor_h &&
	  scaleable == p.scaleable);
}

void PFSObj::draw() const
{
  if (zoom_level != 1)
    render->image_fullscreen_zoom(path, orientation, zoom_level, zoom_position,
				  zoom_v_levels, zoom_h_levels, real_x, real_y, real_w, real_h,
				  scaleable, scale_factor_h, scale_factor_w);
  else
    render->image_fullscreen(path, orientation, real_x, real_y, real_w, real_h,
			     scaleable, scale_factor_h, scale_factor_w);
}
