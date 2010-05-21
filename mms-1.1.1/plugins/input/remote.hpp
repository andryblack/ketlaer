#ifndef REMOTE_HPP
#define REMOTE_HPP

#include "input_device.hpp"

#include "config.hpp"
#include "common.hpp"
#include "remote_config.hpp"

#include "common-output.hpp"

#include <string>
#include <vector>

class Remote : public InputDevice
{
private:
  RemoteConfig *remote_conf;

  std::vector<std::vector<std::string> > valid_keys();
  std::vector<std::vector<std::string> > parse_string(std::string all_chars);

  std::vector<std::vector<std::string> > keys;
  std::vector<std::string> last_keys;

  void generate_search_top()
  {
    keys = valid_keys();

    last_keys.clear();

    for (int i = 0; i < 5; ++i)
      last_keys.push_back("");
  }

  void generate_search_input(std::string& str, std::string &lowercase_search_str,
			     int& search_help_offset, Input input)
  {
    int pos;

    if (input.key == "0")
      pos = 9;
    else if (input.key == "1")
      pos = 0;
    else if (input.key == "2")
      pos = 1;
    else if (input.key == "3")
      pos = 2;
    else if (input.key == "4")
      pos = 3;
    else if (input.key == "5")
      pos = 4;
    else if (input.key == "6")
      pos = 5;
    else if (input.key == "7")
      pos = 6;
    else if (input.key == "8")
      pos = 7;
    else if (input.key == "9")
      pos = 8;
    else if (input.command == "erase") {
      str = str.substr(0, str.size()-1);

      for (int i = 0; i < 5; ++i)
	last_keys[i] = "";

      search_help_offset = -1;

      return;
    } else
      return;

    int amount = vector_lookup(keys, pos).size();

    if ((time(0)-last_keypress) > 2) {
      for (int i = 0; i < 5; ++i)
	last_keys[i] = "";
    }

    int keys_pos = 0;
    for (int p = 5; p > 5-amount; --p) {
      if (vector_lookup(last_keys, p-1) == input.key)
	++keys_pos;
      else
	break;
    }

    if (keys_pos == amount) {
      keys_pos = 0;
      for (int i = 0; i < 5; ++i)
	last_keys[i] = "";
      str = str.substr(0, str.size()-1);
    } else {
      if (keys_pos != 0)
	str = str.substr(0, str.size()-1);
    }

    str += vector_lookup(vector_lookup(keys, pos), keys_pos);

    for (int i = 1; i < 5; ++i)
      last_keys[i-1] = vector_lookup(last_keys, i);
    last_keys[4] = input.key;

    last_keypress = time(0);

    // calculate vertical offset
    search_help_offset = 0;
    for (int i = 0; i < pos; ++i)
      search_help_offset += vector_lookup(keys, i).size();
    search_help_offset += keys_pos;
  }

  void search_top()
  {
    keys = valid_keys();
  }

  void search_input(std::string &search_str, std::string& lowercase_search_str, Input input, int& offset,
		    const boost::function<const std::vector<std::string> (const std::string&)>& list_gen)
  {
    //    std::string last_letter = "";
    //std::string last_key = "";

    int pos;

    //    if (input.command == "complete")
    //  complete_string(search_str, lowercase_search_str, list_gen);

    int result_size = list_gen(lowercase_search_str).size();

    if (input.key == "0")
      pos = 9;
    else if (input.key == "1")
      pos = 0;
    else if (input.key == "2")
      pos = 1;
    else if (input.key == "3")
      pos = 2;
    else if (input.key == "4")
      pos = 3;
    else if (input.key == "5")
      pos = 4;
    else if (input.key == "6")
      pos = 5;
    else if (input.key == "7")
      pos = 6;
    else if (input.key == "8")
      pos = 7;
    else if (input.key == "9")
      pos = 8;
    else if (input.command == "next") {
      if (result_size > 0)
	offset = ++offset % result_size;
      return;
    } else if (input.command == "prev") {
      --offset;
      if (offset == -1)
	offset = result_size-1;
      return;
    } else if (input.command == "erase") {
      search_str = search_str.substr(0, search_str.size()-1);
      last_keypress = time(0) - 3; // erase = new char
      return;
    } else
      return;

    offset = 0;

    std::vector<std::string> searched = list_gen(lowercase_search_str);
    int good_key = 0;

    bool other_key_possible = false;
    bool found_key = false;

    int chars_for_cur_key = vector_lookup(keys, pos).size();

    if (searched.size() != 0) {

      // generate list of all chars already tried for the current key
      std::vector<std::string> f;

      if (last_key == input.key) {
	for (int i = 0; i < chars_for_cur_key; ++i)
	  if (last_letter == "")
	    break;
	  else if (vector_lookup(vector_lookup(keys, pos), i) != last_letter)
	    f.push_back(vector_lookup(vector_lookup(keys, pos), i));
	  else if (vector_lookup(vector_lookup(keys, pos), i) == last_letter) {
	    f.push_back(vector_lookup(vector_lookup(keys, pos), i));
	    break;
	  }
      }

      // get the list of items the last time we searched, to look for new
      // letters possible
      if (f.size() > 0) {

	std::vector<std::string> searched_before = list_gen(lowercase_search_str.substr(0, lowercase_search_str.size()-1));

	for (int i = 0; i < chars_for_cur_key; ++i) {
	  if (found_key)
	    break;
	  for (int j = 0; j < searched_before.size(); ++j) {
	    if (string_format::lowercase(vector_lookup(searched_before, j))[search_str.size()-1]
		== vector_lookup(vector_lookup(keys, pos), i)[0]) {
	      bool g = false;
	      if (i < f.size()) {
		if (vector_lookup(vector_lookup(keys, pos), i) != vector_lookup(f, i))
		  g = true;
	      } else
		g = true;

	      if (g) {
		other_key_possible = true;
		found_key = true;
		good_key = i;
		break;
	      }
	    }
	  }
	}
      }

      if (!other_key_possible) {
	for (int i = 0; i < chars_for_cur_key; ++i) {
	  if (found_key)
	    break;
	  for (int j = 0; j < searched.size(); ++j) {
	    if (string_format::lowercase(vector_lookup(searched, j))[search_str.size()]
		== vector_lookup(vector_lookup(keys, pos), i)[0]) {
	      good_key = i;
	      found_key = true;
	      break;
	    }
	  }
	}
      }
    }

    if (other_key_possible)
      search_str = search_str.substr(0, search_str.size()-1);

    if (found_key) {
      std::string key = vector_lookup(vector_lookup(keys, pos), good_key);
      search_str += key;
      last_letter = key;
    } else
      last_letter = "";

    last_key = input.key;
  }

