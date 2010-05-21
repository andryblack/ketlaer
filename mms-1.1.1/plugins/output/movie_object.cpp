#include "movie_object.hpp"

#include "config.hpp"

#include <sys/shm.h>

MObj::MObj(int sid, int x_pos, int y_pos, int l)
  : GObj(x_pos, y_pos, l), shm_id(sid)
{
  Config *conf = S_Config::get_instance();

  real_w = w = conf->p_h_res()/4;
  real_h = h = conf->p_v_res()/4;
}

bool MObj::operator==(const MObj& m)
{
  return false; // always redraw
}

void MObj::draw() const
{
  Config *conf = S_Config::get_instance();
  DATA32 *data = imlib_image_get_data();
  void *video = shmat(shm_id, 0, 0);
  for (int i = 0; i < h; ++i)
    memcpy(data+(y*conf->p_h_res()+i*conf->p_v_res()), (char *)video+i*4*w, w*4);
  shmdt(video);
  imlib_image_put_back_data(data);
}

void MObj::draw_part(const rect& r) const
{
  draw(); // FIXME
}

bool MObj::transparent() const
{
  return true;
}
