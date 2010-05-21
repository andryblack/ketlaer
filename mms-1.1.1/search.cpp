#include "search.hpp"

#include "global.hpp"
#include "graphics.hpp"
#include "extra_menu.hpp"
#include "busy_indicator.hpp"
#include "updater.hpp"
#include "touch.hpp"
#include "resolution.hpp"
#include "common-feature.hpp"

#include "boost.hpp"

using std::string;

pthread_mutex_t Search::singleton_mutex = PTHREAD_MUTEX_INITIALIZER;

SearchModule::SearchModule(const string& n, const list_gen_func& sgl, const enter_func& e_f)
  : list_gen(sgl), enter(e_f), name(n)
{}

Search::Search()
  : search_word(""), search_module(gettext("all")), pos(-1), em("", true), exit_loop(false)
{
  select_menu_max_width = 250-85;
  em.set_drop_down_pos(conf->p_h_res()-250+85, 85);
  em.add_item(ExtraMenuItem(gettext("all"), "", boost::bind(&Search::set_module, this, gettext("all"))));
  em.add_exit_input(Input("left", "general"));

  res_dependant_calc();

  S_ResolutionManagement::get_instance()->register_callback(boost::bind(&Search::res_dependant_calc, this));
}

void Search::res_dependant_calc()
{
  header_font = "Vera/" + conv::itos(resolution_dependant_font_size(28, conf->p_v_res()));
  element_header_font = "Vera/" + conv::itos(resolution_dependant_font_size(18, conf->p_v_res()));
  element_font = "Vera/" + conv::itos(resolution_dependant_font_size(16, conf->p_v_res()));
  search_text_font = "Vera/" + conv::itos(resolution_dependant_font_size(17, conf->p_v_res()));

  std::pair<int, int> element_size = string_format::calculate_string_size("abcltuwHPMjJg", element_font);
  element_height = element_size.second - 5;

  element_size = string_format::calculate_string_size("abcltuwHPMjJg", element_header_font);
  element_header_height = element_size.second - 5;
}

void Search::set_module(const string& mod)
{
  search_module = mod;
}

void Search::find_element_and_do_action(const result& element)
{
  int p = 0;
  foreach (result& f, results) {
    if (f == element) {
      pos = p;
      action();
      break;
    } else
      ++p;
  }
}

void Search::print_element(const result& r, const result& position, int& y, bool first_element)
{
  if (r.module != cur_module) {

    if (!first_element)
      y += 10;

    render->current.add(new TObj(r.module, element_header_font, 85, y,
				 themes->search_module_font1, themes->search_module_font2,
				 themes->search_module_font3, 3));
    y += element_header_height;
    cur_module = r.module;
  }

  string name = r.name;

  string_format::format_to_size(name, element_font, conf->p_h_res()-2*70-50, true);

  PObj *p = new PFObj(themes->general_marked_large, 70, y,
		      conf->p_h_res()-(2*70), element_height, 1, true);

  S_Touch::get_instance()->register_area(TouchArea(rect(p->x, p->y, p->w, p->h), p->layer, boost::bind(&Search::find_element_and_do_action, this, r)));

  if (r == position) {
    render->current.add(p);

    render->current.add(new TObj(name, element_font, 100, y-3,
				 themes->search_marked_font1, themes->search_marked_font2,
				 themes->search_marked_font3, 3));
  } else {
    render->current.add(new TObj(name, element_font, 100, y-3,
				 themes->search_font1, themes->search_font2,
				 themes->search_font3, 3));
  }
}

