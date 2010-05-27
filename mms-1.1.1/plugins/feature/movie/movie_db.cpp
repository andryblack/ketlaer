#include "movie_db.hpp"

#include "common-feature.hpp"
#include "graphics.hpp"
#include "updater.hpp"
#include "busy_indicator.hpp"
#include "touch.hpp"
#include "resolution.hpp"

#include "Movie_info.h"

// ostringstream
#include <sstream>

#include <vector>
#include <string>

#include <iostream>

using std::vector;
using std::string;
using std::list;
using string_format::str_printf;

MovieDB::MovieDB(const string& filename, bool p)
  : db((conf->p_var_data_dir() + filename).c_str()),
    exit_imdb_search_loop(false), physical(p)
{
  check_db();

#ifdef use_ffmpeg_thumbnailer
  MovieConfig * _mc = S_MovieConfig::get_instance();
  thumb_deepval = (_mc->p_thumbnail_accur() >= 0 && _mc->p_thumbnail_accur() < 10) ? _mc->p_thumbnail_accur() : 3;
  thumb_skipblanks = _mc->p_thumbnail_noblanks();
  if (thumb_skipblanks > 0)
    print_warning("Internal thumbnailing: skip blank frames mode is active");
  if (thumb_skipblanks > 2)
    print_warning(str_printf("Skip blank frames mode is set to %d,\n thumbnailing will be much slower\n",thumb_skipblanks));

  thumb_verbosity = S_Config::get_instance()->p_debug_level();

#else
  thumbnailer = 0;
#endif

  if (!db)
    print_critical(dgettext("mms-movie", "Video database could not be opened or created"), "MOVIE");

  if (!recurse_mkdir(conf->p_var_data_dir(),"movies", &imdb_dir))
    print_critical(dgettext("mms-movie", "Could not create directory ") + imdb_dir, "MOVIE");

  res_dependant_calc_2();

  S_ResolutionManagement::get_instance()->register_callback(boost::bind(&MovieDB::res_dependant_calc_2, this));
}

void MovieDB::res_dependant_calc_2()
{
  // resolution dependent stuff

  int search_box_height = 40;
  if (list_contains(conf->p_input(), string("lirc")) || list_contains(conf->p_input(), string("evdev")))
    search_box_height = 60;

  rows = 3;
  rows_search = 2;

  if (conf->p_v_res() <= 600) {
    int search_box_height = 35;
    if (list_contains(conf->p_input(), string("lirc")) || list_contains(conf->p_input(), string("evdev")))
      search_box_height = 50;

    if (conf->p_v_res() <= 500)
      rows = 2;
  }

  int top_height = 10 * 2 + header_size.second;

  string button_playback_font = "Vera/" + conv::itos(resolution_dependant_font_size(17, conf->p_v_res()));
  std::pair<int, int> element_size = string_format::calculate_string_size("abcltuwHPMjJg", button_playback_font);
  int button_playback_height = element_size.second - 5;

  int bottom_height = static_cast<int>(2 * button_playback_height * 0.85);

  // resolution independant stuff

  AspectRatio ar = aspect_ratio(conf->p_h_res(), conf->p_v_res());

  images_per_row = 5; // 4:3

  if (ar == SIXTEEN_NINE)
    images_per_row = 6;
  else if (ar == SIXTEEN_TEN)
    images_per_row = 6;

  image_height_all = (conf->p_v_res() - top_height - bottom_height)/rows;
  image_height = image_height_all - normal_font_height*2 + 3;

  image_height = static_cast<int>(image_height/1.35); // zoom
  image_height_all_eks_text = image_height + 6;

  image_width_all = (conf->p_h_res()-2*25)/images_per_row;
  image_width = image_width_all - 20;

  image_width = static_cast<int>(image_width/1.35); // zoom

  if (rows != rows_search)
    image_height_all_search = image_height_all;
  else {
    image_height_all_search = (conf->p_v_res() - top_height - bottom_height - search_box_height)/rows_search;

    image_height = image_height_all - normal_font_height*2;

    image_height = static_cast<int>(image_height/1.35); // zoom
    image_height_all_eks_text = image_height + 6;
  }

  y_start = top_height;
}

MovieDB::~MovieDB()
{
  db_mutex.enterMutex();
  db.close();
  db_mutex.leaveMutex();
}

void MovieDB::create_db()
{
  vector<string> fields;
  fields.push_back("version INTEGER NOT NULL");
  db.createTable("Schema", fields);

  db.execute("INSERT INTO Schema VALUES('4')");

  fields.clear();
  fields.push_back("id INTEGER PRIMARY KEY");
  fields.push_back("parent INTEGER");
  fields.push_back("filename TEXT");
  fields.push_back("name TEXT");
  fields.push_back("lname TEXT");
  fields.push_back("is_thumbnailed INTEGER");
  fields.push_back("is_folder INTEGER"); // 1 = true
  db.createTable("Folders", fields);

  fields.clear();
  fields.push_back("id INTEGER PRIMARY KEY");
  fields.push_back("name TEXT");
  fields.push_back("lname TEXT");
  db.createTable("Directors", fields);

  fields.clear();
  fields.push_back("id INTEGER PRIMARY KEY");
  fields.push_back("did INTEGER NOT NULL");
  fields.push_back("mid INTEGER NOT NULL");
  db.createTable("DMovie", fields);

  fields.clear();
  fields.push_back("id INTEGER PRIMARY KEY");
  fields.push_back("name TEXT");
  fields.push_back("lname TEXT");
  fields.push_back("role TEXT");
  fields.push_back("lrole TEXT");
  db.createTable("WritingCredits", fields);

  fields.clear();
  fields.push_back("id INTEGER PRIMARY KEY");
  fields.push_back("wcid INTEGER NOT NULL");
  fields.push_back("mid INTEGER NOT NULL");
  db.createTable("WCMovie", fields);

  fields.clear();
  fields.push_back("id INTEGER PRIMARY KEY");
  fields.push_back("name TEXT");
  fields.push_back("lname TEXT");
  db.createTable("Genre", fields);

  fields.clear();
  fields.push_back("id INTEGER PRIMARY KEY");
  fields.push_back("gid INTEGER NOT NULL");
  fields.push_back("mid INTEGER NOT NULL");
  db.createTable("GMovie", fields);

  fields.clear();
  fields.push_back("id INTEGER PRIMARY KEY");
  fields.push_back("name TEXT");
  fields.push_back("lname TEXT");
  fields.push_back("role TEXT");
  fields.push_back("lrole TEXT");
  db.createTable("Actors", fields);

  fields.clear();
  fields.push_back("id INTEGER PRIMARY KEY");
  fields.push_back("aid INTEGER NOT NULL");
  fields.push_back("mid INTEGER NOT NULL");
  db.createTable("AMovie", fields);

  fields.clear();
  fields.push_back("id INTEGER PRIMARY KEY");
  fields.push_back("title TEXT");
  fields.push_back("ltitle TEXT");

  if (physical)
    fields.push_back("path TEXT");

  fields.push_back("url TEXT");
  fields.push_back("runtime TEXT");
  fields.push_back("tagline TEXT");
  fields.push_back("ltagline TEXT");
  fields.push_back("plot TEXT");
  fields.push_back("lplot TEXT");
  fields.push_back("rating REAL");
  fields.push_back("votes INTEGER");
  fields.push_back("year INTEGER");
  fields.push_back("top250 INTEGER");

  if (physical)
    db.createTable("HDMovie", fields);
  else
    db.createTable("Movie", fields);

  create_indexes();
}

void MovieDB::create_indexes()
{
  db.execute("CREATE INDEX idx_folders_name ON Folders (name)");
  db.execute("CREATE INDEX idx_folders_lname ON Folders (lname)");
  db.execute("CREATE INDEX idx_folders_parent ON Folders (parent)");
  db.execute("CREATE INDEX idx_folders_filename ON Folders (filename)");

  db.execute("CREATE INDEX idx_directors_name ON Directors (lname)");

  db.execute("CREATE INDEX idx_writingcredits_name ON WritingCredits (lname)");
  db.execute("CREATE INDEX idx_writingcredits_role ON WritingCredits (lrole)");

  db.execute("CREATE INDEX idx_genre_name ON Genre (lname)");

  db.execute("CREATE INDEX idx_actors_name ON Actors (lname)");
  db.execute("CREATE INDEX idx_actors_role ON Actors (lrole)");

  db.execute("CREATE INDEX idx_movie_title ON Movie (ltitle)");
  db.execute("CREATE INDEX idx_movie_tagline ON Movie (ltagline)");
  db.execute("CREATE INDEX idx_movie_plot ON Movie (lplot)");
}

