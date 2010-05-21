#include "input_device.hpp"

#include "busy_indicator.hpp"

#include <fstream>
#include <sstream>
#include <iostream>

using std::string;

InputDevice::InputDevice()
  : accept_all(false)
{}

bool InputDevice::parse_keys(const string& plugin, const string& version)
{
  std::pair<bool, std::pair<input_map, input_list> > result = parse_config(plugin, version, false);

  // FIXME: could it just not return an empty list?

  if (!result.first)
    return false;

  input_map imap = result.second.first;
  input_list ilist = result.second.second;

  if (plugin == "default")
    maps.insert(std::make_pair(plugin, std::make_pair(imap, ilist)));
  else {
    std::map<string, std::pair<input_map, input_list> >::iterator element = maps.find("default");
    if (element == maps.end())
      std::cerr << "warning input loaded for plugin before loading main input" << std::endl;
    else {
      // add default values
      std::pair<input_map, input_list> input = element->second;
      imap.insert(input.first.begin(), input.first.end());
      ilist.insert(ilist.end(), input.second.begin(), input.second.end());
    }
    maps.insert(std::make_pair(plugin, std::make_pair(imap, ilist)));
  }

  return true;
}

bool InputDevice::extend_default(const string& plugin, const string& version)
{
  std::pair<bool, std::pair<input_map, input_list> > result = parse_config(plugin, version, true);

  if (!result.first)
    return false;

  input_map imap = result.second.first;
  input_list ilist = result.second.second;

  name_input_map new_maps;

  foreach (const name_input& element, maps) {
    if (element.first != "search") {
      input_map imap_temp = element.second.first;
      imap_temp.insert(imap.begin(), imap.end());
      input_list ilist_temp =element.second.second;
      ilist_temp.insert(ilist_temp.end(), ilist.begin(), ilist.end());
      new_maps.insert(std::make_pair(element.first, std::make_pair(imap_temp, ilist_temp)));
    } else {
      new_maps.insert(element);
    }
  }

  maps = new_maps;

  return true;
}

std::pair<bool, std::pair<input_map, input_list> > InputDevice::parse_config(const string& plugin,
									     const string& version,
									     bool extends)
{
  Config *config = S_Config::get_instance();

  string input;
  string _str= (extends) ? "_extend" : "";
  input = config->p_homedir() + "/input/" + name() + "/" + plugin + _str;

  if (!file_exists(input))
    input =  std::string(mms_config) + "/input/" + name() + "/" + plugin + _str;

  input_map imap;
  input_list ilist;

  std::ifstream In(input.c_str());   //create an input file stream

  if(!In){
    print_critical(gettext("Input configuration file not found ") + input, "INPUT");
    return std::make_pair(false, std::make_pair(imap, ilist));
  } else {

    string str;

    bool version_correct = false;

    while (getline(In, str)) {
      // skips empty and strings which starts with a #
      if (str[0] == '#' || str.size() == 0)
	continue;

      if (!version_correct) {
	version_correct = check_version(input, str, version);
	if (!version_correct)
	  return std::make_pair(false, std::make_pair(imap, ilist));
      }

      string::size_type pos = str.find(",");

      if (pos == string::npos) // bogus line
	continue;

      string mode = str.substr(0, pos);

      string strlast = str.substr(pos+1);
      string::size_type pos2 = strlast.find(",");

      if (pos2 == string::npos) // bogus line
	continue;

      string strlast2 = strlast.substr(pos2+1);
      string::size_type pos3 = strlast2.find(",");

      string command = str.substr(pos+1, pos2);

      string key, mult;

      if (pos3 != string::npos) {
	key = strlast.substr(pos2+1, pos3); // pos3 is relative to strlast2
	mult = strlast2.substr(pos3+1);
      } else {
	key = strlast.substr(pos2+1);
	mult = "";
      }

      string presentation;
      string::size_type pos4 = key.find("|");
      if (pos4 == string::npos) { // no presentation defined
        presentation = key;
      } else {
        presentation = key.substr(pos4+1);
        key = key.substr(0,pos4);
      }

      bool multiple = false;

      if (mult == "yes")
	multiple = true;

      Input putback;

      putback.mode = mode;
      putback.command = command;
      putback.key = key;
      putback.presentation = presentation;
      putback.multiple = multiple;

      print_debug(gettext("Adding ") + mode + " " + command + " " + key + ":" + presentation + " " + putback.name, "INPUT");

      imap.insert(std::make_pair(putback.key, putback));
      ilist.push_back(putback);
    }

    return std::make_pair(true, std::make_pair(imap, ilist));
  }
}

bool InputDevice::check_version(const string& input, const string& str,
				const string& version)
{
  // first line must be a version line
  string::size_type p = str.find("=");
  string param = str.substr(0, p-1);
  string value = str.substr(p+2);

  if (param == "version") {
    if (value == version) {
      return true;
    } else {
      DebugPrint perror(Print::DEBUGGING, DebugPrint::CRITICAL, "INPUT");
      perror.add_line(gettext("Wrong version of input file found: ") + input);
      perror.add_line(gettext("Please upgrade your input file"));
      perror.add_line(gettext("Got version ") + value + gettext(" expected ") + version);
      perror.print();
      ::exit(0);
    }
  } else {
    DebugPrint perror(Print::DEBUGGING, DebugPrint::CRITICAL, "INPUT");
    perror.add_line(gettext("Version string not found"));
    perror.add_line(gettext("Please upgrade your input file"));
    perror.print();
    ::exit(0);
  }

  return false;
}

bool InputDevice::find_input(const string& input, Input &i)
{
  if (input.empty())
    return false;

  string cur_map = S_InputMaster::get_instance()->current_map();

  name_input_map::iterator iter = maps.find(cur_map);

  if (iter != maps.end()) {
    input_map cmap = iter->second.first;
    input_map::iterator l = cmap.find(input);
    if (l != cmap.end()) {
      i = l->second;
      return true;
    } else
      return false;
  } else {
    print_warning(gettext("Unknown cur_map: ") + cur_map + "!", "INPUT");
    return false;
  }
}

input_list InputDevice::cur_list()
{
  string cur_map = S_InputMaster::get_instance()->current_map();

  name_input_map::iterator i = maps.find(cur_map);
  if (i != maps.end())
    return i->second.second;
  else {
    print_warning(gettext("Unknown cur_map: ") + cur_map + "!", "INPUT");
    input_list l;
    return l;
  }
}
