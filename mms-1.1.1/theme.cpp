// used for #ifdef
#include "config.h"

#include "libfspp.hpp"

#include "theme.hpp"

#include "common.hpp"
#include "config.hpp"
#include "startmenu.hpp"
#include "gettext.hpp"

#include "graphics.hpp"
#include "shutdown.hpp"
#include "touch.hpp"
#include "resolution.hpp"

#include <sstream>
#include <fstream>

using std::string;
using std::vector;

pthread_mutex_t Theme::singleton_mutex = PTHREAD_MUTEX_INITIALIZER;

Theme::Theme()
  : pos(0), exit_loop(false), cur_theme("")
{
  load();

  res_dependant_calc();

  S_ResolutionManagement::get_instance()->register_callback(boost::bind(&Theme::res_dependant_calc, this));
}

void Theme::res_dependant_calc()
{
  header_font = "Vera/" + conv::itos(resolution_dependant_font_size(28, conf->p_v_res()));
  element_font = "Vera/" + conv::itos(resolution_dependant_font_size(17, conf->p_v_res()));
  position_font = "Vera/" + conv::itos(resolution_dependant_font_size(18, conf->p_v_res()));

  std::pair<int, int> element_size = string_format::calculate_string_size("abcltuwHPMjJg", element_font);
  element_height = element_size.second - 5;
}

void Theme::startup_updater()
{}

vector<string> Theme::list_themes()
{
  vector<string> files;

  string argv = mms_prefix"/share/mms/themes/";

  if (conf->p_homedir() != "/etc/mms/" && file_exists(conf->p_homedir() + "themes"))
    argv = conf->p_homedir() + "themes/";

  for (file_iterator<file_t, default_order> i (argv); (i != i.end ()); i.advance(false))
    {
      string filename = i->getName();
      files.push_back(filename.substr(argv.size()+1));
    }

  return files;
}

string Theme::mainloop()
{
  Input input;

  list_of_themes = list_themes();

  bool update_needed = true;

  Shutdown *sd = S_Shutdown::get_instance();

  render->device->animation_section_begin();
  int curlayer = render->device->get_current_layer();
  render->device->switch_to_layer(curlayer+1);
  render->device->reset_layout_attribs_nowait();
  render->device->animation_fade(0,1,40,1);
  render->device->animation_section_end();

  while (!exit_loop)
    {
      if (update_needed)
	print();

      input = input_master->get_input();

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
 	  if (pos != 0)
	    --pos;
	  else
	    pos = list_of_themes.size()-1;
 	}
      else if (input.command == "next")
 	{
 	  if (pos != list_of_themes.size()-1)
	    ++pos;
	  else
	    pos = 0;
 	}
      else if (input.command == "action")
	{
	  use_theme();
	}
      else if (input.command == "second_action")
	{
	  ExtraMenu em;

	  foreach (ExtraMenuItem& item, global->menu_items)
	    em.add_persistent_item(item);

	  em.add_item(ExtraMenuItem(gettext("Use theme"), input_master->find_shortcut("action"),
				    boost::bind(&Theme::use_theme, this)));

	  em.add_item(ExtraMenuItem(gettext("Go back"), input_master->find_shortcut("back"),
				    boost::bind(&Theme::exit, this)));

	  em.mainloop();
	}
      else if (input.command == "back" && input.mode == "general")
	{
	  exit();
	}
      else
	update_needed = !global->check_commands(input);
    }

  exit_loop = false;
  render->device->animation_section_begin();
  render->device->layer_active_no_wait(false);
  render->device->switch_to_layer(curlayer);
  render->device->animation_fade(0,1,80,curlayer);
  render->device->animation_section_end();

  return "";
}

void Theme::find_theme_and_use(const string& theme_name)
{
  int new_pos = 0;
  foreach (string& theme, list_of_themes)
    if (theme == theme_name) {
      pos = new_pos;
      break;
    } else
      ++new_pos;

  use_theme();
}

void Theme::use_theme()
{
  if (themes->parser(vector_lookup(list_of_themes, pos), false)) {
    cur_theme = vector_lookup(list_of_themes, pos);
    S_Startmenu::get_instance()->generate_startmenu();
    render->set_theme();

    render->image_mut.enterMutex();
    render->complete_redraw();
    render->image_mut.leaveMutex();
  }
}

void Theme::exit()
{
  exit_loop = true;
}