void MovieDB::check_db()
{
  if (db.hasTable("Schema")) {
    // check for correct version
    SQLQuery *q = db.query("Schema", "SELECT version FROM %t");
    if (q && q->numberOfTuples() > 0) {
      SQLRow &row = (*q)[0];
      if (row["version"] == "4")
	; // correct version
      else if (row["version"] == "3") {
	Print print(gettext("Upgrading video database schema"), Print::INFO);
	db.execute("ALTER TABLE Folders ADD is_thumbnailed INTEGER");
	db.execute("UPDATE Schema SET version=4");
      }	else if (row["version"] != "3") {

	Print print(gettext("Upgrading video database schema"), Print::INFO);

	SQLQuery *dq = db.query("sqlite_master", "SELECT name FROM %t WHERE type='table'");
	if (dq) {
	  for (int i = 0; i < dq->numberOfTuples(); ++i) {
	    db.execute(("DROP TABLE " + (*dq)[i]["name"]).c_str());
	  }
	}

	delete dq;

	dq = db.query("sqlite_master", "SELECT name FROM %t WHERE type='index'");
	if (dq) {
	  for (int i = 0; i < dq->numberOfTuples(); ++i) {
	    db.execute(("DROP INDEX " + (*dq)[i]["name"]).c_str());
	  }
	}

	delete dq;

	create_db();
      }
    }
    delete q;
  } else {
    // create the whole db
    create_db();
  }
}

string MovieDB::clean_up_name_for_imdb(string name)
{
  vector<string> types;
  types.push_back("DVDRip");
  types.push_back("DVDRiP");
  types.push_back("DvDrip");
  types.push_back("XViD");
  types.push_back("XviD");
  types.push_back("AC3");
  types.push_back("PROPER");
  types.push_back("DVDSCR");
  types.push_back("READNFO");
  types.push_back("RETAiL");
  types.push_back(".WS.");
  types.push_back("LiMiTED");
  types.push_back("LIMITED");
  types.push_back("DVD.SCREENER");
  types.push_back("SCREENER");
  types.push_back("TELECINE");
  types.push_back("READ.NFO");
  types.push_back("RETAIL");
  types.push_back("R5.LINE");
  types.push_back("BlueRay");
  types.push_back("BluRay");
  types.push_back("x264");
  types.push_back("720p");
  types.push_back("1080p");
  types.push_back("HDDVD");
  types.push_back("DVD5");
  types.push_back("DVD9");
  types.push_back("Extended");
  types.push_back("Unrated");
  types.push_back("HDTV");
  types.push_back("DTheater");
  types.push_back("5.1");
  types.push_back("BDRip");
  types.push_back("DTS");
  types.push_back("OAR");

  vector<string> group_names;
  group_names.push_back("-ViTE");
  group_names.push_back("-RSSM");
  group_names.push_back("-LMG");
  group_names.push_back("-NeDiVx");
  group_names.push_back("-PUKKA");
  group_names.push_back("-HLS");
  group_names.push_back("-BMT");
  group_names.push_back("-BMB");
  group_names.push_back("-DiAMOND");
  group_names.push_back("-DONE");
  group_names.push_back("-OSiDVD");
  group_names.push_back("-iMBT");
  group_names.push_back("-SAiNTS");
  group_names.push_back("-mVs");
  group_names.push_back("-DMT");
  group_names.push_back("-FLAiTE");
  group_names.push_back("-aXXo");
  group_names.push_back("-SEPTiC");
  group_names.push_back("-hV");
  group_names.push_back("-SiNNERS");
  group_names.push_back("-NBS");
  group_names.push_back("-THOR");
  group_names.push_back("-WPi");
  group_names.push_back("-Chakra");
  group_names.push_back("-LsE");
  group_names.push_back("-HDT");
  group_names.push_back("-BoNE");
  group_names.push_back("-DEFiNiTE");
  group_names.push_back("-mVmHD");
  group_names.push_back("-4HM");
  group_names.push_back("-TQF");
  group_names.push_back("-REVEiLLE");
  group_names.push_back("-CDDHD");
  group_names.push_back("-XOR");
  group_names.push_back("-LOL");
  group_names.push_back("-DivXfacTory");

  int pos;

  foreach (string& type, types)
    if ((pos = name.find(type)) != string::npos)
      name.erase(pos, type.size());

  foreach (string& group_name, group_names)
    if ((pos = name.find(group_name)) != string::npos)
      name.erase(pos, group_name.size());

  for (int i = 0; i < name.size(); ++i)
    if (name[i] == '.')
      name[i] = ' ';

  return name;
}

void MovieDB::reget_movie_information()
{
  CIMDBMovie m = vector_lookup(files, folders.top().second);

  imdb_search_mainloop(clean_up_name_for_imdb(m.name), m, false);
  files[folders.top().second] = m;

  // reset map
  input_master->set_map("movie");
}

void MovieDB::print_info()
{
  if (folders.top().second >= files.size()) // sanity check
    return;

  S_BusyIndicator::get_instance()->idle();

  if (vector_lookup(files, folders.top().second).m_strTitle.empty()) {
    DialogWaitPrint pdialog(2000);
    pdialog.add_line(dgettext("mms-movie", "No IMDb information found about video"));
    pdialog.add_line(dgettext("mms-movie", "Please update information"));
    pdialog.print();
  } else
    print_information(vector_lookup(files, folders.top().second));
}

string MovieDB::id_to_filename(int db_id)
{
  string path;

  db_mutex.enterMutex();

  SQLQuery *q = db.query("Folders", ("SELECT filename FROM %t WHERE id='" + conv::itos(db_id) + "'").c_str());
  if (q && q->numberOfTuples() > 0)
    path = (*q)[0]["filename"];
  else
    assert(false);

  db_mutex.leaveMutex();

  return path;
}

bool MovieDB::search_is_graphical_view(const vector<CIMDBMovie>& cur_files)
{
  return opts.display_mode() == dgettext("mms-movie", "icon view");
}

void MovieDB::search_func()
{
  S_BusyIndicator::get_instance()->idle();

  ScreenUpdater *screen_updater = S_ScreenUpdater::get_instance();

  search_mode = true;
  search_help_offset = -1;

  std::pair<int, int> search_text_size = string_format::calculate_string_size("abcltuwHPMjJg", search_font);
  int search_size = static_cast<int>(search_text_size.second * 0.75);

  if (list_contains(conf->p_input(), string("lirc")) || list_contains(conf->p_input(), string("evdev")))
    search_top_size += search_size;

  print_marked_search_letter();

  list<string> empty;
  empty.push_back("");

  screen_updater->trigger.add(TriggerElement("search marker", empty,  boost::bind(&MovieDB::print_marked_search_letter, this),
					     boost::bind(&MovieDB::clean_up_search_print, this)));
  screen_updater->timer.add(TimeElement("search marker", boost::bind(&MovieDB::check_search_letter, this),
					boost::bind(&MovieDB::print_marked_search_letter, this)));

  int pos = input_master->graphical_search<CIMDBMovie>(files,
						       boost::bind(&MovieDB::print, this, _1),
						       boost::bind(&MovieDB::gen_search_list, this, _1),
						       boost::bind(&MovieDB::id_to_filename, this, _1),
						       boost::bind(&MovieDB::search_is_graphical_view, this, _1),
						       images_per_row, search_mode, search_str, lowercase_search_str,
						       search_depth, offset, search_help_offset,
						       S_BusyIndicator::get_instance());

  screen_updater->trigger.del("search marker");
  screen_updater->timer.del("search marker");

  force_search_print_cleanup();

  if (list_contains(conf->p_input(), string("lirc")) || list_contains(conf->p_input(), string("evdev")))
    search_top_size -= search_size;

  if (pos != -1 && search_depth == dgettext("mms-movie", "current folder"))
    folders.top().second = pos;
  else if (pos != -1)
    change_dir_to_id(pos);
}

