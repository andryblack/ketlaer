#include "config.h"

#include "options.hpp"

#include "common.hpp"
#include "graphics.hpp"
#include "global.hpp"

#include "plugins/plugins.hpp"

#include "gettext.hpp"
#include "shutdown.hpp"
#include "touch.hpp"
#include "resolution.hpp"
#include "common-feature.hpp"

#include "boost.hpp"

#include <fstream>

// mkdir()
#include <sys/stat.h>
#include <sys/types.h>

using std::list;
using std::vector;
using std::string;

Options::Options()
  : exit_loop(false)
{
  im = S_InputMaster::get_instance();
  themes = S_Themes::get_instance();
  render = S_Render::get_instance();
  conf = S_Config::get_instance();
  go = S_GlobalOptions::get_instance();
  global = S_Global::get_instance();

  res_dependant_calc();

  S_ResolutionManagement::get_instance()->register_callback(boost::bind(&Options::res_dependant_calc, this));
}

void Options::res_dependant_calc()
{
  header_font = "Vera/" + conv::itos(resolution_dependant_font_size(28, conf->p_v_res()));
  element_header_font = "Vera/" + conv::itos(resolution_dependant_font_size(20, conf->p_v_res()));
  element_font = "Vera/" + conv::itos(resolution_dependant_font_size(17, conf->p_v_res()));

  std::pair<int, int> element_size = string_format::calculate_string_size("abcltuwHPMjJg", element_font);
  element_height = element_size.second - 5;
}

bool Options::mainloop()
{
  im->save_map();
  im->set_map("default");

  Shutdown *sd = S_Shutdown::get_instance();

  Input input;

  vector<Option*>::iterator iter = val.begin();
  int pos = 0; // position in values, used by print()
  if (!val.size()){
    /* nothing to show */
    DialogWaitPrint pdialog(4000);
    pdialog.add_line(dgettext("mms-audio-xine", "There are no options to show"));
    pdialog.print();

    return false;
  }
  // set the element to the first non-type element
  while ((*iter)->type) {
    vector<Option*>::iterator j = val.end(); --j;
    if (iter != j) {
      ++iter;
      ++pos;
    } else {
      iter = val.begin();
      pos = 0;
    }
  }

  bool reload = false, reconfigure = false;

  bool update_needed = true;

  Render *render = S_Render::get_instance();

  render->device->animation_section_begin(true);
  int curlayer = render->device->get_current_layer();
  render->device->switch_to_layer(curlayer+1);
  render->device->layer_active_no_wait(true);
  render->device->reset_layout_attribs_nowait();
  render->device->set_layout_alpha(0.0, curlayer+1);
  render->device->animation_move( 0, -conf->p_v_res(), 0, 0, 60, curlayer+1);
  render->device->animation_move(0, 0, 0, conf->p_v_res(), 60, curlayer);
  render->device->animation_fade(0,1,100,curlayer+1);
  render->device->animation_fade(1,0,100,curlayer);
  render->device->animation_section_end();

  while (!exit_loop)
    {
      if (update_needed) {
	print((*iter)->english_name, pos);
	print_lcd_menu((*iter)->english_name);
      }

      input = im->get_input();

      if (sd->is_enabled()) {
      	sd->cancel();
      	continue;
      }

      update_needed = true;

      if (input.key == "touch_input") {
	S_Touch::get_instance()->run_callback();
	continue;
      }

      if (input.command == "prev")
 	{
	  do {
	    if (iter != val.begin()) {
	      --iter; --pos;
	    } else {
	      vector<Option*>::iterator j = val.end(); --j;
	      iter = j; pos = val.size() - 1;
	    }
	  } while ((*iter)->type);
 	}
      else if (input.command == "next")
 	{
	  do {
	    vector<Option*>::iterator j = val.end(); --j;
	    if (iter != j) {
	      ++iter; ++pos;
	    } else {
	      iter = val.begin(); pos = 0;
	    }
	  } while ((*iter)->type);
 	}
      else if ((*iter)->english_name == "reload" && input.command == "action")
	{
	  reload = true;
	  break;
	}
      else if (input.command == "action" && (*iter)->callback != NULL)
	{
	  (*iter)->callback();
	}
      else if (input.command == "second_action")
	{
	  ExtraMenu em;

	  foreach (ExtraMenuItem& item, global->menu_items)
	    em.add_persistent_item(item);

	  em.add_item(ExtraMenuItem(gettext("Go back"), im->find_shortcut("back"),
				    boost::bind(&Options::exit, this)));

	  em.mainloop();
	}
      else if (input.command == "back" && input.mode == "general")
	{
	  exit();
	}
      else if (input.command == "left" || input.command == "right")
	{
	  if ((*iter)->english_name == "audio device (music)")
	    reconfigure = true;
	  else if ((*iter)->english_name == "sorting")
	    reload = true;

	  if (input.command == "left") {
	    if ((*iter)->pos != 0) {
	      --(*iter)->pos;
	    } else {
	      (*iter)->pos = (*iter)->values.size() - 1;
	    }
	  }
	  else if (input.command == "right") {
	    if ((*iter)->pos != ((*iter)->values.size() - 1)) {
	      ++(*iter)->pos;
	    } else {
	      (*iter)->pos = 0;
	    }
	  }
	}
      else
	update_needed = !global->check_commands(input);
    }

  exit_loop = false;

  render->device->animation_section_begin();
  render->device->switch_to_layer(curlayer);
  //render->device->make_actions_expire(curlayer);
  render->device->animation_move(0, 0, 0, -conf->p_v_res(),  60, curlayer+1);
  render->device->animation_move(0, conf->p_v_res(), 0, 0, 60, curlayer);
  render->device->animation_fade(1,0,100,curlayer+1);
  render->device->animation_fade(0,1,100,curlayer);
  render->device->animation_section_end();

  im->restore_map();

  if (reconfigure)
    global->reconfigure_playback();

  return reload;
}

