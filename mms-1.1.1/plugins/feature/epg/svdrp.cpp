#include "epg.hpp"
#include "svdrp.hpp"

#include "input.hpp"
#include "common.hpp"
#include "graphics.hpp"
#include "updater.hpp"
#include "common-feature.hpp"
#include "startmenu.hpp"
#include "busy_indicator.hpp"
#include "shutdown.hpp"
#include "extra_menu.hpp"
#include "touch.hpp"

#include "search.hpp"

#include "plugins.hpp"

Svdrp::Svdrp(std::string _hostname, int _port) {
    connections = 0;
    hostname = _hostname;
    port = _port;
    connected = false;
    convert = false;
    locale = "";
    b = &buffer[0];

    svdrp_messages[HELPTEXT] = "Helptext";
    svdrp_messages[EPG_ENTRY] = "Epg-Entry";
    svdrp_messages[IMAGE_DATA] = "Image Data (base 64)";
    svdrp_messages[SERVICE_READY] = "VDR-Service is ready";
    svdrp_messages[SERVICE_CLOSE] = "VDR-Service closes Send-Channel";
    svdrp_messages[SVDRP_OK] = "Requested Action OK, finished";
    svdrp_messages[SEND_EPG] = "Start sending Epg-Data";
    svdrp_messages[ABORTED] = "Requested Action aborted, local Error on processing";
    svdrp_messages[SYNTAX_COMMAND] = "Syntax Error, unknown Command";
    svdrp_messages[SYNTAX_ARGUMENT] = "Syntax Error in Parameter or Argument ";
    svdrp_messages[COMMAND_NOT_IMPLEMENTED] = "Command not Implemented";
    svdrp_messages[PARAMETER_NOT_IMPLEMENTED] = "Parameter not implemented";
    svdrp_messages[ACTION_REJECTED] = "Requested Action rejected";
    svdrp_messages[ACTION_FAILED] = "Requested Action failed";

    svdrp_messages[SOCKET_FAILED] = "Failed creating the Socket";
    svdrp_messages[CONNECT_FAILED] = "Failed connecting";
    svdrp_messages[RECEIVE_FAILED] = "Receive failed";
    svdrp_messages[SEND_FAILED] = "Send failed";
    svdrp_messages[NO_HOST] = "No Host specified";
    svdrp_messages[UNKNOWN_HOST] = "Unknown host";
}

Svdrp::~Svdrp() {
    if (connected) {
        Disconnect();
    }
}

bool Svdrp::Connect() {
    int ret;
    struct hostent *hostp;
    struct hostent defaulthost;
    struct in_addr defaultaddr;
    std::string defaultname;
    char *defaultlist[2];
    std::string svdrl;
    bool last;
    bool result = false;

    if (!connected) {
        if (!hostname.empty()) {
            if (isdigit(hostname.at(1) )) {
                defaultlist[0] = NULL;
                defaultlist[1] = NULL;
                if (inet_aton(hostname.c_str(), &defaultaddr) != 0) {
                    defaultname = hostname;
                    defaultlist[0] = (char*) &defaultaddr;
                    defaulthost.h_name = (char *)defaultname.c_str();
                    defaulthost.h_addr_list = defaultlist;
                    defaulthost.h_length = sizeof(struct in_addr);
                    defaulthost.h_addrtype = AF_INET;
                    defaulthost.h_aliases = 0;

                    hostp = &defaulthost;
                } else {
                    hostp = gethostbyname(hostname.c_str());
                }
            } else {
                hostp = gethostbyname(hostname.c_str());
            }

            if (!hostp) {
                errorcode = UNKNOWN_HOST;
                errormessage = svdrp_messages[errorcode];

                message = string_format::str_printf("Svdrp %d, %s (%d - %s)", errorcode, errormessage.c_str(), errno, strerror(errno));
                print_debug(message);
                result = false;
            } else {
                m_sockaddr.sin_family = AF_INET;
                m_sockaddr.sin_port = htons(port);

                char **p;
                for (p = hostp->h_addr_list; ((p) && (*p)); p++) {
                    sckt = socket(hostp->h_addrtype, SOCK_STREAM, 0);

                    if (sckt < 0) {
                        errorcode = SOCKET_FAILED;
                        errormessage = svdrp_messages[errorcode];
                        
                        message = string_format::str_printf("Svdrp %d %s, (%d - %s)", errorcode, errormessage.c_str(), errno, strerror(errno));

                        print_debug(message);

                    }
                    memcpy((char *)&m_sockaddr.sin_addr, *p, hostp->h_length);

                    int retries = 20, ret = -1;

                    do {
                        ret = connect(sckt, ( sockaddr * ) &m_sockaddr, sizeof (m_sockaddr ));

                        retries--;

                        if (ret != 0 && retries > 0) {
                            errorcode = CONNECT_FAILED;
                            errormessage = svdrp_messages[errorcode];

                            message = string_format::str_printf("Svdrp %d %s, (%d - %s)", errorcode, errormessage.c_str(), errno, strerror(errno));
                            print_debug(message);

                            mmsSleep(2);
                        }
                    } while (ret != 0 && retries > 0);
                    if (ret == 0) {
                        result = true;
                        break;
                    }
                    close(sckt);
                }
                if (ret != SVDRP_OK) {
                    errorcode = CONNECT_FAILED;
                    errormessage = svdrp_messages[errorcode];
                    
                    message = string_format::str_printf("Svdrp %d %s, (%d - %s)", errorcode, errormessage.c_str(), errno, strerror(errno));
                    print_debug(message);
                    
                    result = false;
                }
            }

            /* Set Non-Blocking
             */
            /*
             int flags;
             flags = fcntl(sckt, F_GETFL );
             flags |= O_NONBLOCK;
             fcntl( sckt, F_SETFL, flags );
             */

            if (ProcessResult(svdrl, &last)) {
                connected = true;
                connections++;
            }
            result = connected;
        } else {
            errorcode = NO_HOST;
            errormessage = svdrp_messages[errorcode];
            result = false;
        }
    } else {
        result = false;
    }
    if (!result) {
        PrintErrorMessage();
    }
    return result;
}

bool Svdrp::Quit() {
    bool result = false;
    std::string svdrl;
    bool last;

    if (connected) {
        if (send("QUIT\n")) {
            if (ProcessResult(svdrl, &last)) {
                connected = false;
                connections--;
                result = true;
            }
        }
    }
    connected = false;
    if (!result) {
        PrintErrorMessage();
    }
    return result;
}

bool Svdrp::Disconnect() {
    bool result = true;

    if (connected) {
        result = false;
        if (Quit()) {
            result = true;
        }
        close(sckt);
    }
    if (!result) {
        PrintErrorMessage();
    }
    return result;
}

time_t Svdrp::maketime_t(int _year, int _mon, int _day, int _hour, int _min, int _sec) {

    time_t rawtime;
    struct tm *tm_raw;

    time(&rawtime);
    tm_raw = localtime(&rawtime);

    tm_raw->tm_year = _year - 1900;
    tm_raw->tm_mon = _mon - 1;
    tm_raw->tm_mday = _day;
    tm_raw->tm_hour = _hour;
    tm_raw->tm_min = _min;
    tm_raw->tm_sec = _sec;

    return mktime(tm_raw);
}

void Svdrp::ClearTimer(std::vector<SvdrpTimerEvent *> *_timers) {
    for (std::vector<SvdrpTimerEvent *>::iterator te=_timers->begin(); te != _timers->end(); te++) {
        (*te)->ClearAssignments();
        delete *te;
    }

    _timers->clear();
}