void MovieDB::search_imdb()
{
  CIMDBMovie m;
  if (imdb_search_mainloop("", m, true))
    files.push_back(m);

  // reset map
  input_master->set_map("movie");
}

void MovieDB::prev()
{
  if (files.size() > images_per_row) {
    for (int i = 0; i < images_per_row; ++i) {
      if (folders.top().second == 0) {
	folders.top().second = files.size()-1;
	i += images_per_row-files.size()%images_per_row;
      }
      else
	folders.top().second -= 1;
    }
  }
}

void MovieDB::next()
{
  if (files.size() > images_per_row) {
    for (int i = 0; i < images_per_row; ++i) {
      if (folders.top().second == files.size()-1 && i == 0) {
	folders.top().second = 0;
	break;
      } else if (folders.top().second == files.size()-1) {
	break;
      } else {
	folders.top().second += 1;
      }
    }
  }
}

void MovieDB::left()
{
  if (folders.top().second != 0)
    folders.top().second -= 1;
  else
    folders.top().second = files.size() - 1;
}

void MovieDB::right()
{
  folders.top().second = (folders.top().second+1)%files.size();
}

void MovieDB::page_up()
{
  int jump = conf->p_jump();

  if (search_mode)
    jump = images_per_row*rows_search;
  else
    jump = images_per_row*rows;

  if (files.size() > jump) {
    int diff = folders.top().second-jump;
    if (folders.top().second == 0)
      folders.top().second = (files.size()-1)+diff;
    else if (diff < 0)
      folders.top().second = 0;
    else
      folders.top().second = diff;
  }
}

void MovieDB::page_down()
{
  int jump = conf->p_jump();

  if (search_mode)
    jump = images_per_row*rows_search;
  else
    jump = images_per_row*rows;

  if (files.size() > jump) {
    if (folders.top().second > (files.size() - jump) && folders.top().second != (files.size()-1))
      folders.top().second = files.size()-1;
    else
      folders.top().second = (folders.top().second+jump)%files.size();
  }
}

void MovieDB::one_up()
{
  if (search_mode)
    return;

  if (folders.size() > 1)
    go_back();
  else if (folders.size() == 1)
    exit();
}

void MovieDB::go_back()
{
  if (folders.size() > 1) {
    folders.pop();

    load_current_dirs();

    if (files.size() != 0)
      S_BackgroundUpdater::get_instance()->run_once(boost::bind(&MovieDB::check_for_changes, this));
    else
      one_up();
  }
}

void MovieDB::remove_from_db(int index)
{
  CIMDBMovie m = vector_lookup(files, index);

  db_mutex.enterMutex();
  m.Delete(physical, db, imdb_dir);
  db_mutex.leaveMutex();

  files[index] = CIMDBMovie((Multifile)m);
}

void MovieDB::print_information(const CIMDBMovie& m)
{
  printing_information = true;

  render->prepare_new_image();

#if 0
  render->current.add(new PObj(themes->movie_background, 0, 0, 0, SCALE_FULL));
#else
  render->current.add(new BgRObj(0));
#endif

  // determine if the title already contains year, this is often the case with
  // movies produced several times
  bool show_year = true;
  string::size_type first_paran = m.m_strTitle.find('(');
  if (first_paran != string::npos) {
    string::size_type second_paran = m.m_strTitle.find(')');
    if (second_paran != string::npos)
      if (second_paran - first_paran == 5)
	show_year = false;
  }

  string title = m.m_strTitle;
  if (show_year)
    title += " (" + conv::itos(m.m_iYear) + ")";

  string_format::format_to_size(title, information_header_font, conf->p_h_res()-2*45, true);
  render->current.add(new TObj(title, information_header_font, 45, 15,
			       themes->movie_header_font1, themes->movie_header_font2,
			       themes->movie_header_font3, 3));

  std::pair<int, int> element_size = string_format::calculate_string_size("abcltuwHPMjJg", normal_font);
  int information_header_height = element_size.second;

  int y = 15 + information_header_height + 30;
  int x = 45;

  PObj *p;

  int w = 100, h = 140;

  if (conf->p_v_res() < 600) {
    w = 72;
    h = 100;
  }

  if (file_exists(m.cover_path(imdb_dir))) {
    render->create_scaled_image_wrapper_upscaled(m.cover_path(imdb_dir), w, h);
    p = new PFObj(m.cover_path(imdb_dir), x, y, w, h, false, 3);
  } else {
    string picture_path = render->default_path + "movie/b_mcover.png";
    render->create_scaled_image_wrapper_upscaled(picture_path, w, h);
    p = new PFObj(picture_path, x, y, w, h, false, 3);
  }

  render->current.add(p);

  render->current.add(new TObj(dgettext("mms-movie", "Director: ") + m.m_strDirector, information_font, x + p->w + 10, y,
			       themes->movie_font1, themes->movie_font2, themes->movie_font3, 3));
  y += information_font_height;

  render->current.add(new TObj(dgettext("mms-movie", "Writing Credits: ") + m.m_strWritingCredits, information_font, x + p->w + 10, y,
			       themes->movie_font1, themes->movie_font2, themes->movie_font3, 3));

  if (3*information_font_height > h)
    y += information_font_height;
  else
    y += h - 2*information_font_height;

  string runtime = dgettext("mms-movie", "Runtime: ") + string_format::trim(m.m_strRuntime);
  string_format::format_to_size(runtime, information_font, conf->p_h_res()-(45 + (x + p->w + 10)), true);

  render->current.add(new TObj(runtime, information_font, x + p->w + 10, y,
			       themes->movie_font1, themes->movie_font2, themes->movie_font3, 3));

  y += 15 + information_font_height;

  string genre_string = dgettext("mms-movie", "Genre: ");
  bool first = false;
  foreach (const string& genre, m.genres) {
    if (first)
      genre_string += " /";
    else
      first = true;
    genre_string += ' ' + genre;
  }

  string_format::format_to_size(genre_string, information_font, conf->p_h_res()-2*45, true);
  render->current.add(new TObj(genre_string, information_font, x, y,
			       themes->movie_font1, themes->movie_font2,
			       themes->movie_font3, 3));

  y += static_cast<int>(1.5*information_font_height);

  int max_lines = (conf->p_v_res() - y - static_cast<int>(2.5*information_font_height) - 70) / information_font_height;

  int pos = 0, lines = 0;

  string tagline = string_format::trim(m.m_strTagLine);

  if (!tagline.empty()) {
    tagline = dgettext("mms-movie", "Tagline: ") + tagline;

    while (pos != tagline.size()) {

      if (lines == max_lines)
	break;

      string t = string_format::get_line(tagline, pos, conf->p_h_res()-100, information_font);

      if (lines == max_lines - 1 && pos != tagline.size()) {
	t = t.substr(0, t.size()-3);
	t += "...";
      }

      render->current.add(new TObj(t, information_font, x, y + lines*information_font_height,
				   themes->movie_font1, themes->movie_font2,
				   themes->movie_font3, 3));
      ++lines;
    }

    y += lines*information_font_height;
    y += static_cast<int>(0.5*information_font_height);
  }

  string tmp_plot = string_format::trim(m.m_strPlot);

  int plot_pos = tmp_plot.find('\n');

  if (plot_pos != string::npos)
    tmp_plot = string_format::trim(tmp_plot.substr(0, plot_pos-1));

  string plot = dgettext("mms-movie", "Plot: ") + tmp_plot;

  int tag_lines = lines;
  pos = 0, lines = 0;

  while (pos != plot.size() && tag_lines + lines != max_lines) {

    string t = string_format::get_line(plot, pos, conf->p_h_res()-100, information_font);

    if (tag_lines + lines == max_lines - 1 && pos != plot.size()) {
      t = t.substr(0, t.size()-3);
      t += "...";
    }

    render->current.add(new TObj(t, information_font, x, y + lines*information_font_height,
				 themes->movie_font1, themes->movie_font2,
				 themes->movie_font3, 3));
    ++lines;
  }

  if (lines != 0){
    y += lines*information_font_height;
    y += static_cast<int>(0.5*information_font_height);
  }

  render->current.add(new TObj(dgettext("mms-movie", "Rating:"),
			       information_font, x, y, themes->movie_font1, themes->movie_font2,
			       themes->movie_font3, 3));

  int x_size = string_format::calculate_string_width(dgettext("mms-movie", "Rating:"), information_font);

  int nr_stars = int(m.m_fRating);
  bool half_star = true;
  if (m.m_fRating - nr_stars == 0)
    half_star = false;

  for (int t = 0; t < nr_stars; ++t)
    render->current.add(new PObj(themes->movie_rating_icon, x+x_size+t*30, y, 3, NOSCALING));

  float extra_space = 0.5;
  if (half_star) {
    render->current.add(new PObj(themes->movie_rating_icon_half, x+x_size+nr_stars*30, y, 3, NOSCALING));
    extra_space += 0.7;
  }

  render->current.add(new TObj(conv::ftos(m.m_fRating) + " (" + m.m_strVotes + " votes)",
			       information_font, int(x+x_size+(nr_stars+extra_space)*30), y,
			       themes->movie_font1, themes->movie_font2, themes->movie_font3, 3));

  render->draw_and_release("Movie information 1");

  input_master->get_input();

  render->prepare_new_image();

#if 0
  render->current.add(new PObj(themes->movie_background, 0, 0, 0, SCALE_FULL));
#else
  render->current.add(new BgRObj(0));
#endif

  render->current.add(new TObj(title, information_header_font, 45, 15,
			       themes->movie_header_font1, themes->movie_header_font2,
			       themes->movie_header_font3, 3));

  y = 90;
  x = 45;

  render->current.add(new TObj(dgettext("mms-movie", "Cast: "), information_font, x, y,
			       themes->movie_font1, themes->movie_font2,
			       themes->movie_font3, 3));

  y += information_font_height;

  int actors = 0;

  int max_actors = (conf->p_v_res() - 90 - 70 - information_font_height) / information_font_height;

  int max_x = 0;
  for (vector<std::pair<string, string> >::const_iterator i = m.actors.begin(),
	 end = m.actors.end(); i != end && actors < max_actors; ++i, ++actors)
    max_x = std::max(max_x, string_format::calculate_string_width(i->first, information_font));

  actors = 0;

  if (max_x > conf->p_h_res()-(2*45+30))
    max_x = conf->p_h_res()-(2*45+30)/2;

  for (vector<std::pair<string, string> >::const_iterator i = m.actors.begin(),
	 end = m.actors.end(); i != end && actors < max_actors; ++i, ++actors) {
    string cast_member = i->first;
    string_format::format_to_size(cast_member, information_font, max_x, true);
    render->current.add(new TObj(cast_member, information_font, x+30, y,
				 themes->movie_font1, themes->movie_font2,
				 themes->movie_font3, 3));

    string cast_role = " as   " + i->second;
    string_format::format_to_size(cast_role, information_font, conf->p_h_res()-(2*45+30)-(max_x+1), true);
    render->current.add(new TObj(cast_role, information_font, x+max_x+1+30, y,
				 themes->movie_font1, themes->movie_font2,
				 themes->movie_font3, 3));

    y += information_font_height;
  }

  render->draw_and_release("Movie information 2");

  input_master->get_input();

  printing_information = false;
}

