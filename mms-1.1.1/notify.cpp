
//#define DLOG

#include "notify.hpp"

#include "libfspp.hpp"

extern "C" {
#include <inotifytools/inotifytools.h>
#include <inotifytools/inotify.h>
}

#include "updater.hpp"

#include "boost.hpp"

#include <stack>

#include <cassert>
#include <common.hpp>

using std::list;
using std::vector;
using std::string;
using string_format::str_printf;

pthread_mutex_t Notify::singleton_mutex = PTHREAD_MUTEX_INITIALIZER;

int Notify::notify_timeout()
{
  return 1000; // ms
}

void Notify::check_for_changes()
{
  // minimize locking of the update_items structure, since calling the callback
  // function might take some time
  vector<NotifyUpdate> items_needing_update = get_plugins();

  //  std::cout << "checking for changes:" << items_needing_update.size() << std::endl;

  foreach (NotifyUpdate& update, items_needing_update)
    foreach (notify_item& item, plugins_watching)
      if (!die && update.plugin_name == item.plugin_name) {
	dprintf("would call callback function for: %s\n", update.plugin_name.c_str());
	dprintf("%d:%s\n",  update.type,  update.path.c_str());
	item.callback(update.type, update.path);
	break;
      }
}

Notify::Notify()
  : die(false)
{
  BackgroundUpdater *bg_updater = S_BackgroundUpdater::get_instance();

  bg_updater->timer.add(TimeElement("notify", boost::bind(&Notify::notify_timeout, this),
				    boost::bind(&Notify::check_for_changes, this)));

  inotifytools_initialize();
}

void Notify::stop()
{
  die = true;
  lock.enterMutex();
  terminate(); /* thread */
  lock.leaveMutex();

  foreach (notify_item& item, plugins_watching) {
    for (list<string>::reverse_iterator i = item.dirs.rbegin(),
	   iend = item.dirs.rend(); i != iend; ++i) {
      if (inotifytools_wd_from_filename(i->c_str()) < 1)
	std::cout << "Notify:: Warning: trying to remove non-existing watch from '" << i->c_str() << "'" << std::endl;
      else
	inotifytools_remove_watch_by_filename(i->c_str());
    }
  }
}

notify_item::notify_item(const std::string& pn, const std::list<std::string>& d, const notify_callback_func& c)
  : plugin_name(pn), dirs(d), callback(c)
{}

void Notify::register_plugin(const string& name, const list<string>& dir_passed,
			     const notify_callback_func& callback)
{
  lock.enterMutex();

  std::cout << "Notify:: registered: " << name << std::endl;

  plugins_watching.push_back(notify_item(name, dir_passed, callback));

  lock.leaveMutex();
}

