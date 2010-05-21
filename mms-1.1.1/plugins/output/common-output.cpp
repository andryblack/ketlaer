#include "renderer.hpp"
#include "themes.hpp"
#include "graphics.hpp"
#include "updater.hpp"
#include "common-feature.hpp"

#include "boost.hpp"

#include <sstream>
#include <cmath>

using std::string;
using std::list;

void print_dialog_step1(const list<string>& messages, Overlay &o)
{
  print_dialog_time_wait(messages, o, 0);
}

void print_dialog_step2(Overlay &o)
{
  Render *render = S_Render::get_instance();

  render->wait_and_aquire();
  o.cleanup();
  render->image_mut.leaveMutex();
}

void print_dialog(const list<string>& messages)
{
  Overlay overlay = Overlay("dialog");

  print_dialog_step1(messages, overlay);

  // wait for keypress
  S_InputMaster::get_instance()->get_input();

  print_dialog_step2(overlay);
}

void print_dialog_time_wait(const list<string>& messages, Overlay& overlay, int time)
{
  Render *render = S_Render::get_instance();
  Themes *themes = S_Themes::get_instance();
  Config *conf = S_Config::get_instance();

  if ((messages.size() * 20) > conf->p_v_res())
    print_warning(gettext("Too many strings to print"), "PrintDialog");
  else if (messages.size() == 0)
    print_warning(gettext("Nothing to print"), "PrintDialog");

  int max_x = 0, max_y = 0;

  string message_font = "Vera/" + conv::itos(resolution_dependant_font_size(17, conf->p_v_res()));

  // determine size
  foreach (const string& message, messages) {
    std::pair<int, int> dimensions = string_format::calculate_string_size(message, message_font);
    max_x = std::max(dimensions.first, max_x);
    max_y = std::max(dimensions.second, max_y);
  }

  int x = (conf->p_h_res()-(max_x+30))/2;
  int y = (conf->p_v_res()-(max_y*messages.size()+30))/2;

  render->wait_and_aquire();

  overlay.add(new RObj(x-15+2, y-15+2, max_x+30+1, max_y*messages.size()+30+1,
		       0, 0, 0, 225, 4));
  overlay.add(new RObj(x-15, y-15, max_x+30, max_y*messages.size()+30,
		       themes->dialog_background1, themes->dialog_background2,
		       themes->dialog_background3, themes->dialog_background4, 5));

  foreach (const string& message, messages) {
    overlay.add(new TObj(message, message_font, x, y, themes->dialog_font1,
			 themes->dialog_font2, themes->dialog_font3, 6));

    y += max_y;
  }

  render->draw_and_release("dialog", true);

  if (overlay.name == "dialog default") {

    if (time != 0) {
      mmsUsleep(time/2*1000);

      render->image_mut.enterMutex();
      overlay.cleanup();

      mmsUsleep(time/2*1000);
    }

    else {
      Input input;
      InputMaster *im = S_InputMaster::get_instance();
      while (!(input.command == "back" )){
	mmsUsleep(200000);
	input = im->get_input();
      }

      render->image_mut.enterMutex();
      overlay.cleanup();
      mmsUsleep(200000);
    }

    render->image_mut.leaveMutex();
  }
}

void print_dialog_wait(const list<string>& messages, int time)
{
  Overlay dialog = Overlay("dialog default");
  print_dialog_time_wait(messages, dialog, time);
}

void print_dialog_fixed_size(const string& message, int max_horizontal_length, Overlay& dialog)
{
  Render *render = S_Render::get_instance();
  Themes *themes = S_Themes::get_instance();
  Config *conf = S_Config::get_instance();

  render->wait_and_aquire();

  if (dialog.elements.size() > 0)
    dialog.partial_cleanup();

  int x = (conf->p_h_res()-max_horizontal_length)/2+15;
  int y = (conf->p_v_res()-20)/2+15;

  int max_x = max_horizontal_length;
  int max_y = 20;

  dialog.add(new RObj(x-15+2, y-15+2, max_x+30+1, max_y+30+1,
		      0, 0, 0, 200, 3));
  dialog.add(new RObj(x-15, y-15, max_x+30, max_y+30,
		      themes->dialog_background1, themes->dialog_background2,
		      themes->dialog_background3, themes->dialog_background4, 4));

  string tmp_message = message;

  string message_font = "Vera/" + conv::itos(resolution_dependant_font_size(17, conf->p_v_res()));

  string_format::format_to_size(tmp_message, message_font, max_horizontal_length, true);

  dialog.add(new TObj(tmp_message, message_font, x, y-5, themes->dialog_font1,
		      themes->dialog_font2, themes->dialog_font3, 5));

  render->draw_and_release("fixed dialog", true);
}

void print_progressbar()
{
  Config *conf = S_Config::get_instance();
  Render *render = S_Render::get_instance();
  Themes *themes = S_Themes::get_instance();
  ScreenUpdater *screen_updater = S_ScreenUpdater::get_instance();

  render->wait_and_aquire();

  string message_font = "Vera/" + conv::itos(resolution_dependant_font_size(17, conf->p_v_res()));
  int message_font_height = graphics::calculate_font_height(message_font, conf);

  int x_size = string_format::calculate_string_width(screen_updater->header, message_font);

  int box_width = (x_size < 350) ? 350 : x_size + 50;
  int box_height = message_font_height + 75;

  int x = (conf->p_h_res()-box_width)/2;
  int y = (conf->p_v_res()-box_height)/2;

  if (screen_updater->progressbar.elements.size() > 0)
    screen_updater->progressbar.partial_cleanup();

  screen_updater->progressbar.add(new RObj(x+2, y+2, box_width+1, box_height+1, 0, 0, 0, 200, 0));
  screen_updater->progressbar.add(new RObj(x, y, box_width, box_height,
					   themes->dialog_background1, themes->dialog_background2,
					   themes->dialog_background3, themes->dialog_background4, 1));

  screen_updater->progressbar.add(new TObj(screen_updater->header, message_font, x + (box_width/2) - (x_size/2),
					   y+10, themes->dialog_font1, themes->dialog_font2,
					   themes->dialog_font3, 2));

  int blocks = std::min((screen_updater->total_progressbar > 0) ? int(std::ceil((double(screen_updater->status_progressbar)/screen_updater->total_progressbar)*5.0)) : 0, 5);

  for (int i = 0; i < blocks; ++i)
    screen_updater->progressbar.add(new PObj(themes->progressbar_bar, x + message_font_height +i*62, 
					     y + 10 + message_font_height + 10, 2, NOSCALING));

  render->draw_and_release("progressbar", true);
}

void cleanup_progressbar()
{
  Render *render = S_Render::get_instance();
  ScreenUpdater *screen_updater = S_ScreenUpdater::get_instance();

  render->wait_and_aquire();
  screen_updater->progressbar.cleanup();
  render->image_mut.leaveMutex();
}

int check_progressbar()
{
  return 100; // ms
}