void Options::exit()
{
  exit_loop = true;
}

void Options::print_element(const string& element, const string& value,
			    bool marked, int x, int y, int max_size, int max_element_size)
{
  if (marked)
    render->current.add(new PFObj(themes->general_marked, x-25, y-5,
				  40+max_size+50, element_height, 1, true));

  render->current.add(new TObj(element, element_font, x, y-8, themes->options_font1,
			       themes->options_font2, themes->options_font3, 2));

  render->current.add(new TObj(value, element_font, x + max_element_size + 50, y-8,
			       themes->options_font1, themes->options_font2,
			       themes->options_font3, 2));
}

void Options::print_reload_element(bool marked, int x, int y, int max_size, int max_element_size)
{
  if (marked) {
    int x_start = (conf->p_h_res()-max_size)/2;

    render->current.add(new PFObj(themes->general_marked, x_start-50, y+2,
				  90+max_size+50, element_height, 1, true));
  }

  render->current.add(new TObj(gettext("reload"), element_font, x, y, themes->options_font1,
			       themes->options_font2, themes->options_font3, 2));
}


void Options::print(const string& marking, int pos)
{
  render->prepare_new_image();

  render->current.add(new PObj(themes->options_background, 0, 0, 0, SCALE_FULL));

  std::pair<int, int> header_size = string_format::calculate_string_size("abcltuwHPMjJg", header_font);
  int header_box_size = static_cast<int>(header_size.second * 0.75);

  PObj *back = new PFObj(themes->startmenu_options, 25, 10, header_box_size, header_box_size, 2, true);

  render->current.add(back);

  S_Touch::get_instance()->register_area(TouchArea(rect(back->x, back->y, back->w, back->h), back->layer,
						   boost::bind(&Options::exit, this)));

  if (themes->show_header) {
    TObj *t = new TObj(translated_header, header_font, 25 + header_box_size + 10, 10 + (header_box_size - header_size.second)/2,
		       themes->options_header_font1, themes->options_header_font2,
		       themes->options_header_font3, 2);

    render->current.add(t);

    S_Touch::get_instance()->register_area(TouchArea(rect(t->x, t->y, t->w, t->h), t->layer, global->toggle_playlist));
  }

  int y = (conf->p_h_res()>800)?105:70;

  string button_playback_font = graphics::resolution_dependant_font_wrapper(17, conf);
  int button_playback_height = graphics::calculate_font_height(button_playback_font, conf);

  int bottom = static_cast<int>(2 * button_playback_height * 0.85);

  int reload_height = static_cast<int>(1.25*element_height);

  if (header != gettext("Options"))
    y += reload_height;

  bool direction_down = true;
  int space = vector_lookup(val, pos)->type ? element_height * 2 : element_height;
  int max_space = conf->p_v_res() - y - bottom;
  int divert_pos_down = 0, divert_pos_up = 0;
  int cur_pos = 0, nr_elements = 0;
  bool direction_up_ok = true, direction_down_ok = true;

  while (true) {
    if (!direction_down) {
      cur_pos = pos + divert_pos_up + 1;
      if (cur_pos <= val.size() - 1) {
	space += vector_lookup(val, cur_pos)->type ? element_height * 2 : element_height;
	if (space > max_space) {
	  if (!direction_down_ok)
	    break;
	  space -= vector_lookup(val, cur_pos)->type ? element_height * 2 : element_height;
	  direction_up_ok = false;
	  direction_down = true;
	  continue;
	}
	nr_elements += 1;
	divert_pos_up += 1;
      } else
	direction_up_ok = false;

      direction_down = true;
    } else {
      cur_pos = pos - divert_pos_down - 1;
      if (cur_pos >= 0) {
	space += vector_lookup(val, cur_pos)->type ? element_height * 2 : element_height;
	if (space > max_space) {
	  if (!direction_up_ok)
	    break;
	  space -= vector_lookup(val, cur_pos)->type ? element_height * 2 : element_height;
	  direction_down_ok = false;
	  direction_down = false;
	  continue;
	}

	nr_elements += 1;
	divert_pos_down += 1;
      } else
	direction_down_ok = false;

      direction_down = false;
    }

    if (!direction_up_ok && !direction_down_ok)
      break;
  }

  int max_element = 0, max_value = 0;

  bool reload_found = false;

  // calculate size
  foreach (Option *option, val) {

    if (option->english_name != "reload" && !option->type) {

      int element_size = string_format::calculate_string_width(option->name, element_font);

      foreach (string& option_value, option->values) {
	int value_size = string_format::calculate_string_width(option_value, element_font);
	max_value = std::max(value_size, max_value);
      }

      max_element = std::max(element_size, max_element);
    } else if (option->english_name == "reload") {
      int element_size = string_format::calculate_string_width(option->name, element_font);
      max_element = std::max(element_size, max_element);
      reload_found = true;
    }
  }

  if (max_element > conf->p_h_res()/2 - 20)
    max_element = conf->p_h_res()/2 - 20;

  if (max_value > conf->p_h_res()/2 - 20)
    max_value = conf->p_h_res()/2 - 20;

  int max = max_element + max_value;

  if (max == 0)
    max = 70;

  if (header != gettext("Options") && reload_found) {
    int reload_size = string_format::calculate_string_width(gettext("reload"), element_font);
    int x = (conf->p_h_res()-reload_size)/2;
    print_reload_element(marking == "reload", x, y-reload_height, max, max_element);
  }

  int x = (conf->p_h_res()-max)/2;

  int startpos = pos - divert_pos_down;

  for (int i = startpos, end = val.size(); i < end; ++i) {

    if (i - startpos > nr_elements)
      break;

    Option *o = vector_lookup(val, i);

    if (o->type) {

      if (i != startpos)
	y += (element_height*2)/5;

      render->current.add(new TObj(o->name, element_header_font, x-25, y,
				   themes->options_font1, themes->options_font2,
				   themes->options_font3, 1));
      y += (element_height*2*4)/5;

    } else if (o->english_name != "reload") {

      string name = o->name;
      string_format::format_to_size(name, element_font, max_element, false);
      string value = vector_lookup(o->values, o->pos);
      string_format::format_to_size(value, element_font, max_value, false);
      if (o->callback != NULL)
	value += " [...]";
      print_element(name, value, o->english_name == marking, x, y, max, max_element);
      y += element_height;

    }
  }

  render->draw_and_release("options");
}

