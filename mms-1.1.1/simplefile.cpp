#include "simplefile.hpp"

std::string Simplefile::to_string() const
{
  return name;
}

Simplefile::Simplefile()
  : id(0), name(""), lowercase_name(""), path(""), type(""), media_id("invalid")
{}

bool Simplefile::operator!=(const Simplefile& rhs) const
{
  return (this->id != rhs.id);
}

bool Simplefile::operator==(const Simplefile& rhs) const
{
  return (this->id == rhs.id);
}

bool Simplefile::operator<(const Simplefile& rhs) const
{
  return (this->name < rhs.name);
}