bool Svdrp::GetTimer(std::vector<SvdrpTimerEvent *> *_timers) {
    std::string cmd, svdrl;
    bool last = false;
    bool result = false;
    SvdrpTimerEvent *ti;

    int timer;
    int flags;
    int channel;
    char *daysbuf;
    std::string days;
    char *startbuf;
    time_t start;
    char *stopbuf;
    time_t stop;
    int priority;
    int lifetime;
    char *filebuf;
    std::string filename;
    char *infobuf;
    std::string info;

    int year;
    int mon;
    int day;
    int hour;
    int min;
    int i;
    int wday;

    cmd = string_format::str_printf("LSTT\n");

    if (send(cmd)) {
        do {
            result = ProcessResult(svdrl, &last);

            if (!result) {
                if (errorcode == ACTION_REJECTED) { // No Timers
                    result = true;
                    last = true;
                }
                break;
            }

            if (convert) {
                svdrl = string_format::convert_locale(svdrl, locale, "UTF-8");
            }

            int fields = sscanf(svdrl.c_str(), "%d %d :%d :%a[^:]:%a[^:]:%a[^:]:%d :%d:%a[^:]:%a[^\n^\r]", &timer, &flags,
                    &channel, &daysbuf, &startbuf, &stopbuf, &priority, &lifetime, &filebuf, &infobuf);

            if (fields < 9) {
                fprintf(stderr, "Error parsing Timer-String, Locale of VDR-HOST is %s\n", locale.c_str());
                fprintf(stderr, "Error parsing Timer-String, Number of Colons is %d String is >%s<\n", fields, svdrl.c_str());
                return false;
            }

            days.assign(daysbuf);

            filename.assign(filebuf);

            if (fields > 9) {
                info.assign(infobuf);
            } else {
                info.assign("");
            }

            // Patch "|" in Filename and Info - they stand for ":"
            int colon = 0;

            do {
                colon = filename.find('|', colon);
                if (colon >= 0) {
                    filename.replace(colon, 1, ":");
                }
            } while (colon >= 0 && colon < filename.size());

            colon = 0;

            do {
                colon = info.find('|', colon);
                if (colon >= 0) {
                    info.replace(colon, 1, ":");
                }
            } while (colon >= 0 && colon < info.size());

            if (isdigit(daysbuf[0])) {
                sscanf(daysbuf, "%4d-%2d-%2d", &year, &mon, &day);
                sscanf(startbuf, "%02d%02d", &hour, &min);
                start = maketime_t(year, mon, day, hour, min, 0);
                sscanf(stopbuf, "%02d%02d", &hour, &min);
                stop = maketime_t(year, mon, day, hour, min, 0);
            } else {
                time_t rawtime;
                tm *now;

                time(&rawtime);
                now = localtime(&rawtime);

                sscanf(startbuf, "%02d%02d", &hour, &min);
                now->tm_hour = hour;
                now->tm_min = min;
                now->tm_sec = 0;
                start = mktime(now);

                sscanf(stopbuf, "%02d%02d", &hour, &min);

                now->tm_hour = hour;
                now->tm_min = min;
                now->tm_sec = 0;
                stop = mktime(now);

                wday = now->tm_wday;
                if (wday > 0) {
                    wday -=1;
                } else {
                    wday = 6;
                }

                for (i = 0; ((i < 7) && (days.substr( (wday + i) % WEEKDAYS, 1)) == "-"); i++) {
                }

                debug_message = string_format::str_printf("Today is Day %d next repetitive is %d", wday, (wday + i) % 7);
                print_debug(debug_message);

                start += (i * SECONDS_PER_DAY);
                stop += (i * SECONDS_PER_DAY);
                wday = (wday + i) %WEEKDAYS;
            }

            if ((long)stop < (long)start) {
                stop += SECONDS_PER_DAY;
            }

            debug_message = string_format::str_printf("NewTimer %s %s", filename.c_str(), info.c_str());
            print_debug(debug_message);

            ti = new SvdrpTimerEvent(timer, flags,channel, days, wday, start, stop,
                    priority, lifetime, filename, info);

            if (ti) {
                _timers->push_back(ti);
            }
            free(daysbuf);
            free(startbuf);
            free(stopbuf);
            free(filebuf);
            free(infobuf);
        } while (!last);
    }
    if (!result) {
        PrintErrorMessage();
    }
    return result;
}

bool Svdrp::GetTimer(int _t, SvdrpTimerEvent **_timer) {
    std::string cmd, svdrl;
    bool last = true;
    bool result = false;

    int timer;
    int flags;
    int channel;
    char *daysbuf;
    std::string days;
    char *startbuf;
    long start;
    char *stopbuf;
    long stop;
    int priority;
    int lifetime;
    char *filebuf;
    std::string filename;
    char *infobuf;
    std::string info;

    int year;
    int mon;
    int day;
    int hour;
    int min;
    int i;
    int wday;

    cmd = string_format::str_printf("LSTT %d\n", _t);

    if (send(cmd)) {
        if (ProcessResult(svdrl, &last)) {

            if (convert) {
                svdrl = string_format::convert_locale(svdrl, locale, "UTF-8");
            }

            int fields = sscanf(svdrl.c_str(), "%d %d :%d :%a[^:]:%a[^:]:%a[^:]:%d :%d:%a[^:]:%a[^\n^\r]", &timer, &flags,
                    &channel, &daysbuf, &startbuf, &stopbuf, &priority, &lifetime, &filebuf, &infobuf);

            if (fields < 9) {
                fprintf(stderr, "Error parsing Timer-String, Locale of VDR-HOST is %s\n", locale.c_str());
                fprintf(stderr, "Error parsing Timer-String, Number of Colons is %d String is >%s<\n", fields, svdrl.c_str());
                return false;
            }

            filename.assign(filebuf);

            if (fields > 9) {
                info.assign(infobuf);
            } else {
                info.assign("");
            }

            // Patch "|" in Filename and Info - they stand for ":"
            int colon = 0;

            do {
                colon = filename.find('|', colon);
                if (colon >= 0) {
                    filename.replace(colon, 1, ":");
                }
            } while (colon >= 0 && colon < filename.size());

            colon = 0;

            do {
                colon = info.find('|', colon);
                if (colon >= 0) {
                    info.replace(colon, 1, ":");
                }
            } while (colon >= 0 && colon < info.size());

            if (isdigit(*daysbuf)) {
                sscanf(daysbuf, "%4d-%2d-%2d", &year, &mon, &day);
                sscanf(startbuf, "%02d%02d", &hour, &min);
                start = maketime_t(year, mon, day, hour, min, 0);
                sscanf(stopbuf, "%02d%02d", &hour, &min);
                stop = maketime_t(year, mon, day, hour, min, 0);
            } else {
                time_t rawtime;
                tm *now;

                time(&rawtime);
                now = localtime(&rawtime);

                sscanf(startbuf, "%02d%02d", &hour, &min);
                now->tm_hour = hour;
                now->tm_min = min;
                now->tm_sec = 0;
                start = mktime(now);

                sscanf(stopbuf, "%02d%02d", &hour, &min);

                now->tm_hour = hour;
                now->tm_min = min;
                now->tm_sec = 0;
                stop = mktime(now);

                wday = now->tm_wday;
                if (wday > 0) {
                    wday -=1;
                } else {
                    wday = 6;
                }

                for (i = 0; ((i < WEEKDAYS) && (days.substr( (wday + i) % WEEKDAYS, 1)) == "-"); i++) {
                }

                debug_message = string_format::str_printf("Today is Day %d next repetitive is %d", wday, (wday + i) % 7);
                print_debug(debug_message);

                start += (i * SECONDS_PER_DAY);
                stop += (i * SECONDS_PER_DAY);
                wday = (wday + i) %WEEKDAYS;
            }

            if ((long)stop < (long)start) {
                stop += SECONDS_PER_DAY;
            }

            *_timer = new SvdrpTimerEvent(timer, flags,channel, days, wday, start, stop,
                    priority, lifetime, filename, info);
            free(daysbuf);
            free(startbuf);
            free(stopbuf);
            free(filebuf);
            free(infobuf);

            result = true;
        } else {
            if (errorcode == ACTION_REJECTED) { // No Timers
                result = true;
                last = true;
            }
        }
    }

    if (!result) {
        PrintErrorMessage();
    }
    return result;
}

bool Svdrp::DeleteTimer(int _timer) {
    std::string cmd, svdrl;
    bool result = false;
    bool last = false;

    cmd = string_format::str_printf("DELT %d\n", _timer);

    if (send(cmd)) {
        if (ProcessResult(svdrl, &last)) {
            result = true;
        }
    }
    if (!result) {
        PrintErrorMessage();
    }
    return result;
}

