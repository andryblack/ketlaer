#ifndef SIMPLE_AUDIO_HPP
#define SIMPLE_AUDIO_HPP

#include "audio.hpp"

class SimpleAudio : public AudioTemplate<Simplefile>
{
private:

  void reparse_file(const std::string& path);
  bool reload_dir(const std::string& path);
  void reload_current_dirs();

  // audio add specific
  void enter_dir();

  void secondary_menu_in_add(ExtraMenu &em);

  void add_file_to_list(std::vector<Simplefile>& list, const Simplefile& el)
  {
    list.push_back(el);
  }

  bool load_playlist_from_db(const std::vector<LastFM::Track>& UNUSED(tracks))
  {return false;}

  // add an audio dir recursively (all the files in it) to the playlist
  std::vector<Simplefile> add_dir(const std::string& dir, bool from_media = false);
  // read a dir and spit out a list of files and dirs
  std::vector<Simplefile> rdir(const std::string& argv);

  void print_element(const Simplefile& r, const Simplefile& position, int y);

  void intelligent_random_prev()
  {}
  Simplefile intelligent_random_next()
  {
    Simplefile s;
    return s;
  }

  void audio_screensaver_next_helper(int UNUSED(x), int& UNUSED(y))
  {}

  void options_reloaded()
  {}

  void print_audiotrack_helper(int &x_pos);

  bool print_audio_screensaver_helper(int &x_pos, int & UNUSED(y), int& UNUSED(max_x_pic), int& UNUSED(max_y_pic),
				      bool UNUSED(vert_center))
  {
    x_pos = 100;
    return false;
  }

  void get_audiotrack_info(std::string& buffer, std::string& artist, std::string& album,
			   std::string& title);

  void search_func()
  {
    S_BusyIndicator::get_instance()->idle();

    int *position;
    if (mode == ADD)
      position = &folders.top().second;
    else
      position = &playlist_pos_int;

    input_master->search<Simplefile>(*files, position,
				     boost::bind(&SimpleAudio::print, this, _1),
				     boost::bind(&SimpleAudio::search_compare, this, _1),
				     boost::bind(&SimpleAudio::get_name_from_file, this, _1),
				     search_mode, search_str, lowercase_search_str, offset);
  }

  void page_up();
  void page_down();

public:
  void read_dirs();

  std::string get_name_from_file(const Simplefile& d);
  std::string get_short_name_from_file(const Simplefile& d);

  std::string mainloop(AudioMode mode);

  void print(const std::vector<Simplefile>& files);
};

#endif
