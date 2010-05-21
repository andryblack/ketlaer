#ifndef KEYBOARD_HPP
#define KEYBOARD_HPP

#include "config.h"

#include "input_device.hpp"
#include "config.hpp"
#include "common-output.hpp"
#include "renderer.hpp"
#ifdef use_sdl
#include <SDL/SDL.h>
#endif

// keyboard
#include <termios.h>
#include "termio/getch2.h"
#include "termio/keycodes.h"

#include <list>
#include <string>

#include <iostream>

class Keyboard : public InputDevice
{
private:
  // delay for input
  int halfdelay_time;

  int tty_mode;
  struct termios orig_tty;
  struct termios new_tty;

#ifdef use_sdl
  static int EventFilter(const SDL_Event *event);
  unsigned long long next_time;
  unsigned long long this_time;
  unsigned long long start_scroll;

  int last_valid_x, last_valid_y, consecutive_scroll_ok;

  bool scroll_up;
  Render * ren;
  std::string getSDLkey();
  int SDLWaitEvent(SDL_Event *event);
  std::string get_key_string(SDLKey key);
#endif

  void normal();
  void cbreak();
  volatile bool running;

  std::list<std::string> not_accepted_keys();

  std::list<std::string> not_accepted_chars;

  void generate_search_top()
  {
    not_accepted_chars = not_accepted_keys();
  }

  void generate_search_input(std::string& str, std::string &lowercase_search_str,
			     int& search_help_offset, Input input)
  {
    if (input.command == "erase" && input.mode == "search") {
      if (str.size() != 0) {
	str = str.substr(0, str.size()-1);
	lowercase_search_str = string_format::lowercase(str);
      }
    } else {
      bool found_match = false;

      if (input.key == "SPACE")
	input.key = " ";
      else
	foreach (std::string& not_accepted_char, not_accepted_chars)
	  if (input.key == not_accepted_char)
	    found_match = true;

      if (!found_match) {
	str += input.key;
	lowercase_search_str = string_format::lowercase(str);
      }
    }
  }

  void search_top()
  {
    not_accepted_chars = not_accepted_keys();
  }

  void search_input(std::string &search_str, std::string &lowercase_search_str, Input input, int& offset,
		    const boost::function<const std::vector<std::string> (const std::string&)>& list_gen)
  {
		//if (input.command == "complete")
		//  complete_string(search_str, lowercase_search_str, list_gen);

    int i;

    generate_search_input(search_str, lowercase_search_str, i, input);

	  int result_size = list_gen(lowercase_search_str).size();
	  // std::cout << "input command:" << input.command << std::endl;
	  // std::cout << "result size:" << result_size << std::endl;

    if (result_size > 0) {
      if (input.command == "next")
	offset = (offset + 1) % result_size;
      else if (input.command == "prev") {
	--offset;
	if (offset == -1)
	  offset = result_size-1;
		  } else {
	offset = 0;
    }
  }
  }

  void search_graphical_input(std::string &search_str, std::string &lowercase_search_str,
			      Input input, int& offset, int& pos, int& search_help_offset,
			      bool graphical_print, int images_per_row, int result_size)
  {
    // FIXME: refactor this function

    generate_search_input(search_str, lowercase_search_str, search_help_offset, input);

    if (offset == -1) {
      if (input.command == "next")
	offset = 0;
      else
	return;
    } else if (graphical_print && offset < images_per_row && input.command == "prev") {
      offset = -1;
      return;
    } else if (!graphical_print && offset == 0 && input.command == "prev") {
      offset = -1;
      return;
    }

    if (graphical_print) {
      if (input.command == "prev") {
	if (result_size > images_per_row) {
	  for (int i = 0; i < images_per_row; ++i) {
	    if (offset == 0) {
	      offset = result_size-1;
	      i += images_per_row-result_size%images_per_row;
	    }
	    else
	      offset -= 1;
	  }
	}
      }
      else if (input.command == "next")
	{
	  if (result_size > images_per_row) {
	    for (int i = 0; i < images_per_row; ++i) {
	      if (offset == result_size-1 && i == 0) {
		offset = 0;
		break;
	      } else if (offset == result_size-1) {
		break;
	      } else {
		offset += 1;
	      }
	    }
	  }
	}
      else if (input.command == "left")
	{
	  if (offset != 0)
	    offset -= 1;
	  else
	    offset = result_size - 1;
	}
      else if (input.command == "right")
	{
	  offset = (offset+1)%result_size;
	}
    } else {
      if (input.command == "next")
	offset = ++offset % result_size;
      else if (input.command == "prev") {
	--offset;
	if (offset == -1)
	  offset = result_size-1;
      }
    }
  }


public:

  void run();
  bool init();

  Keyboard();
  ~Keyboard();

  std::string name()
  {
    return "keyboard";
  }

  void suspend();
  void wake_up();
};

#endif
