#ifndef TEXT_OBJECT_HPP
#define TEXT_OBJECT_HPP

#include "graphic_object.hpp"

#include <string>

#include "renderer.hpp"

class TObj : public GObj
{
public:

  std::string font;
  std::string text;
  int font_color1, font_color2, font_color3;

  TObj(const std::string& t, const std::string& f,
       int x_pos, int y_pos, int f1, int f2, int f3, int layer);

  bool operator==(const TObj& t);

  void draw() const;
  void draw_part(const rect& r) const;
  bool transparent() const;

  int font_size() const;

  TObj* clone() const { return new TObj(*this); }
};

#endif
