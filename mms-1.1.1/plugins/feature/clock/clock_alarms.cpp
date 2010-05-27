#include "clock.hpp"

#include "image.hpp"
#include "graphics.hpp"
#include "shutdown.hpp"
#include "busy_indicator.hpp"


using std::vector;
using std::string;

void Clock::alarms_mainloop()
{

  BusyIndicator *busy_indicator = S_BusyIndicator::get_instance();
  InputMaster *input_master  = S_InputMaster::get_instance();

  input_master->save_map();
  input_master->set_map("alarm");

  Shutdown *sd = S_Shutdown::get_instance();

  Input input;

  alarm_pos = 0;

  Alarm cur_alarm;

  bool update_needed = true;

  render->device->animation_section_begin();
  int curlayer = render->device->get_current_layer();
  render->device->switch_to_layer(curlayer+1);
  render->device->layer_active_no_wait(true);
  render->device->reset_layout_attribs_nowait();
  render->device->animation_move( 0, -conf->p_v_res(), 0, 0, 60, curlayer+1);
  render->device->animation_move(0, 0, 0, conf->p_v_res(), 60, curlayer);
  render->device->animation_fade(0,1,100,curlayer+1);
  render->device->animation_fade(1,0,100,curlayer);
  render->device->animation_section_end();


  while (!exit_edit_alarms_loop)
    {
      if (update_needed) {
	if (alarms.size() > 0 && alarm_pos >= 0) 
          cur_alarm = vector_lookup(alarms, alarm_pos);

	print_edit_alarm(alarms, cur_alarm, alarm_pos);
      }

      std::cout << alarm_pos << std::endl;

      input = input_master->get_input_busy_wrapped(busy_indicator);

      if (sd->is_enabled()) {
      	sd->cancel();
      	continue;
      }

      if (input.key == "touch_input") {
	S_Touch::get_instance()->run_callback();
	continue;
      }

      update_needed = true;

      if (input.command == "back" && input.mode == "general")
	{
	  exit_edit_alarms();
	}
      else if (input.command == "prev")
 	{
 	  if (alarm_pos != 0)
	    --alarm_pos;
	  else
	    alarm_pos = alarms.size()-1;
 	}
      else if (input.command == "next")
 	{
 	  if (alarm_pos != alarms.size()-1)
	    ++alarm_pos;
	  else
	    alarm_pos = 0;
 	}
      else if (input.command == "page_up")
 	{
	  if (alarms.size() > conf->p_jump()) {
	    int new_pos = alarm_pos-conf->p_jump();
	    if (alarm_pos == 0)
	      alarm_pos = alarms.size()-1+new_pos;
	    else if (new_pos < 0)
	      alarm_pos = 0;
	    else
	      alarm_pos = new_pos;
	  }
 	}
      else if (input.command == "page_down")
 	{
	  if (alarms.size() > conf->p_jump()) {
	    if (alarm_pos > (alarms.size() - conf->p_jump()) && alarm_pos != (alarms.size()-1))
	      alarm_pos = alarms.size()-1;
	    else
	      alarm_pos = (alarm_pos+conf->p_jump()) % alarms.size();
	  }
 	}
      else if (input.command == "delete")
	{
	  delete_alarm(alarm_pos);
	  find_next_alarm();
          activate_alarm();
	  save_alarms();
	}
      else if (input.command == "add")
	{
          Alarm na;
          if (add_alarm_mainloop(na)) {
 	    add_alarm(na);
            find_next_alarm();
            activate_alarm();
            save_alarms();
          }
	}
      else if (input.command == "action")
        {
	  if (alarm_pos + 1 <= alarms.size()) {
	    Alarm na = vector_lookup(alarms, alarm_pos);
	    if (add_alarm_mainloop(na)) {
	      delete_alarm(alarm_pos);
	      add_alarm(na);
	      find_next_alarm();
	      activate_alarm();
	      save_alarms();
	    }
	  }
        }
      else
	update_needed = !global->check_commands(input);
    }

  exit_edit_alarms_loop = false;
  input_master->restore_map();

  render->device->animation_section_begin();
  render->device->switch_to_layer(curlayer);
  //render->device->make_actions_expire(curlayer);
  render->device->animation_move(0, 0, 0, -conf->p_v_res(),  60, curlayer+1);
  render->device->animation_move(0, conf->p_v_res(), 0, 0, 60, curlayer);
  render->device->animation_fade(1,0,100,curlayer+1);
  render->device->animation_fade(0,1,100,curlayer);
  render->device->animation_section_end();

  return;
}

