#ifndef GRAPHICAL_MOVIE_HPP
#define GRAPHICAL_MOVIE_HPP

#include "movie_db.hpp"
#include "singleton.hpp"

#include <list>
#include <string>

class GraphicalMovie : public MovieDB
{
private:
  // commands
  void secondary_menu();
  void enter_dir();
  void print_movie_info();

  void action();

  bool change_dir_to_id(int db_id);

  std::vector<CIMDBMovie> rdir_hd(const std::string& argv, bool& db_load_succes, bool insert_into_db);

  bool rdir_internal(const std::string& filename, const std::string& argv,
		     std::vector<CIMDBMovie> &cur_files, bool& db_load_succes, bool file_in_db);
  void insert_file_into_db(const std::string& filename, const std::string& parent);
  std::pair<int, bool> db_info(const std::string& file, bool is_dir);
  std::vector<std::string> check_db_for_folders(const std::string& parent);

  std::string path_of_imdb_movie(const CIMDBMovie& c);

  void check_for_changes();

  bool reload_dir(const std::string& path);
  void reload_current_dirs();

  void reenter(int id);
  std::vector<std::pair<std::string, int> > gen_search_list(const std::string& search_word);

  int find_position_in_folder_list(const std::list<std::string>& files, const std::string& filename);

  std::vector<CIMDBMovie> parse_dir(const std::list<std::string>& dirs);
  std::vector<CIMDBMovie> rdir(const std::string& argv);

  bool imdb_message_not_displayed;

  virtual void save_runtime_settings();
  virtual void load_runtime_settings();

  virtual void update_thumbnailed_status(int id);

  bool reload_dirs;
  /* for dir_contains_known_files() */
  std::string ext_mask;


public:

  GraphicalMovie();

  void startup_updater();

  void check_db_consistency();

  std::string mainloop();
};

#endif
