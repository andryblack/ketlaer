#define X_DISPLAY_MISSING

#define UGLY_FONT_FLICKER_HACK

#include <Imlib2.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <map>

using std::string;

static std::map<string, std::pair<int, int> > size_map;

bool set_font(const string& font_name)
{
  Imlib_Font font = imlib_load_font(font_name.c_str());
  if (!font) {
    std::cerr << "The font " << font_name << " could not be loaded" << std::endl;
    return false;
  } else
    imlib_context_set_font(font);

  return true;
}

int calculate_string_size(const std::string& text, const std::string& font)
{
  int width, height;

  if (set_font(font)) {
    // " " is there to compensate for imlib fix when drawing
    imlib_get_text_size((text + " ").c_str(), &width, &height);
    imlib_free_font();

    // during rendering we add black borders
    width += 2;
    height += 2;
  }

  return width;
}

void parse_translation(const std::string& filename)
{
  std::ifstream in;

  in.open(filename.c_str(), std::ios::in);
  
  string str;

  string english_string = "";
  string translated_string = "";

  bool english_ok = false;
  bool translation_ok = false;
  
  unsigned long linenumb = 0;
  unsigned long line;

  while (getline(in, str)) {

    linenumb++;

    // has state changed?
    if (str.find("msgstr ") != string::npos && str[0] != '#') {
      translation_ok = true;
      english_ok = false;
      line=linenumb;
    } else if (str.find("msgid ") != string::npos && str[0] != '#') {
      std::map<string, std::pair<int, int> >::iterator i = size_map.find(english_string);
      if (i != size_map.end()) {
	std::stringstream font_string;
	font_string << "Vera/" << i->second.first;
	int size = calculate_string_size(translated_string, font_string.str());
	if (size > i->second.second && i->second.second != 0) {
	  std::cout << "WARNING: On line " << line <<" string possibly too large, size " << size << ", max size " << i->second.second << std::endl;
	  string tmp_string = translated_string;
	  int tmp_size = size;
	  do {
	    tmp_string = tmp_string.substr(0, tmp_string.size()-1);
	    tmp_size = calculate_string_size(tmp_string, font_string.str());
	  } while (tmp_size > i->second.second);

	  std::cout << "size of translated string is: " << translated_string.size() 
		    << " chars, ok string size is around: " 
		    << tmp_string.size() << " chars" << std::endl;

	  std::cout << translated_string << std::endl << std::endl;
	}
      } else {
	std::cerr << "WARNING: size of string not found:" << english_string << std::endl;
      }
      
      english_string = "";
      translated_string = "";
      english_ok = true;
      translation_ok = false;
    }

    if (english_ok) {
      string::size_type start_of_string = str.find("\"");
      string::size_type end_of_string = str.rfind("\"");
      if (start_of_string != string::npos && end_of_string != string::npos) {
	english_string += str.substr(start_of_string+1, 
				    end_of_string-start_of_string-1);

      }
    } else if (translation_ok) {
      string::size_type start_of_string = str.find("\"");
      string::size_type end_of_string = str.rfind("\"");
      if (start_of_string != string::npos && end_of_string != string::npos) {
	translated_string += str.substr(start_of_string+1, 
					end_of_string-start_of_string-1);
      }
    }
  }
}

void parse_length(const std::string& filename)
{
  std::ifstream in;

  in.open(filename.c_str(), std::ios::in);
  
  string str;

  string english_string = "";
  string translated_string = "";

  bool english_ok = false;
  bool translation_ok = false;
  
  while (getline(in, str)) {

    // has state changed?
    if (str.find("msgstr ") != string::npos && str[0] != '#') {
      translation_ok = true;
      english_ok = false;
    } else if (str.find("msgid ") != string::npos && str[0] != '#') {
      int size;
      int font_size = 17;

      string::size_type font = translated_string.find(",");
      if (font != string::npos) {
	size = atoi(translated_string.substr(0, font).c_str());
	font_size = atoi(translated_string.substr(font+1).c_str());
      } else
	size = atoi(translated_string.c_str());

      size_map.insert(std::make_pair(english_string, std::make_pair(font_size, size)));
      english_string = "";
      translated_string = "";
      english_ok = true;
      translation_ok = false;
    }

    if (english_ok) {
      string::size_type start_of_string = str.find("\"");
      string::size_type end_of_string = str.rfind("\"");
      if (start_of_string != string::npos && end_of_string != string::npos) {
	english_string += str.substr(start_of_string+1, 
				    end_of_string-start_of_string-1);

      }
    } else if (translation_ok) {
      string::size_type start_of_string = str.find("\"");
      string::size_type end_of_string = str.rfind("\"");
      if (start_of_string != string::npos && end_of_string != string::npos) {
	translated_string += str.substr(start_of_string+1, 
					end_of_string-start_of_string-1);
      }
    }
  }
}

int main(int argc,char* argv[])
{
  if (argc != 2) {
    std::cerr << "wrong number of args, usage: check-size LANG.po" << std::endl;
    return 0;
  }

  //imlib_add_path_to_font_path("/usr/share/mms/fonts");
  imlib_add_path_to_font_path("../fonts");

  // first open check_length and create a size map
  parse_length("check-length.po");

  parse_translation(argv[1]);
}
