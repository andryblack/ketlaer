#ifndef SVDRP_HPP_
#define SVDRP_HPP_

#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <stdlib.h>
#include <string>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <sstream>
#include <iostream>
#include <list>
#include <vector>
#include <map>
#include "common.hpp"

#ifndef EPG_H
#include "epg.hpp"
#endif

#define VIDEO_4_3	0x00 
#define VIDEO_16_9	0x01
#define VIDEO_BW	0x02

#define AUDIO_STEREO 1
#define AUDIO_DUAL 	 2
#define AUDIO_DD_2_0 4
#define AUDIO_DD_5_1 8
#define AUDIO_LISTEN 16
#define AUDIO_NUMB 32

using namespace std;

#define BUFLEN 1024
#define SHORT_BUF 256

#define SECONDS_PER_DAY (24* 60 * 60)

typedef enum SVDRP_RET {};

#define HELPTEXT					214
#define EPG_ENTRY					215
#define IMAGE_DATA					216
#define SERVICE_READY				220
#define SERVICE_CLOSE				221
#define OK							250
#define SEND_EPG					354
#define ABORTED						451
#define SYNTAX_COMMAND				500
#define SYNTAX_ARGUMENT 			501
#define COMMAND_NOT_IMPLEMENTED 	502
#define PARAMETER_NOT_IMPLEMENTED 	504
#define ACTION_REJECTED 			550
#define ACTION_FAILED				554

#define SVDRP_OK					 0

#define SOCKET_FAILED			-1
#define CONNECT_FAILED			-2
#define RECEIVE_FAILED			-3
#define SEND_FAILED				-4
#define NO_HOST					-5
#define UNKNOWN_HOST			-6

#define TIMER_ACTIVE 	1
#define TIMER_INSTANT 	2
#define TIMER_VPS		4
#define TIMER_RECORDING 8

class SvdrpTimerEvent : public Event {
private:
    int number;
    int flag;
    int channel;
    std::string days;
    bool repetitive;
    int wday;
    bool day[WEEKDAYS];

public:
    BaseChannel *basechannel;
    std::vector<EpgEvent *> epgevents;

    int priority;
    int lifetime;

    SvdrpTimerEvent(int _number, int _flags, int _channel, std::string _days, int _wday, time_t _start, time_t _end, int _priority,
            int _lifetime, std::string _filename, std::string _info);
    ~SvdrpTimerEvent();

    int Number() {
        return number;
    }
    void SetNumber(int _number) {
        number = _number;
    }
    bool IsActive() {
        return flag & TIMER_ACTIVE ? true : false;
    }
    bool IsInstant() {
        return flag & TIMER_INSTANT ? true : false;
    }
    bool IsVPS() {
        return flag & TIMER_VPS ? true : false;
    }
    bool IsRecording() {
        return flag & TIMER_RECORDING ? true : false;
    }
    bool IsRepetitive() {
        return repetitive;
    }
    int Channel() {
        return channel;
    }
    std::string Days() {
        return days;
    }
    int Wday() {
        return wday;
    }
    void ClearAssignments();
    bool Day(int _wday) {
        return day[_wday];
    }

    void SetDay(int _wday, bool _value);

    int Flags() {
        return flag;
    }
};

class SvdrpEvent : public EpgEvent {
private:
    int id; // Unique ID (215-E[1]
    int channel; // 215-C(1)
    std::string channel_id; // 215-C(1)
    std::string channel_name; // 215-C(2)
    // time_t start;					// 215-E[2] Begin
    // long duration;				// 215-E[3] Duration
    // time_t stop;					// start + duration
    int tableid; // 215-E[4] TableID
    int version; // 215-E[5] Version

    // std::string title;			// 215-T
    std::string short_title; // 215-S
    // std::string description; 	// 215-D
    time_t vps; // 215-V


public:
    SvdrpEvent(int _id, std::string _channel_id, time_t _start, long _duration, int _tableid, int _version);
    ~SvdrpEvent();

    void SetTitle(std::string _title) {
        title = _title;
    }
    std::string Title() {
        return title;
    }

    void SetOrgTitle(std::string _org_title) {
        org_title = _org_title;
    }
    std::string OrgTitle() {
        return org_title;
    }

    void SetShortTitle(std::string _short_title) {
        short_title = _short_title;
    }
    std::string ShortTitle() {
        return short_title;
    }

    void SetVPS(long _vps) {
        vps = _vps;
    }

    int Id() {
        return id;
    }
    int Channel() {
        return channel;
    }

    time_t Vps() {
        return vps;
    }

    int aspect;
    int sound;

    std::vector<std::string> video; // 215-X 1
    std::vector<std::string> audio; // 215-X 2
    std::vector<std::string> other; // 215-X ?

    int TableID() {
        return tableid;
    }
    int Version() {
        return version;
    }
};

class SvdrpEpgEvent : public SvdrpEvent {
private:
    int timer_num; // Associated Timer	
    SvdrpTimerEvent *timer;
    std::string page_of_pages;
    std::pair<int, int> pages_size;
    int pages_y;
    bool cursor_is_in_header;
    int day_marked;
    int page;
    int pages;
    std::vector<int> index_to_page;
    int index;
    int round;

    char time_str[20];
    std::ostringstream time;
    std::pair<int, int> time_size;

public:
    SvdrpEpgEvent(BaseChannel *ch, int _id, std::string _channel_id, time_t _start, long _duration, int _tableid, int _version);
    ~SvdrpEpgEvent();

