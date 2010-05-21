#include "extra_menu.hpp"

#include "renderer.hpp"
#include "common.hpp"
#include "themes.hpp"
#include "graphics.hpp"
#include "config.hpp"
#include "global.hpp"
#include "updater.hpp"

#include "touch.hpp"

#include "boost.hpp"

#include "busy_indicator.hpp"
#include "shutdown.hpp"

ExtraMenuItem::ExtraMenuItem(const std::string c, const std::string& s, const callback_func& cb,
			     bool cleanup_after)
  : command(c), shortcut(s), cleanup_after_calling(cleanup_after), callback(cb)
{}

bool ExtraMenuItem::operator== (const ExtraMenuItem& rhs)
{
  return command == rhs.command;
}

void ExtraMenu::add_item(const ExtraMenuItem& e)
{
  items.push_back(std::make_pair(e, false));
}

void ExtraMenu::add_persistent_item(const ExtraMenuItem& e)
{
  items.push_back(std::make_pair(e, true));
}

ExtraMenu::ExtraMenu(const std::string& h, bool dd)
  : header(h), drop_down(dd), pos(0), o(Overlay("extra menu")), starting_layer(0)
{
  exit_inputs.push_back(Input("back", "general"));
  exit_inputs.push_back(Input("second_action", "general"));

  Config *conf = S_Config::get_instance();

  header_font = "Vera/" + conv::itos(resolution_dependant_font_size(20, conf->p_v_res()));
  element_font = "Vera/" + conv::itos(resolution_dependant_font_size(17, conf->p_v_res()));

  std::pair<int, int> element_size = string_format::calculate_string_size("abcltuwHPMjJg", element_font);
  element_height = element_size.second - 5;
}

void ExtraMenu::set_drop_down_pos(int x_pos, int y_pos)
{
  dd_x = x_pos;
  dd_y = y_pos;
}

void ExtraMenu::cleanup()
{
  Render *render = S_Render::get_instance();

  render->wait_and_aquire();
  o.cleanup();
  render->image_mut.leaveMutex();
}

void ExtraMenu::add_exit_input(const Input& input)
{
  exit_inputs.push_back(input);
}

int ExtraMenu::mainloop()
{
  InputMaster *im = S_InputMaster::get_instance();
  Global *global = S_Global::get_instance();
  Config *conf = S_Config::get_instance();
  ScreenUpdater *screen_updater = S_ScreenUpdater::get_instance();

  conf->s_sam(true);

  BusyIndicator *busy_indicator = S_BusyIndicator::get_instance();
  Shutdown *sd = S_Shutdown::get_instance();

  busy_indicator->idle();

  audio_fullscreen_was_running = true;

  if (!screen_updater->timer.status("audio_fullscreen"))
    audio_fullscreen_was_running = false;
  else
    // make sure audio fullscreen cleans up after itself
    screen_updater->timer.run_and_deactivate("audio_fullscreen");

  Input input;
  int _ret = -1;
  bool update_needed = true;

  while (true)
    {
      bool value_found = false;

      foreach (Input& exit_input, exit_inputs)
	if (exit_input == input) {
	  value_found = true;
	  break;
	}

      if (value_found)
	break;

      if (update_needed)
	print();

      input = im->get_input();

      if (sd->is_enabled()) {
      	sd->cancel();
      	continue;
      }

      update_needed = true;

      if (input.key == "touch_input") {
	if (S_Touch::get_instance()->run_callback()) {
	  _ret = pos;
	  break;
	}
      }

      if (input.command == "prev")
	{
	  if (pos != 0)
	    --pos;
	  else
	    pos = items.size()-1;
 	}
      else if (input.command == "next")
 	{
	  if (pos != items.size()-1)
	    ++pos;
	  else
	    pos = 0;
 	}
      else if (input.command == "action")
	{
	  wrapped_callback(vector_lookup(items, pos).first);

          return pos;
	}
      else
	update_needed = !global->check_commands(input, true);
    }

  cleanup();

  if (audio_fullscreen_was_running)
    screen_updater->timer.activate("audio_fullscreen");

  conf->s_sam(false);
  return _ret;
}

