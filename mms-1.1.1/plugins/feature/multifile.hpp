#ifndef ENTRY_HPP
#define ENTRY_HPP

#include <string>
#include <list>

#include "gsimplefile.hpp"

class Multifile : public GSimplefile
{
public:
  std::string filetype;
  std::list<std::string> filenames;
};

#endif