    RObj *Print(Render *render, Themes *themes, int x, int y, int width, int height, bool selected, std::string fontname);

    void PrintDetails(Render *render, Themes *themes, int width, int height, bool with_timer, SvdrpTimerEvent *timer);
    void PrintDescription(Render *render, Themes *themes, int width, int height);
    bool ToggleTimer(Render *render, Themes *themes, int width, int height);

    void AssignTimer(SvdrpTimerEvent *_timer) {
        timer = _timer;
    }
    bool HasTimer() {
        return timer == NULL ? false : true;
    }
    SvdrpTimerEvent *Timer() {
        return timer;
    }
    void Cleartimer() {
        timer = NULL;
        timer_num = 0;
    }
};

class Recording : public SvdrpEvent {
private:
    int number;
public:
    Recording(int _number, int _id, std::string _channel_id, time_t _start, long _duration, int _tableid, int _version);
    ~Recording();
    RObj *Print(Render *render, Themes *themes, int x, int y, int width, int height, bool selected, std::string fontname) {
        return NULL;
    }
    ;
    void PrintDescription(Render *render, Themes *themes, int width, int height) {
    }
    ;
    bool ToggleTimer(Render *render, Themes *themes, int width, int height) {
        return false;
    }
    ;
    bool HasTimer() {
        return false;
    }
    ;
    std::string annotation;
    int Number() {
        return number;
    }
};

class SvdrpChannel : public Channel {
public:
    std::string name;
    std::string Shortname;
    std::string Provider;
    int Frequency;
    std::string Param;
    std::string Source;
    int Rate;
    std::string Vpid;
    std::string Apid;
    int Tpid;
    std::string Caid;
    int Sid;
    int Nid;
    int TNid;
    int Tid;
    int Rid;
    std::vector<SvdrpEpgEvent> *Program;

    SvdrpChannel(int _number, std::string _name);
    SvdrpChannel(int _number, std::string _name, std::string _shortname, std::string _provider, int _frequency, std::string _param,
            std::string _source, int _rate, std::string _vpid, std::string _apid, int _tpid, std::string _caid, int _sid, int _nid,
            int _tid, int _rid);
    ~SvdrpChannel();

    bool IsTV() {
        return Vpid != "0";
    }
    bool IsRadio() {
        return Vpid == "0";
    }
};

class Svdrp {
private:
    std::string debug_message;
    int errorcode;
    std::string errormessage;
    std::string message;

    bool connected;
    std::string hostname;
    int port;
    int sckt;
    sockaddr_in m_sockaddr;

    char buffer[BUFLEN], *b;

    int rcv_bytes;
    int snd_bytes;
    struct tm *zeit;
    struct tm *nun;

    map<int, std::string> svdrp_messages;

    bool send(const std::string) const;
    int recv(std::string&) const;
    bool Quit();

    time_t maketime_t(int _year, int _mon, int _day, int _hour, int _min, int _sec);

    int connections;

    bool convert;
    std::string locale;
public:
    Svdrp(std::string _hostname, int _port);
    ~Svdrp();
    bool Connect();
    bool Disconnect();
    bool IsConnected() {
        return connected;
    }
    bool ProcessResult(std::string&, bool *_last);
    int Error() {
        return errorcode;
    }
    std::string ErrorMessage() {
        return errormessage;
    }
    std::string Message() {
        return message;
    }

    bool ListChannels(std::vector<int> *_channelnumbers, std::vector<Channel> *_channels);
    // bool ListChannels(std::vector<Channel> *_channels);

    void ClearTimer(std::vector<SvdrpTimerEvent *> *_timers);
    bool GetTimer(std::vector<SvdrpTimerEvent *> *_timers);
    bool GetTimer(int _t, SvdrpTimerEvent **_timer);

    std::vector<EpgEvent *> GetEpgEvents(BaseChannel *ch, int _channel);

    bool GetListOfRecordings(std::vector<int> *_list_of_recordings);
    bool GetRecording(int _num, Recording **_recording);
    bool GetAllRecordings(std::vector<Recording> *_recordings);

    bool DeleteTimer(int _timer);

    bool NewTimer(int _flags, int _channel, std::string _days, time_t _start, time_t _end, int _priority, int _lifetime,
            std::string _filename, std::string _info);
    bool ModifyTimer(int _timer, int _flags, int _channel, std::string _days, time_t _start, time_t _stop, int _priority,
            int _lifetime, std::string _filename, std::string _info);
    bool ModifyTimerOnOff(int _timer, bool _on);

    bool SwitchtoChannel(int _channel);

    void PrintErrorMessage();

    bool AssignTimersAndEpgEvents(std::vector<Channel> *_channels, std::vector<SvdrpTimerEvent *> *_timers);

    void SetLocale(const std::string _locale) {
        convert = true;
        locale = _locale;
    }
    void ClearLocale() {
        convert = false;
        locale = "";
    }
};

// SocketException class


#ifndef SvdrpException_class
#define SvdrpException_class

#include <string>

class SvdrpException {
public:
    SvdrpException ( std::string s ) : m_s ( s ) {};
    ~SvdrpException () {};

    std::string description() {return m_s;}

private:

    std::string m_s;

};

#endif

#endif /*SVDRP_HPP_*/