void Theme::print_string_element(const string& r, const string& position, int y)
{
  PObj *marked = new PFObj(themes->general_marked, 68, y,
			   static_cast<int>((conf->p_h_res()-(2*68))*2.0/3.0)-10, element_height, 1, true);

  if (r == position)
    render->current.add(marked);

  S_Touch::get_instance()->register_area(TouchArea(rect(marked->x, marked->y,
							marked->w, marked->h), marked->layer,
						   boost::bind(&Theme::find_theme_and_use, this, r)));

  if (r == cur_theme)
    render->current.add(new TObj(r, element_font, 72, y-3,
				 themes->themes_marked_font1, themes->themes_marked_font2,
				 themes->themes_marked_font3, 3));
  else
    render->current.add(new TObj(r, element_font, 72, y-3,
				 themes->themes_font1, themes->themes_font2,
				 themes->themes_font3, 3));

  int max_y_pic = std::min(static_cast<int>((conf->p_h_res()-(2*68))*1.0/3.0), conf->p_v_res() - 200 - 100);
  int max_x_pic = max_y_pic;

  std::ostringstream cur_theme;
  cur_theme << mms_prefix << "/share/mms/themes/"
	    << r << "/thumbnail.png";

  string picture_path = cur_theme.str();

  if (r == position && file_exists(picture_path)) {
    if (!file_exists(render->image_get_fullpath(picture_path, max_x_pic, max_y_pic))) {
      render->create_scaled_image_wrapper(picture_path, max_x_pic, max_y_pic);
      // to insert the image into the transmap so that it won't fail when
      // we draw the screen again
      bool tmp;
      render->image_dimensions(render->image_get_fullpath(picture_path, max_x_pic, max_y_pic), &tmp);
    }

    y = 105;

    if (conf->p_v_res() == 405)
      y = 85;

    render->current.add(new PFObj(picture_path, 68+static_cast<int>((conf->p_h_res()-(2*68))*2.0/3.0),
				  y, max_x_pic, max_y_pic, false, 3));
  }
}

void Theme::print()
{
  render->prepare_new_image();

  render->current.add(new PObj(themes->themes_background, 0, 0, 0, SCALE_FULL));

  std::pair<int, int> header_size = string_format::calculate_string_size("abcltuwHPMjJg", header_font);
  int header_box_size = static_cast<int>(header_size.second * 0.75);

  PObj *back = new PFObj(themes->startmenu_themes, 25, 10, header_box_size, header_box_size, 2, true);

  render->current.add(back);

  S_Touch::get_instance()->register_area(TouchArea(rect(back->x, back->y, back->w, back->h),
						   back->layer, boost::bind(&Theme::exit, this)));

  if (themes->show_header) {
    TObj *t = new TObj(gettext("Themes"), header_font, 25 + header_box_size + 10, 10 + (header_box_size - header_size.second)/2,
		       themes->themes_header_font1, themes->themes_header_font2,
		       themes->themes_header_font3, 2);

    render->current.add(t);

    S_Touch::get_instance()->register_area(TouchArea(rect(t->x, t->y, t->w, t->h),
						     t->layer, global->toggle_playlist));
  }

  std::ostringstream position;
  position << pos+1 << "/" << list_of_themes.size();

  int x = string_format::calculate_string_width(position.str(), position_font);

  render->current.add(new TObj(position.str(), position_font, conf->p_h_res()-(60+x), 20,
			       themes->themes_font1, themes->themes_font2,
			       themes->themes_font3, 2));

  print_range<string>(list_of_themes, vector_lookup(list_of_themes, pos), pos,
		      boost::bind(&Theme::print_string_element, this, _1, _2, _3), element_height);

  render->draw_and_release("themes");
}

void Theme::save()
{
  std::ofstream file;

  string path = conf->p_var_data_dir() + "options/ThemeRuntime";

  file.open(path.c_str());

  if (!file) {
    print_warning(gettext("Could not write options to file ") + path, "THEME");
  } else {
    file << "cur_theme," << cur_theme << std::endl;
  }

  file.close();
}

void Theme::load()
{
  // set the cur_theme to something sane as default in case the other things go wrong.
  cur_theme = "standard";

  string path = conf->p_var_data_dir() + "options/ThemeRuntime";

  std::ifstream in;
  in.open(path.c_str(), std::ios::in);

  if (!in.is_open()) {
    print_critical(gettext("Could not open options file ") + path, "THEME");
  } else {

    string name, value, str;

    while (getline(in, str)) {

      string::size_type comma = str.find(",");

      if (comma == string::npos)
	continue;

      name = str.substr(0,comma);
      value = str.substr(comma+1);

      if (name == "cur_theme")
 	cur_theme = value;
    }
    in.close();
  }

  bool loaded_ok = true;

  if (!themes->parser(cur_theme, true)) {
    loaded_ok = false;
    if (cur_theme != "standard" && themes->parser("standard", true)) {
      print_info(gettext("Falling back to standard theme"), "THEMES");
      cur_theme = "standard";
      loaded_ok = true;
    }
  }

  if (!loaded_ok) {
    print_critical(gettext("Couldn't find any themes to open"), "THEMES");
    ::exit(1);
  }
}
