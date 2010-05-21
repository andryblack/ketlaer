#ifndef PICTURES_HPP
#define PICTURES_HPP

#include <stack>

#include "cpp_sqlitepp.hpp"

#include "module.hpp"
#include "simplefile.hpp"
#include "options.hpp"
#include "singleton.hpp"
#include "picture_config.hpp"

#ifdef use_inotify
#include "notify.hpp"
#endif

class Picture : public Simplefile
{
public:
  Picture(Simplefile s)
    : Simplefile(s)
  {}

  Picture()
  {}

  Picture(const std::string& filename)
  {
    path = filename;
  }

  int db_id;
};

class Pictures : public Module
{
private:


  /* for dir_contains_known_files() */
  std::string ext_mask;

  SQLDatabase db;

  ost::Mutex db_mutex;

  ost::Mutex fullscreen_mutex;

  bool navigating_media;
  std::list<std::string> top_media_folders;

  void create_indexes();
  void create_db();
  void check_db();

  // helper functions, use these instead of the raw get/set_db_orientation
  void set_db_orientation_lookup();
  int get_db_orientation_lookup();

  int get_db_orientation(int id);
  void set_db_orientation(int id);

  Picture get_file_from_path(const std::string& path, MyPair filetype, bool file_in_db = true);

  bool search_is_graphical_view(const std::vector<Picture>& cur_files);
  std::string id_to_filename(int db_id);
  bool change_dir_to_id(int db_id);

  void reenter(int id);
  std::vector<std::pair<std::string, int> > gen_search_list(const std::string& search_word);

  void action_no_find_recursion();

  // commands
  void action();
  void go_back();
  void recurse_dirs();
  void options();
  void search_func();
  void exit();
  void exit_fullscreen();

  // fullscreen commands
  void zoom();
  void suspend();
  void rotate_left();
  void rotate_right();
  void options_fullscreen();

  void deactivate_updaters();
  void activate_updaters();

  bool enter_dir();

  bool exit_loop;

  void find_element_and_do_action(const Picture& picture);
  void fullscreen_left();
  void fullscreen_right();

  bool rdir_internal(const std::string& filename, const std::string& argv,
		     std::vector<Picture> &cur_files, bool file_in_db);
  void insert_file_into_db(const std::string& filename, const std::string& parent);
  int db_id(const std::string& file, bool is_dir);
  std::vector<std::string> check_db_for_folders(const std::string& parent);

  void insert_picture_into_db(const std::string& filename, const std::string& name);
  int find_position_in_folder_list(const std::list<std::string>& files, const std::string& filename);

  bool in_fullscreen;

  bool search_compare(const Simplefile& s);
  std::string get_name_from_file(const Simplefile& e);

  void print(const std::vector<Picture>& pic_list);
  void prepare_and_show_fullscreen();
  void pictures_fullscreen();

  // helper functions, to minimize code copying, functions used in mainloops.
  void prev();
  void next();

  void prev_random();
  void next_random();
  void set_random_files();

  std::vector<std::string> images_in_dir(const std::list<std::string>& dirs);

  std::vector<Picture> parse_dirs_recursion();
  void find_recursion_file(bool random = false);
  bool find_recursion_pos();

  // helper
  bool already_tested(const std::vector<std::string>& tested_dirs, std::string dir);

  int pos_recursion;
  std::vector<Picture> recurse_files;

  int pos_random;
  std::vector<Picture> random_files;

  void prev_no_skip_folders();
  void next_no_skip_folders();

  void prev_skip_folders();
  void next_skip_folders();

  bool testdir(const std::string& dir);

  Picture addfile(const std::string& name, const MyPair& filetype);
  std::vector<Picture> rdir(const std::string& argv);
  std::vector<Picture> parse_dir(const std::list<std::string>& dirs);

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

  int orientation;

  int zoom_level; // 1 or 2 currently
  int zoom_position; // 1 to X, where 1..max_pos_v is top, etc.
  int zoom_pos_h, zoom_pos_v;
  int max_pos_h, max_pos_v;

  void res_dependant_calc();

  class PicOpts : public Options
  {
  public:
    PicOpts();
    ~PicOpts();

    std::string dir_order()
    {
      return dir_order_p->english_values[dir_order_p->pos];
    }

    std::string slideshow()
    {
      return slideshow_p->values[slideshow_p->pos];
    }

    std::string zoom()
    {
      return zoom_p->values[zoom_p->pos];
    }

    std::string show_preview()
    {
      return preview_p->values[preview_p->pos];
    }

    std::string show_info()
    {
	return info_p->values[info_p->pos];
    }

    std::string recurse_in_fullscreen()
    {
      return recurse_p->values[recurse_p->pos];
    }

    std::string thumbnail_extract()
    {
      return thumbnail_extract_p->values[thumbnail_extract_p->pos];
    }

    std::string slideshow_random()
    {
      return random_p->values[random_p->pos];
    }

    std::vector<Option*> values()
    {
      return val;
    }

  private:
    Option *reload_p, *dir_order_p, *slideshow_p, *zoom_p, *preview_p, *info_p, *recurse_p, *thumbnail_extract_p, *random_p;
  };

  PicOpts opts;

  struct file_sort
  {
    bool operator()(const Simplefile& lhs, const Simplefile& rhs);
  };

  // background thumbnail extraction
  void background_thumbnails();
  int check_thumbnail();

  bool thumbnailed_all_files;
  std::vector<std::string> all_image_files;

  // status of fullscreen
  bool pause;
  long int last_press;

  PictureConfig *pic_conf;

  std::vector<Picture> rdir_hd(const std::string& argv, bool insert_into_db);
  void reload_current_dirs();
  void update_db_current_dirs();

  // list of folders from config, the names are normalized to have / at the end
  std::list<std::string> picture_folders;
  void set_folders();
  void check_db_consistency();

#ifdef use_inotify
  // notify
  void fs_change(NotifyUpdate::notify_update_type type, const std::string& path);
#endif
  bool reload_dir(const std::string& path);
  void reparse_current_dir();

  void check_for_changes();

  bool reload_dirs;

  std::string header_font;
  std::string search_font;
  std::string search_select_font;
  std::string normal_font;
  std::string position_font;
  std::string fullscreen_font;

  int normal_font_height;
  int header_box_size;

  bool exit_startmenu;

  void exit_fullscreen_to_smenu();

public:

  Pictures();
  ~Pictures();

  PicOpts* get_opts()
  {
    return &opts;
  }

  void startup_updater();

  // gcc 2.95 fix

  // For traversing the filesystem. The first element is the dirs and the next is the
  // position in the list generated from the dirs
  std::stack<std::pair<std::list<std::string>, int > > folders;

  void print_fullscreen(const Simplefile& e, const std::string& l, const std::string& r);

  std::vector<Picture> pic_list;

  void reset();

  void load_current_dirs();
  void read_dirs();

  void play_pictures_cd();

  std::string mainloop();

  void pictures_check();
  int check_picture_time();
};

#endif
