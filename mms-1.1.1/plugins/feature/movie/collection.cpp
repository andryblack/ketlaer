#include "collection.hpp"

#include "search.hpp"
#include "busy_indicator.hpp"
#include "shutdown.hpp"
#include "touch.hpp"

using std::string;
using std::vector;
using std::list;

MovieCollection::MovieCollection()
  : MovieDB("collection.db", false)
{
  opts.clear();

  // FIXME: if (!havemydir("movies", &imdb_dir)) check?

  // FIXME: WTF?
  loaded_correctly = false;
}

void MovieCollection::action()
{
  // has sanity check for empty list
  print_info();
}

void MovieCollection::startup_updater()
{
  S_Search::get_instance()->register_module(SearchModule(dgettext("mms-movie", "Video Coll."),
							 boost::bind(&MovieCollection::gen_search_list, this, _1),
							 boost::bind(&MovieCollection::reenter, this, _1)));
}

void MovieCollection::options()
{
  DialogWaitPrint pdialog(dgettext("mms-movie", "No options available for video collection"), 2000);
}

bool MovieCollection::change_dir_to_id(int db_id)
{
  int pos = 0;

  foreach (CIMDBMovie& movie, files) {
    if (db_id == movie.db_id) {
      folders.top().second = pos;
      return true;
    } else
      ++pos;
  }

  return false;
}

void MovieCollection::reenter(int db_id)
{
  if (!loaded_correctly)
    read_dirs();

  if (change_dir_to_id(db_id))
    mainloop();
}

vector<std::pair<string, int> > MovieCollection::gen_search_list(const string& search_word)
{
  vector<std::pair<string, int> > result;

  db_mutex.enterMutex();

  SQLQuery *q = db.query("Actors", ("SELECT DISTINCT Movie.id as id, Movie.title as title FROM %t INNER JOIN AMovie ON Actors.id = AMovie.aid INNER JOIN Movie ON Movie.id = AMovie.mid WHERE fuzzycmp('" +
			 search_word + "', lname, 1) OR fuzzycmp('" + search_word + "', ltitle, 1) OR fuzzycmp('" +
			 search_word + "', ltagline, 1) OR fuzzycmp('" + search_word + "', lplot, 1)").c_str());

  if (q) {
    for (int i = 0; i < q->numberOfTuples(); ++i) {
      SQLRow &row = (*q)[i];
      result.push_back(std::make_pair(row["title"], conv::atoi(row["id"])));
    }
  }
  delete q;

  db_mutex.leaveMutex();

  return result;
}

vector<CIMDBMovie> MovieCollection::parse_dir(const list<string>& dirs)
{
  // Dummy function, this is not needed for this plugin

  std::vector<CIMDBMovie> empty;
  return empty;
}

void MovieCollection::read_dirs()
{
  // try to find the movie
  db_mutex.enterMutex();

  files.clear();
  if (folders.size() > 0)
    folders.pop();

  SQLQuery *q = db.query("Movie", "SELECT id,title FROM %t order by title asc");

  if (q) {
    for (int i = 0; i < q->numberOfTuples(); ++i) {
      SQLRow &movie = (*q)[i];
      CIMDBMovie tmp;
      tmp.m_strTitle = movie["title"];
      tmp.db_id = conv::atoi(movie["id"]);
      tmp.Load(physical, db);
      tmp.name = tmp.m_strTitle;
      files.push_back(tmp);
    }
  }

  delete q;

  db_mutex.leaveMutex();

  list<string> templs; templs.push_back("");
  folders.push(std::make_pair(templs, 0));

  loaded_correctly = true;
}

void MovieCollection::remove_movie(int index)
{
  remove_from_db(index);

  read_dirs();

  if (index <= files.size()-1) {
    folders.pop();
    list<string> templs; templs.push_back("");
    folders.push(std::make_pair(templs, index));
  }
}

// commands
void MovieCollection::secondary_menu()
{
  ExtraMenu em;

  em.add_item(ExtraMenuItem(dgettext("mms-movie", "Print information"), input_master->find_shortcut("action"),
			    boost::bind(&MovieCollection::print_info, this)));

  if (files.size() > 0)
    em.add_item(ExtraMenuItem(dgettext("mms-movie", "Update video information"), input_master->find_shortcut("reget"),
			      boost::bind(&MovieCollection::reget_movie_information, this)));

  if (files.size() > 0)
    em.add_item(ExtraMenuItem(dgettext("mms-movie", "Remove video"),
			      input_master->find_shortcut("delete_file_info"),
			      boost::bind(&MovieCollection::remove_movie, this,
					  folders.top().second)));

  em.add_item(ExtraMenuItem(dgettext("mms-movie", "Search IMDb"), input_master->find_shortcut("search_imdb"),
			    boost::bind(&MovieCollection::search_imdb, this)));

  em.add_item(ExtraMenuItem(dgettext("mms-movie", "Return to startmenu"), input_master->find_shortcut("back"),
			    boost::bind(&MovieCollection::exit, this)));

  add_standard(em);

  foreach (ExtraMenuItem& item, global->menu_items)
    em.add_persistent_item(item);

  conf->s_sam(true);

  em.mainloop();
}

string MovieCollection::mainloop()
{
  BusyIndicator *busy_indicator = S_BusyIndicator::get_instance();
  Shutdown *sd = S_Shutdown::get_instance();

  input_master->set_map("movie");

  Input input;

  bool update_needed = true;

  Movie *mov = get_class<Movie>(dgettext("mms-movie", "Video"));

  while (!exit_loop)
    {
      if (update_needed)
	print(files);

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
	  if (files.size() > 0) {
	    if (mov->opts.display_mode() == dgettext("mms-movie", "list view")) {
	      if (folders.top().second != 0)
		folders.top().second -= 1;
	      else
		folders.top().second = files.size() - 1;
	    } else {
	      prev();
	    }
	  }
 	}
      else if (input.command == "next")
 	{
	  if (files.size() > 0) {
	    if (mov->opts.display_mode() == dgettext("mms-movie", "list view"))
	      folders.top().second = (folders.top().second+1)%files.size();
	    else
	      next();
	  }
 	}
      else if (input.command == "left")
	{
	  if (files.size() > 0)
	    if (mov->opts.display_mode() == dgettext("mms-movie", "icon view"))
	      left();
	}
      else if (input.command == "right")
 	{
	  if (files.size() > 0)
	    if (mov->opts.display_mode() == dgettext("mms-movie", "icon view"))
	      right();
 	}
      else if (input.command == "back")
	{
	  exit();
	}
      else if (input.command == "reget")
	{
	  if (files.size() > 0)
	    reget_movie_information();
	}
      else if (input.command == "search_imdb")
	{
	  search_imdb();
	  read_dirs();
	}
      else if (input.command == "action")
	{
	  action();
	}
      else if (input.command == "search")
	{
	  search_func();
        }
      else if (input.command == "second_action")
	{
	  secondary_menu();
	}
      else
	MovieTemplate<CIMDBMovie>::movie_mainloop_common(input);

      update_needed = !global->check_commands(input);
    }

  exit_loop = false;

  return "";
}
