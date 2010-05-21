#ifndef SEARCH_HPP
#define SEARCH_HPP

#include "module.hpp"
#include "singleton.hpp"

#include "boost.hpp"

#include <string>
#include <vector>

class SearchModule
{
public:
  typedef boost::function<std::vector<std::pair<std::string, int> > (const std::string&)> list_gen_func;
  typedef boost::function<void (int)> enter_func;

  std::string name;
  list_gen_func list_gen;
  enter_func enter;

  SearchModule(const std::string& n, const list_gen_func& sgl, const enter_func& e_f);
};

class result
{
public:
  bool operator==(const result& r) const
  {
    return (module == r.module && name == r.name && id == r.id);
  }

  std::string module;
  std::string name;
  int id;
};

class Search : public Module
{
private:
  std::vector<SearchModule> modules;

  std::vector<result> results;

  std::string cur_module;

  std::string search_word;
  std::string search_module;
  int pos;

  ExtraMenu em;

  void search();

  void set_module(const std::string& mod);

  void print(const std::string& search_word);
  void print_element(const result& r, const result& position, int& y, bool first_element);

  bool exit_loop;

  void find_element_and_do_action(const result& result);
  void action();
  void exit();

  std::string header_font;
  std::string element_header_font;
  std::string element_font;
  std::string search_text_font;

  int element_height;
  int element_header_height;

  void res_dependant_calc();

  int select_menu_max_width;

public:

  static pthread_mutex_t singleton_mutex;

  Search();

  void register_module(const SearchModule& s);

  Options* get_opts()
  { return 0; }

  void startup_updater()
  {}

  std::string mainloop();
};

typedef Singleton<Search> S_Search;

#endif