void Options::print_lcd_menu(const string& marking)
{
  if (!global->lcd_output_possible())
    return;

  std::string lcd_header = "Options";

  vector<Option*>::iterator j = val.begin();
  while ((*j)->english_name != marking) {
    ++j;
  }

  std::string cur_name = "> " + (*j)->name;
  if (!(*j)->type && (*j)->values.size() > 0)
    cur_name += ": " + vector_lookup((*j)->values, (*j)->pos);
  std::string prev_name = "", next_name = "";

  vector<Option*>::iterator jend = val.end();
  --jend;

  if (val.size() == 2) {

    j = val.begin();
    if ((*j)->english_name != marking) {
      ++j;
    }
    if (j == jend)
      j = val.begin();
    else
      ++j;

    next_name = (*j)->name;
    if (!(*j)->type && (*j)->values.size() > 0)
      next_name += ": " + vector_lookup((*j)->values, (*j)->pos);

  } else {

    j = val.begin();
    while ((*j)->english_name != marking) {
      ++j;
    }

    // prev
    if (j != val.begin()) {
      --j;
      prev_name = (*j)->name;
      if (!(*j)->type && (*j)->values.size() > 0)
	prev_name += ": " + vector_lookup((*j)->values, (*j)->pos);
      ++j;
    } else {
      j = jend;
      prev_name = (*j)->name;
      if (!(*j)->type && (*j)->values.size() > 0)
	prev_name += ": " + vector_lookup((*j)->values, (*j)->pos);
      j = val.begin();
    }

    // next
    if (j != jend)
      ++j;
    else
      j = val.begin();

    next_name = (*j)->name;
    if (!(*j)->type && (*j)->values.size() > 0)
      next_name += ": " + vector_lookup((*j)->values, (*j)->pos);
  }

  if (global->lcd_rows() > 1) global->lcd_add_output(string_format::pretty_pad_string(lcd_header,global->lcd_pixels(),' '));
  if (global->lcd_rows() > 3) global->lcd_add_output(prev_name.substr(0, global->lcd_pixels()));
  global->lcd_add_output(cur_name.substr(0, global->lcd_pixels()));
  if (global->lcd_rows() > 2) global->lcd_add_output(next_name.substr(0, global->lcd_pixels()));
  global->lcd_print();
}