bool Svdrp::NewTimer(int _flags, int _channel, std::string _days, time_t _start, time_t _stop, int _priority, int _lifetime,
        std::string _filename, std::string _info) {

    std::string cmd, svdrl;
    bool result = false;
    bool last = false;

    tm *start_time, *stop_time;

    stop_time = localtime(&_stop);
    string stops = string_format::str_printf("%02d%02d", stop_time->tm_hour, stop_time->tm_min);

    start_time = localtime(&_start);
    string starts = string_format::str_printf("%02d%02d", start_time->tm_hour, start_time->tm_min);

    string timers;

    if (isdigit(*_days.c_str() ) ) {
        timers = string_format::str_printf("%4d-%02d-%02d:%s:%s", start_time->tm_year + 1900, start_time->tm_mon + 1,
                start_time->tm_mday, starts.c_str(), stops.c_str());
    } else {
        timers = string_format::str_printf("%s:%s:%s", _days.c_str(), starts.c_str(), stops.c_str());
    }

    // Patch ":" in Filename and Info - to be "|"
    int colon = 0;

    do {
        colon = _filename.find(':', colon);
        if (colon >= 0) {
            _filename.replace(colon, 1, "|");
        }
    } while (colon >= 0 && colon < _filename.size());

    colon = 0;

    do {
        colon = _info.find(':', colon);
        if (colon >= 0) {
            _info.replace(colon, 1, "|");
        }
    } while (colon >= 0 && colon < _info.size());

    // NEWT 1:8:50:50:Damages - Im Netz der Macht:<bla>

    cmd = string_format::str_printf("NEWT %d:%d:%s:%d:%d:%s:%s\n", _flags, _channel, timers.c_str(), _priority, _lifetime,
            _filename.c_str(), _info.c_str());

    if (convert) {
        cmd = string_format::convert_locale(cmd, "UTF-8", locale);
    }

    print_debug(cmd);

    if (send(cmd)) {
        if (ProcessResult(svdrl, &last)) {
            result = true;
        }
    }
    print_debug(svdrl);

    if (!result) {
        PrintErrorMessage();
    }

    return result;

}

bool Svdrp::ModifyTimer(int _timer, int _flags, int _channel, std::string _days, time_t _start, time_t _stop, int _priority,
        int _lifetime, std::string _filename, std::string _info) {

    std::string cmd, svdrl;
    bool result = false;
    bool last = false;

    tm *start_time, *stop_time;

    stop_time = localtime(&_stop);
    string stops = string_format::str_printf("%02d%02d", stop_time->tm_hour, stop_time->tm_min);

    start_time = localtime(&_start);
    string starts = string_format::str_printf("%02d%02d", start_time->tm_hour, start_time->tm_min);

    string timers;

    if (isdigit(*_days.c_str() ) ) {
        timers = string_format::str_printf("%4d-%02d-%02d:%s:%s", start_time->tm_year + 1900, start_time->tm_mon + 1,
                start_time->tm_mday, starts.c_str(), stops.c_str());
    } else {
        timers = string_format::str_printf("%s:%s:%s", _days.c_str(), starts.c_str(), stops.c_str());
    }

    // Patch ":" in Filename and Info - to be "|"
    int colon = 0;

    do {
        colon = _filename.find(':', colon);
        if (colon >= 0) {
            _filename.replace(colon, 1, "|");
        }
    } while (colon >= 0 && colon < _filename.size());

    colon = 0;

    do {
        colon = _info.find(':', colon);
        if (colon >= 0) {
            _info.replace(colon, 1, "|");
        }
    } while (colon >= 0 && colon < _info.size());

    // NEWT 1:8:50:50:Damages - Im Netz der Macht:<bla>

    cmd = string_format::str_printf("MODT %d %d:%d:%s:%d:%d:%s:%s\n", _timer, _flags, _channel, timers.c_str(), _priority,
            _lifetime, _filename.c_str(), _info.c_str());

    if (convert) {
        cmd = string_format::convert_locale(cmd, "UTF-8", locale);
    }

    print_debug(cmd);

    if (send(cmd)) {
        if (ProcessResult(svdrl, &last)) {
            result = true;
        }
    }
    print_debug(svdrl);

    if (!result) {
        PrintErrorMessage();
    }

    return result;

}

bool Svdrp::ModifyTimerOnOff(int _timer, bool _on) {
    std::string cmd, svdrl;
    bool result = false;
    bool last = false;

    cmd = string_format::str_printf("MODT %d %s\n", _timer, _on ? "ON" : "OFF");

    if (send(cmd)) {
        if (ProcessResult(svdrl, &last)) {
            result = true;
        }
    }

    if (!result) {
        PrintErrorMessage();
    }

    return result;
}

bool Svdrp::SwitchtoChannel(int _channel) {
    std::string cmd, svdrl;
    bool result = false;
    bool last = false;

    cmd = string_format::str_printf("CHAN %d\n", _channel);

    if (send(cmd)) {
        if (ProcessResult(svdrl, &last)) {
            result = true;
        }
    }

    if (!result) {
        PrintErrorMessage();
    }

    return result;
}

std::vector<EpgEvent *> Svdrp::GetEpgEvents(BaseChannel *ch, int _channel) {
    std::string cmd, svdrl;
    std::vector<EpgEvent *> _program;
    bool last = false;
    bool result = false;
    std::vector<EpgEvent *>::iterator e;
    SvdrpEpgEvent *ev;

    int pos;

    int id;

    char *chidbuf;
    std::string channel_id;

    char *namebuf;
    std::string channel_name;
    std::string title;
    std::string short_title;
    std::string description;
    std::string audio_prop;

    time_t start;
    long duration;
    time_t vps;

    int tableid;
    int version;

    int x_type;

    char token;

    cmd = string_format::str_printf("LSTE %d\n", _channel);

    if (send(cmd)) {
        do {
            result = ProcessResult(svdrl, &last);

            if (!result || last) {
                if (errorcode == ACTION_REJECTED) { // No Epg-data
                    result = true;
                    last = true;
                }
                break;
            }

            std::string org;
            sscanf(svdrl.c_str(), "%c", &token);
            pos = svdrl.find(" ");
            svdrl.erase(0, pos + 1);

            org = svdrl;
            if (convert) {
                svdrl = string_format::convert_locale(svdrl, locale, "UTF-8");
            }

            switch (token) {
            case 'C': // Channelinfo
                sscanf(svdrl.c_str(), "%a[^ ] %a[^\n^\r]", &chidbuf, &namebuf);

                channel_id.assign(chidbuf);
                channel_name.assign(namebuf);

                break;
            case 'E': // Event (Make new Event - Entry)
                sscanf(svdrl.c_str(), "%d %ld %ld %x %x", &id, &start, &duration, &tableid, &version);

                ev = new SvdrpEpgEvent(ch, id, channel_id, start, duration, tableid, version);
                break;
            case 'T': // Title
                title = svdrl;
                pos = title.find("\n");
                if (pos > 0) {
                    title.erase(pos);
                }
                if (ev) {
                    ev->SetTitle(title);
                }
                pos = org.find("\n");
                if (pos > 0) {
                    org.erase(pos);
                }
                if (ev) {
                    ev->SetOrgTitle(org);
                }
                break;
            case 'S': // Short Title
                short_title = svdrl;

                pos = short_title.find("\n");
                if (pos > 0) {
                    short_title.erase(pos);
                }
                if (ev) {
                    ev->SetShortTitle(short_title);
                }
                break;
            case 'D': // Description
                description = svdrl;

                pos = description.find("\n");
                if (pos > 0) {
                    description.erase(pos);
                }

                if (ev) {
                    ev->desc = description;
                }
                break;
            case 'X': // Info for Video/Audiostream
                sscanf(svdrl.c_str(), "%d ", &x_type);
                svdrl.erase(0, 2);

                switch (x_type) {
                case 1: // Video-Description
                    ev->video.push_back(svdrl);
                    if (svdrl.find("16:9")) {
                        ev->aspect |= VIDEO_16_9;
                    }
                    break;
                case 2: // Audio-Description
                    ev->audio.push_back(svdrl);
                    audio_prop = string_format::lowercase(svdrl);

                    if (audio_prop.find(string_format::lowercase("stereo")) != string::npos) {
                        ev->sound |= AUDIO_STEREO;
                    }

                    if (audio_prop.find(string_format::lowercase(dgettext("mms-epg", "Dualchannel"))) != string::npos
                            || audio_prop.find(string_format::lowercase(dgettext("mms-epg", "2-channel"))) != string::npos) {
                        ev->sound |= AUDIO_DUAL;
                    }
                    if (audio_prop.find(string_format::lowercase(dgettext("mms-epg", "Dolby Digital"))) != string::npos) {
                        ev->sound |= AUDIO_DD_2_0;
                        if (audio_prop.find(string_format::lowercase("5.1")) != string::npos) {
                            ev->sound |= AUDIO_DD_5_1;
                            ev->sound ^= AUDIO_DD_2_0;
                        }
                    }
                    if (audio_prop.find(string_format::lowercase(dgettext("mms-epg", "Listening Movie"))) != string::npos) {
                        ev->sound |= AUDIO_LISTEN;
                    }
                    break;
                default: // Other
                    ev->other.push_back(svdrl);
                    break;
                }

                break;
            case 'V': // VPS-Time given
                sscanf(svdrl.c_str(), "%ld", &vps);
                if (ev) {
                    ev->SetVPS(vps);
                }
                break;
            case 'e': // Event finished
                // Sort into vector
                for (e = _program.begin(); e != _program.end() && start > (*e)->start; e++) {
                }
                if (e == _program.end()) {
                    _program.push_back(ev);
                } else {
                    _program.insert(e, ev);
                }
                break;
            case 'c': // Channel finished
                break;
            default:
                fprintf(stderr, "Unknown Token <%c>!\n", token);
                break;
            }
        } while (!last);
    }

    if (!result) {
        PrintErrorMessage();
    }
    return _program;
}

