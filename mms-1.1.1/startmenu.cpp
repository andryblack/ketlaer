// used for #ifdef
#include "config.h"

#include "startmenu.hpp"

//Input
#include "input.hpp"

#include "gettext.hpp"
#include "renderer.hpp"
#include "config.hpp"
#include "graphics.hpp"
#include "updater.hpp"
#include "plugins.hpp"
#include "input.hpp"
#include "busy_indicator.hpp"
#include "shutdown.hpp"
#include "extra_menu.hpp"

#include "theme.hpp"
#include "search.hpp"
#include "resolution.hpp"

#include "touch.hpp"

#include "boost.hpp"

#ifdef use_inotify
#include "notify.hpp"
#endif

// ostringstream
#include <sstream>

#include <iostream>

using std::list;
using std::string;
using std::vector;

pthread_mutex_t Startmenu::singleton_mutex = PTHREAD_MUTEX_INITIALIZER;

startmenu_item::startmenu_item(const string& n, const string& a, const string& p, int i,
			       const callback_func& f)
  : name(n), action(a), pic(p), id(i), callback(f)
{
}

void Startmenu::StartmenuOpts::set_options()
{
  // header
  header = gettext("Options");

  // values
  vector<string> empty_values;

  // global options
  if (conf->p_cdpath().size() > 1 || go->audio_device_video->values.size() > 1 || go->audio_device_audio->values.size() > 1) {
    g_o = new Option(true, gettext("Global Options"), "Global Options", 0,
		     empty_values, empty_values);
    val.push_back(g_o);

    if (conf->p_cdpath().size() > 1)
      val.push_back(go->cd_device);

    if (go->audio_device_audio->values.size() > 1)
      val.push_back(go->audio_device_audio);

    if (go->audio_device_video->values.size() > 1)
      val.push_back(go->audio_device_video);
  }

  Plugins *plugins = S_Plugins::get_instance();
  foreach (FeaturePlugin *plugin, plugins->fp_data)
    if (plugin->module && plugin->module->get_opts()) {

      plugin->module->get_opts()->load();

      std::vector<Option*> plugin_values = plugin->module->get_opts()->values();

      std::vector<Option*> tmp_val;

      for (unsigned int i = 0; i < plugin_values.size(); ++i) {
	string name = plugin_values[i]->english_name;
        if (name != "reload" && name != "sorting" &&
	    name != "cd device" && name != "audio device (music)" && name != "audio device (movies)")
	  tmp_val.push_back(plugin_values[i]);
      }

      if (tmp_val.size() < 1)
	continue;

      val.push_back(new Option (true, plugin->module->get_opts()->get_header(),
				plugin->module->get_opts()->get_english_header(),
				0, empty_values, empty_values));

      for (unsigned int i = 0; i < tmp_val.size(); ++i) {
	  val.push_back(tmp_val[i]);
      }
    }

  load();
}

Startmenu::StartmenuOpts::~StartmenuOpts()
{
  // global
  delete g_o;
}

void Startmenu::StartmenuOpts::save()
{
  // intentionally left blank
}

void Startmenu::StartmenuOpts::load()
{
  // intentionally left blank
}

void Startmenu::print_lcd_menu(const startmenu_row& row, const startmenu_item& current_item)
{
  if (!global->lcd_output_possible())
    return;

  if (global->lcd_rows() > 2) global->lcd_add_output(string_format::pretty_pad_string("Startmenu",global->lcd_pixels(),' '));
  if (global->lcd_rows() > 3) global->lcd_add_output("");
  if (global->lcd_rows() > 1) global->lcd_add_output(string_format::pretty_print_string_copy(row.name + ":", global->lcd_pixels()));
  global->lcd_add_output(current_item.name);
  global->lcd_print();
}

void open_close_func()
{
  S_Cd::get_instance()->openclose_cddrive();
}

void switch_resolution()
{
  S_Render::get_instance()->device->switch_mode();
  S_ResolutionManagement::get_instance()->resolution_change();
}