void MovieDB::imdb_download_movie(CIMDBMovie &m, const CIMDBUrl& url)
{
  S_BusyIndicator::get_instance()->busy();

  Overlay overlay_details = Overlay("dialog");
  DialogStepPrint pdialog2(dgettext("mms-movie", "Getting details about video"),
			   overlay_details);

  if (movie_conf->p_information_source() == "english")
    imdb.GetDetails(url, m);
  else if (movie_conf->p_information_source() == "german")
    imdb.GetDetailsOnOFDB(url, m);
  else if (movie_conf->p_information_source() == "italian")
    imdb.GetDetailsOnCMO(url, m);
  else
    print_critical(dgettext("mms-movie", "value of information_source not valid:") +
		   movie_conf->p_information_source(), "MOVIE");

  if (m.name.empty()) {
    m.name = m.m_strTitle;
    m.lowercase_name = string_format::lowercase(m.name);
  }

  // fixme: gif?
  string cover = imdb_dir + m.name + "_cover.jpg";
  // delete any previous cover, the new movie might not have a cover
  if (file_exists(cover))
    run::external_program("rm " + string_format::escape_string(cover));

  if (imdb.Download(m.m_strPictureURL, cover)) {
    render->create_scaled_image_wrapper_upscaled(m.cover_path(imdb_dir), image_width, image_height);
    // to insert the image into the transmap so that it won't fail when
    // we draw the screen again
    bool tmp;
    render->image_dimensions(render->image_get_fullpath(m.cover_path(imdb_dir),
							image_width,
							image_height),
			     &tmp);
  }

  pdialog2.print_step2();
}

void MovieDB::find_movie_in_imdb(const string& search_string,
				 IMDB_MOVIELIST& results, int& pos)
{
  BusyIndicator *busy_indicator = S_BusyIndicator::get_instance();

  Overlay overlay_lookup = Overlay("dialog");
  DialogStepPrint pdialog2(dgettext("mms-movie", "Looking up video in IMDb database"),
			   overlay_lookup);

  busy_indicator->busy();

  if (movie_conf->p_information_source() == "english")
    imdb.FindMovie(search_string, results);
  else if (movie_conf->p_information_source() == "german")
    imdb.FindMovieOnOFDB(search_string, results);
  else if (movie_conf->p_information_source() == "italian")
    imdb.FindMovieOnCMO(search_string, results);

  else
    print_critical(dgettext("mms-movie", "value of information_source not valid:") +
		   movie_conf->p_information_source(), "MOVIE");

  if (results.size() > 0)
    pos = 0;

  searched_imdb = true;

  busy_indicator->idle();

  pdialog2.print_step2();
}

bool MovieDB::imdb_search_mainloop(const string& search_word, CIMDBMovie &m, bool search)
{
  BusyIndicator *busy_indicator = S_BusyIndicator::get_instance();
  ScreenUpdater *screen_updater = S_ScreenUpdater::get_instance();
  screen_updater->timer.deactivate("audio_fullscreen");

  input_master->save_map();
  input_master->set_map("default");

  Input input;

  int pos = -1;
  IMDB_MOVIELIST results;

  searched_imdb = false;

  if (!search_word.empty())
    find_movie_in_imdb(search_word, results, pos);

  // set to idle not matter if the search word is empty or not
  busy_indicator->idle();

  Overlay o = Overlay("imdb search");

  imdb_found_match = false;

  string search_string = search_word;

  std::list<Input> exit_inputs;
  exit_inputs.push_back(Input("back", "general"));

  while (!exit_imdb_search_loop)
    {
      if (pos == -1) {
	boost::function<void (const std::string&)> print_func =
	  boost::bind(&MovieDB::imdb_search_screen, this, _1, results, pos, boost::ref(o), m);

	string gen = search_string;
	int tmp;

	std::list<string> empty;
	empty.push_back("");
  
	search_mode = true;

	print_marked_search_letter();
	
	screen_updater->timer.deactivate("audio_fullscreen");

	screen_updater->trigger.add(TriggerElement("search marker", empty,
						   boost::bind(&MovieDB::print_marked_search_letter, this),
						   boost::bind(&MovieDB::clean_up_search_print, this)));
	screen_updater->timer.add(TimeElement("search marker",
					      boost::bind(&MovieDB::check_search_letter, this),
					      boost::bind(&MovieDB::print_marked_search_letter, this)));

	if (input_master->generate_string(gen, search_help_offset, print_func, exit_inputs).first)
	  search_string = gen;
	else {
	  exit_imdb_search_loop = true;

	  screen_updater->trigger.del("search marker");
	  screen_updater->timer.del("search marker");
	  screen_updater->timer.activate("audio_fullscreen");

	  force_search_print_cleanup();
	  
	  search_mode = false;

	  continue;
	}

	screen_updater->trigger.del("search marker");
	screen_updater->timer.del("search marker");
	screen_updater->timer.activate("audio_fullscreen");

	force_search_print_cleanup();

	search_mode = false;

	find_movie_in_imdb(search_string, results, pos);

	continue;

      } else
	imdb_search_screen(search_string, results, pos, o, m);

      input = input_master->get_input();

      if (input.key == "touch_input") {
	S_Touch::get_instance()->run_callback();
	continue;
      }

      if (input.command == "back" && input.mode == "general")
	{
	  exit_imdb_search_loop = true;
	}
      else if (input.command == "prev")
	{
	  if (pos != -1)
	    --pos;
 	}
      else if (input.command == "next")
 	{
	  if (pos != results.size()-1)
	    ++pos;
 	}
      else if (input.command == "info")
	{
	  render->wait_and_aquire();
	  o.cleanup();
	  render->image_mut.leaveMutex();

	  // download movie and show it
	  CIMDBMovie tmp;
	  imdb_download_movie(tmp, vector_lookup(results, pos));
	  print_information(tmp);

	  o = Overlay("imdb search");
	}
      else if (input.command == "action")
	{
	  imdb_action(m, results, pos);
	}
      global->check_commands(input, true);
    }

  render->wait_and_aquire();
  o.cleanup();
  render->image_mut.leaveMutex();

  screen_updater->timer.activate("audio_fullscreen");

  input_master->restore_map();

  exit_imdb_search_loop = false;

  return imdb_found_match;
}

