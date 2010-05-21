#ifndef NOTIFY_HPP
#define NOTIFY_HPP

#include "singleton.hpp"

#include "boost.hpp"

#include <cc++/thread.h>

#include <string>
#include <vector>
#include <list>

class NotifyUpdate
{
public:
  enum notify_update_type { DELETE_FILE = 0, DELETE_DIR, CREATE_FILE, CREATE_DIR, MOVE, WRITE_CLOSE_FILE, UNKNOWN };
  notify_update_type type;
  std::string path;
  std::string plugin_name;

  bool operator==(const NotifyUpdate& other) const
  {
    return type == other.type && plugin_name == other.plugin_name && path == other.path;
  }

  NotifyUpdate(const std::string& plugin_name, const std::string& type,
	       const std::string& path);
};

typedef boost::function<void (NotifyUpdate::notify_update_type,
			      const std::string&)> notify_callback_func;

class notify_item
{
public:

  notify_item(const std::string& pn, const std::list<std::string>& d,
	      const notify_callback_func& callback);

  std::string plugin_name;
  std::list<std::string> dirs;
  notify_callback_func callback;

  bool operator<(const notify_item& other) const
  {
    return plugin_name < other.plugin_name;
  }
};

class Notify : public ost::Thread
{
  bool die;

  // updater
  void check_for_changes();
  int notify_timeout();

  std::vector<notify_item> plugins_watching;
  std::vector<NotifyUpdate> update_items;

  void run();

  ost::Mutex lock;

  void add_notification(const std::string& plugin_name, const std::string& type,
			const std::string& path);
  bool valid_type(const std::string& type);

public:

  Notify();

  static pthread_mutex_t singleton_mutex;

  void stop();

  // will overwrite the watch dir if plugin is already watching something
  void register_plugin(const std::string& name, const std::list<std::string>& dirs,
		       const notify_callback_func& callback);

  // returns all the plugins that needs to update
  const std::vector<NotifyUpdate> get_plugins();
};

typedef Singleton<Notify> S_Notify;

#endif