string Startmenu::mainloop()
{
  InputMaster *input_master = S_InputMaster::get_instance();

  Input input;

  input_master->set_map("default");

  list<startmenu_row>::iterator startmenu_iter = startmenu.begin();
  list<startmenu_item>::iterator startmenu_iter_row = startmenu_iter->items.begin();

  BusyIndicator *busy_indicator = S_BusyIndicator::get_instance();
  Shutdown *sd = S_Shutdown::get_instance();

  Plugins *plugins = S_Plugins::get_instance();
  foreach (FeaturePlugin *plugin, plugins->fp_data)
    if (plugin->module != 0)
      plugin->module->startup_updater();

  if (conf->p_openclose()) {
    global->add_second_menu_item(ExtraMenuItem(gettext("Open/Close Tray"),
					       input_master->find_shortcut("eject"),
					       &open_close_func));
    global->add_command(GlobalCommand(&open_close_func, "eject", gettext("Open/Close Tray")));
  }

  opts.set_options();

  // updater
  ScreenUpdater *screen_updater = S_ScreenUpdater::get_instance();

//  screen_updater->start();
  screen_updater->activate();

  BackgroundUpdater *background_updater = S_BackgroundUpdater::get_instance();

//  background_updater->start();
  background_updater->activate();

#ifdef use_inotify
  S_Notify::get_instance()->start();
#endif

  // autostart plugin
  if (!conf->p_autostart().empty()) {
    S_Global::get_instance()->autostarting = false;
    unsigned int p_button = 0;
    string p_name = conf->p_autostart();
    size_t pos = p_name.find(":");
    if (pos) {
      p_button = conv::atoi(p_name.substr(pos + 1));
      p_name = p_name.substr(0, pos);
    }
    foreach (startmenu_row& row, startmenu) {
      if (row.name == p_name) {
        print_debug("Autostarting plugin " + row.name);
        list<startmenu_item>::iterator as_item = row.items.begin();
        std::advance(as_item, p_button < row.items.size() ? p_button : row.items.size() - 1);
        as_item->callback();
	input_master->set_map("default"); /* restores input map */
      }
    }
  }

  bool update_needed = true;
  while (!exit_loop)
    {
      if (update_needed) {
	print(*startmenu_iter, *startmenu_iter_row);
	print_lcd_menu(*startmenu_iter, *startmenu_iter_row);
      }

      input = input_master->get_input_busy_wrapped(busy_indicator);

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
 	  if (startmenu_iter != startmenu.begin()) {
	    --startmenu_iter;
	    startmenu_iter_row = startmenu_iter->items.begin();
	  }
 	}
      else if (input.command == "next")
 	{
 	  list<startmenu_row>::iterator j = startmenu.end(); --j;
 	  if (startmenu_iter != j) {
 	    ++startmenu_iter;
	    startmenu_iter_row = startmenu_iter->items.begin();
	  }
 	}
      else if (input.command == "left")
 	{
 	  if (startmenu_iter_row != startmenu_iter->items.begin())
 	    --startmenu_iter_row;
 	}
      else if (input.command == "right")
 	{
 	  list<startmenu_item>::iterator j = startmenu_iter->items.end(); --j;
 	  if (startmenu_iter_row != j)
 	    ++startmenu_iter_row;
 	}
      else if (input.command == "page_up")
 	{
	  int jump = conf->p_jump();

	  while (jump-- > 0) {
	    if (startmenu_iter != startmenu.begin()) {
	      --startmenu_iter;
	      startmenu_iter_row = startmenu_iter->items.begin();
	    } else
	      break;
	  }
 	}
      else if (input.command == "page_down")
 	{
 	  list<startmenu_row>::iterator j = startmenu.end(); --j;
	  int jump = conf->p_jump();

	  while (jump-- > 0) {
	    if (startmenu_iter != j) {
	      ++startmenu_iter;
	      startmenu_iter_row = startmenu_iter->items.begin();
	    } else
	      break;
	  }
 	}
      else if (input.command == "second_action")
	{
	  ExtraMenu em;

	  foreach (ExtraMenuItem& item, global->menu_items)
	    em.add_persistent_item(item);

	  em.mainloop();
	}
      else if (input.command == "action" && startmenu_iter_row->action != "quit")
 	{
	  print_debug("calling feature plugin mainloop");
	  foreach (startmenu_row& row, startmenu)
	    foreach (startmenu_item& item, row.items) {
	      if (item.action == startmenu_iter_row->action) {
		print_debug("Enter plugin action " + row.name);
		item.callback();

		// set the input map back to startmenu
		input_master->set_map("default");
		break;
	    }
	  }
	}
      else if (input.command == "action" && startmenu_iter_row->action == "quit")
	{
	  quit_func();
	}
      else
	// FIXME: ?
	// make back work for playlist commands
	//input_master->cur_map = &input_master->standard;
	update_needed = !global->check_commands(input);
    }
  return "quit";
}

