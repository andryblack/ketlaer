#ifndef SIMPLEFILE_HPP
#define SIMPLEFILE_HPP

#include <string>

class Simplefile
{
public:
  int id;
  std::string name;
  std::string lowercase_name;
  std::string path;
  std::string type;
  std::string media_id;
  std::string to_string() const;

  Simplefile();
  bool operator!=(const Simplefile& rhs) const;
  bool operator==(const Simplefile& rhs) const;
  bool operator<(const Simplefile& rhs) const;
};

#endif
