#ifndef RECTANGLE_OBJECT_HPP
#define RECTANGLE_OBJECT_HPP

#include "graphic_object.hpp"

class RObj : public GObj
{
public:
  int r, g, b, a;

  RObj(int x_pos, int y_pos, int width, int height,
       int red, int green, int blue, int alpha, int layer);

  bool operator==(const RObj& r);

  void draw() const;
  bool transparent() const;

  RObj* clone() const { return new RObj(*this); }
};

#endif