void Startmenu::print(const startmenu_row& row, const startmenu_item& current_item)
{
  Render *render = S_Render::get_instance();
  Themes *themes = S_Themes::get_instance();

  render->prepare_new_image();

#if 0
  render->current.add(new PObj(themes->startmenu_background, 0, 0, 0, SCALE_FULL));
#else
  render->current.add(new BgRObj(0));
#endif

  int pos = 0;
  foreach (startmenu_row& cur_row, startmenu) {
    if (cur_row.name == row.name)
      break;
    ++pos;
  }

  int y_top = 120;
  int y_bottom = global->getBottomHeight();
  int x = 0;

  if (conf->p_h_res() == 720)
    y_top = 40;

  int max_y = element_height + 5 + 5;

  int nr_items = startmenu.size();
  if (max_y*startmenu.size()+y_top+y_bottom > conf->p_v_res())
    nr_items = (conf->p_v_res()-y_top-y_bottom)/max_y;

  int box_height = max_y*nr_items;

  int y = (conf->p_v_res()-(box_height))/2;

  int start = pos - (nr_items/2);

  // special cases
  if (nr_items < startmenu.size()) {
    if (pos >= startmenu.size() - (nr_items/2)) // end
      start = startmenu.size() - nr_items;
    else if ((nr_items/2) >= pos) // beginning
      start = 0;
  } else { // list smaller than nr_items
    start = 0;
  }

  int i = 0;
  list<startmenu_row>::const_iterator iter = startmenu.begin();
  for (int t = 0; t < start; ++t)
    ++iter;
  for (; iter != startmenu.end() && i < nr_items; ++iter, ++i)
    {
      if (iter->name == row.name) {

	foreach (const startmenu_item& item, iter->items) {

	  if (current_item.name == item.name) {

	    render->current.add(new PFObj(themes->startmenu_marked_icon, static_cast<int>(conf->p_h_res()-(3*(max_y+20))+x-1/5.0*max_y), y, static_cast<int>(7/5.0*max_y), max_y, 3, true));


	    render->current.add(new PFObj(themes->general_marked, 65, y,
					  conf->p_h_res()-(3*(max_y+20))+x-65+max_y+7, max_y, 1, true));

	    render->current.add(new TObj(item.name, element_font, 75, y+5,
					 themes->startmenu_font1, themes->startmenu_font2,
					 themes->startmenu_font3, 2));
	  }

	  PObj *p = new PFObj(item.pic, conf->p_h_res()-(3*(max_y+20))+x, y, max_y, max_y, 2, true);

	  render->current.add(p);
	  S_Touch::get_instance()->register_area(TouchArea(rect(p->x, p->y, p->w, p->h), p->layer, item.callback));
	  x += max_y + 20;
	}
      } else {

	render->current.add(new TObj(iter->name, element_font, 75, y+5,
				     themes->startmenu_font1, themes->startmenu_font2,
				     themes->startmenu_font3, 2));

	foreach (const startmenu_item& item, iter->items) {
	  PObj *p = new PFObj(item.pic, conf->p_h_res()-(3*(max_y+20))+x, y, max_y, max_y, 2, true);
	  render->current.add(p);
	  S_Touch::get_instance()->register_area(TouchArea(rect(p->x, p->y, p->w, p->h), p->layer, item.callback));
	  x += max_y+20;
	}
      }
      x = 0;
      y += max_y;
    }

#ifndef use_notify_area
  // render the logo in the startmenu
  render->current.add(new PObj("logo.png", conf->p_h_res()-85, conf->p_v_res()-41, 2, NOSCALING));
#endif

  render->draw_and_release("startmenu");
}

