#ifndef GSIMPLEFILE_HPP
#define GSIMPLEFILE_HPP

#include "simplefile.hpp"

#include <string>

// class used as base class for graphical elements

class GSimplefile : public Simplefile
{
public:
  // returns the empty string if no cover is found
  std::string find_cover_in_current_dir() const;

  GSimplefile();
  GSimplefile(const Simplefile s);
};

#endif