bool Svdrp::GetListOfRecordings(std::vector<int> *_list_of_recordings) {
    std::string cmd, svdrl;
    bool last = false;
    bool result = false;

    int rec_num;

    cmd = string_format::str_printf("LSTR\n");
    if (send(cmd)) {
        do {
            result = ProcessResult(svdrl, &last);

            if (!result) {
                if (errorcode == ACTION_REJECTED) { // No Recordings
                    result = true;
                    last = true;
                }
                break;
            }

            sscanf(svdrl.c_str(), "%d ", &rec_num);
            if (rec_num > 0) {
                _list_of_recordings->push_back(rec_num);
            }
        } while (!last);
    }

    if (!result) {
        PrintErrorMessage();
    }
    return result;
}

bool Svdrp::GetRecording(int _num, Recording **_recording) {
    std::string cmd, svdrl;
    bool last = false;
    bool result = false;
    std::vector<Recording>::iterator e;
    Recording *re;

    int pos;

    int id;

    char *chidbuf;
    std::string channel_id;

    char *namebuf;
    std::string channel_name;
    std::string title;
    std::string short_title;
    std::string description;
    char *annobuf;

    time_t start;
    long duration;
    time_t vps;

    int tableid;
    int version;

    int x_type;

    char token;

    cmd = string_format::str_printf("LSTR %d\n", _num);

    if (send(cmd)) {
        do {
            result = ProcessResult(svdrl, &last);

            if (!result || last)
                break;

            sscanf(svdrl.c_str(), "%c", &token);
            pos = svdrl.find(" ");
            svdrl.erase(0, pos + 1);

            if (convert) {
                svdrl = string_format::convert_locale(svdrl, locale, "UTF-8");
            }

            switch (token) {
            case 'C': // Channelinfo
                sscanf(svdrl.c_str(), "%a[^ ] %a[^\n^\r]", &chidbuf, &namebuf);

                channel_id.assign(chidbuf);
                channel_name.assign(namebuf);
                break;
            case 'E': // Event (Make new Event - Entry)
                sscanf(svdrl.c_str(), "%d %ld %ld %x %x", &id, &start, &duration, &tableid, &version);

                re = new Recording(_num, id, channel_id, start, duration, tableid, version);
                break;
            case 'T': // Title
                title = svdrl;
                pos = title.find("\n");
                if (pos > 0) {
                    title.erase(pos);
                }
                if (re) {
                    re->SetTitle(title);
                }
                break;
            case 'S': // Short Title
                short_title = svdrl;

                pos = short_title.find("\n");
                if (pos > 0) {
                    short_title.erase(pos);
                }
                if (re) {
                    re->SetShortTitle(short_title);
                }
                break;
            case 'D': // Description
                description = svdrl;

                pos = description.find("\n");
                if (pos > 0) {
                    description.erase(pos);
                }

                if (re) {
                    re->desc = description;
                }
                break;
            case 'X': // Info for Video/Audiostream
                sscanf(svdrl.c_str(), "%d ", &x_type);
                svdrl.erase(0, 2);

                if (re) {
                    switch (x_type) {
                    case 1: // Video-Description
                        re->video.push_back(svdrl);
                        break;
                    case 2: // Audio-Description
                        re->audio.push_back(svdrl);
                        break;
                    default: // Other
                        re->other.push_back(svdrl);
                        break;
                    }
                }
                break;
            case 'V': // VPS-Time given
                sscanf(svdrl.c_str(), "%ld", &vps);
                if (re) {
                    re->SetVPS(vps);
                }
                break;
            case '@': // Annotation
                sscanf(svdrl.c_str(), "%a[^\n^\r]", &annobuf);
                if (re) {
                    re->annotation.assign(annobuf);
                }
                break;
            default:
                fprintf(stderr, "Unknown Token <%c>!\n", token);
                break;
            }
        } while (!last);
    }
    if (!result) {
        PrintErrorMessage();
        delete re;
        *_recording = NULL;
    } else {
        *_recording = re;
    }
    return result;
}

bool Svdrp::GetAllRecordings(std::vector<Recording> *_list_of_recordings) {
    std::string cmd, svdrl;
    bool result = false;

    Recording *rec;

    std::vector<int> recs;

    if (GetListOfRecordings(&recs)) {
        result = true;
        if (!recs.empty()) {
            int i = 0;
            do {
                GetRecording(i + 1, &rec);
                if (rec) {
                    _list_of_recordings->push_back(*rec);
                } else {
                    result = false;
                    break;
                }
                i++;
            } while (i < recs.size());
        }
    }

    if (!result) {
        PrintErrorMessage();
    }
    return result;
}

bool Svdrp::ListChannels(std::vector <int> *_channelnumbers, std::vector<Channel> *_channels) {
    std::vector<int>::iterator it;
    SvdrpChannel *ch;
    bool last = true;
    bool result = false;

    std::string cmd, svdrl;

    cmd = string_format::str_printf("LSTC\n");

    if (send(cmd)) {
        do {
            result = ProcessResult(svdrl, &last);

            if (convert) {
                svdrl = string_format::convert_locale(svdrl, locale, "UTF-8");
            }

            std::string a_string;
            int number;

            char *namebuf;
            std::string name;
            std::string shortname;
            std::string provider;

            int frequency;
            char *parambuf;
            std::string param;
            char *sourcebuf;
            std::string source;
            int rate;
            char *vpidbuf;
            std::string vpid;
            char *apidbuf;
            std::string apid;
            int tpid;
            char *caidbuf;
            std::string caid;
            int sid, nid, tid, rid;

            int fields = sscanf(svdrl.c_str(), "%d %a[^:]:%d :%a[^:]:%a[^:] :%d :%a[^:]:%a[^:]:%d :%a[^:]:%d :%d :%d :%d ",
                    &number, &namebuf, &frequency, &parambuf, &sourcebuf, &rate, &vpidbuf, &apidbuf, &tpid, &caidbuf, &sid, &nid,
                    &tid, &rid);

            if (fields != 14) {
                return false;
            }

            int pos;
            a_string.assign(namebuf);

            pos = a_string.find_last_of(";");
            if (pos >= 0) {
                provider = a_string.substr(pos + 1);
                a_string.erase(pos);
            } else {
                provider = "";
            }

            pos = a_string.find(",");
            if (pos > 0) {
                shortname = a_string.substr(pos + 1);
                a_string.erase(pos);
            } else {
                shortname = "";
            }

            pos = a_string.find(";");
            if (pos >= 0) {
                name = a_string.substr(0, pos);
            } else {
                name = a_string;
            }

            param.assign(parambuf);
            source.assign(sourcebuf);
            vpid.assign(vpidbuf);
            apid.assign(apidbuf);
            caid.assign(caidbuf);

            if (_channelnumbers) {
                for (it = _channelnumbers->begin(); it != _channelnumbers->end(); it++) {
                    if (*it == number) {
                        ch
                                = new SvdrpChannel(number, name, shortname, provider, frequency, param, source, rate, vpid, apid, tpid, caid, sid, nid, tid, rid);
                        if (ch) {
                            _channels->push_back(*ch);
                        }
                    }
                }
            } else {
                ch
                        = new SvdrpChannel(number, name, shortname, provider, frequency, param, source, rate, vpid, apid, tpid, caid, sid, nid, tid, rid);
                if (ch) {
                    _channels->push_back(*ch);
                }
            }

            free(namebuf);
            free(parambuf);
            free(sourcebuf);
            free(vpidbuf);
            free(apidbuf);
            free(caidbuf);

            result = true;
        } while (!last);
    }
    if (!result) {
        PrintErrorMessage();
    }
    return result;
}

