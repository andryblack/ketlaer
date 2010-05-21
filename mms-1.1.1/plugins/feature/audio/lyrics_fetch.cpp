#include "lyrics_fetch.hpp"
#include "lyrics_fetch_helper.hpp"

#include "config.hpp"
#include "common-feature.hpp"
//#include "print.hpp"

// for cache
#include <fstream>
#include <iostream>

using std::string;

//pthread_mutex_t LyricsFetch::singleton_mutex = PTHREAD_MUTEX_INITIALIZER;

LyricsFetch::LyricsFetch(std::string _artist, std::string _title)
	: busy(true), str_buffer("")
{
  artist = _artist;
  title = _title;

  conf = S_Config::get_instance();
  
  if (!recurse_mkdir(conf->p_var_data_dir(),"lyrics/", &lyrics_dir)) {
      DebugPrint perror(dgettext("mms-audio", "Could not create directory ") + lyrics_dir,
			Print::DEBUGGING, DebugPrint::CRITICAL, "LYRICS");
      //FIXME: shouldn't this be critical???
  }

  // FIXME: take value from audio module
  screensaver_lyric_font = graphics::resolution_dependant_font_wrapper(22, conf);

  // check required data 
  if (artist.size() > 0 && title.size() > 0) {
    // start thread
    start();
  }
}

void LyricsFetch::run()
{
  std::string str_buffer;
  bool from_cache = false; 

  str_buffer = LoadCache();
  if (str_buffer.size() > 0) {
    from_cache = true;
  } else {
    LFH_lyricwiki wiki = LFH_lyricwiki(artist, title);
    wiki.fetch();
    if (wiki.status()) {
      str_buffer = wiki.get_text();
    } else {
      LFH_leoslyrics leo = LFH_leoslyrics(artist, title);
      leo.fetch();
      if (leo.status())
        str_buffer = leo.get_text();
    }    
  }


  parse_text(str_buffer);

  if (!from_cache && vecLyric.size()) 
    SaveCache(vecLyric); 

  busy = false;
}

bool LyricsFetch::is_busy() {
  return busy;
}

bool LyricsFetch::get_ready() 
{
  return (!busy && vecLyric.size() > 0);
}

std::vector<std::string> LyricsFetch::get_lyric() 
{
  return vecLyric;
}

bool LyricsFetch::parse_text(const string& text, const string& nsec)
{
  string::size_type cur_pos, next_pos, nsec_length;
  
  vecLyric.clear();

  string buff;
  cur_pos = 0;

  next_pos = text.find(nsec);
  if (next_pos == std::string::npos) {
    if (nsec == "\r\n")
      return parse_text(text, "\n");
    if (nsec == "\n")
      return parse_text(text, "\xD");
    else 
      return false;
  }

  while (cur_pos != std::string::npos && text.size() > 0) {
    nsec_length = (cur_pos == 0 ? 0 : nsec.size());
    next_pos = text.find(nsec, cur_pos + nsec_length);
    buff = text.substr(cur_pos + nsec_length, next_pos - cur_pos - nsec_length);
    addLine(buff);
    cur_pos = next_pos;
  }

  return vecLyric.size() > 1;
}

void LyricsFetch::addLine(const std::string& line)
{
  string nline = string_format::trim(string_format::ConvertHTMLToAnsi(line));
  std::pair<int, int> size = string_format::calculate_string_size(nline, screensaver_lyric_font);

  int scr_size = (conf->p_h_res() * 0.80);
  int n_lines = (size.first / scr_size);
  int n_lines_rest = (size.first % scr_size);
  if (n_lines_rest > 0)
	n_lines++;

  if (n_lines <= 1) {
	vecLyric.push_back(nline);
  } else {
	int chars = int(nline.size()/n_lines);
        string::size_type cur_space, next_space, start_space;
        string buff;
	cur_space = 0;
	for (int i = 0; i < n_lines; i++) {
	  start_space = (cur_space == 0 ? cur_space : cur_space + 1);
	  next_space = nline.find(" ",(i + 1) * chars);
	  buff = nline.substr(start_space, next_space - start_space);
	  vecLyric.push_back(buff);
          cur_space = next_space;
	}
  }
}

std::string LyricsFetch::LoadCache()
{
  std::ifstream file;
  std::string buff_text = "";
  file.open(filename().c_str());
  if (file.is_open())
    {
      string tmpBuff;
      while(!file.eof()) {
	getline(file, tmpBuff);
	if (tmpBuff != "") {
	  buff_text += tmpBuff;
	  buff_text += "\n";
	}
      }
      file.close();
    }
  return buff_text;
}

void LyricsFetch::SaveCache(std::vector<std::string> vLyr)
{
  if (!vLyr.size())
    return;

  std::ofstream file;

  file.open(filename().c_str());
  if (file.is_open())
  {
    foreach (std::string line, vLyr) {
      file << line << std::endl;
    }
    file.close();
  }
}

std::string LyricsFetch::filename()
{
  return (lyrics_dir + string_format::lowercase(string_format::convert(artist)) + " - " + string_format::lowercase(string_format::convert(title)));
}