void ExtraMenu::wrapped_callback(const ExtraMenuItem& i)
{
  S_BusyIndicator::get_instance()->busy_no_print();

  if (!i.cleanup_after_calling)
    cleanup();

  if (audio_fullscreen_was_running)
    S_ScreenUpdater::get_instance()->timer.activate("audio_fullscreen");

  if (i.callback) {
    S_Global::get_instance()->set_playback_offset(0);
    i.callback();
  }

  if (i.cleanup_after_calling)
    cleanup();

  S_Config::get_instance()->s_sam(false);

  pos = 0;
  foreach (persistence_pair& pair, items)
    if (pair.first == i)
      break;
    else
      ++pos;
}

void ExtraMenu::print()
{
  Render *render = S_Render::get_instance();
  Themes *themes = S_Themes::get_instance();
  Config *conf = S_Config::get_instance();

  render->wait_and_aquire();

  S_Touch::get_instance()->clear();

  if (o.elements.size() > 0)
    o.partial_cleanup();

  std::pair<int, int> element_size = string_format::calculate_string_size("abcltuwHPMjJg", header_font);
  int header_height = element_size.second;

  int max_x = conf->p_h_res()-250, max_y = element_height;

  int nr_items = items.size();
  if (max_y*items.size()+30+header_height > conf->p_v_res()-50)
    nr_items = (conf->p_v_res()-50-30-header_height)/max_y;

  int box_height = max_y*nr_items+30+header_height;

  int x = (conf->p_h_res()-(max_x+30))/2;
  int y = (conf->p_v_res()-(box_height))/2;

  int start = pos - (nr_items/2);

  // special cases
  if (nr_items < items.size()) {
    if (pos >= items.size() - (nr_items/2)) // end
      start = items.size() - nr_items;
    else if ((nr_items/2) >= pos) // beginning
      start = 0;
  } else { // list smaller than nr_items
    start = 0;
  }

  if (drop_down) {
    x = dd_x;
    y = dd_y;
    box_height = max_y*items.size()+30;
    max_x = 0;
    foreach (persistence_pair& pair, items)
      max_x = std::max(max_x, string_format::calculate_string_width(pair.first.command, element_font));
  }

  o.add(new RObj(x-15+2, y-15+2, max_x+30+1, box_height+1, 0, 0, 0, 200, starting_layer));
  o.add(new RObj(x-15, y-15, max_x+30, box_height,
		 themes->dialog_background1, themes->dialog_background2,
		 themes->dialog_background3, themes->dialog_background4, starting_layer+1));

  int x_size;

  if (!header.empty()) {
    x_size = string_format::calculate_string_width(header, header_font);

    o.add(new TObj(header, header_font, (conf->p_h_res()-x_size)/2, y-5,
		   themes->dialog_font1, themes->dialog_font2, themes->dialog_font3, starting_layer+3));

    y += header_height;
  }

  if (pos == -1)
    o.add(new RObj(x-5, y-3, max_x+10, element_height+6, 0, 0, 0, 225, starting_layer+2));

  int i = 0;

  for (std::vector<std::pair<ExtraMenuItem, bool> >::const_iterator iter = items.begin()+start;
       iter != items.end() && i < nr_items; ++iter, ++i) {
    if (i+start == pos) {
      o.add(new PFObj(themes->general_marked_large, x-5, y,
		      max_x, element_height, starting_layer+2, true));
    }

    o.add(new TObj(iter->first.command, element_font, x, y-2, themes->dialog_font1,
		   themes->dialog_font2, themes->dialog_font3, starting_layer+3));

    //check if button png file exists in theme
    std::string buttonpath;
    buttonpath = render->default_path + "buttons/" + iter->first.shortcut + ".png";

    if (file_exists(buttonpath)) {
      //file exists, show png
      buttonpath = "buttons/" + iter->first.shortcut + ".png";
      x_size = 24;
      o.add(new PObj(buttonpath, x + max_x - x_size - 5, y, starting_layer+3, NOSCALING));
    }
    else {
      //png does not exists, show text
      x_size = string_format::calculate_string_width(iter->first.shortcut, element_font);
      o.add(new TObj(iter->first.shortcut, element_font, x + max_x - x_size - 5, y,
		     themes->dialog_font1, themes->dialog_font2,
		     themes->dialog_font3, starting_layer+3));
    }


    S_Touch::get_instance()->register_area(TouchArea(rect(x, y, max_x, element_height), 13, boost::bind(&ExtraMenu::wrapped_callback, this, iter->first)));

    y += element_height;
  }

  render->draw_and_release("extra menu screen", true);
}