void Svdrp::PrintErrorMessage() {
    print_info(string_format::str_printf("Svdrp-Error : %4d, %s >%s<\n", errorcode, errormessage.c_str(), message.c_str()));
}

bool Svdrp::send(const std::string _s) const {
    int status =:: send ( sckt, _s.c_str(), _s.size(), MSG_NOSIGNAL );

    if ( status == -1 ) {
        return false;
    } else {
        return true;
    }
}

int Svdrp::recv(std::string& _s) const {
    bool fully_received = false;
    static std::string rcvd;
    int pos;
    char buf [ BUFLEN + 1 ];

    _s = rcvd;

    int status, len = 0;

    if (!_s.empty()) {

        pos = _s.find("\n");

        if (pos == 0) {
            _s = "";
            rcvd = "";
            len = strlen(_s.c_str());
            return len;
        } else if (pos > 0) {
            rcvd = _s.substr(pos + 1);
            _s.erase(pos + 1);
            len = strlen(_s.c_str());
            return len;
        }
    }

    memset(buf, 0, BUFLEN + 1);
    status =:: recv (sckt, buf, BUFLEN, 0 );

    while (status != -1 && status != 0 && !fully_received) {

        if ( status == -1 ) {
            return 0;
        } else if ( status == 0 ) {
            return 0;
        } else {
            rcvd.assign(&buf[0]);
            _s.append(rcvd);
            len += status;
            pos = _s.find("\n");
            if (pos >= 0) {
                if (pos == 0) {
                    _s = "";
                    rcvd = _s.substr(1);
                } else {
                    rcvd = _s.substr(pos + 1);
                    _s.erase(pos + 1);
                }
                fully_received = true;
            } else {
                fully_received = false;
                memset ( buf, 0, BUFLEN + 1 );
                status = ::recv (sckt, buf, BUFLEN, 0 );
            }
        }
    }

    return len;
}

bool Svdrp::ProcessResult(std::string& rcv, bool *_last) {
    bool result = false;
    int returncode;
    *_last = true;

    recv(rcv);

    if (!rcv.empty()) {

        if (rcv.at(3) == '-') {
            sscanf(rcv.c_str(), "%d-", &returncode);
            *_last = false;
        } else {
            sscanf(rcv.c_str(), "%d ", &returncode);
        }
        rcv.erase(0, 4);

        errorcode = returncode;
        errormessage = svdrp_messages[errorcode];
        message = rcv;

        switch (returncode) {
        case SERVICE_READY:
        case SERVICE_CLOSE:
        case EPG_ENTRY:
        case OK:
            result = true;
            break;
        case HELPTEXT:
            perror("Server terminated !");
            break;
        case IMAGE_DATA:
            perror("Image-Data!");
            break;
        default:
            break;
        }
        return result;
    }
    return result;
}

SvdrpChannel::SvdrpChannel(int _number, std::string _name) :
    Channel() {
    number = _number;
    name = _name;
    epgsource = SVDRP;
    display_name = _name;
    logo = "";

    // Program = new std::vector<SvdrpEpgEvent>;
}

SvdrpChannel::SvdrpChannel(int _number, std::string _name, std::string _shortname, std::string _provider, int _frequency,
        std::string _param, std::string _source, int _rate, std::string _vpid, std::string _apid, int _tpid, std::string _caid,
        int _sid, int _nid, int _tid, int _rid) :
    Channel() {

    char cid[256];

    if (_rid)
        snprintf(cid, sizeof(cid), "-%d-%d-%d-%d", _nid, _tid, _sid, _rid);
    else
        snprintf(cid, sizeof(cid), "-%d-%d-%d", _nid, _tid, _sid);

    id = _source;
    id.append((char *)&cid[0]);

    number = _number;
    name = _name;
    epgsource = SVDRP;

    Shortname = _shortname;
    Provider = _provider;
    Frequency = _frequency;
    Param = _param;
    Source = _source;
    Rate = _rate;
    Vpid = _vpid;
    Apid = _apid;
    Tpid = _tpid;
    Caid = _caid;
    Sid = _sid;
    Nid = _nid;
    Tid = _tid;
    Rid = _rid;

    display_name = _name;
    logo = "";
}

SvdrpChannel::~SvdrpChannel() {
}

SvdrpTimerEvent::SvdrpTimerEvent(int _number, int _flag, int _channel, std::string _days, int _wday, time_t _start, time_t _stop,
        int _priority, int _lifetime, std::string _filename, std::string _info) :
    Event() {

    number = _number;
    flag = _flag;
    channel = _channel;
    days = _days;
    wday = _wday;
    start = _start;
    stop = _stop;
    duration = stop - start;
    priority = _priority;
    lifetime = _lifetime;
    title = _filename;
    desc = _info;
    epgevents.clear();

    repetitive = false;

    if (!isdigit(*_days.c_str())) {
        repetitive = true;
    }

    for (int i = 0; i < WEEKDAYS; i++) {
        day[i] = false;
    }

    if (!isdigit(*_days.c_str())) {
        for (int i = 0; i < WEEKDAYS; i++) {
            if (days.substr(i, 1) != "-") {
                day[i] = true;
            }
        }
    }
}

SvdrpTimerEvent::~SvdrpTimerEvent() {

}

void SvdrpTimerEvent::ClearAssignments() {
    std::vector<EpgEvent *>::iterator ev;
    SvdrpEpgEvent *sev;

    for (ev = epgevents.begin(); ev != epgevents.end(); ev++) {
        sev = (SvdrpEpgEvent *)(*ev);

        sev->Cleartimer();
    }
    epgevents.clear();
}

void SvdrpTimerEvent::SetDay(int _wday, bool _value) {
    int i;
    char time_str[20];
    std::string wd[WEEKDAYS] = { "M", "T", "W", "T", "F", "S", "S" };

    strftime(time_str, 20, "%F", localtime(&start));

    day[_wday] = _value;

    repetitive = false;
    days.assign("");

    for (i = 0; i < WEEKDAYS; i++) {
        if (day[i]) {
            repetitive = true;
            days.append(wd[i]);
        } else {
            days.append("-");
        }
    }
    if (!repetitive) {
        days = string_format::str_printf("%s", &time_str[0]);
    }
    fprintf(stderr, "SetDay %d, %s, %s\n", _wday, _value ? "TRUE" : "FALSE", days.c_str());
}

SvdrpEvent::SvdrpEvent(int _id, std::string _channel_id, time_t _start, long _duration, int _tableid, int _version) :
    EpgEvent() {
    id = _id;
    channel_id = _channel_id;
    start = _start;
    duration = _duration;
    stop = start + duration;
    tableid = _tableid;
    version = _version;
}

SvdrpEvent::~SvdrpEvent() {
    video.clear();
    audio.clear();
    other.clear();
}

SvdrpEpgEvent::SvdrpEpgEvent(BaseChannel *ch, int _id, std::string _channel_id, time_t _start, long _duration, int _tableid,
        int _version) :
    SvdrpEvent(_id, _channel_id, _start, _duration, _tableid, _version) {
    basechannel = ch;
    timer = NULL;
    sound = 0;
    aspect = 0;
}

SvdrpEpgEvent::~SvdrpEpgEvent() {
}