void Notify::run()
{
  lock.enterMutex();
  foreach (notify_item& item, plugins_watching) {
    foreach (string& dir, item.dirs) {
      if (die)
	break;
      fprintf(stderr, "Notify:: watching '%s'\n", dir.c_str());

     if(inotifytools_watch_recursively(dir.c_str(), IN_CREATE | IN_DELETE | IN_DELETE_SELF |
				     IN_CLOSE_WRITE | IN_MOVE_SELF | IN_MOVE | IN_UNMOUNT) < 1){
       //error....
       int errn = inotifytools_error();
       if (errn != ENOSPC)
         fprintf(stderr, str_printf("Notify:: Having troubles with '%s': %s\n", dir.c_str(), s_strerror(errn).c_str()).c_str());
       else{
         int tt = inotifytools_get_num_watches();
         int tmax = inotifytools_get_max_user_watches();
         fprintf(stderr, "\nInotify watch limit is too low (system wide limit: %d, used by mms: %d, used elsewhere: %d).\nTo increase the Inotify watch limit, type 'echo %d > /proc/sys/fs/inotify/max_user_watches' as root or change the value 'fs.inotify.max_user_watches' in /etc/sysctl.conf\n\n", tmax, tt, tmax-tt, tmax*2);
       }
     }

    }
  }
  lock.leaveMutex();

  fprintf(stderr, "Notify:: finished watching all dirs'\n");

  struct inotify_event *event;
  char event_data[4096];

  while (!die){
    event = inotifytools_next_event(-1);

    if (event == NULL)
      break;

    inotifytools_snprintf(event_data, 4096, event, "%w%f!!!!!%e");

    lock.enterMutex();

    bool found_match = false;

    string str_event_data = string(event_data);
    string::size_type pos;

    if ((pos = str_event_data.rfind("!!!!!")) == string::npos) {
      std::cerr << "Notify:: error malformed inotify event" << std::endl;
      lock.leaveMutex();
      continue;
    }

    string str_filename = str_event_data.substr(0, pos);
    string type = str_event_data.substr(pos+5);

    //    std::cout << str_filename << ":" << type << std::endl;

    foreach (notify_item& item, plugins_watching) {
      foreach (string& dir, item.dirs) {
	if (str_filename.find(dir) != string::npos) {
	  //	  std::cout << "found a watching folder:" << item.plugin_name << std::endl;
	  if (type == "CREATE,ISDIR") {
	    std::cout << "notify::updating watch" << std::endl;
	    // teach inotifytools about new dir added...
	    inotifytools_remove_watch_by_filename(dir.c_str());
	    inotifytools_watch_recursively(dir.c_str(), IN_CREATE | IN_DELETE | IN_DELETE_SELF |
					   IN_CLOSE_WRITE | IN_MOVE_SELF | IN_MOVE | IN_UNMOUNT);
	  }

	  // workaround the fact that taglib opens files in write mode for reading
	  bool file_ok = true;
	  if (type == "CLOSE_WRITE,CLOSE")
	    if (modification_time(str_filename) != time(0)) // (time(0) - 1) > 
	      file_ok = false;

	  if (file_ok){
	    /* remove file from internal cache, so that mms is forced to stat() it again */
	    file_tools::remove_from_cache(str_filename);
	    add_notification(item.plugin_name, type, str_filename);
	  }

	  found_match = true;
	  break;
	}
      }
    }

    lock.leaveMutex();
  }
}

void Notify::add_notification(const string& plugin_name, const string& type, const string& path)
{
  //  std::cout << "add notification" << std::endl;
  if (valid_type(type)) {
    //    std::cout << "type ok: " << plugin_name << ":" << type << ":" << path << std::endl;

    NotifyUpdate update = NotifyUpdate(plugin_name, type, path);
    // remove all update_items in queue if this item superseeded them
    if (type == "DELETE_SELF") {
      for (vector<NotifyUpdate>::iterator iter = update_items.begin();
	   iter != update_items.end();) {
	std::cout << "Notify:: checking: " << path << std::endl;
	if (plugin_name == iter->plugin_name && iter->path.find(path) != string::npos) {
	  std::cout << "Notify:: superseeded: " << iter->path << std::endl;
	  iter = update_items.erase(iter);
	} else
	  ++iter;
      }
    }

    for (vector<NotifyUpdate>::iterator iter = update_items.begin();
	 iter != update_items.end(); ++iter)
      if (*iter == update) {
	//	std::cout << "skipping a duplicate update" << std::endl;
	return;
      }

    update_items.push_back(update);
  }
}

bool Notify::valid_type(const string& type)
{
  return (type == "DELETE" || type == "DELETE_SELF" || type == "CREATE" || type == "CREATE,ISDIR" || type == "CLOSE_WRITE,CLOSE" || type.find("MOVED_TO") != string::npos || type.find("MOVED_FROM") != string::npos);
}

NotifyUpdate::NotifyUpdate(const string& plugin, const string& t, const string& the_path)
  : path(the_path), plugin_name(plugin)
{
  if (t == "DELETE")
    type = DELETE_FILE;
  else if (t == "DELETE_SELF")
    type = DELETE_DIR;
  else if (t == "CREATE")
    type = CREATE_FILE;
  else if (t == "CREATE,ISDIR")
    type = CREATE_DIR;
  else if (t == "MOVED_TO")
    type = MOVE;
  else if (t == "CLOSE_WRITE,CLOSE")
    type = WRITE_CLOSE_FILE;
  else
    type = UNKNOWN;

  if (type == DELETE_DIR) {
    if (path[path.size() - 1] == '/')
      path = path.substr(0, path.size()-1);
    string::size_type pos_backslash = path.rfind('/');
    if (pos_backslash != string::npos) {
      path = path.substr(0, pos_backslash+1);
    }
  }
}

const vector<NotifyUpdate> Notify::get_plugins()
{
  lock.enterMutex();
  vector<NotifyUpdate> items = update_items;
  update_items.clear();
  lock.leaveMutex();
  return items;
}
