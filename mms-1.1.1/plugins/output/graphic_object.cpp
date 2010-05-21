#include "graphic_object.hpp"

GObj::GObj(int new_x, int new_y, int l)
  : dirty(false), x(new_x), y(new_y), layer(l),
    real_x(new_x), real_y(new_y)
{}

GObj::GObj(int new_x, int new_y, int l, int width, int height)
  : dirty(false), x(new_x), y(new_y), layer(l), w(width), h(height),
    real_x(new_x), real_y(new_y), real_w(width), real_h(height)
{}

