
#include "renderer.hpp"
#include "rectangle_object.hpp"

RObj::RObj(int x_pos, int y_pos, int width, int height, 
     int red, int green, int blue, int alpha, int l)
  : GObj(x_pos, y_pos, l, width, height), r(red), g(green), b(blue), a(alpha)
{}

bool RObj::operator==(const RObj& rhs)
{
  return (op_eq(rhs) && r == rhs.r && g == rhs.g && b == rhs.b && a == rhs.a);
}

void RObj::draw() const
{
  S_Render::get_instance()->rectangle(x, y, w, h, r, g, b, a);
}

bool RObj::transparent() const
{
  return (a != 255);
}