bool Clock::add_alarm_mainloop(Alarm &alarm)
{
  InputMaster *im = S_InputMaster::get_instance();
  Global *global = S_Global::get_instance();
  Config *conf = S_Config::get_instance();
  ScreenUpdater *screen_updater = S_ScreenUpdater::get_instance();

  BusyIndicator *busy_indicator = S_BusyIndicator::get_instance();
  Shutdown *sd = S_Shutdown::get_instance();


  busy_indicator->idle();

  dow = alarm.dayWeek;
  hour = alarm.time.getHour();
  min = alarm.time.getMinute();

  bool handle_min = false;

  bool audio_fullscreen_was_running = true;

  if (!screen_updater->timer.status("audio_fullscreen"))
    audio_fullscreen_was_running = false;
  else
    // make sure audio fullscreen cleans up after itself
    screen_updater->timer.run_and_deactivate("audio_fullscreen");

  Input input;
  bool update_needed = true;
  bool exit_loop = false;
  bool added_alarm = false;
  while (!exit_loop)
    {

      if (update_needed)
	print_add_alarm();

      input = im->get_input();

      if (sd->is_enabled()) {
      	sd->cancel();
      	continue;
      }

      update_needed = true;

      if (input.key == "touch_input") {
	if (S_Touch::get_instance()->run_callback()) {
	  break;
	}
      }

      if (input.command == "page_up")
	{
	  if (dow != 0)
	    --dow;
	  else
	    dow = 6;
 	}
      else if (input.command == "page_down")
 	{
	  if (dow != 6)
	    ++dow;
	  else
	    dow = 0;
 	}
      else if (input.command == "left" || input.command == "right")
	handle_min = (handle_min ? false : true);
      else if (input.command == "prev")
        {
           if (!handle_min) {
            if (hour != 23)
	      ++hour;
	    else
	      hour = 0;
          } else {
            if (min != 59)
	      ++min;
	    else
	      min = 0;
          }
        } 
      else if (input.command == "next")
	{
           if (!handle_min) {
            if (hour != 0)
	      --hour;
	    else
	      hour = 23;
          } else {
            if (min != 0)
	      --min;
	    else
	      min = 59;
          }
	}
      else if (input.command == "action")
	{
	  alarm = Alarm(dow, hour, min);
          added_alarm = true;
          exit_loop = true;
	}
      else if (input.command == "back")
	{
          exit_loop = true;
	}
      else
	update_needed = !global->check_commands(input, true);
    }

  cleanup();

  if (audio_fullscreen_was_running)
    screen_updater->timer.activate("audio_fullscreen");

  return added_alarm;
}

void Clock::cleanup()
{
  Render *render = S_Render::get_instance();

  render->wait_and_aquire();
  o.cleanup();
  render->image_mut.leaveMutex();
}

void Clock::print_add_alarm()
{
  Render *render = S_Render::get_instance();
  Themes *themes = S_Themes::get_instance();
  Config *conf = S_Config::get_instance();


  render->wait_and_aquire();
  S_Touch::get_instance()->clear();

  if (o.elements.size() > 0)
  o.partial_cleanup();

   int max_x = conf->p_h_res()-250, max_y = 30;
   int box_height = max_y*2+30+35;

   int x = (conf->p_h_res()-(max_x+30))/2;
   int y = (conf->p_v_res()-(box_height))/2;

  o.add(new RObj(x-15+2, y-15+2, max_x+30+1, box_height+1, 0, 0, 0, 200, 0));
  o.add(new RObj(x-15, y-15, max_x+30, box_height,
		 themes->dialog_background1, themes->dialog_background2,
		 themes->dialog_background3, themes->dialog_background4, 1));

  int x_size;
  string header = dgettext("mms-clock", "Add Alarm");
  x_size = string_format::calculate_string_width(header, "Vera/20");

    o.add(new TObj(header, "Vera/20", (conf->p_h_res()-x_size)/2, y-5,
		   themes->dialog_font1, themes->dialog_font2, themes->dialog_font3, 3));
    y += 35;

    o.add(new TObj(dgettext("mms-clock","Day: ")+dayOfWeek(dow), "Vera/17", x, y, themes->dialog_font1,
		   themes->dialog_font2, themes->dialog_font3, 3));
    y += 30;
    o.add(new TObj(dgettext("mms-clock","Time: ") + doubleZero(hour) + ":"+ doubleZero(min), "Vera/17", x, y, themes->dialog_font1,
		   themes->dialog_font2, themes->dialog_font3, 3));


  render->draw_and_release("add alarm screen", true);

}

void Clock::print_edit_alarm(const std::vector<Alarm>& cur_alarm, const Alarm& position, const int int_position)
{
  string header_font = "Vera/" + conv::itos(resolution_dependant_font_size(28, conf->p_v_res()));

  render->prepare_new_image();

#if 0
  render->current.add(new PObj(themes->options_background, 0, 0, 0, SCALE_FULL));
#else
  render->current.add(new BgRObj(0));
#endif

  std::pair<int, int> header_size = string_format::calculate_string_size("abcltuwHPMjJg", header_font);
  int header_box_size = static_cast<int>(header_size.second * 0.75);

  PObj *back = new PFObj(themes->startmenu_options, 25, 10, header_box_size, header_box_size, 2, true);

  render->current.add(back);

  S_Touch::get_instance()->register_area(TouchArea(rect(back->x, back->y, back->w, back->h), back->layer,
						   boost::bind(&Clock::exit_edit_alarms, this)));

  TObj *t = new TObj(dgettext("mms-clock", "Alarms"), header_font, 25 + header_box_size + 10, 10 + (header_box_size - header_size.second)/2,
		     themes->options_header_font1, themes->options_header_font2,
		     themes->options_header_font3, 2);

  render->current.add(t);

  print_range<Alarm>(cur_alarm, position, int_position, boost::bind(&Clock::print_edit_alarm_string_element, this, _1, _2, _3), list_size.second - 5);

  render->draw_and_release("Alarm Edit");
}


void Clock::print_edit_alarm_string_element(const Alarm& r, const Alarm& position, int y)
{
  string name = dayOfWeek(r.dayWeek) + " " +  doubleZero(r.time.getHour()) + ":" + doubleZero(r.time.getMinute());
  string_format::format_to_size(name, "Vera/16", conf->p_h_res()-160, true);

  int width = conf->p_h_res()-(2*67);
  if (conf->p_h_res() == 720)
    width -= 5;

  if (r == position)
    render->current.add(new PFObj(themes->general_marked_large, 67, y + 3,
				  width, list_size.second* 0.85, 1, true));

  render->current.add(new TObj(name, list_font, 75, y, themes->options_font1, themes->options_font2, themes->options_font3, 3));
}

void Clock::exit_edit_alarms()
{
  exit_edit_alarms_loop = true;
}
