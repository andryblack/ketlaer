#ifndef GRAPHICAL_AUDIO_HPP
#define GRAPHICAL_AUDIO_HPP

#include "audio.hpp"

#include "cpp_sqlitepp.hpp"

#include "dbaudiofile.hpp"

#ifdef use_imms
#include "clientstub.h"
#endif

class GraphicalAudio : public AudioTemplate<Dbaudiofile>
{
  friend class Dbaudiofile;

private:

  SQLDatabase db;

  std::string ext_mask;
  ost::Mutex db_mutex;

  void create_indexes();
  void create_playlist_db();
  void create_db();
  void check_db();
  void check_db_consistency();
  bool search_is_graphical_view(const std::vector<Dbaudiofile>& files);
  std::string id_to_filename(int db_id);
  bool change_dir_to_id(int db_id);

  std::vector<std::pair<std::string, int> > gen_search_list(const std::string& search_word);

  void reenter(int id);

  void secondary_menu_commands(ExtraMenu &em);
  void secondary_menu_in_add(ExtraMenu &em);

  std::vector<std::string> genres_for_artist(const std::string& artist);

  bool load_playlist_from_db(const std::vector<LastFM::Track>& tracks);

  // touch
  void find_element_and_do_action_add(const Dbaudiofile& audio_file);
  void find_element_and_do_action_playlist(const Dbaudiofile& audio_file);
  void find_element(const Dbaudiofile& audio_file);

  // commands
  void print_information();
  // audio add specific
  void enter_dir();

  void generate_similar_track_playlist();

  void one_up();

  void check_for_changes();

  void reparse_file(const std::string& path);
  bool reload_dir(const std::string& path);
  void reload_current_dirs();

  void search_func();

  void add_file_to_list(std::vector<Dbaudiofile>& list, const Simplefile& el)
  {
    list.push_back(Dbaudiofile(el));
  }

  void print_element(Dbaudiofile& r, const Dbaudiofile& position,
		     int y, bool picture_dir);

  // set db_id and playlist_id
  virtual void ids(Dbaudiofile& p);
  virtual void remove_track_from_playlist(const Dbaudiofile& p);
  virtual void remove_all_tracks_from_playlist();
  virtual void add_track_to_playlist(Dbaudiofile& p);
  virtual void add_tracks_to_playlist(const std::vector<Dbaudiofile>& vp);
  virtual void new_playlist();

  // helper function for add_dir to add files in the correct order based on
  // track nr
  void add_tracks_from_dir(std::vector<Dbaudiofile> &files_in_dir,
			   std::vector<Dbaudiofile> &cur_files);

  std::vector<Dbaudiofile> rdir_hd(const std::string& argv, bool insert_into_db);

  bool rdir_internal(const std::string& filename, const std::string& argv,
		     std::vector<Dbaudiofile> &cur_files, bool in_db);
  void insert_file_into_db(const std::string& filename, const std::string& parent);
  void insert_cover_into_db(const std::string& filename, const std::string& type);
  std::vector<std::string> check_db_for_folders(const std::string& parent);

  // add an audio dir recursively (all the files in it) to the playlist
  std::vector<Dbaudiofile> add_dir(const std::string& dir, bool from_media = false);
  // read a dir and spit out a list of files and dirs
  std::vector<Dbaudiofile> rdir(const std::string& argv);

  virtual void audio_screensaver_next_helper(int x, int& y);

  void intelligent_random_prev();
  Simplefile intelligent_random_next();

  void print_audiotrack_helper(int &x_pos);
  bool print_audio_screensaver_helper(int &x_pos, int &y, int& max_x_pic, int& max_y_pic,
				      bool vert_center);

  void get_audiotrack_info(std::string& buffer, std::string& artist, std::string& album,
			   std::string& title);

  struct FilterOps
  {
    static void set_next(int next)
    {
      GraphicalAudio *audio = get_class<GraphicalAudio>(dgettext("mms-audio", "Audio"));
      audio->next_track_pos = next;
    }
    static void reset_selection()
    {
      GraphicalAudio *audio = get_class<GraphicalAudio>(dgettext("mms-audio", "Audio"));
      audio->next_track_pos = -1;
    }
    static string get_item(int index)
    {
      GraphicalAudio *audio = get_class<GraphicalAudio>(dgettext("mms-audio", "Audio"));
      return (index > audio->playlist_size())? "" : audio->get_playlist_element(index);
    }
    static int get_length()
    {
      GraphicalAudio *audio = get_class<GraphicalAudio>(dgettext("mms-audio", "Audio"));
      return audio->playlist_size();
    }
  };

#ifdef use_imms
  typedef IMMSClient<FilterOps> IMMSC;
  IMMSC *imms;
#endif

  std::string imms_cur_path;

  bool file_exists_in_db(const std::string& file);
  int db_id(const string& file, bool is_dir);

  bool extracted_all_files;
  std::stack<std::string> all_metadata_files;

  bool no_folders_in_dir(const std::vector<Dbaudiofile>& cur_files);
  void print_pic_element(int cur_pos, int j, int& x, int& y, const Dbaudiofile& s, int& y_pos_offset, double scale1, double scale2, const std::string& pic);
  void print_graphical(std::vector<Dbaudiofile>& files);
  void print_tracklist(std::vector<Dbaudiofile>& cur_files);

  void options_reloaded();

  // general get cover function
  std::string get_cover(const Simplefile& file);

  // helper functions for getting a cover from a dir
  std::string get_cover_from_dir(const std::string& dir);
  std::string get_first_cover_in_dir(const std::string& dir);

  int y_start;

  int image_width;
  int image_height;
  int image_height_all_eks_text;

  // including text and space around image
  int image_width_all;
  int image_height_all;
  int image_height_all_search;

  std::pair<int, int> s_header;		// Size of Header
  int header_box_size;
  int images_per_row;
  int rows;
  int rows_search;

  int helper_letters_height;

  void res_dependant_calc_2();

  bool exit_choose_cover_loop;

  bool is_in_add;

  void find_cover_element_and_do_action(const std::string& pic);
  void use_cover();

  std::vector<std::string> covers;
  int cover_pos;

  void print_choose_cover(const std::vector<std::string>& covers, int cover_pos, Overlay& o);
  void choose_cover();

  void page_up();
  void page_down();

  int check_search();

  bool reload_dirs;

public:
  GraphicalAudio();
  ~GraphicalAudio();

  int next_track_pos;

  void read_dirs();

  // metadata extraction
  void extract_metadata();

  std::string get_name_from_file(const Dbaudiofile& d);
  std::string get_short_name_from_file(const Dbaudiofile& d);

  std::string mainloop(AudioMode mode);

  void print_modifying(std::vector<Dbaudiofile>& cur_files);

  void print(const std::vector<Dbaudiofile>& files)
  {
    std::cerr << "WARNING: use sparingly" << std::endl;
    std::vector<Dbaudiofile> tmp_files = files;
    print_modifying(tmp_files);
  }

  void startup_updater();
};

#endif
