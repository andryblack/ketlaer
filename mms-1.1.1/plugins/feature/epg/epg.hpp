#ifndef EPG_H
#define EPG_H

#include "tinyxml.h"

#include "module.hpp"
#include "options.hpp"
#include "epg_config.hpp"

#include "cpp_sqlitepp.hpp"

#include <cc++/thread.h>

#define DB_VERSION 4

#define BORDER_TOP 		(epg->border_top)
#define BORDER_BOTTOM 	(epg->border_bottom)
#define BORDER_LEFT		(epg->border_left)
#define BORDER_RIGHT	(epg->border_right)
#define SPACING			(epg->spacing)

#define SECONDS_PER_MINUTE 60

#define WEEKDAYS		7

enum EpgSource {
    XML,
    SVDRP
};

class BaseChannel {
public:
    BaseChannel() {
    }
    ;
    ~BaseChannel() {
    }
    ;
    int number;
    EpgSource epgsource;
    std::string id;
    std::string display_name;
    std::string logo;
};

class Event {
public:
    time_t str_to_time(const char *s);
    Event();
    ~Event();
    time_t start;
    time_t stop;
    long duration;
    std::string title;
    std::string org_title; // Unconverted (Local) Title - for Timer-association and creation
    std::string desc;
    bool operator<(const Event& rhs) const;
};

class EpgEvent : public Event {
public:
    EpgEvent();
    ~EpgEvent();
    void no_op() {
    }
    ;
    virtual RObj *Print(Render *render, Themes *themes, int x, int y, int width, int height, bool seclected, std::string fontname) = 0;
    virtual void PrintDescription(Render *render, Themes *themes, int width, int height) = 0;
    virtual bool ToggleTimer(Render *render, Themes *themes, int width, int height) = 0;
    virtual bool HasTimer() = 0;
    BaseChannel *basechannel;
};

class XmlEpgEvent : public EpgEvent {
public:
    XmlEpgEvent(BaseChannel *ch, TiXmlElement *element, std::string tz);
    XmlEpgEvent(BaseChannel *ch, time_t _start, long _duration, std::string _title);
    ~XmlEpgEvent();
    RObj *Print(Render *render, Themes *themes, int x, int y, int width, int height, bool selected, std::string fontname);
    void PrintDescription(Render *render, Themes *themes, int width, int height);
    bool ToggleTimer(Render *render, Themes *themes, int width, int height) {
        return false;
    }
    ;
    bool HasTimer() {
        return false;
    }
    ;
};

class Channel : public BaseChannel {
public:
    Channel();
    ~Channel();
    std::vector<EpgEvent *> program;
};

class XMLChannel : public Channel {
public:
    XMLChannel(int _number, TiXmlElement *element);
};

class EpgUpdate : public ost::Thread {
private:
    virtual void run();
};

#ifndef SVDRP_HPP_
#include "svdrp.hpp"
#endif

class Epg : public Module {
protected:

    // SQLDatabase db;

    // ost::Mutex db_mutex;
    ost::Mutex refresh_mutex;
    ost::Mutex svdrp_mutex;
    ost::Mutex search_mutex;

    void reenter();

    // commands
    void print_description();

    bool create_timer(SvdrpTimerEvent *Ste);
    // bool create_timer(SvdrpEpgEvent *_epgevent);
    bool modify_timer(SvdrpTimerEvent *Ste);
    // bool delete_timer(SvdrpEpgEvent *_epgevent);
    bool delete_timer(SvdrpTimerEvent *ste);

    void options();
    void exit();

    bool exit_loop;

    bool valid_program();
    void find_next_program();
    void find_valid_program();

    bool find_logos();

    void parse_channels(std::string _channels, std::vector<int> *_channelnumbers);

    bool move_element_down();
    bool move_element_up();

    // the position of the currently selected channel on the screen.
    // starts at 0 and goes up the number of channels on the screen minus 1.
    int sel_channel_num;

    // iterator pointing the currently selected program.
    std::vector<EpgEvent *>::iterator program_iter;
    std::vector<EpgEvent *>::iterator program_iter_end;

    // then screen spans cur_time to cur_time + some hours depending on the screen
    // size.
    time_t cur_time;
    // sel_time is the start time of the currently selected program
    time_t sel_time;

