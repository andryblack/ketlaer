#ifndef MODULE_HPP
#define MODULE_HPP

#include "config.h"

#include "common.hpp"
#include "themes.hpp"
#include "renderer.hpp"
#include "config.hpp"
#ifdef use_notify_area
#include "notify_area.hpp"
#endif
#include "simplefile.hpp"
#include "options.hpp"
#include "global.hpp"
#include "image.hpp"

#include "cpp_sqlitepp.hpp"

#include "cd.hpp"

#include "rectangle_object.hpp"

#include "input.hpp"

#include <cmath>
#include <string>
#include <list>
#include <vector>

#define X_DISPLAY_MISSING

#include <Imlib2.h>

class Module
{
protected:

  // implemented in terms of:
  InputMaster *input_master;
  Themes *themes;
  Render *render;
  Config *conf;
  Global *global;

  Cd *cd;

  // search variables
  bool search_mode;
  std::string search_str;
  std::string lowercase_search_str;
  int offset;
  int search_help_offset;
  bool search_need_cleanup;
  int search_top_size;
  std::string search_depth;

  std::vector<int> letters;

  Overlay overlay_search;

  int check_metadata();

  int check_search_letter();
  void print_marked_search_letter();
  void clean_up_search_print();
  void force_search_print_cleanup();

  void print_range_top(int int_position, int list_size, int element_size,
		       int& y, int& start, int& range);

  template<typename T>
  void print_range(const std::vector<T>& files, const T& position, const int int_position,
		   const boost::function<void (const T&, const T&, int)>& print_func, const int element_size)
  {
    int y, start, range;
    print_range_top(int_position, files.size(), element_size, y, start, range);

    // goto start of print
    typename std::vector<T>::const_iterator iter = files.begin() + start;

    // print
    for (int j = 0; j < range; ++j)
      {
	print_func(*iter, position, y);

	y += element_size;

	++iter;
      }
  }

  template<typename T>
  void print_range_modifying(std::vector<T>& files, const T& position,
			     const int int_position,
			     const boost::function<void (T&, const T&, int)>& print_func,
			     const int element_size)
  {
    int y, start, range;
    print_range_top(int_position, files.size(), element_size, y, start, range);

    // goto start of print
    typename std::vector<T>::iterator iter = files.begin() + start;

    // print
    for (int j = 0; j < range; ++j)
      {
	print_func(*iter, position, y);

	y += element_size;

	++iter;
      }
  }


  void folder_difference(const std::vector<std::string>& new_list,
			 const std::vector<std::string>& old_list,
			 std::vector<std::string>& new_files,
			 std::vector<std::string>& removed_files);

  // dir
  int id;

  Simplefile addsimplefile(const std::string& name, const MyPair& filetype, bool from_media = false);

  // notify
  bool visible;

  // database stuff
  std::string get_parent_id(const std::string& parent, SQLDatabase& db,
			    const std::list<std::string>& top_folders);

  std::string get_parent_id_movie(const std::string& parent, SQLDatabase& db,
				  const std::list<std::string>& top_folders);

  std::string get_parent_id_common(const std::string& parent, SQLDatabase& db,
				   const std::list<std::string>& top_folders, const std::string& db_insert_string);

public:

  // FIXME: maybe convert to this, nicer code
  virtual Options *get_opts() = 0;
  virtual bool init();

  bool loaded_correctly;
  bool reload_needed;

  // must be implemented by modules
  virtual void startup_updater() = 0;
  // must be implemented by modules if external programs might use their
  // resources such as the soundcard
  virtual void begin_external() {}
  virtual void end_external() {}

  virtual ~Module() {}

  Module();

#ifdef use_notify_area
  NotifyArea *nArea;

#endif
  virtual std::string mainloop() = 0;
};

#endif