RObj *SvdrpEpgEvent::Print(Render *render, Themes *themes, int x, int y, int width, int height, bool selected, std::string fontname) {
    RObj *r;
    string name = title;
    string_format::format_to_size(name, fontname, width - 10, false);

    if (selected) {
        if (timer == NULL) {
            r = new RObj(x, y, width - 5, height,
                    themes->epg_marked1,
                    themes->epg_marked2,
                    themes->epg_marked3,
                    themes->epg_marked4, 3);
        } else {
            r = new RObj(x, y, width - 5, height,
                    themes->epg_timer_marked1,
                    themes->epg_timer_marked2,
                    themes->epg_timer_marked3,
                    themes->epg_timer_marked4, 3);
        }
        render->current.add(r);
        render->current.add(new TObj(name, fontname, x + 2, y,
                themes->epg_marked_font1,
                themes->epg_marked_font2,
                themes->epg_marked_font3, 4));
    } else {
        if (timer == NULL) {
            r = new RObj(x, y, width - 5, height,
                    themes->epg_boxes1,
                    themes->epg_boxes2,
                    themes->epg_boxes3,
                    themes->epg_boxes4, 3);
        } else {
            r = new RObj(x, y, width - 5, height,
                    themes->epg_timer_boxes1,
                    themes->epg_timer_boxes2,
                    themes->epg_timer_boxes3,
                    themes->epg_timer_boxes4, 3);
        }
        render->current.add(r);
        render->current.add(new TObj(name, fontname, x + 2, y,
                themes->epg_boxes_font1,
                themes->epg_boxes_font2,
                themes->epg_boxes_font3, 4));
    }
    return r;
}

void SvdrpEpgEvent::PrintDescription(Render *render, Themes *themes, int width, int height) {
    InputMaster *input_master = S_InputMaster::get_instance();
    Epg *epg = get_class<Epg>(dgettext("mms-epg", "Electronic Program Guide"));
    Input input;
    BusyIndicator *busy_indicator = S_BusyIndicator::get_instance();

    cursor_is_in_header = false;
    day_marked = 0;

    index = 0;
    round = 1;

    string::iterator it = desc.begin();
    std::ostringstream next_word;

    int y= BORDER_TOP;

    bool cancel = false;

    page = 0;
    pages = 0;

    index_to_page.clear();

    index_to_page.push_back(-1);

    epg->animation = true;

    while (!cancel) {

        PrintDetails(render, themes, width, height, false, NULL);

        epg->animation = false;

        bool refresh = false;
        while (!refresh && !cancel) {
            input = input_master->get_input_busy_wrapped(busy_indicator);

            if (input.key == "touch_input") {
                cancel = true;
                refresh = false;
                break;
            }

            if (input.command == "back" || input.command == "cancel") {
                cancel = true;
                refresh = false;
            } else if (input.command == "action") {
                cancel = true;
                refresh = false;
            } else if (input.command == "toggle_timer") {
                cancel = true;
                refresh = true;
                epg->toggle_timer(this);
            } else if (input.command == "prev" && page > 0) {
                page--;
                index = index_to_page[page];
                refresh = true;
            } else if (input.command == "next" && page < pages) {
                page++;
                index = index_to_page[page];
                refresh = true;
            }
        }
        ++round;
        y = BORDER_TOP;
    }

    epg->FadeOut();
}