void MovieDB::imdb_action(CIMDBMovie &m, const IMDB_MOVIELIST& results, int pos)
{
  imdb_download_movie(m, vector_lookup(results, pos));

  db_mutex.enterMutex();
  m.Save(physical, db);
  db_mutex.leaveMutex();

  imdb_found_match = true;
  exit_imdb_search_loop = true;
}

void MovieDB::imdb_search_screen(string search_word, const IMDB_MOVIELIST& results, int pos, Overlay& o, CIMDBMovie &m)
{
  // pos ranges from [-1, results.size()-1], -1 denotes the enter search word box

  S_Touch::get_instance()->clear();

  render->wait_and_aquire();

  if (o.elements.size() > 0)
    o.partial_cleanup();

  int x, y, max_x = conf->p_h_res()-250, max_y = dialog_font_height;

  int max = std::min(10, int(results.size()));
  if (max == 0)
    max = 1; // for the no results found mesage

  int box_height = max_y*max+dialog_font_height+7+dialog_header_font_height+30+dialog_font_height;

  if (!searched_imdb)
    box_height = max_y*max+30+35;

  x = (conf->p_h_res()-(max_x+30))/2;
  y = (conf->p_v_res()-(box_height))/2;

  search_top_size = y - 100;

  o.add(new RObj(x-15+2, y-15+2, max_x+30+1, box_height+1, 0, 0, 0, 200, 0));
  o.add(new RObj(x-15, y-15, max_x+30, box_height,
		 themes->dialog_background1, themes->dialog_background2,
		 themes->dialog_background3, themes->dialog_background4, 1));

  string header = dgettext("mms-movie", "IMDb lookup");

  int x_size = string_format::calculate_string_width(header, dialog_header_font);

  o.add(new TObj(header, dialog_header_font, (conf->p_h_res()-x_size)/2, y-5,
		 themes->dialog_font1, themes->dialog_font2, themes->dialog_font3, 3));
  y += dialog_header_font_height;

  if (pos == -1)
    o.add(new PFObj(themes->general_marked_large, x-5, y+3,
		    max_x+10, dialog_font_height, 3, true));

  string s = dgettext("mms-movie", "Search: ");
  x_size = string_format::calculate_string_width(s, dialog_font);
  string_format::format_to_size(search_word, dialog_font, max_x-x_size, true);

  o.add(new RObj(x-4, y+5, max_x+8, dialog_font_height-5, 0, 0, 0, 200, 2));
  o.add(new TObj(s + search_word, dialog_font, x, y, themes->dialog_font1,
		 themes->dialog_font2, themes->dialog_font3, 4));
  y += dialog_font_height + 7;

  if (!searched_imdb)
    ; // the goggles do nothing
  else if (results.size() == 0)
    o.add(new TObj(dgettext("mms-movie", "No results found..."), dialog_font, x, y,
		   themes->dialog_font1, themes->dialog_font2, themes->dialog_font3, 3));
  else {

    int nr_elements = 10;
    int i, start;
    i = std::max(0, pos-nr_elements+1);
    start = i;

    for (IMDB_MOVIELIST::const_iterator iter = results.begin()+i;
	 iter != results.end() && i < (start + nr_elements); ++iter, ++i) {

      PObj *p = new PFObj(themes->general_marked_large, x-5, y+3,
			  max_x+10, dialog_font_height, 2, true);

      S_Touch::get_instance()->register_area(TouchArea(rect(p->x, p->y, p->w, p->h), p->layer,
						       boost::bind(&MovieDB::imdb_action, this, m, results, pos)));

      if (i == pos)
	o.add(p);

      string title = iter->m_strTitle;
      string_format::format_to_size(title, dialog_font, max_x, true);
      o.add(new TObj(title, dialog_font, x, y, themes->dialog_font1,
		     themes->dialog_font2, themes->dialog_font3, 3));

      y += dialog_font_height;
    }

    string result_string = conv::itos(results.size());

    if (results.size() == 1)
      result_string += dgettext("mms-movie", " result found");
    else
      result_string += dgettext("mms-movie", " results found");

    x_size = string_format::calculate_string_width(result_string, dialog_font);

    o.add(new TObj(result_string, dialog_font, x+max_x-x_size, y, themes->dialog_font1,
		   themes->dialog_font2, themes->dialog_font3, 3));
  }

  render->draw_and_release("imdb search screen", true);
}

string MovieDB::get_name_from_file(const CIMDBMovie& m)
{
  string name = m.name;
  if (!m.real_name.empty())
    name = m.real_name;
  else if (m.m_strTitle != "")
    name = m.m_strTitle;

  return name;
}

bool MovieDB::create_thumbnail(const string& file, const string& thumbnail, int width, int height)
{
#ifdef use_ffmpeg_thumbnailer
  thumbnailer::thumbnailer thumb;
  thumb.init((thumb_verbosity > 2));
  thumb.deepval = thumb_deepval;
  thumb.skipblanks = thumb_skipblanks;
  std::string _dir = thumbnail.substr(0, thumbnail.rfind('/')+1);
  abs_recurse_mkdir(_dir, NULL);
  thumbnailer::movie_specs ms;
  thumb.openmovie(file,&ms);
  bool ret = thumb.createthumbnail(ms.length*1000/8, width, height, true, thumbnail);
  thumb.closemovie();
  return ret;
#else
  if (thumbnailer != 0)
    return thumbnailer->player->create_thumbnail(file, thumbnail, width, height);
  else
    return false;
#endif
}

void MovieDB::find_element_and_do_action(const CIMDBMovie& movie_file)
{
  int p = 0;
  foreach (CIMDBMovie& f, files) {
    if (f == movie_file) {
      folders.top().second = p;
      if (search_mode)
	input_master->got_touch_action();
      else
	action();
      break;
    } else
      ++p;
  }
}

// print a video element
void MovieDB::print_movie_element(CIMDBMovie& m, const CIMDBMovie& position, int y)
{
  string name;

  int x = 70;

  if (m.m_strTitle.empty()) {

    name = m.name;

    if (m.type != "file")
      name += "/";

  } else {

    // determine if the title already contains year, this is often the case with
    // movies produced several times
    bool show_year = true;
    string::size_type first_paran = m.m_strTitle.find('(');
    if (first_paran != string::npos) {
      string::size_type second_paran = m.m_strTitle.find(')');
      if (second_paran != string::npos)
	if (second_paran - first_paran == 5)
	  show_year = false;
    }

    name = string_format::trim(m.m_strTitle);
    if (show_year)
      name += " (" + conv::itos(m.m_iYear) + ")";
  }

  int right_width = conf->p_v_res() / 3;

  string_format::format_to_size(name, list_font, conf->p_h_res()-2*55-right_width-25, true);

  // touch
  PObj *p = new PFObj(themes->general_marked_large, x-5, y+3,
		      conf->p_h_res()-2*55-right_width-20, list_font_height, 2, true);

  S_Touch::get_instance()->register_area(TouchArea(rect(p->x, p->y, p->w, p->h), p->layer,
						   boost::bind(&MovieDB::find_element_and_do_action, this, m)));

  if (m == position) {
    render->current.add(p);
    print_extra_information(m);
  }

  render->current.add(new TObj(name, list_font, x+5, y,
			       themes->movie_font1, themes->movie_font2,
			       themes->movie_font3, 3));
}

