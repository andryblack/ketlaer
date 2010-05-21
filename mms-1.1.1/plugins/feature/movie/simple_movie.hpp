#ifndef SIMPLE_MOVIE_HPP
#define SIMPLE_MOVIE_HPP

#include "movie.hpp"
#include "singleton.hpp"

class SimpleMovie : public MovieTemplate<Multifile>
{
private:
  // commands
  void secondary_menu();
  void enter_dir();
  void go_back();
  void search_func();

  void print(const std::vector<Multifile>& files);
  void print_movie_element(const Multifile& r, const Multifile& position, int y);

  bool reload_dir(const std::string& path);
  void reload_current_dirs();

  std::vector<Multifile> parse_dir(const std::list<std::string>& dirs);
  std::vector<Multifile> rdir(const std::string& argv);

  void page_up();
  void page_down();

public:
  std::string mainloop();
};

#endif
