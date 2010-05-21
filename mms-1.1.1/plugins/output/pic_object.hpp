#ifndef PIC_OBJECT_HPP
#define PIC_OBJECT_HPP

#include "graphic_object.hpp"

#include "renderer.hpp"

#include <string>

class PObj : public GObj
{
public:
  std::string path;

  Scaling scaleable;
  double scale_factor_h;
  double scale_factor_w;

  int orientation;

  // if theme is set, the path is relative to the theme folder
  PObj(const std::string& p, int x_pos, int y_pos, int layer, Scaling scaleable, bool theme = true);

  // scale an image to a fixed width. Independant of the resolution. Also for theme pictures
  PObj(const std::string& p, int x_pos, int y_pos, int width, int layer);

  // custom width and height
  PObj(const std::string& p, int x_pos, int y_pos, int width, int height, bool theme, int l);

  // crop an image to a fixed width. Independant of the resolution.
  PObj(const std::string& p, int x_pos, int y_pos, double width_percentage, int layer);

  bool operator==(const PObj& p);
  void draw() const;
  bool transparent() const;

  PObj* clone() const { return new PObj(*this); }

protected:
  Render *render;

  bool is_transparent;

  // helper constructor for PFObj
  PObj(int new_x, int new_y, const std::string& path, int layer,
       Scaling scaleable, int orientation);
};

// full path picture object
class PFObj : public PObj
{
public:
  bool black_bordered;

  bool border;

  // scales the image on the fly
  PFObj(const std::string& path, int x_pos, int y_pos,
	int max_x, int max_y, int scaled_x, int scaled_y, int layer);

  // scales the thumbnail on the fly
  PFObj(const std::string& path, int x_pos, int y_pos, int scaled_x, int scaled_y, int layer, bool theme);

  // uses a prescaled image on disc
  PFObj(const std::string& path, int x_pos, int y_pos,
	int max_x, int max_y, bool border, int layer);

  PFObj(const std::string& path, int x_pos, int y_pos, bool border, int layer);

  bool operator==(const PFObj& p);

  void set_border_position(int bx, int by, int bw, int bh);

  void draw() const;

  PFObj* clone() const { return new PFObj(*this); }
};

// fullscreen picture object
class PFSObj : public PObj
{
public:
  bool zoom;
  int zoom_level, zoom_position;
  int zoom_v_levels, zoom_h_levels;

  PFSObj(const std::string& path, bool zoom, int orientation,
	 int zoom_level, int zoom_position, int zvl, int zhl, int layer);

  bool operator==(const PFSObj& p);

  void draw() const;

  PFSObj* clone() const { return new PFSObj(*this); }
};

#endif