void MovieDB::right_align_extra_info_text(string type, const string& value, 
					  int x, int y, int text_width)
{
  int value_length = string_format::calculate_string_width(value, extra_information_font);

  string_format::format_to_size(type, extra_information_font, text_width-value_length+5, true);
  
  render->current.add(new TObj(type, extra_information_font, x+5, y, themes->movie_font1,
			       themes->movie_font2, themes->movie_font3, 3));
  
  render->current.add(new TObj(value, extra_information_font, x + 5 + text_width - value_length,
			       y, themes->movie_font1, themes->movie_font2, themes->movie_font3, 3));
}

void MovieDB::print_extra_information(CIMDBMovie& m)
{
  int right_width = conf->p_v_res() / 3;

  int text_width = right_width - 5;

  int x = conf->p_h_res()-right_width-55;

  int y = y_start;

  if (search_mode)
    y += search_top_size;

  if (conf->p_v_res() == 405)
    y = 85;

  int info_width = text_width-10, info_height = info_width/4*3;

  int real_image_height = info_height;

  if (!m.m_strTitle.empty()) {

    string cover_path = m.cover_path(imdb_dir);

    if (file_exists(cover_path)) {

      render->create_scaled_image_wrapper_upscaled(cover_path, info_width, info_height);
      PFObj *p = new PFObj(cover_path, x, y, info_width, info_height, false, 3);

      p->x = p->real_x = x + (right_width-p->w)/2;
      p->real_x -= 1;

      real_image_height = p->h;

      render->current.add(p);
    }
  } else if (m.type == "file") {

    string thumbnail = render->image_get_fullpath(m.filenames.front(), info_width, info_height);

    bool thumbnail_ok = file_exists(thumbnail);

    // try thumbnail
    if (!thumbnail_ok && !m.thumbnailed_before_failed) {
      thumbnail_ok = create_thumbnail(m.filenames.front(), thumbnail, info_width, info_height);
      if (!thumbnail_ok) {
	m.thumbnailed_before_failed = true;
	// update db
	update_thumbnailed_status(m.db_id);
      }
    }

    if (thumbnail_ok) {
      PFObj *p = new PFObj(thumbnail, x, y, false, 4);

      p->x = p->real_x = x + (right_width - (p->w + 2))/2;
      p->y = p->real_y = y + (info_height - (p->h + 2))/2;

      if (p->h < info_width/4*3) {
	((PFObj* )p)->set_border_position(x + (right_width - (info_width + 10))/2,
					  p->y - (info_width/4*3 - p->h)/2,
					  info_width, info_width/4*3);
      } else {
	p->real_x = p->x - 1;
	p->real_y = p->y - 1;
      }

      real_image_height = p->real_h;

      render->current.add(p);
    }
  }

  y += real_image_height + 3;

  if (!m.m_strTitle.empty()) {

    right_align_extra_info_text(dgettext("mms-movie", "Rating: "), conv::ftos(m.m_fRating),
				x, y, text_width);

    y += extra_information_font_height;

    right_align_extra_info_text(dgettext("mms-movie", "Runtime: "), 
				string_format::trim(m.m_strRuntime), x, y, text_width);

    y += extra_information_font_height;

    if (conf->p_v_res() != 405) {

      string genres = dgettext("mms-movie", "Genre: ");

      int max_genres = 3;
      int genre_count = 0;
      bool first = true;

      foreach (const string& genre, m.genres) {
	if (first) {
	  genres += genre;
	  first = false;
	} else
	  genres += ", " + genre;

	++genre_count;

	if (genre_count == max_genres)
	  break;
      }

      vector<string> genre_lines;

      string_format::wrap_strings(genres, extra_information_font, text_width, genre_lines);

      int line_nr = 0;

      foreach (string& genre_line, genre_lines)  {
	int value_length = string_format::calculate_string_width(genre_line, extra_information_font);
	if (line_nr++ == 0)
	  value_length = text_width;

	render->current.add(new TObj(genre_line, extra_information_font, 
				     x + 5 + text_width - value_length, y,
				     themes->movie_font1, themes->movie_font2,
				     themes->movie_font3, 3));

	y += extra_information_font_height;
      }
    }
  }

  y += 7;

  if (m.type == "file") {

    Movie_info *mv = new Movie_info;

    if (mv->probe_movie(m.filenames.front())) {

      MOVIE_RESPONSE_INFO *a = new MOVIE_RESPONSE_INFO;
      mv->Fetch_Movie_data(a);
      std::vector<string> v;

      string text;

      if (a->got_movie_duration) {
	right_align_extra_info_text(dgettext("mms-movie", "Duration: "), a->movie_duration_str, 
				    x, y, text_width);
	y += extra_information_font_height;
      }

      if (a->got_image_size) {
	right_align_extra_info_text(dgettext("mms-movie", "Resolution: "), a->resolution, 
				    x, y, text_width);
	y += extra_information_font_height;
      }

      if (a->got_aspect_ratio) {
	right_align_extra_info_text(dgettext("mms-movie", "Geometry: "), a->aspect_ratio_str, 
				    x, y, text_width);
	y += extra_information_font_height;
      }

      if (a->got_audio_type) {
	right_align_extra_info_text(dgettext("mms-movie", "Audio codec: "), a->audio_type, 
				    x, y, text_width);
	y += extra_information_font_height;
      }

      if (a->got_audio_bitrate) {
	right_align_extra_info_text(dgettext("mms-movie", "Audio bitrate: "), a->audio_bitrate, 
				    x, y, text_width);
	y += extra_information_font_height;
      }

      delete a;
    }
    delete mv;
  }
}

void MovieDB::print_modifying(vector<CIMDBMovie>& cur_files)
{
  string display_mode = opts.display_mode();

  if (!physical) // collection
    display_mode = opts.display_mode();

  if (display_mode == dgettext("mms-movie", "icon view"))
    print_icon_view(cur_files);
  else
    print_list_view(cur_files);
}

void MovieDB::print_list_view(vector<CIMDBMovie>& cur_files)
{
  render->prepare_new_image();

#if 0
  render->current.add(new PObj(themes->movie_background, 0, 0, 0, SCALE_FULL));
#else
  render->current.add(new BgRObj(0));
#endif

  PFObj *back = new PFObj(themes->startmenu_movie_dir, 25, 10, header_box_size, header_box_size, 2, true);

  render->current.add(back);

  S_Touch::get_instance()->register_area(TouchArea(rect(back->x, back->y, back->w, back->h),
						   back->layer, boost::bind(&MovieDB::one_up, this)));

  if (themes->show_header) {
    string header;

    if (physical)
      header = dgettext("mms-movie", "Videos");
    else
      header = dgettext("mms-movie", "Video Collection");

    if (folders.size() > 1) {
      string top_folder = folders.top().first.front();

      if (top_folder[top_folder.size()-1] == '/')
	top_folder = top_folder.substr(0, top_folder.size()-1);

      assert(top_folder.rfind('/') != string::npos);
      header += " - " + top_folder.substr(top_folder.rfind('/')+1);
      string_format::format_to_size(header, header_font, conf->p_h_res()-260, false);
    }

    std::pair<int, int> header_size = string_format::calculate_string_size(header, header_font);

    TObj *t = new TObj(header, header_font, 25 + header_box_size + 10, 10 + (header_box_size - header_size.second)/2,
		       themes->movie_header_font1, themes->movie_header_font2,
		       themes->movie_header_font3, 2);

    render->current.add(t);

    S_Touch::get_instance()->register_area(TouchArea(rect(t->x, t->y, t->w, t->h), t->layer, global->toggle_playlist));
  }

  if (search_mode) {
    int y = 10 + header_box_size + 10;

    std::pair<int, int> search_text_size = string_format::calculate_string_size("abcltuwHPMjJg", search_font);
    int search_size = static_cast<int>(search_text_size.second * 0.75);

    if (offset == -1)
      render->current.add(new RObj(0, y, conf->p_h_res(), search_size + 5, 0, 0, 0, 215, 2));
    else
      render->current.add(new RObj(0, y, conf->p_h_res(), search_size + 5,
				   themes->general_search_rectangle_color1,
				   themes->general_search_rectangle_color2,
				   themes->general_search_rectangle_color3,
				   themes->general_search_rectangle_color4, 2));

    render->current.add(new PFObj(themes->general_search, 47, y, search_size, search_size, 3, true));

    std::pair<int, int> search_select_sizes = string_format::calculate_string_size(search_depth, search_select_font);

    int max_x = conf->p_h_res()-search_select_sizes.first - 25;

    string sw = search_str;

    string s = dgettext("mms-movie", "Search: ");
    int x_size = string_format::calculate_string_width(s, search_font);
    string_format::format_to_size(sw, search_font, max_x-x_size, true);

    int search_text_height = string_format::calculate_string_size(s + sw, search_font).second;

    render->current.add(new TObj(s + sw, search_font, 47 + search_size + 10,
				 y + (search_size + 5 - search_text_height)/2,
				 themes->search_font1, themes->search_font2, themes->search_font3, 3));

    int search_select_height = string_format::calculate_string_size(search_depth, search_select_font).second;

    render->current.add(new TObj(search_depth, search_select_font, max_x,
				 y + (search_size + 5 - search_select_height)/2,
				 themes->search_font1, themes->search_font2, themes->search_font3, 3));
  }

  int pos_in_list;

  if (search_mode) {
    if (offset == -1)
      pos_in_list = 0;
    else
      pos_in_list = offset;
  } else
    pos_in_list = folders.top().second;

  if (cur_files.size() > 0) {
    print_range_modifying<CIMDBMovie>(cur_files, vector_lookup(cur_files, pos_in_list), pos_in_list,
				      boost::bind(&MovieDB::print_movie_element, this, _1, _2, _3), list_font_height);

    std::ostringstream pos;

    pos << pos_in_list + 1 << "/" << cur_files.size();

    int x = string_format::calculate_string_width(pos.str(), position_font);

    render->current.add(new TObj(pos.str(), position_font, conf->p_h_res()-(60+x), 20,
				 themes->movie_font1, themes->movie_font2,
				 themes->movie_font3, 3));
  }

  render->draw_and_release("movie-graphical");
}

