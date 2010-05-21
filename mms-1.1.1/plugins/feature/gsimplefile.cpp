#include "gsimplefile.hpp"

#include "libfspp.hpp"
#include "config.hpp"
#include "common.hpp"

#include "boost.hpp"

#include <vector>

using std::string;
using std::vector;

GSimplefile::GSimplefile(const Simplefile s)
  : Simplefile(s)
{}

GSimplefile::GSimplefile()
{}

string GSimplefile::find_cover_in_current_dir() const
{
  if (path.empty())
    return "";

  Config *conf = S_Config::get_instance();

  string pic = "";
  string lowercase_pic = "";

  vector<string> pictures_found;

  MyPair filetype;

  string::size_type pos;

  if ((pos = path.rfind("/")) != string::npos) {
    string folder = path.substr(0, pos);

    // determine if there is a picture in this dir...
    for (file_iterator<file_t, default_order> i (folder); i != i.end (); i.advance(false))
      if (check_type(i->getName(), conf->p_filetypes_p()) != emptyMyPair)
	pictures_found.push_back(i->getName());

    if (!pictures_found.empty()) {
      pic = pictures_found.front(); // default value
      lowercase_pic = string_format::lowercase(pic);
      foreach (string& picture, pictures_found) {
	string filename = string_format::lowercase(picture);
	if (filename.rfind("front") != string::npos) {
	  pic = picture;
	  break;
	} else if (filename < lowercase_pic) {
	  pic = picture; // default value
	  lowercase_pic = string_format::lowercase(pic);
	}
      }
    }
  }

  return pic;
}

