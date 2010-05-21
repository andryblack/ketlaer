#ifndef __CLIENTSTUB_H_
#define __CLIENTSTUB_H_

#include "giosocket.h"
#include "clientstubbase.h"

#include <cc++/thread.h>

#include <stdlib.h>
#include <errno.h>

#include <sstream>
#include <iostream>

using std::stringstream;
using std::ostringstream;
using std::cerr;
using std::endl;

template <typename Ops>
class IMMSClient : public IMMSClientStub, protected GIOSocket, public ost::Thread
{
 public:
 IMMSClient()
   : connected(false), db(get_imms_root("imms2.db").c_str())
    {
      database_exists = file_exists(get_imms_root("imms2.db"));
    }

  ~IMMSClient()
    { terminate(); }

  bool connect()
  {
    int fd = socket_connect(get_imms_root("socket"));
    if (fd > 0)
      {
	init(fd);
	connected = true;
	write_command("IMMS");
	return true;
      }
    cerr << "Connection failed: " << strerror(errno) << endl;
    return false;
  }

  void run()
  {
    GMainLoop *main_loop = g_main_loop_new(NULL, TRUE);
    g_main_loop_run(main_loop);
  }

  int get_rating(const std::string& path)
  {
    int rating = 0;

    if (database_exists) {
      SQLQuery *q = db.query("Identify", ("SELECT uid FROM %t WHERE path = '" + string_format::escape_db_string(path) + "'").c_str());
      if (q && q->numberOfTuples() > 0) {
	std::string uid = (*q)[0]["uid"];

	SQLQuery *q2 = db.query("Ratings", ("SELECT rating FROM %t WHERE uid = '" + uid + "'").c_str());
	if (q && q->numberOfTuples() > 0)
	  rating = conv::atoi((*q)[0]["rating"]);

	delete q2;

      }
      delete q;
    }

    return rating;
  }

  virtual void write_command(const string &line)
  { if (isok()) GIOSocket::write(line + "\n"); }

  virtual void process_line(const string &line)
  {
    stringstream sstr;
    sstr << line;

#if defined(DEBUG) && 1
    std::cout << "< " << line << endl;
#endif

    string command = "";
    sstr >> command;

    if (command == "ResetSelection")
      {
	Ops::reset_selection();
	return;
      }
    if (command == "TryAgain")
      {
	write_command("SelectNext");
	return;
      }
    if (command == "EnqueueNext")
      {
	int next;
	sstr >> next;
	Ops::set_next(next);
	return;
      }
    if (command == "PlaylistChanged")
      {
	IMMSClientStub::playlist_changed(Ops::get_length());
	return;
      }
    if (command == "GetPlaylistItem")
      {
	int i;
	sstr >> i;
	send_item("PlaylistItem", i);
	return;
      }
    if (command == "GetEntirePlaylist")
      {
	for (int i = 0; i < Ops::get_length(); ++i)
	  send_item("Playlist", i);
	write_command("PlaylistEnd");
	return;
      }

    cerr << "IMMS: Unknown command: " << command << endl;
  }
  virtual void connection_lost() { connected = false; }

  bool check_connection()
  {
    if (isok())
      return true;

    system("immsd &");

    return connect();
  }

  bool isok() { return connected; }
 private:
  bool connected;

  SQLDatabase db;

  bool database_exists;

  string get_imms_root(const string &file)
  {
    static string dotimms;
    if (dotimms == "")
      {
	dotimms = s_getenv("HOME");
	dotimms.append("/.imms/");
      }
    return dotimms + file;
  }

  void send_item(const char *command, int i)
  {
    ostringstream osstr;
    osstr << command << " " << i << " " << Ops::get_item(i);
    write_command(osstr.str());
  }
};

#endif