void MovieDB::print_icon_view(vector<CIMDBMovie>& cur_files)
{
  render->prepare_new_image();

#if 0
  render->current.add(new PObj(themes->movie_background, 0, 0, 0, SCALE_FULL));
#else
  render->current.add(new BgRObj(0));
#endif

  std::pair<int, int> header_size = string_format::calculate_string_size("abcltuwHPMjJg", header_font);
  int header_box_size = static_cast<int>(header_size.second * 0.75);

  PObj *back = new PFObj(themes->startmenu_movie_dir, 25, 10, header_box_size, header_box_size, 2, true);

  render->current.add(back);

  S_Touch::get_instance()->register_area(TouchArea(rect(back->x, back->y, back->w, back->h),
						   back->layer, boost::bind(&MovieDB::one_up, this)));

  if (themes->show_header) {
    string header;

    if (physical)
      header = dgettext("mms-movie", "Videos");
    else
      header = dgettext("mms-movie", "Video Collection");

    if (folders.size() > 1) {
      string top_folder = folders.top().first.front();

      if (top_folder[top_folder.size()-1] == '/')
	top_folder = top_folder.substr(0, top_folder.size()-1);

      assert(top_folder.rfind('/') != string::npos);
      header += " - " + top_folder.substr(top_folder.rfind('/')+1);
      string_format::format_to_size(header, header_font, conf->p_h_res()-260, false);
    }

    std::pair<int, int> header_size = string_format::calculate_string_size(header, header_font);

    TObj *t = new TObj(header, header_font, 25 + header_box_size + 10, 10 + (header_box_size - header_size.second)/2,
		       themes->movie_header_font1, themes->movie_header_font2,
		       themes->movie_header_font3, 2);

    render->current.add(t);

    S_Touch::get_instance()->register_area(TouchArea(rect(t->x, t->y, t->w, t->h), t->layer, global->toggle_playlist));
  }

  if (search_mode) {
    int y = 10 + header_box_size + 10;

    std::pair<int, int> search_text_size = string_format::calculate_string_size("abcltuwHPMjJg", search_font);
    int search_size = static_cast<int>(search_text_size.second * 0.75);

    if (offset == -1)
      render->current.add(new RObj(0, y, conf->p_h_res(), search_size + 5, 0, 0, 0, 215, 2));
    else
      render->current.add(new RObj(0, y, conf->p_h_res(), search_size + 5,
				   themes->general_search_rectangle_color1,
				   themes->general_search_rectangle_color2,
				   themes->general_search_rectangle_color3,
				   themes->general_search_rectangle_color4, 2));

    render->current.add(new PFObj(themes->general_search, 47, y, search_size, search_size, 3, true));

    std::pair<int, int> search_select_sizes = string_format::calculate_string_size(search_depth, search_select_font);

    int max_x = conf->p_h_res()-search_select_sizes.first - 25;

    string sw = search_str;

    string s = dgettext("mms-movie", "Search: ");
    int x_size = string_format::calculate_string_width(s, search_font);
    string_format::format_to_size(sw, search_font, max_x-x_size, true);

    int search_text_height = string_format::calculate_string_size(s + sw, search_font).second;

    render->current.add(new TObj(s + sw, search_font, 47 + search_size + 10,
				 y + (search_size + 5 - search_text_height)/2,
				 themes->search_font1, themes->search_font2, themes->search_font3, 3));

    int search_select_height = string_format::calculate_string_size(search_depth, search_select_font).second;

    render->current.add(new TObj(search_depth, search_select_font, max_x,
				 y + (search_size + 5 - search_select_height)/2,
				 themes->search_font1, themes->search_font2, themes->search_font3, 3));
  }

  // print the range
  int pos, start, pos_in_list, cur_rows, cur_image_height_all;

  if (search_mode) {
    if (offset == -1)
      pos_in_list = 0;
    else
      pos_in_list = offset;
    cur_rows = rows_search;
    cur_image_height_all = image_height_all_search;
  } else {
    pos_in_list = folders.top().second;
    cur_rows = rows;
    cur_image_height_all = image_height_all;
  }

  int nr_in_last_row = cur_files.size()%images_per_row;
  if (nr_in_last_row == 0)
    nr_in_last_row = images_per_row;

  if (cur_files.size() > cur_rows*images_per_row) {
    if (pos_in_list < std::ceil(double(cur_rows)/2.0)*images_per_row) {
      pos = pos_in_list;
      start = 0;
    } else if (pos_in_list >= cur_files.size() - (images_per_row+nr_in_last_row)) { // last two rows
      start = cur_files.size()-((cur_rows-1)*images_per_row+nr_in_last_row);
      pos = pos_in_list-start;
    } else {
      pos = pos_in_list%images_per_row+images_per_row;
      if (pos == images_per_row-1)
	pos = 2*images_per_row-1;
      start = pos_in_list-pos;
    }
  } else {
    pos = pos_in_list;
    start = 0;
  }

  int x = (conf->p_h_res()-images_per_row*image_width_all)/2;
  int x_start = x;
  int y = y_start;

  if (search_mode) {
    std::pair<int, int> search_text_size = string_format::calculate_string_size("abcltuwHPMjJg", search_font);
    int search_size = search_text_size.second;

    y += search_size;

    if (list_contains(conf->p_input(), string("lirc")) || list_contains(conf->p_input(), string("evdev")))
      y += 49;
  }

  for (int j = 0; j < cur_rows*images_per_row && j < cur_files.size(); ++j) {

    CIMDBMovie s = vector_lookup(cur_files, start);

    string cover_path = s.cover_path(imdb_dir);

    int y_pos_offset = 0;

    int pic_width = image_width;
    int pic_height = image_height;
    int pic_width_all = image_width_all;
    int pic_height_all_eks_text = image_height_all_eks_text;

    int full_width = static_cast<int>(pic_width * 1.35);
    int full_height = static_cast<int>(pic_height * 1.35);

    if (pos == j) {
      pic_width = full_width;
      pic_height = full_height;
      pic_width_all += pic_width - image_width;
      pic_height_all_eks_text += pic_height - image_height;
    }

    if (file_exists(cover_path)) {

      if (pos == j) {
	y_pos_offset = (pic_height - image_height)/2;

	x -= (pic_width - image_width)/2;
	y -= y_pos_offset;
      }

      render->create_scaled_image_wrapper_upscaled(cover_path, full_width, full_height);
      // use pic with max_width and max_height and scale it in software
      PFObj *p = new PFObj(cover_path, x, y, full_width, full_height, pic_width, pic_height, 3);

      p->x = p->real_x = x + (pic_width_all - p->real_w)/2;
      p->y = p->real_y = y + (pic_height_all_eks_text - p->real_h)/2;

      p->real_x -= 1;
      p->real_y -= 1;

      if (pos == j) {
	x += (pic_width - image_width)/2;
	y += y_pos_offset * 2;
      }

      S_Touch::get_instance()->register_area(TouchArea(rect(p->x, p->y, p->w, p->h), p->layer,
						       boost::bind(&MovieDB::find_element_and_do_action, this, s)));

      render->current.add(p);

    } else {

      if (s.type != "file") {

	int pic_width = static_cast<int>(image_width/1.15);
	int pic_height = static_cast<int>(image_height/1.15);
	int pic_width_all = image_width_all;
	int pic_height_all_eks_text = image_height_all_eks_text;

	int max_pic_width = static_cast<int>(pic_width * 1.40);
	int max_pic_height = static_cast<int>(pic_height * 1.40);

	string picture_path = render->default_path + themes->general_normal_dir;

	if (pos == j) {
	  pic_width = max_pic_width;
	  pic_height = max_pic_height;
	  pic_width_all += pic_width - image_width;
	  pic_height_all_eks_text += pic_height - image_height;
	  x -= (pic_width - image_width)/2;
	  y_pos_offset = (pic_height - image_height)/2;
	  y -= y_pos_offset;
	  picture_path = render->default_path + themes->movie_dir_marked;
	}

	render->create_scaled_image_wrapper(picture_path, max_pic_width, max_pic_height);
	// use pic with max_width and max_height and scale it in software
	PFObj *p = new PFObj(picture_path, x, y, max_pic_width, max_pic_height, pic_width, pic_height, 3);

	p->x = p->real_x = x + (pic_width_all - p->real_w)/2;
	p->y = p->real_y = y + (pic_height_all_eks_text - p->real_h)/2;

	p->real_x -= 1;
	p->real_y -= 1;

	if (pos == j) {
	  x += (pic_width - image_width)/2;
	  y += y_pos_offset * 2; // shift text also
	}

	render->current.add(p);

	S_Touch::get_instance()->register_area(TouchArea(rect(p->x, p->y, p->w, p->h), p->layer,
							 boost::bind(&MovieDB::find_element_and_do_action, this, s)));

      } else {

	string thumbnail = render->image_get_fullpath(s.filenames.front(), full_width, full_height);

	bool thumbnail_ok = file_exists(thumbnail);

	// try thumbnail
	if (!thumbnail_ok && !s.thumbnailed_before_failed) {
	  thumbnail_ok = create_thumbnail(s.filenames.front(), thumbnail, full_width, full_height);
	  if (!thumbnail_ok) {
	    s.thumbnailed_before_failed = true;
	    cur_files[start] = s;
	    // update db
	    update_thumbnailed_status(s.db_id);
	  }
	}

	PObj *p;
	if (thumbnail_ok) {

	  if (pos == j) {
	    y_pos_offset = (pic_height - image_height)/2;

	    x -= (pic_width - image_width)/2;
	    y -= y_pos_offset;
	  }

	  p = new PFObj(s.filenames.front(), x, y, full_width, full_height, pic_width, pic_height, 4);

	  p->x = p->real_x = x + (pic_width_all - p->real_w)/2;
	  p->y = p->real_y = y + (pic_height_all_eks_text - p->real_h)/2;

	} else {

	  pic_width = static_cast<int>(image_width/1.15);
	  pic_height = static_cast<int>(image_height/1.15);
	  pic_width_all = image_width_all;
	  pic_height_all_eks_text = image_height_all_eks_text;

	  int max_pic_width = static_cast<int>(pic_width * 1.40);
	  int max_pic_height = static_cast<int>(pic_height * 1.40);

	  string picture_path = render->default_path + "movie/b_mcover.png";

	  if (pos == j) {
	    pic_width = max_pic_width;
	    pic_height = max_pic_height;
	    pic_width_all += pic_width - image_width;
	    pic_height_all_eks_text += pic_height - image_height;
	    x -= (pic_width - image_width)/2;
	    y_pos_offset = (pic_height - image_height)/2;
	    y -= y_pos_offset;
	  }

	  render->create_scaled_image_wrapper(picture_path, max_pic_width, max_pic_height);
	  // use pic with max_width and max_height and scale it in software
	  p = new PFObj(picture_path, x, y, max_pic_width, max_pic_height, pic_width, pic_height, 4);

	  p->x = p->real_x = x + (pic_width_all - p->real_w)/2;
	  p->y = p->real_y = y + (pic_height_all_eks_text - p->real_h)/2;
	}

	p->real_x -= 1;
	p->real_y -= 1;

	if (pos == j) {
	  x += (pic_width - image_width)/2;
	  y += y_pos_offset * 2;
	}

	S_Touch::get_instance()->register_area(TouchArea(rect(p->x, p->y, p->w, p->h), p->layer,
							 boost::bind(&MovieDB::find_element_and_do_action, this, s)));

	render->current.add(p);
      }
    }

    string name = get_name_from_file(s);

    string old_name = name;

    string_format::format_to_size(name, normal_font, image_width_all-10, false);

    string name_second_part = old_name.substr(name.size());

    if (name_second_part.size() > 0 && name_second_part[0] != ' ') {
      // splitting words
      string::size_type last_whole_word = name.rfind(' ');
      if (last_whole_word != string::npos) {
	name = name.substr(0, last_whole_word);
	name_second_part = old_name.substr(name.size());
      }
    }

    int x_size = string_format::calculate_string_width(name, normal_font);

    string_format::format_to_size(name_second_part, normal_font, image_width_all-10, false);

    int x_size2 = string_format::calculate_string_width(name_second_part, normal_font);

    if (pos == j) {
      render->current.add(new TObj(name, normal_font, x + (image_width_all-x_size)/2, y+image_height_all_eks_text,
				   themes->pictures_marked_font1, themes->pictures_marked_font2,
				   themes->pictures_marked_font3, 3));
      render->current.add(new TObj(name_second_part, normal_font, x + (image_width_all-x_size2)/2,
				   y+image_height_all_eks_text+normal_font_height,
				   themes->pictures_marked_font1, themes->pictures_marked_font2,
				   themes->pictures_marked_font3, 3));
      y -= y_pos_offset;
    } else {
      render->current.add(new TObj(name, normal_font, x + (image_width_all-x_size)/2, y+image_height_all_eks_text,
				   themes->pictures_font1, themes->pictures_font2,
				   themes->pictures_font3, 3));
      render->current.add(new TObj(name_second_part, normal_font, x + (image_width_all-x_size2)/2,
				   y+image_height_all_eks_text+normal_font_height,
				   themes->pictures_font1, themes->pictures_font2,
				   themes->pictures_font3, 3));
    }

    x += image_width_all;

    if (start == cur_files.size()-1) {
      if ((j+1)%images_per_row != 0)
	j += (images_per_row-(j+1)%images_per_row);

      x = x_start;
      y += cur_image_height_all;

      start = 0;
      continue;

    } else
      ++start;

    if ((j+1)%images_per_row == 0) {
      y += cur_image_height_all;
      x = x_start;
    }
  }

  std::ostringstream position;

  if (cur_files.size() > 0) {
    if (search_mode) {
      if (cur_files.size() > 0)
	position << pos_in_list + 1 << "/" << cur_files.size();
      else
	position << "";
    } else
      position << folders.top().second+1 << "/" << cur_files.size();

    int x_size = string_format::calculate_string_width(position.str(), position_font);

    render->current.add(new TObj(position.str(), position_font, conf->p_h_res()-(60+x_size), 20,
				 themes->movie_font1, themes->movie_font2,
				 themes->movie_font3, 3));
  }

  render->draw_and_release("movie-graphical");
}
