#ifndef MOVIE_OBJECT_HPP
#define MOVIE_OBJECT_HPP

#include "graphic_object.hpp"

#include "renderer.hpp"

class MObj : public GObj
{
public:

  MObj(int shm_id, int x_pos, int y_pos, int l);

  bool operator==(const MObj& m);

  void draw() const;
  void draw_part(const rect& r) const;
  bool transparent() const;

  MObj* clone() const { return new MObj(*this); }

private:
  int shm_id;
};

#endif
