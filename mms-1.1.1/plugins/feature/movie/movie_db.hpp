#ifndef MOVIE_DB_HPP
#define MOVIE_DB_HPP

#include "config.h"

#include "movie.hpp"
#include "imdb.hpp"

#include "cpp_sqlitepp.hpp"
#include "singleton.hpp"

#ifdef use_ffmpeg_thumbnailer
#include "thumbnailer.h"
#endif

class MovieDB : public MovieTemplate<CIMDBMovie>
{
protected:
  SQLDatabase db;

  ost::Mutex db_mutex;

  // commands
  virtual void secondary_menu() = 0;
  void reget_movie_information();
  void print_info();
  void search_func();
  void search_imdb();
  void prev();
  void next();
  void left();
  void right();

  void page_up();
  void page_down();

  void one_up();
  void go_back();
  virtual void action() = 0;
  void find_element_and_do_action(const CIMDBMovie& movie_file);

  void remove_from_db(int index);

#ifdef use_ffmpeg_thumbnailer
  int thumb_verbosity;
  int thumb_deepval;
  int thumb_skipblanks;
#else
  MoviePlayerPlugin *thumbnailer;
#endif

  std::string clean_up_name_for_imdb(std::string name);

  bool search_is_graphical_view(const std::vector<CIMDBMovie>& cur_files);
  std::string id_to_filename(int db_id);
  virtual bool change_dir_to_id(int db_id) = 0;

  virtual void update_thumbnailed_status(int id) {};

  // helper
  void imdb_download_movie(CIMDBMovie &m, const CIMDBUrl& url);

  bool imdb_search_mainloop(const std::string& search_word, CIMDBMovie &m, bool search);
  void imdb_search_screen(std::string search_word, const IMDB_MOVIELIST& results, int position, Overlay& o, CIMDBMovie &m);

  void imdb_action(CIMDBMovie &m, const IMDB_MOVIELIST& results, int pos);

  bool exit_imdb_search_loop;
  bool imdb_found_match;

  bool create_thumbnail(const std::string& file, const std::string& thumbnail, int width, int height);

  void print_information(const CIMDBMovie& m);
  void right_align_extra_info_text(std::string type, const std::string& value, 
				   int x, int y, int text_width);
  void print_extra_information(CIMDBMovie& m);
  void print_movie_element(CIMDBMovie& m, const CIMDBMovie& position, int y);
  void print_modifying(std::vector<CIMDBMovie>& files);
  void print(const std::vector<CIMDBMovie>& files)
  {
    std::cerr << "WARNING: use sparingly (movie)" << std::endl;
    std::vector<CIMDBMovie> tmp_files = files;
    print_modifying(tmp_files);
  }

  void print_list_view(std::vector<CIMDBMovie>& cur_files);
  void print_icon_view(std::vector<CIMDBMovie>& cur_files);

  void find_movie_in_imdb(const std::string& search_string,
			  IMDB_MOVIELIST& results, int& pos);

  std::string get_name_from_file(const CIMDBMovie& m);

  virtual std::vector<std::pair<std::string, int> > gen_search_list(const std::string& search_word) = 0;

  void check_db();
  void create_db();
  void create_indexes();

  bool searched_imdb;

  int image_width;
  int image_height;
  int image_height_all_eks_text;
  int image_height_all_search;

  // including text and space around image
  int image_width_all;
  int image_height_all;

  int images_per_row;
  int rows;
  int rows_search;

  int y_start;

  std::string imdb_dir;

  CIMDB imdb;

  // true means that the movies reside on hd
  bool physical;

  void res_dependant_calc_2();

public:

  bool is_physical()
  {
    return physical;
  }

  SQLDatabase* get_db() { return &db; }

  // physical means that it exists on the hd
  MovieDB(const std::string& filename, bool physical);
  ~MovieDB();
};

#endif
