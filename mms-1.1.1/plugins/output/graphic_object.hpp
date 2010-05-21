#ifndef GRAPHIC_OBJECT_HPP
#define GRAPHIC_OBJECT_HPP

#include <utility>

class GObj
{
public:

  bool dirty;

  int x, y, layer, w, h;

  // some objects might have borders and other stuff that we need to take into
  // account
  int real_x, real_y, real_w, real_h;

  GObj(int new_x, int new_y, int l);
  GObj(int new_x, int new_y, int l, int width, int height);

  virtual ~GObj(){};

  virtual GObj* clone() const = 0;

  bool op_eq(const GObj& g)
  {
    return (x == g.x && y == g.y &&
	    w == g.w && h == g.h && real_x == g.real_x &&
	    real_y == g.real_y && real_w == g.real_w &&
	    real_h == g.real_h && layer == g.layer);
  }

  static bool sorting_order(GObj* lhs, GObj* rhs)
  {
    return lhs->layer < rhs->layer;
  }

  virtual void draw() const = 0;
  virtual bool transparent() const = 0;

  // move to a new position
  void move(int new_x, int new_y)
  { x = new_x; y = new_y; }
};

class TempGObj : public GObj
{
public:
  TempGObj(int new_x, int new_y, int l, int width, int height)
    : GObj(new_x, new_y, l, width, height)
  {};

  void draw() const {}
  bool transparent() const { return false; }
  TempGObj* clone() const { return new TempGObj(*this); }
};

struct SDescLayer
{
  template<typename T>
  bool operator()(const std::pair<GObj*, T>& lhs, const std::pair<GObj*, T>& rhs)
  {
    return lhs.first->layer < rhs.first->layer;
  }

  bool operator()(const GObj* lhs, const GObj* rhs)
  {
    return lhs->layer < rhs->layer;
  }
};

#endif