Startmenu::Startmenu()
  : exit_loop(false)
{
  generate_startmenu();
  conf = S_Config::get_instance();
  global = S_Global::get_instance();

  res_dependant_calc();

  S_ResolutionManagement::get_instance()->register_callback(boost::bind(&Startmenu::res_dependant_calc, this));
}

void Startmenu::res_dependant_calc()
{
  element_font = "Vera/" + conv::itos(resolution_dependant_font_size(22, conf->p_v_res()));
  std::pair<int, int> element_size = string_format::calculate_string_size("abcltuwHPMjJg", element_font);

  element_height = element_size.second;
}

void search_func()
{
  S_Search::get_instance()->mainloop();
}

void options_func()
{
  S_BusyIndicator::get_instance()->idle();
  S_Startmenu::get_instance()->opts.mainloop();
}

void themes_func()
{
  S_BusyIndicator::get_instance()->idle();
  S_Theme::get_instance()->mainloop();
}

void Startmenu::quit_func()
{
  ExtraMenu em(gettext("Do you really want to exit?"));

  if (conf->p_show_quit())
    em.add_item(ExtraMenuItem(gettext("Quit"), "", NULL));
  if (!conf->p_shutdown_script().empty())
    em.add_item(ExtraMenuItem(gettext("Halt System"), "", NULL));
  em.add_item(ExtraMenuItem(gettext("Cancel"), "", NULL));

  int ret = em.mainloop();

  if (ret == 0 && conf->p_show_quit()) {
    conf->s_shutdown_script("");
    exit_loop = true;
  } else if (!conf->p_shutdown_script().empty() && (ret == 1 && conf->p_show_quit() || ret == 0 && !conf->p_show_quit()))
    exit_loop = true;
}

void Startmenu::generate_startmenu()
{
  Themes *themes = S_Themes::get_instance();
  Config *conf = S_Config::get_instance();

  // generate menues.
  list<startmenu_row> start_menu;
  startmenu_row row;

/*
  Category stored in plugin_name() is what menu displays when unselected
  ie Audio, Video, Midi, Picture, EPG, TV, NET...
  priority was for column dynamic allocation (0..2) (unused)
*/

  Plugins *plugins = S_Plugins::get_instance();
  foreach (FeaturePlugin *plugin, plugins->fp_data) {
    if (plugin->module != 0 && plugin->module->init() && plugin->items().size() > 0) {
      print_debug("Registering plugin features for " + plugin->plugin_name());
      row.name = plugin->plugin_name();
      row.items = plugin->items();
      start_menu.push_back(row);
    }
  }

  row.items.clear();

  // search button
  row.items.push_back(startmenu_item(gettext("Search"), "search",
				     themes->startmenu_search, 0, &search_func));

  // options button
  row.items.push_back(startmenu_item(gettext("Options"), "options",
				     themes->startmenu_options, 0, &options_func));

  // themes button
  row.items.push_back(startmenu_item(gettext("Themes"), "themes",
				     themes->startmenu_themes, 1, &themes_func));

  if (row.items.size() != 0) {
    row.name = gettext("Utilities");
    start_menu.push_back(row);
    row.items.clear();
  }

#ifdef use_res_switch
  if (conf->p_alternative_h_res() != 0 && conf->p_alternative_v_res() != 0) {
    row.items.push_back(startmenu_item(gettext("Switch resolution"), "switchresolution",
				       "startmenu/switch_resolution.png", 0, &switch_resolution));
    row.name = gettext("Switch resolution");
    start_menu.push_back(row);
    row.items.clear();
  }
#endif

  // Open/Close Tray button
  if (conf->p_openclose()) {
    row.items.push_back(startmenu_item(gettext("Open/Close Tray"), "openclose",
				       themes->startmenu_open_close, 0, &open_close_func));

    row.name = gettext("Open/Close Tray");
    start_menu.push_back(row);
    row.items.clear();
  }

  if (conf->p_show_quit() || !conf->p_shutdown_script().empty()) {
    // quit button
    row.name = gettext("Quit");
    row.items.push_back(startmenu_item(gettext("Quit"), "quit", themes->startmenu_quit, 0,
				       boost::bind(&Startmenu::quit_func, this)));
    start_menu.push_back(row);
    row.items.clear();
  }

  startmenu = start_menu;
}
