#ifndef COLLECTION_HPP
#define COLLECTION_HPP

#include "movie_db.hpp"
#include "singleton.hpp"

#include <string>

class MovieCollection : public MovieDB
{
private:
  void secondary_menu();

  void action();

  bool reload_dir(const std::string& path) { return false; }
  void reload_current_dirs() {}

  bool change_dir_to_id(int db_id);

  void remove_movie(int index);

  void reenter(int id);
  std::vector<std::pair<std::string, int> > gen_search_list(const std::string& search_word);

  std::vector<CIMDBMovie> parse_dir(const std::list<std::string>& dirs);

  void options();

public:
  MovieCollection();

  bool loaded_correctly;

  void startup_updater();

  std::string mainloop();

  void read_dirs();
};

#endif