    int y_start;

    int timeslots;
    int x_start;

    double scale_factor;

    bool epg_script_found;

    EPGConfig *epg_conf;

public:
    Epg();
    ~Epg();

    Options* get_opts() {
        return 0;
    }
    static pthread_mutex_t singleton_mutex;

    EpgUpdate *epg_updater;
    // gcc 2.95 fix
    void print();

    void get_data();
    virtual bool check_tv_data();
    bool read_xml_file(std::vector<Channel> *channel);

    // For refreshing the Data :
    std::vector<Channel> new_channels;

    std::vector<Channel> channels;

    // an iterator to the first channel visible on the screen
    std::vector<Channel>::iterator cur_channel;

    // an iterator to the currentlt selected channel
    std::vector<Channel>::iterator sel_channel;

    bool update_running;

    void update_time();

    void BeginAnimation();
    void EndAnimation();
    void FadeOut();

    std::string mainloop();
    std::string show_timers();
    void print_timers();

    void print_timelines(double diff);
    void print_channel(int y, BaseChannel *cur_chan);
    void print_channel(int y, std::vector<Channel>::iterator cur_chan);
    void print_description(EpgEvent *p);
    bool switch_to_channel();
    bool delete_timer();
    bool toggle_timer();
    bool toggle_timer(EpgEvent *ev);

    std::vector<EpgEvent *> all_events;
    void search_func();
    void search_touch_action(const vector<EpgEvent *>& events, int pos);
    void search_action(const vector<EpgEvent *>& events);
    void search_print(const vector<EpgEvent *>& e);
    void print_marked_search_letter();
    bool search_compare(const EpgEvent *e);
    std::string search_get_title(const EpgEvent *e);

    void epg_print();
    int check_epg_print();
    int check_timers_print();

    void update_epg_datafile();
    int check_epg_datafile();

    void startup_updater();

    void TimeStamp(const std::string message);
    // SVDRP-Stuff
    bool GetSvdrpData();

    void update_svdrp_data();
    int check_svdrp_data();

    std::string timezone;
    
    bool use_svdrp;
    bool pipe_is_nl;
    Svdrp *svdrp;

    std::vector<int> *svdrp_channelnumbers;
    std::vector<SvdrpChannel> SvdrpChannels;

    std::vector<SvdrpTimerEvent *> Timers;
    int timer_lead;
    int timer_trailer;
    std::vector<Recording> *Recordings;
    time_t lastrefresh;

    // Resolution-Independent-Stuff
    bool display_channel_name;
    bool display_channel_logo;
    bool all_logos;

    int max_logo_size_x;

    std::string header_font;
    std::string title_font;
    std::string subtitle_font;
    std::string normal_font;
    std::string annotation_font;
    std::string grid_font;
    std::string search_font;
    std::string search_select_font;
    std::string weekday_font;

    std::pair<int, int> header_size; // Size of Header
    std::pair<int, int> title_size; // Size of Title
    std::pair<int, int> subtitle_size; // Size of Subtitle
    std::pair<int, int> normal_size; // Size of Normal Text
    std::pair<int, int> annotation_size; // Size of Annotaton
    std::pair<int, int> grid_size; // Size of Grid-Font
    std::pair<int, int> channel_size; // Size of Channel-Text
    std::pair<int, int> search_size; // Size of Search-Text
    std::pair<int, int> search_select_size; // Size for Letters below Search-Input
    std::pair<int, int> weekday_size[WEEKDAYS +1]; // Size for Weekdays

    int total_wd_x;

    int header_box_size;
    int search_box_size;

    int logo_x;
    int channel_x;

    int grid_header_x;
    int grid_header_y;
    int grid_x;
    int grid_y;
    int grid_spacing;

    int border_top, border_bottom, border_left, border_right;
    int spacing;

    int interval;

    int timeslot_interval;

    int search_position;

    bool ende;

    std::string weekdays[WEEKDAYS];
    std::pair<int, int> weekday_xy[WEEKDAYS];

    bool rec_on_off;
    int cursor;
    int start;
    int rows;

    int curlayer;
    bool animation;

    SvdrpTimerEvent *curtimer;
    EpgEvent *cur_event;
};

#endif
