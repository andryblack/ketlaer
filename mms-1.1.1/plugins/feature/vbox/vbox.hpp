#ifndef VBOX_HPP
#define VBOX_HPP


#include "module.hpp"
#include "options.hpp"
#include "config.h"
#include "vbox.hpp"
#include "vbox_config.hpp"
#include "simplefile.hpp"
#include <time.h>
#include "vboxclient.hpp"
#include "libvbox.hpp"

#ifdef use_notify_area
#include "notify.hpp"
#include "boost.hpp"
#endif

#include "feature/audio/audio_s.hpp"

#define MAXBUF 1024
#define BORDER_LEFT 15
#define BORDER_RIGHT 15
#define BORDER_TOP 20
#define BORDER_BOTTOM 130
#define MAXBOXES 256
#define INTERVALL 60
#define SPACING 10


class VBox : public Module
{
 public:
  VBox();
  
#ifdef use_notify_area
  void ShowNotifyArea();
#endif

  Options* get_opts()
  {
    return 0;
  }

  void startup_updater()
  {}
  bool Connect();
  void Disconnect();
  std::string Toggle(int nr);
  std::string Delete(int nr);
  void toggle_new();
  void toggle_delete();
  std::string GetRecording(int nr);
  bool IsNew(int nr);
  bool CheckDelete();
  void GetData(int nr);
  std::string Refresh();
  void BeginAnimation();
  void EndAnimation();
  std::string Print(std::string status);
  std::string mainloop();
  void exit();
  std::string TouchandPlay(int offset);
  std::string Play();
 private:
	bool exit_loop;
    int curlayer;
    VBoxConfig *vbox_conf;
    
	char buffer[MAXBUF];
	std::string buf;

	cVBOXClient *VBoxC[MAXBOXES];
	int new_messages[MAXBOXES];

    int intervall;
	
	int num_new;
    time_t latest;
    struct tm time, *zeit;
    char *b;
	struct Simplefile cur_track;
	Audio_s *as;
	std::list<std::string> vbox_configs;
	int num_vbox;
	int vboxes;

	std::string header_font;
	std::string normal_font;
	int header_box_size;
	bool animation;
	  
    int i, 					// Counter
    	height,				// height of Listframe
    	width, 				// width of Listframe
    	cursor,				// Cursor-Position on Screen (Ranges from 1 to lines_to_display)
    	first_row,			// First Row to Display (Ranges from 0 to number_of_messages - lines_to_display)
    	number_of_messages,	// Number of Voice - Messages on Server
    	subheader_y,		// Top Anchor of Subheader
    	tableheader_y,	    // Top Anchor of Header for Listview
    	start_x,			// Left Anchor of Listframe
    	start_y,			// Top Anchor of Listframe
    	font_height,		// Height of Font - to calculate Lines on Screen
    	lines_to_display,	// How many Lines to Display - depends on Resolution and Height of Font and Borders
    	anchor,				// Anchor of Cursor
    	col_new,			// X-Position of New-Flag
    	col_time,			// X-Position of Time
    	col_duration,		// X-Position of Duration
    	col_id,				// X-Position of CallerId
    	col_name,			// X-Positioh of Name
    	columns,			// Number of Columns to Display
    	spacing				// Spacing between Columns
    	;
    
    	std::pair<int, int> s_header;		// Size of Header
    	std::pair<int, int> s_new;			// Size of New-Flag
    	std::pair<int, int> s_time;			// Size of Time
    	std::pair<int, int> s_duration;		// Size of Duration
    	std::pair<int, int> s_id;			// Size of CallerId
    	std::pair<int, int> s_name;			// Size of Name
    	std::pair<int, int> s_status;		// Size of Statusline
    	;
    
    // Data of current entry :
    bool 		is_new;		// If Message is new
    bool 		is_marked_deleted;		// If Message is marked to delete
    std::string timestr;	// Time as string
    std::string duration;	// Duration as string
    std::string callerid;	// CallerId
    std::string name;		// Name of Caller
    
    // Configuration (Line by Line)
    std::string c_name;
    std::string c_host;
    int c_port;
    std::string c_username;
    std::string c_password;
};

#endif

