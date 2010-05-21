#ifndef LYRICSFETCH_HPP
#define LYRICSFETCH_HPP

#include "config.hpp"

#include "cc++/thread.h"

class LyricsFetch : public ost::Thread
{
public: 

  LyricsFetch(std::string _artist, std::string _title);

  std::string filename();
  std::string LoadCache();
  void SaveCache(std::vector<std::string> vLyr);

  bool is_busy();
  bool get_ready();

  std::vector<std::string> get_lyric();
  
private:

  bool busy;

  std::string artist;
  std::string title;

  std::vector<std::string> vecLyric;
  
  std::string screensaver_lyric_font;
  std::string lyrics_dir;

  std::string str_buffer;

  Config *conf;

  bool parse_text(const std::string& text, const std::string& nsec = "\r\n");
  void addLine(const std::string& line);

  virtual void run();
};

#endif