void Search::print(const string& search_w)
{
  render->prepare_new_image();

  render->current.add(new PObj(themes->search_background, 0, 0, 0, SCALE_FULL));

  std::pair<int, int> header_size = string_format::calculate_string_size("abcltuwHPMjJg", header_font);
  int header_box_size = static_cast<int>(header_size.second * 0.75);

  PObj *back = new PFObj(themes->general_search, 25, 10, header_box_size, header_box_size, 2, true);

  S_Touch::get_instance()->register_area(TouchArea(rect(back->x, back->y, back->w, back->h), back->layer, boost::bind(&Search::exit, this)));

  render->current.add(back);

  S_Touch::get_instance()->register_area(TouchArea(rect(back->x, back->y, back->w, back->h), back->layer,
						   boost::bind(&Search::exit, this)));

  if (themes->show_header) {
    TObj *t = new TObj(gettext("Search"), header_font, 25 + header_box_size + 10, 10 + (header_box_size - header_size.second)/2,
		       themes->search_header_font1, themes->search_header_font2,
		       themes->search_header_font3, 2);

    render->current.add(t);

    S_Touch::get_instance()->register_area(TouchArea(rect(t->x, t->y, t->w, t->h), t->layer, global->toggle_playlist));
  }

  int x = 85, y = 10 + header_box_size + 10;

  string s = gettext("Search: ");
  std::pair<int, int> search_size = string_format::calculate_string_size(s, search_text_font);

  int text_size = conf->p_h_res() - select_menu_max_width - 10 - (x - 5) - 10;

  PFObj *p = new PFObj(themes->general_marked_large, x-5, y, text_size,
		       search_size.second, 2, true);

  int search_box_height = p->real_h;

  if (pos == -1)
    render->current.add(p);
  else
    delete p;

  string sw = search_w;

  string_format::format_to_size(sw, search_text_font, text_size - search_size.first, true);

  render->current.add(new TObj(s + sw, search_text_font, x, y, themes->search_font1,
			       themes->search_font2, themes->search_font3, 3));

  render->current.add(new TObj(search_module, search_text_font, (x-5) + text_size + 10, y,
			       themes->search_font1, themes->search_font2, themes->search_font3, 3));

  y += search_box_height + 10;

  if (list_contains(conf->p_input(), string("lirc")) || list_contains(conf->p_input(), string("evdev")))
    y += 49;

  if (results.size() > 0) {

    string button_playback_font = graphics::resolution_dependant_font_wrapper(17, conf);
    int button_playback_height = graphics::calculate_font_height(button_playback_font, conf);

    int bottom = static_cast<int>(2 * button_playback_height * 0.85);

    bool direction_down = true;
    int space = element_header_height + element_height + 10;
    int max_space = conf->p_v_res()-(y+bottom+20);
    int divert_pos_down = 0, divert_pos_up = 0;
    int cur_pos = 0, nr_elements = 0;
    bool direction_up_ok = true, direction_down_ok = true;
    bool down_failed = false, up_failed = false;

    // the value (bool) is dummy, we just need a fast lookup mechanism
    std::map<string, bool> available_modules;
    if (pos != -1)
      available_modules.insert(std::make_pair(vector_lookup(results, pos).module, true));
    else
      available_modules.insert(std::make_pair(vector_lookup(results, 0).module, true));

    string module;

    int last_add;

    while (true) {

      if (!direction_down) {
	cur_pos = pos + divert_pos_up + 1;
	if (cur_pos <= results.size() - 1) {
	  module = vector_lookup(results, cur_pos).module;
	  if (available_modules.find(module) == available_modules.end()) {
	    space += element_header_height + element_height + 10;
	    available_modules.insert(std::make_pair(module, true));
	  } else {
	    space += element_height;
	  }

	  if (space > max_space) {
	    if (down_failed)
	      break;
	    else {
	      up_failed = true;
	      direction_down = true;
	      continue;
	    }
	  } else {
	    nr_elements += 1;
	    divert_pos_up += 1;
	  }
	} else {
	  up_failed = true;
	  direction_up_ok = false;
	}

	direction_down = true;
      } else {
	cur_pos = pos - divert_pos_down - 1;
	if (cur_pos >= 0) {
	  module = vector_lookup(results, cur_pos).module;
	  if (available_modules.find(module) == available_modules.end()) {
	    space += element_header_height + element_height + 10;
	    last_add = element_header_height + element_height + 10;
	    available_modules.insert(std::make_pair(module, true));
	  } else {
	    space += element_height;
	    last_add = element_height;
	  }

	  if (space > max_space) {
	    if (up_failed)
	      break;
	    else {
	      space -= last_add;
	      down_failed = true;
	      direction_down = false;
	      continue;
	    }
	  } else {
	    nr_elements += 1;
	    divert_pos_down += 1;
	  }
	} else {
	  down_failed = true;
	  direction_down_ok = false;
	}

	direction_down = false;
      }

      if (!direction_up_ok && !direction_down_ok)
	break;
    }

    cur_module = "";

    int startpos = pos - divert_pos_down;

    // pos == -1
    if (startpos < 0)
      startpos = 0;

    render->current.add(new RObj(55, y-10, conf->p_h_res()-2*55, max_space,
				 themes->general_rectangle_color1, themes->general_rectangle_color2,
				 themes->general_rectangle_color3, themes->general_rectangle_color4, 1));

    result cur_element;

    if (pos != -1)
      cur_element = vector_lookup(results, pos);

    bool first_element = true;

    for (int i = startpos, end = results.size(); i < end; ++i) {

      if (i - startpos > nr_elements)
	break;

      print_element(vector_lookup(results, i), cur_element, y, first_element);

      first_element = false;

      y += element_height;
    }
  }

  render->draw_and_release("search");
}

