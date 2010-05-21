#ifndef IMAGE_HPP
#define IMAGE_HPP

#include "graphic_object.hpp"

#define X_DISPLAY_MISSING

#include <Imlib2.h>

#include <vector>
#include <string>

class Image
{
public:

  Image();
  ~Image();

  typedef std::vector<GObj*> gelements;
  typedef std::vector<gelements> layers;

  // elements consists of 10 layers of objects
  // and then 10 layers of overlay on top of that
  layers elements;

  Imlib_Image image_data;

  void clear();
  void clear_no_delete();

  void clone(const layers& l);

  void add(GObj *g);

  // clients is supposed to clean up after themselves. clear() and friends
  // doesn't work with overlay, please use the helper class Overlay
  void overlay_add(GObj *g);
};

class Overlay
{
public:

  Overlay(const std::string& n)
    : name(n)
  {}

  std::string name;

  std::vector<GObj*> elements;

  void add(GObj *g);

  // deletes the current overlay
  void cleanup();

  void partial_cleanup();

  std::vector<GObj*> temp_elements;

private:
  void partial_cleanup_final();
};

#endif
