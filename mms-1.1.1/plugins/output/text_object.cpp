#include "text_object.hpp"

#include "renderer.hpp"
#include "common.hpp"

TObj::TObj(const std::string& t, const std::string& f,
	   int x_pos, int y_pos, int f1, int f2, int f3, int l)
  : GObj(x_pos, y_pos, l), font(f), text(t),
    font_color1(f1), font_color2(f2), font_color3(f3)
{
  std::pair<int, int> dimensions = string_format::calculate_string_size(t, f);

  real_w = w = dimensions.first;
  real_h = h = dimensions.second;
}

bool TObj::operator==(const TObj& t)
{
  return (op_eq(t) && font == t.font && font_color1 == t.font_color1 &&
	  font_color2 == t.font_color2 && font_color3 == t.font_color3 &&
	  text == t.text);
}

void TObj::draw() const
{
  S_Render::get_instance()->text(text, font, x, y, font_color1, font_color2, font_color3);
}

void TObj::draw_part(const rect& r) const
{
  // FIXME: buggy
  S_Render::get_instance()->text_part(text, font, font_color1, font_color2, font_color3, x, y, w, h, r);
}

bool TObj::transparent() const
{
  return true;
}

int TObj::font_size() const
{
  return conv::atoi(font.substr(font.rfind('/')));
}