void Search::register_module(const SearchModule& s)
{
  if (s.name != gettext("all")) {
    modules.push_back(s);
    em.add_item(ExtraMenuItem(s.name, "", boost::bind(&Search::set_module, this, s.name)));
    string element_font = "Vera/" + conv::itos(resolution_dependant_font_size(17, conf->p_v_res()));
    int name_width = string_format::calculate_string_width(s.name, element_font);
    if (name_width > select_menu_max_width) {
      select_menu_max_width = name_width;
      em.set_drop_down_pos(conf->p_h_res() - select_menu_max_width - 10, 85);
      std::cout << "setting drop down position" << std::endl;
    }
  } else {
    // FIXME: complain loudly
    ;
  }
  
  print_debug("registering module:" + s.name, "SEARCH");
}

class SearchDialogFunctor
{
 private:
  void (*fpt)(const string&, int, Overlay&);
  Overlay o;

 public:

  SearchDialogFunctor(void(*_fpt)(const string&, int, Overlay&))
    : o(Overlay("fixed size dialog"))
  { fpt=_fpt; };

  ~SearchDialogFunctor()
  {
    Render *render = S_Render::get_instance();
    render->image_mut.enterMutex();
    o.cleanup();
    render->image_mut.leaveMutex();
  }

  void call(const string& r)
  { return (*fpt)(gettext("Search: ") + r, 450, o); };
};

void Search::action()
{
  result cur = vector_lookup(results, pos);
  foreach (SearchModule& module, modules)
    if (cur.module == module.name) {
      module.enter(cur.id);
      break;
    }
}

void Search::exit()
{
  exit_loop = true;
}

