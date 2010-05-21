#ifndef GAME_HPP
#define GAME_HPP

#include "config.h"

#include "cpp_sqlitepp.hpp"

#include "game_config.hpp"

#include "module.hpp"
#include "options.hpp"
#include "config.hpp"
#include "multifile.hpp"
#include "gettext.hpp"

#ifdef use_inotify
#include "notify.hpp"
#endif

#include <stack>
#include <vector>
#include <list>

class GameEntry : public Multifile
{
public:
  GameEntry(const std::string& filename)
  {
    path = filename;
    filenames.push_back(filename);
  }

  GameEntry()
  {}

  int db_id;
};

class Game : public Module
{
private:
  SQLDatabase db;

  ost::Mutex db_mutex;

  void create_db();
  void create_indexes();
  void check_db();
  void check_db_consistency();
  std::vector<std::pair<std::string, int> > gen_search_list(const std::string& search_word);

  bool search_is_graphical_view(const std::vector<GameEntry>& cur_files);
  std::string id_to_filename(int db_id);
  bool change_dir_to_id(int db_id);

  bool rdir_internal(const std::string& filename, const std::string& argv,
		     std::vector<GameEntry> &cur_files, bool file_in_db);
  void insert_file_into_db(const std::string& filename, const std::string& parent);
  int db_id(const std::string& file, bool is_dir);
  std::vector<std::string> check_db_for_folders(const std::string& parent);

  void reenter(int db_id);
  int find_position_in_folder_list(const std::list<std::string>& files,
				   const std::string& filename);

  std::vector<GameEntry> rdir_hd(const std::string& argv, bool insert_into_db);

  void reload_current_dirs();

#ifdef use_inotify
  // notify
  void fs_change(NotifyUpdate::notify_update_type type, const std::string& path);
#endif
  bool reload_dir(const std::string& path);
  void reparse_current_dir();
  void update_db_current_dirs();

  void check_for_changes();

  // list of folders from config, the names are normalized to have / at the end
  std::list<std::string> game_folders;
  void set_folders();

  // commands
  void enter_dir();
  void go_back();
  void action_play();
  void options();
  void search_func();
  void exit();

  bool exit_loop;

  void one_up();
  void find_element_and_do_action(const GameEntry& game);
  void action();

  bool search_compare(const GameEntry& s);
  std::string get_name_from_file(const GameEntry& e);

  void print(const std::vector<GameEntry>& files);
  void print_game_element(const GameEntry& r, const GameEntry& position, int y);

  std::string get_cover(const std::string& s);
  void graphical_print(const std::vector<GameEntry>& files);

  // playback
  void playgame(const GameEntry file);
  void showcd(int count);
  void playpsx();

  // dir
  std::string testdir(int *count);

  GameEntry addfile(const std::string& name, const MyPair& filetype);
  GameEntry add_dir(const std::string& filename);

  std::vector<GameEntry> rdir(const std::string& argv);
  std::vector<GameEntry> parse_dir(const std::list<std::string>& dirs);

  // For traversing the filesystem. The first element is the dirs and the next is the
  // position in the list generated from the dirs
  std::stack<std::pair<std::list<std::string>, int > > folders;

  std::vector<GameEntry> files;

  int image_width;
  int image_height;
  int image_height_all_eks_text;

  // including text and space around image
  int image_width_all;
  int image_height_all;
  int image_height_all_search;

  int images_per_row;
  int rows;
  int rows_search;

  int y_start;

  GameConfig *game_conf;

  class GameOpts : public Options
  {
  public:
    GameOpts();
    ~GameOpts();

    std::vector<Option*> values()
    {
      return val;
    }

    std::string dir_order()
    {
      return dir_order_p->english_values[dir_order_p->pos];
    }

  private:
    Option *reload_p, *dir_order_p;
  };

  GameOpts opts;

  struct file_sort
  {
    bool operator()(const GameEntry& lhs, const GameEntry& rhs);
  };

  bool reload_dirs;

  std::string header_font;
  std::string search_font;
  std::string search_select_font;
  std::string normal_font;
  std::string position_font;
  std::string list_font;

  int normal_font_height;
  int list_font_height;

  std::pair<int, int> header_size;		// Size of Header
  int header_box_size;

  void res_dependant_calc();

public:

  int files_size() { return files.size(); }

  void reset();

  void load_current_dirs();
  void read_dirs();

  void determine_media();

  GameOpts* get_opts()
  {
    return &opts;
  }

  void startup_updater();

  std::string mainloop();

  Game();
  ~Game();
};

#endif