  std::string last_letter;
  std::string last_key;

  int last_keypress;

  void search_graphical_input(std::string &search_str, std::string &lowercase_search_str,
			      Input input, int& offset, int& pos, int& search_help_offset,
			      bool graphical_print, int images_per_row, int result_size)
  {
    search_help_offset = -1;

    if (result_size > 0) {
      if (offset == -1 && input.command == "next") {
	offset = 0;
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
	      } else
		offset -= 1;
	    }
	  }
	  return;
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
	    return;
	  }
	else if (input.command == "left")
	  {
	    if (offset != 0)
	      offset -= 1;
	    else
	      offset = result_size - 1;
	    return;
	  }
	else if (input.command == "right")
	  {
	    offset = (offset+1)%result_size;
	    return;
	  }
      } else {
	if (input.command == "next") {
	  offset = ++offset % result_size;
	  return;
	} else if (input.command == "prev") {
	  if (offset == -1)
	    return;

	  --offset;
	  if (offset == -1)
	    offset = result_size-1;

	  return;
	}
      }
    }

    int remote_pos;

    if (input.key == "0")
      remote_pos = 9;
    else if (input.key == "1")
      remote_pos = 0;
    else if (input.key == "2")
      remote_pos = 1;
    else if (input.key == "3")
      remote_pos = 2;
    else if (input.key == "4")
      remote_pos = 3;
    else if (input.key == "5")
      remote_pos = 4;
    else if (input.key == "6")
      remote_pos = 5;
    else if (input.key == "7")
      remote_pos = 6;
    else if (input.key == "8")
      remote_pos = 7;
    else if (input.key == "9")
      remote_pos = 8;
    else if (input.command == "erase") {
      search_help_offset = -1;
      search_str = search_str.substr(0, search_str.size()-1);
      lowercase_search_str = string_format::lowercase(search_str);
      return;
    } else
      return;

    std::vector<std::string> cur_key = vector_lookup(keys, remote_pos);
    int chars_for_cur_key = cur_key.size();

    bool new_key = true;

    if (last_key == input.key && (time(0)-last_keypress) <= 2)
      new_key = false;

    int good_key = -1;

    if (!new_key) {
      bool found_old_key = false;
      std::string old_search_str = search_str;
      for (int i = 0; i < chars_for_cur_key; ++i) {
	if (found_old_key) {
	  search_str.replace(search_str.size()-1, vector_lookup(cur_key, i).size(), vector_lookup(cur_key, i));
	  last_letter = vector_lookup(cur_key, i);
	  good_key = i;
	  break;
	} else if (vector_lookup(cur_key, i) == last_letter) {
	  found_old_key = true;
	}
      }
      if (good_key == -1)
	search_str = old_search_str;
    } else if (new_key || good_key == 0) {

      last_letter = "";

      for (int i = 0; i < chars_for_cur_key; ++i) {
	search_str += vector_lookup(cur_key, i);
	last_letter = vector_lookup(cur_key, i);
	good_key = i;
	break;
      }
    }

    lowercase_search_str = string_format::lowercase(search_str);

    if (good_key == -1) {
      search_help_offset = -1;
    } else {
      // calculate vertical offset
      search_help_offset = 0;
      for (int i = 0; i < remote_pos; ++i)
	search_help_offset += vector_lookup(keys, i).size();
      search_help_offset += good_key;
    }

    if (!last_letter.empty())
      last_key = input.key;
    else
      last_key = "";

    last_keypress = time(0);
  }

public:

  Remote();
};

#endif
