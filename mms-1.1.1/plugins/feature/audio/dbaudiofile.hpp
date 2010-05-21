#ifndef DBAUDIOFILE_HPP
#define DBAUDIOFILE_HPP

#include "gsimplefile.hpp"
#include "audio.hpp"

class GraphicalAudio;

class Dbaudiofile : public GSimplefile
{
public:
  std::string artist;
  std::string album;
  std::string title;
  int year;
  int bitrate;
  int length;
  int rating;
  int track;

  int db_id;

  int playlist_id; // defined if the file is inside a playlist

  std::string to_string() const;
  std::string short_to_string() const;
  std::string get_name_from_file_in_picture_dir() const;

  void set_values(const Dbaudiofile& s);

  Dbaudiofile();
  Dbaudiofile(const GSimplefile& s);
  // create object by looking up path in db
  Dbaudiofile(const std::string& p);
  // create object by looking up id in db
  Dbaudiofile(int id);

  void get_info_from_db(const string& path, GraphicalAudio *ga);

  bool is_fetched() const;

  bool operator==(const Dbaudiofile& rhs) const;

private:
  bool fetched;
};

#endif