void SvdrpEpgEvent::PrintDetails(Render *render, Themes *themes, int width, int height, bool with_timer, SvdrpTimerEvent *timer) {

    int nl = 0, pos = 0;

    Epg *epg = get_class<Epg>(dgettext("mms-epg", "Electronic Program Guide"));

    render->prepare_new_image();

    fprintf(stderr, "Animate : %s\n", epg->animation ? "TRUE" : "FALSE");

    if (epg->animation) {
        epg->BeginAnimation();
    }

    int y= BORDER_TOP;

    PObj *icon;

    if (with_timer) {
        icon = new PFObj(themes->startmenu_timer, BORDER_LEFT, y, epg->header_box_size, epg->header_box_size, 1, true);
    } else {
        icon = new PFObj(themes->startmenu_epg, BORDER_LEFT, y, epg->header_box_size, epg->header_box_size, 1, true);
    }
    S_Touch::get_instance()->register_area(TouchArea(rect(icon->x, icon->y, icon->w, icon->h), icon->layer, boost::bind(&SvdrpEpgEvent::no_op,
            this)));

    render->current.add(new PObj(themes->epg_background, 0, 0, 0, SCALE_FULL));

    render->current.add(icon);

    std::string detail_header;

    if (with_timer) {
        if (timer == NULL) {
            detail_header = dgettext("mms-epg", "EPG - Create Timer");
        } else {
            detail_header = dgettext("mms-epg", "EPG - Modify Timer");
        }
    } else {
        detail_header = dgettext("mms-epg", "EPG");
    }
    render->current.add(new TObj(detail_header, epg->header_font, BORDER_LEFT + epg->header_box_size + SPACING, y + (epg->header_box_size - epg->header_size.second) / 2,
            themes->epg_header_font1, themes->epg_header_font2,
            themes->epg_header_font3, 1));

    y += epg->header_size.second + SPACING;

    page_of_pages = string_format::str_printf("%d/%d", page + 1, pages + 1);
    pages_size = string_format::calculate_string_size(page_of_pages, epg->annotation_font);

    if (with_timer) {
        render->current.add(new RObj(BORDER_LEFT, y - SPACING/2, width - BORDER_LEFT - BORDER_RIGHT, 2 * (epg->weekday_size[0].second + SPACING),
                themes->general_rectangle_color1, themes->general_rectangle_color2,
                themes->general_rectangle_color3, themes->general_rectangle_color4, 1));

        int wd_ts_x= BORDER_LEFT + SPACING;
        int wd_ts_y = y;

        for (int wd = 0; wd < WEEKDAYS; wd++) {
            epg->weekday_xy[wd].first = 0;
            epg->weekday_xy[wd].second = 0;

            epg->weekday_xy[wd].first = wd_ts_x
                    + (int) ( (width - BORDER_LEFT - BORDER_RIGHT - ( 2 * SPACING ) - (epg->total_wd_x ) ) / 7 * wd );
            epg->weekday_xy[wd].second = wd_ts_y;

            if (wd < WEEKDAYS - 1) {
                if ((wd_ts_x + epg->weekday_size[wd + 1].first + epg->weekday_size[wd + 1].second + SPACING) > width - BORDER_LEFT
                        - BORDER_RIGHT) {
                    wd_ts_y += SPACING + epg->weekday_size[wd].second;
                    wd_ts_x = BORDER_LEFT + SPACING;
                }
            }

            if (timer->Day(wd)) {
                render->current.add(new PFObj(themes->epg_checked, epg->weekday_xy[wd].first, epg->weekday_xy[wd].second,
                        epg->weekday_size[wd].second, epg->weekday_size[wd].second, 3, true));
                render->current.add(new TObj(epg->weekdays[wd], epg->weekday_font, epg->weekday_xy[wd].first + epg->weekday_size[wd].second + SPACING/2, epg->weekday_xy[wd].second,
                        themes->epg_timer_marked1, themes->epg_timer_marked2, themes->epg_timer_marked3, 3));
            } else {
                render->current.add(new PFObj(themes->epg_unchecked, epg->weekday_xy[wd].first, epg->weekday_xy[wd].second,
                        epg->weekday_size[wd].second, epg->weekday_size[wd].second, 3, true));
                render->current.add(new TObj(epg->weekdays[wd], epg->weekday_font, epg->weekday_xy[wd].first + epg->weekday_size[wd].second + SPACING/2, epg->weekday_xy[wd].second,
                        themes->epg_header_font1, themes->epg_header_font2, themes->epg_header_font3, 3));
            }

            wd_ts_x += epg->weekday_size[wd].first + epg->weekday_size[wd].second + SPACING;

        }

        y += (pages_size.second * 3) + 2 * SPACING;
    }

    pages_y = y - pages_size.second;

    strftime(time_str, 20, "%x", localtime(&start));
    time << time_str;
    time_size = string_format::calculate_string_size(time.str(), epg->annotation_font);

    render->current.add(new TObj(time.str(), epg->annotation_font, BORDER_LEFT + SPACING, pages_y,
            themes->epg_header_font1, themes->epg_header_font2, themes->epg_header_font3, 1));
    time.str("");

    render->current.add(new RObj(BORDER_LEFT, y, width - BORDER_LEFT - BORDER_RIGHT, height - y - BORDER_BOTTOM,
            themes->general_rectangle_color1, themes->general_rectangle_color2,
            themes->general_rectangle_color3, themes->general_rectangle_color4, 1));

    y += SPACING;

    int x_off = 0;

    if (epg->display_channel_logo && !basechannel->logo.empty()) {
        render->create_scaled_image_wrapper_upscaled(basechannel->logo, epg->title_size.second, epg->title_size.second);
        render->current.add(new PFObj(basechannel->logo, BORDER_LEFT + SPACING, y, epg->title_size.second, epg->title_size.second, false, 2));
        x_off = epg->title_size.second + SPACING;
    }

    if (epg->display_channel_name) {
        string_format::format_to_size(basechannel->display_name, epg->title_font, width - BORDER_LEFT - BORDER_RIGHT - SPACING
                + x_off, true);

        render->current.add(new TObj(basechannel->display_name, epg->title_font, BORDER_LEFT + SPACING + x_off, y,
                themes->epg_font1, themes->epg_font2, themes->epg_font3, 2));
    }

    int from_right_to_left = width - BORDER_RIGHT - SPACING - epg->title_size.second;

    if (sound & AUDIO_STEREO) {
        render->current.add(new PFObj(themes->epg_b_epg_stereo, from_right_to_left, y, epg->title_size.second, epg->title_size.second, 1, true));
        from_right_to_left -= (SPACING + epg->title_size.second);
    }

    if (sound & AUDIO_DUAL) {
        render->current.add(new PFObj(themes->epg_b_epg_dual, from_right_to_left, y, epg->title_size.second, epg->title_size.second, 1, true));
        from_right_to_left -= (SPACING + epg->title_size.second);
    }

    if (sound & AUDIO_DD_2_0) {
        render->current.add(new PFObj(themes->epg_b_epg_dd_2_0, from_right_to_left, y, epg->title_size.second, epg->title_size.second, 1, true));
        from_right_to_left -= (SPACING + epg->title_size.second);
    }

    if (sound & AUDIO_DD_5_1) {
        render->current.add(new PFObj(themes->epg_b_epg_dd_5_1, from_right_to_left, y, epg->title_size.second, epg->title_size.second, 1, true));
        from_right_to_left -= (SPACING + epg->title_size.second);
    }

    if (sound & AUDIO_LISTEN) {
        render->current.add(new PFObj(themes->epg_b_epg_hoerfilm, from_right_to_left, y, epg->title_size.second, epg->title_size.second, 1, true));
        from_right_to_left -= (SPACING + epg->title_size.second);
    }

    if (sound & AUDIO_NUMB) {
        render->current.add(new PFObj(themes->epg_b_epg_numb, from_right_to_left, y, epg->title_size.second, epg->title_size.second, 1, true));
        from_right_to_left -= (SPACING + epg->title_size.second);
    }

    if (aspect & VIDEO_16_9) {
        render->current.add(new PFObj(themes->epg_b_epg_16_9, from_right_to_left, y, epg->title_size.second, epg->title_size.second, 1, true));
        from_right_to_left -= (SPACING + epg->title_size.second);
    }

    y += epg->title_size.second;

    strftime(time_str, 20, "%H:%M", localtime(&start));
    time << time_str << " - ";
    strftime(time_str, 20, "%H:%M", localtime(&stop));
    time << time_str;

    render->current.add(new TObj(time.str(), epg->subtitle_font, BORDER_LEFT + SPACING, y,
            themes->epg_font1, themes->epg_font2, themes->epg_font3, 2));

    x_off = string_format::calculate_string_width(time.str(), epg->subtitle_font);

    time.str("");

    string_format::format_to_size(title, epg->subtitle_font, width - BORDER_LEFT - BORDER_RIGHT - SPACING - x_off, true);

    render->current.add(new TObj(title, epg->subtitle_font, BORDER_LEFT + SPACING + x_off + SPACING, y,
            themes->epg_font1, themes->epg_font2, themes->epg_font3, 2));

    y += epg->subtitle_size.second;

    int start_y = y;

    int lines_of_text = (height - y - BORDER_BOTTOM) / epg->normal_size.second;
    int box_width = width - BORDER_LEFT - BORDER_RIGHT - (2 * SPACING);
    int x= BORDER_LEFT + SPACING;

    // determine Number of Pages
    if (index_to_page[0] == -1) {
        index_to_page[0] = 0;
        index = 0;
        while (index < desc.size()) {
            string descr = string_format::get_line(desc, index, box_width, epg->normal_font);
            // Svdrp - Special - | means Newline !!!!
            nl = descr.find('|');
            if (nl >= 0 && epg->pipe_is_nl) {
                descr = descr.substr(0, nl);
                // Adjust Pointer into string backwards (One behind Newline |):
                index = pos + nl + 1;
            }
            pos = index; // Remember last position

            y += epg->normal_size.second;

            if (y >= start_y + lines_of_text * epg->normal_size.second) {
                pages++;
                index_to_page.push_back(index);
                y = start_y;
            }
        }
        index = 0;
        y = start_y;
    }

    nl = 0;
    pos = 0;

    while (index < desc.size()) {
        string descr = string_format::get_line(desc, index, box_width, epg->normal_font);
        // Svdrp - Special - | means Newline !!!!
        nl = descr.find('|');
        if (nl >= 0 && epg->pipe_is_nl) {
            descr = descr.substr(0, nl);
            // Adjust Pointer into string backwards (One behind Newline |):
            index = pos + nl + 1;
        }
        pos = index; // Remember last position

        render->current.add(new TObj(descr, epg->normal_font, x, y,
                themes->epg_font1, themes->epg_font2,
                themes->epg_font3, 2));
        y += epg->normal_size.second;

        if (y >= start_y + lines_of_text * epg->normal_size.second)
            break;
    }

    page_of_pages = string_format::str_printf("%d/%d", page + 1, pages + 1);
    pages_size = string_format::calculate_string_size(page_of_pages, epg->annotation_font);

    render->current.add(new TObj(page_of_pages, epg->annotation_font, width - BORDER_RIGHT - pages_size.first,
            pages_y,
            themes->epg_header_font1, themes->epg_header_font2,
            themes->epg_header_font3, 1));

    if (this->HasTimer()) {
        std::string h_timer;

        h_timer = this->Timer()->IsActive() ? dgettext("mms-epg", "Active Timer") : "";

        h_timer += this->Timer()->IsRecording() ? dgettext("mms-epg", ", Is recording") : "";

        h_timer += this->Timer()->IsVPS() ? dgettext("mms-epg", ", (VPS)") : "";

        render->current.add(new TObj(h_timer, epg->annotation_font, BORDER_LEFT + time_size.first + SPACING,
                pages_y,
                themes->epg_timer_marked1, themes->epg_timer_marked2,
                themes->epg_timer_marked3, 1));
    }

    if (with_timer) {
        if (cursor_is_in_header) {
            render->current.add(new PFObj(themes->general_marked, epg->weekday_xy[day_marked].first,
                    epg->weekday_xy[day_marked].second,
                    epg->weekday_size[day_marked].first + epg->weekday_size[day_marked].second + SPACING,
                    epg->weekday_size[day_marked].second, 1, true));

        }
    }

    if (epg->animation) {
        epg->EndAnimation();
    }

    render->draw_and_release("EPG Toggle Timer");
}