void Options::save()
{
  if (val.size() == 0) // don't save nothing
    return;

  std::ofstream file;

  string options_dir;
  if (!recurse_mkdir(conf->p_var_data_dir(), "options/", & options_dir))
    std::cerr << gettext("Could not create options directory: ") << options_dir << std::endl;

  string name = save_name;
  if (save_name.empty()) {
    name = get_english_header();
    std::cerr << gettext("Warning: module ") << name << gettext(" did not set save_name") << std::endl;
  }

  string path = options_dir + name;

  file.open(path.c_str());

  if (!file) {
    print_critical(gettext("Could not write options to file ") + path, "OPTIONS");
  } else {
    foreach (Option *option, val) {
      if (!option->type && option->english_name != "reload")
	file << option->english_name << ","
	     << vector_lookup(option->english_values, option->pos) << std::endl;
    }
  }

  file.close();
}

void Options::load()
{
  string name = save_name;
  if (save_name.empty()) {
    name = get_english_header();
    std::cerr << gettext("Warning: module ") << name << gettext(" did not set save_name") << std::endl;
  }

  string path = conf->p_var_data_dir() + "options/" + name;

  std::ifstream In(path.c_str());

  if(!In){
    print_critical(gettext("Could not open options file ") + path, "OPTIONS");
  } else {

    string name, value, str;
    int i;

    while (getline(In, str)) {

      string::size_type comma = str.find(",");

      name = str.substr(0,comma);
      value = str.substr(comma+1);

      foreach (Option *option, val)
	if (!option->type && option->english_name == name) {
	  i = 0;
	  foreach (string& english_value, option->english_values)
	    if (value == english_value) {
	      option->pos = i;
	      break;
	    } else
	      ++i;
	}
    }
  }
}