string Search::mainloop()
{
  Input input;

  print(search_word);

  std::list<Input> exit_inputs;
  exit_inputs.push_back(Input("back", "general"));
  exit_inputs.push_back(Input("right", "general"));
  exit_inputs.push_back(Input("next", "general"));

  bool in_select_module = false;

  S_BusyIndicator::get_instance()->idle();

  search_mode = true;

  std::list<string> empty;
  empty.push_back("");

  ScreenUpdater *screen_updater = S_ScreenUpdater::get_instance();

  bool update_needed = true;

  int search_size = string_format::calculate_string_size("abcltuwHPMjJg", search_text_font).second;

  PFObj *p = new PFObj(themes->general_marked_large, 0, 0, 10, search_size, 2, true);
  int search_box_height = p->real_h;
  delete p;

  while (!exit_loop)
    {
      if (pos == -1 && !in_select_module) {

	screen_updater->timer.deactivate("audio_fullscreen");

	string old_search_word = search_word;

	input_master->set_map("search");

	search_top_size = 24 + search_box_height;

	print_marked_search_letter();

	screen_updater->trigger.add(TriggerElement("search marker", empty,
						   boost::bind(&Search::print_marked_search_letter, this),
						   boost::bind(&Search::clean_up_search_print, this)));
	screen_updater->timer.add(TimeElement("search marker",
					      boost::bind(&Search::check_search_letter, this),
					      boost::bind(&Search::print_marked_search_letter, this)));

	std::pair<bool, Input> res = input_master->generate_string(search_word, search_help_offset,
								   boost::bind(&Search::print, this, _1),
								   exit_inputs);

	screen_updater->trigger.del("search marker");
	screen_updater->timer.del("search marker");

	force_search_print_cleanup();

	if (res.first) {

	  S_BusyIndicator::get_instance()->busy();

	  results.clear();
	  foreach (SearchModule& module, modules)
	    if (search_module == module.name || search_module == gettext("all")) {
	      std::vector<std::pair<string, int> > temp = module.list_gen(search_word);
	      for (std::vector<std::pair<string, int> >::iterator ti = temp.begin(), tiend = temp.end();
		   ti != tiend; ++ti) {
		result r;
		r.module = module.name;
		r.name = ti->first;
		r.id = ti->second;
		results.push_back(r);
	      }
	    }

	  S_BusyIndicator::get_instance()->idle();

	  if (results.size() > 0) {
	    input_master->set_map("default");
	    pos = 0;
	  } else {
	    DialogWaitPrint pdialog(gettext("No search results found"), 2000);
	    continue;
	  }

	} else {

	  if (res.second.command == "exit" || res.second.command == "back") {
	    search_word = old_search_word;
	    exit_loop = true;
	  } else if (res.second.command == "right") {
	    in_select_module = true;
	    continue;
	  } else if (res.second.command == "next" && results.size() > 0) {
	    input_master->set_map("default");
	    pos = 0;
	    continue;
	  } else
	    input = res.second;
	}

	S_BusyIndicator::get_instance()->idle();

	screen_updater->timer.activate("audio_fullscreen");

	continue;

      } else if (pos == -1 && in_select_module) {

	screen_updater->timer.deactivate("audio_fullscreen");

	em.mainloop();

	S_BusyIndicator::get_instance()->idle();

	print(search_word);
	in_select_module = false;
	screen_updater->timer.activate("audio_fullscreen");

	continue;

      } else {
	if (update_needed)
	  print(search_word);

	update_needed = true;
      }

      input = input_master->get_input();

      if (input.key == "touch_input") {
	S_Touch::get_instance()->run_callback();
	S_BusyIndicator::get_instance()->idle();
	continue;
      }

      if (input.command == "prev")
	{
	  if (pos != -1)
	    --pos;

	  // if we go up again we need to draw again since otherwise the first
	  // element will be marked
	  if (pos == -1)
	    print(search_word);
 	}
      else if (input.command == "next")
 	{
	  if (pos != results.size()-1)
	    ++pos;
 	}
      else if (input.command == "left")
 	{
	  string current_module = vector_lookup(results, pos).module;

	  int old_pos = pos;

	  while (pos > 0) {
	    if (vector_lookup(results, pos).module != current_module) {
	      if (old_pos - 1 == pos) {
		current_module = vector_lookup(results, pos).module;
		continue;
	      }

	      pos = pos + 1;
	      break;
	    } else
	      --pos;
	  }
 	}
      else if (input.command == "right")
	{
	  string current_module = vector_lookup(results, pos).module;

	  int old_pos = pos;
	  bool element_found = false;

	  while (pos < results.size()) {
	    if (vector_lookup(results, pos).module != current_module) {
	      element_found = true;
	      break;
	    } else
	      ++pos;
	  }

	  if (!element_found)
	    pos = old_pos;
	}
      else if (input.command == "page_down")
 	{
	  int jump = conf->p_jump();
	  while (pos != results.size()-1 && jump-- > 0)
	    ++pos;
 	}
      else if (input.command == "page_up")
 	{
	  int jump = conf->p_jump();
	  while (pos != -1 && jump-- > 0)
	    --pos;

	  // if we go up again we need to draw again since otherwise the first
	  // element will be marked
	  if (pos == -1)
	    print(search_word);
 	}
      else if (input.command == "action")
	{
	  action();
	  S_BusyIndicator::get_instance()->idle();
	}
      else if (input.command == "back" && input.mode == "general")
	{
	  pos = -1;
	}
      else if (input.command == "startmenu" && input.mode == "general")
	{
	  exit_loop = true;
	}
      else {
	update_needed = !global->check_commands(input, false);
	S_BusyIndicator::get_instance()->idle();
      }
    }

  exit_loop = false;
  search_mode = false;

  return "";
}