bool SvdrpEpgEvent::ToggleTimer(Render *render, Themes *themes, int width, int height) {
    InputMaster *input_master = S_InputMaster::get_instance();
    Epg *epg = get_class<Epg>(dgettext("mms-epg", "Electronic Program Guide"));
    Input input;
    BusyIndicator *busy_indicator = S_BusyIndicator::get_instance();

    cursor_is_in_header = false;
    day_marked = 0;
    SvdrpTimerEvent *Timer;

    index = 0;
    round = 1;

    string::iterator it = desc.begin();
    std::ostringstream next_word;

    int y= BORDER_TOP;

    bool cancel = false;

    page = 0;
    pages = 0;

    index_to_page.clear();

    index_to_page.push_back(-1);

    if (timer == NULL) {
        std::string timestr;

        strftime(time_str, 20, "%F", localtime(&start));
        timestr = string_format::str_printf("%s", &time_str[0]);

        std::string debug_message = string_format::str_printf("Date of Time is %s", timestr.c_str());
        print_debug(debug_message);

        Timer = new SvdrpTimerEvent(0, 0, basechannel->number, timestr, 0,
                start - (epg->timer_lead * SECONDS_PER_MINUTE),
                stop + (epg->timer_trailer * SECONDS_PER_MINUTE), 50, 50, title, "<created by mms>");
    } else {
        Timer = timer;
    }

    Timer->start = start - epg->timer_lead * SECONDS_PER_MINUTE;
    Timer->stop = stop + epg->timer_trailer * SECONDS_PER_MINUTE;

    epg->animation = true;
    while (!cancel) {
        PrintDetails(render, themes, width, height, true, Timer);
        epg->animation = false;

        bool refresh = false;

        while (!refresh) {

            input = input_master->get_input_busy_wrapped(busy_indicator);

            if (input.key == "touch_input") {
                cancel = true;
                refresh = false;
                break;
            }

            if (input.command == "back" || input.command == "cancel") {
                cancel = true;
                epg->animation = true;
                return false;
            } else if (input.command == "action") {
                if (cursor_is_in_header) {
                    Timer->SetDay(day_marked, !Timer->Day(day_marked));
                    page = 0;
                    index = index_to_page[0];
                    refresh = true;
                } else {
                    cancel = true;
                    timer = Timer;
                    return true;
                }
            } else if (input.command == "delete_timer" && Timer->Number() != 0) {
                ExtraMenu em(dgettext("mms-epg", "Delete Timer?"));
                em.add_item(ExtraMenuItem(dgettext("mms-epg", "Yes"), "", NULL));
                em.add_item(ExtraMenuItem(dgettext("mms-epg", "No"), "", NULL));
                int ret = em.mainloop();

                if (ret == 0) {
                    cancel = true;
                    Timer->SetNumber(Timer->Number() * -1);
                    timer = Timer;
                    return true;
                }
            } else if (input.command == "prev" && page >= 0) {
                if (page == 0) {
                    cursor_is_in_header = true;
                    page = 0;
                    index = index_to_page[0];
                } else {
                    page--;
                    index = index_to_page[page];
                }
                refresh = true;
            } else if (input.command == "next" && page <= pages) {
                if (cursor_is_in_header) {
                    page = 0;
                } else if (page < pages) {
                    page++;
                }
                cursor_is_in_header = false;
                index = index_to_page[page];
                refresh = true;
            } else if (input.command == "left" && cursor_is_in_header) {
                day_marked--;
                if (day_marked < 0) {
                    day_marked = WEEKDAYS - 1;
                }
                page = 0;
                index = index_to_page[0];
                refresh = true;
            } else if (input.command == "right" && cursor_is_in_header) {
                day_marked++;
                if (day_marked >= WEEKDAYS) {
                    day_marked = 0;
                }
                page = 0;
                index = index_to_page[0];
                refresh = true;
            }
        }
        ++round;
        y = BORDER_TOP;
    }
    epg->FadeOut();

    return false;
}

Recording::Recording(int _number, int _id, std::string _channel_id, time_t _start, long _duration, int _tableid, int _version) :
    SvdrpEvent(_id, _channel_id, _start, _duration, _tableid, _version) {
    number = _number;
}

Recording::~Recording() {
}

bool Svdrp::AssignTimersAndEpgEvents(std::vector<Channel> *_channels, std::vector<SvdrpTimerEvent *> *_timers) {
    // Associate Timers

    std::vector<Channel>::iterator ch;
    std::vector<EpgEvent *>::iterator evi;
    SvdrpEpgEvent *ev;
    std::vector<SvdrpTimerEvent *>::iterator ti;
    int i;

    print_info("Associate Timers");

    bool assigned;
    for (ti = _timers->begin(); ti != _timers->end(); ti++) {
        SvdrpTimerEvent *ste = *(ti);
        assigned = false;

        char time_str[20];
        std::ostringstream str_timer;

        strftime(time_str, 20, "%x %H:%M", localtime(&(ste->start)));

        str_timer.str("from ");

        str_timer << time_str;

        strftime(time_str, 20, "%x %H:%M", localtime(&(ste->stop)));

        str_timer << " to " << time_str;

        time_t rawtime;
        tm *now;

        time(&rawtime);
        now = localtime(&rawtime);
        int wday = ste->Wday();

        debug_message = string_format::str_printf("%sTimer <%s> , Channel %d, %s Timersday is : %d ",
                ste->IsRepetitive() ? "Repetitive-" : "", ste->title.c_str(), ste->Channel(), str_timer.str().c_str(), ste->Wday());
        print_info(debug_message);

        for (ch = _channels->begin(); ch != _channels->end() && ste->Channel() != ch->number; ch++) {
        }
        if (ch != _channels->end()) { // Found right Channel
            if (ste->IsRepetitive()) {

                time_t be = ste->start;
                time_t en = ste->stop;

                evi = ch->program.begin();

                while (evi != ch->program.end()) {
                    ev = (SvdrpEpgEvent *)(*evi);
                    char time_str[20];
                    std::ostringstream str_event;

                    str_event.str("from ");

                    strftime(time_str, 20, "%x %H:%M", localtime(&(ev->start)));
                    str_event << time_str;

                    strftime(time_str, 20, "%x %H:%M", localtime(&(ev->stop)));
                    str_event << " to " << time_str;

                    str_timer.str("from ");

                    strftime(time_str, 20, "%x %H:%M", localtime(&be));
                    str_timer << time_str;

                    strftime(time_str, 20, "%x %H:%M", localtime(&en));
                    str_timer << " to " << time_str;

                    debug_message = string_format::str_printf("Compare Timer <%s> %s with <%s> %s", ste->title.c_str(),
                            str_timer.str().c_str(), ev->title.c_str(), str_event.str().c_str());
                    print_debug(debug_message);

                    if (ev->start >= be && ev->start < en && ev->title == ste->title) {
                        // Found Match

                        print_debug("MATCH !!");

                        ev->AssignTimer(ste);
                        ste->epgevents.push_back(&(*ev));
                        assigned = true;

                        debug_message = string_format::str_printf("Timer <%s> %s assigned to <%s> %s", ste->title.c_str(),
                                str_timer.str().c_str(), ev->title.c_str(), str_event.str().c_str());
                        print_info(debug_message);
                    }
                    if (ev->stop > en) {
                        for (i = 1; ((i < 7) && (!ste->Day((wday + i) % 7) )); i++) {
                        }

                        debug_message = string_format::str_printf("Current Day is %d next repetitive is %d", wday, (wday + i) % 7);
                        print_debug(debug_message);

                        debug_message = string_format::str_printf("Add %d Days to get to next Repetition", i);
                        print_debug(debug_message);

                        be += (i * SECONDS_PER_DAY);
                        en += (i * SECONDS_PER_DAY);
                        wday = (wday + i) % 7;
                    }
                    evi++;
                }
            } else {
                for (evi = ch->program.begin(); evi != ch->program.end(); evi++) {
                    ev = (SvdrpEpgEvent *)(*evi);
                    if (ev->start >= ste->start && ev->start < ste->stop) {
                        char time_str[20];
                        std::ostringstream str_event;

                        str_event.str("from ");

                        strftime(time_str, 20, "%x %H:%M", localtime(&(ev->start)));
                        str_event << time_str;

                        strftime(time_str, 20, "%x %H:%M", localtime(&(ev->stop)));
                        str_event << " to " << time_str;

                        str_timer.str("from ");

                        strftime(time_str, 20, "%x %H:%M", localtime(&(ste->start)));
                        str_timer << time_str;

                        strftime(time_str, 20, "%x %H:%M", localtime(&(ste->stop)));
                        str_timer << " to " << time_str;

                        if (ste->IsInstant()) {
                            ev->AssignTimer(ste);
                            ste->epgevents.push_back(&(*ev));
                            assigned = true;

                            debug_message = string_format::str_printf("Instant-Timer <%s> %s assigned to <%s> %s",
                                    ste->title.c_str(), str_timer.str().c_str(), ev->title.c_str(), str_event.str().c_str());
                            print_info(debug_message);
                        } else {
                            if (ev->title == ste->title) {
                                ev->AssignTimer(ste);
                                ste->epgevents.push_back(&(*ev));
                                assigned = true;

                                debug_message = string_format::str_printf("Timer <%s> %s assigned to <%s> %s", ste->title.c_str(),
                                        str_timer.str().c_str(), ev->title.c_str(), str_event.str().c_str());
                                print_info(debug_message);
                            }
                        }
                    }
                }
            }
        }

        if (!assigned) {
            debug_message = string_format::str_printf(
                    "%sTimer <%s> start : %d stop %d NOT Assigned to EPG-Event - no match found!",
                    ste->IsRepetitive() ? "Repetitive-" : "", ste->title.c_str(), ste->start, ste->stop);
            print_info(debug_message);
        }
    }
    return true;
}

