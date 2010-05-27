#define _XOPEN_SOURCE /* glibc2 needs this */

#include "config.h"

#include "epg.hpp"

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

#include <time.h>

#include "gettext.hpp"

#include "boost.hpp"

#include <iostream>
#include <list>
#include <sstream>
#include <algorithm>

// time of last change
#include <sys/stat.h>

#include "tinyxml.h"

#include "svdrp.hpp"

#ifdef use_nls
#include <langinfo.h>
#include <locale.h>
#endif

using std::string;
using std::vector;
using std::list;

pthread_mutex_t Epg::singleton_mutex= PTHREAD_MUTEX_INITIALIZER;

time_t From_Timezone2Timezone(time_t t, const char *from_zone, const char *to_zone) {
    char *tz;

    time_t now_t, to_t, from_t, utct_t, utcf_t,diff_t;
    struct tm *tm_to, *tm_from, *tm_utct, *tm_utcf;

    // Remember System's TZ
    tz = (char *)getenv("TZ");

    // Set Timezone to convert to
    if (to_zone) {
        if (*to_zone != 0) {
            setenv("TZ", to_zone, 1);
            tzset();
        }
    }

    // Get current Time
    now_t = time(NULL);

    // Convert Current Time to UTC/GMT
    tm_utct = gmtime(&now_t);
    utct_t  = mktime(tm_utct);

    // Convert Current Time to Local Time (to_zone by now)
    tm_to = localtime(&now_t);
    to_t = mktime(tm_to);

    // Set Timezone to convert from
    setenv("TZ", from_zone, 1);
    tzset();

    // Get Current Time
    now_t = time(NULL);

    // Convert Current Time to UTC/GMT
    tm_utcf = gmtime(&now_t);
    utcf_t  = mktime(tm_utcf);

    // Convert Current Time to Local Time (from_zone by now)
    tm_from = localtime(&now_t);
    from_t = mktime(tm_from);

    // Restore Timzone
    if (tz) {
        if (*tz != 0) {
            setenv("TZ", tz, 1);
        } else {
            unsetenv("TZ");
        }
    } else {
        unsetenv("TZ");
    }
    tzset();

    // Calculate Offset between Timezones - UTC is reference
    diff_t = (to_t  - utct_t) - (from_t - utcf_t);

    print_debug(string_format::str_printf("Offset between From - Zone <%s> and UTC is %d (sec)\n", from_zone, from_t - utcf_t));
    print_debug(string_format::str_printf("Offset between To   - Zone <%s> and UTC is %d (sec)\n", to_zone, to_t - utct_t));
    print_debug(string_format::str_printf("Offset between Zone's <%s> and <%s> is %d (sec)\n", from_zone, to_zone, diff_t));

    return t + diff_t;
}

time_t From_Timezone2Local(time_t t, const char *from) {
    return From_Timezone2Timezone(t, from, "");
}

time_t From_Local2Timezone(time_t t, const char *to) {
    return From_Timezone2Timezone(t, "", to);
}

time_t Event::str_to_time(const char *s) {
    struct tm tm;
    char * datetimetz;

#ifdef __UCLIBC__
    char Y[5], m[3], d[3], H[3], M[3];
    char t[32];

    s += 0; strncpy(Y, s, 4); Y[4] = 0;
    s += 4; strncpy(m, s, 2); m[2] = 0;
    s += 2; strncpy(d, s, 2); d[2] = 0;
    s += 2; strncpy(H, s, 2); H[2] = 0;
    s += 2; strncpy(M, s, 2); M[2] = 0;
    sprintf(t, "%s-%s-%s %s:%s:00 +0000", Y, m, d, H, M);
#endif

    //fix unset value
    tm.tm_isdst=0;
    //get timezone
    tzset();

#ifdef __UCLIBC__
    datetimetz = strptime(t, "%Y-%m-%d %H:%M:%S", &tm);
#else
    datetimetz = strptime(s, "%Y%m%d%H%M%S", &tm);
#endif

    return mktime(&tm);
}

bool Event::operator<(const Event& rhs) const
{
    return (this->start < rhs.start);
}

Event::Event() {
    start = 0;
    stop = 0;
    duration = 0;
}

Event::~Event() {

}

EpgEvent::EpgEvent() :
    Event() {
}

EpgEvent::~EpgEvent() {

}

RObj *XmlEpgEvent::Print(Render *render, Themes *themes, int x, int y, int width, int height, bool selected, std::string fontname) {
    RObj *r;
    string name = title;
    string_format::format_to_size(name, fontname, width - 10, false);

    if (selected) {
        r = new RObj(x, y, width - 5, height,
                themes->epg_marked1,
                themes->epg_marked2,
                themes->epg_marked3,
                themes->epg_marked4, 3);

        render->current.add(r);
        render->current.add(new TObj(name, fontname, x + 2, y,
                themes->epg_marked_font1,
                themes->epg_marked_font2,
                themes->epg_marked_font3, 4));
    } else {
        r = new RObj(x, y, width - 5, height,
                themes->epg_boxes1,
                themes->epg_boxes2,
                themes->epg_boxes3,
                themes->epg_boxes4, 3);

        render->current.add(r);
        render->current.add(new TObj(name, fontname, x + 2, y,
                themes->epg_boxes_font1,
                themes->epg_boxes_font2,
                themes->epg_boxes_font3, 4));
    }
    return r;
}

void XmlEpgEvent::PrintDescription(Render *render, Themes *themes, int width, int height) {
    char time_str[20];
    std::ostringstream time;

    InputMaster *input_master = S_InputMaster::get_instance();
    Epg *epg = get_class<Epg>(dgettext("mms-epg", "Electronic Program Guide"));
    BusyIndicator *busy_indicator = S_BusyIndicator::get_instance();
    Input input;

    int i = 0;
    int round = 1;
    string::iterator it = desc.begin();
    std::ostringstream next_word;

    int nl = 0, pos = 0;

    int y= BORDER_TOP;

    bool cancel = false;

    std::vector<int> index_to_page;
    int page = 0;
    int pages = 0;
    int pages_y;

    std::string page_of_pages;
    std::pair<int, int> pages_size;

    index_to_page.push_back(-1);

    epg->animation = true;

    while (!cancel) {

        render->prepare_new_image();

        if (epg->animation) {
            epg->BeginAnimation();
        }

        PObj *icon = new PFObj(themes->startmenu_epg, BORDER_LEFT, y, epg->header_box_size, epg->header_box_size, 1, true);

        S_Touch::get_instance()->register_area(TouchArea(rect(icon->x, icon->y, icon->w, icon->h), icon->layer, boost::bind(&XmlEpgEvent::no_op,
                this)));

#if 0
        render->current.add(new PObj(themes->epg_background, 0, 0, 0, SCALE_FULL));
#else
        render->current.add(new BgRObj(0));
#endif

        render->current.add(icon);

        std::string header;

        render->current.add(new TObj("EPG", epg->header_font, BORDER_LEFT + epg->header_box_size + SPACING, y + (epg->header_box_size - epg->header_size.second) / 2,
                themes->epg_header_font1, themes->epg_header_font2,
                themes->epg_header_font3, 1));

        y += epg->header_size.second + SPACING;

        page_of_pages = string_format::str_printf("%d/%d", page + 1, pages + 1);
        pages_size = string_format::calculate_string_size(page_of_pages, epg->annotation_font);
        pages_y = y - pages_size.second;

        strftime(time_str, 20, "%x", localtime(&start));
        time << time_str;

        render->current.add(new TObj(time.str(), epg->annotation_font, BORDER_LEFT + SPACING, pages_y,
                themes->epg_header_font1, themes->epg_header_font2, themes->epg_header_font3, 1));
        time.str("");

        render->current.add(new RObj(BORDER_LEFT, y, width - BORDER_LEFT - BORDER_RIGHT, height - y - BORDER_BOTTOM,
                themes->general_rectangle_color1, themes->general_rectangle_color2,
                themes->general_rectangle_color3, themes->general_rectangle_color4, 2));

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
            i = 0;
            while (i < desc.size()) {
                string descr = string_format::get_line(desc, i, box_width, epg->normal_font);

                pos = i; // Remember last position

                y += epg->normal_size.second;

                if (y >= start_y + lines_of_text * epg->normal_size.second) {
                    pages++;
                    index_to_page.push_back(i);
                    y = start_y;
                }
            }
            i = 0;
            y = start_y;
        }

        nl = 0;
        pos = 0;

        while (i < desc.size()) {
            string descr = string_format::get_line(desc, i, box_width, epg->normal_font);

            pos = i; // Remember last position

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

        if (epg->animation) {
            epg->EndAnimation();
        }
        epg->animation = false;

        render->draw_and_release("EPG description");

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
            } else if (input.command == "prev" && page > 0) {
                page--;
                i = index_to_page[page];
                refresh = true;
            } else if (input.command == "next" && page < pages) {
                page++;
                i = index_to_page[page];
                refresh = true;
            }
        }
        ++round;
        y = BORDER_TOP;

    }
    epg->FadeOut();
}

XmlEpgEvent::XmlEpgEvent(BaseChannel *ch, TiXmlElement *element, std::string tz) :
    EpgEvent() {
    TiXmlNode *node;
    TiXmlText *text;

    basechannel = ch;

    if (element->Attribute("start")) {
        start = From_Timezone2Local(str_to_time(element->Attribute("start")), tz.c_str());
    }

    if (element->Attribute("stop")) {
        stop = From_Timezone2Local(str_to_time(element->Attribute("stop")), tz.c_str());
    }

    duration = 0;
    if (stop > start)
        duration = stop - start;

    if (element->FirstChild("title")) {
        node = element->FirstChild("title");
        if (node) {
            node = node->FirstChild();
            if (node) {
                text = node->ToText();
                if (text)
                    title = text->Value();
            }
        }
    }

    if (element->FirstChild("title")) {
        node = element->FirstChild("title");
        if (node) {
            node = node->FirstChild();
            if (node) {
                text = node->ToText();
                if (text)
                    title = text->Value();
            }
        }
    }

    if (element->FirstChild("desc")) {
        node = element->FirstChild("desc");
        if (node) {
            node = node->FirstChild();
            if (node) {
                text = node->ToText();
                if (text)
                    desc = text->Value();
            }
        }
    }
}

XmlEpgEvent::XmlEpgEvent(BaseChannel *ch, time_t _start, long _duration, string _title) :
    EpgEvent() {
    basechannel = ch;
    start = _start;
    stop = start + _duration;
    duration = _duration;
    title = _title;
}

XmlEpgEvent::~XmlEpgEvent() {

}

Channel::Channel() {

}

Channel::~Channel() {
    std::vector<EpgEvent *>::iterator ev;

    // Delete all Epg-Events;
    for (ev = program.begin(); ev != program.end(); ev++) {
        delete (*ev);
    }
    program.clear();
}

XMLChannel::XMLChannel(int _number, TiXmlElement *element) :
    Channel() {
    TiXmlNode *node = 0;
    TiXmlText *text = 0;
    epgsource = XML;
    number = _number;
    logo = "";

    if (element->Attribute("id")) {
        id = element->Attribute("id");
    }

    if (element->FirstChild("display-name")) {
        node = element->FirstChild("display-name");
        if (node != 0) {
            node = node->FirstChild();
            if (node != 0) {
                text = node->ToText();
                display_name = text->Value();
            }
        }
    }
}

void Epg::update_time() {
    struct tm *tm;

    cur_time = time(0);
    tm = localtime(&cur_time);
    if (tm->tm_min < timeslot_interval) {
        tm->tm_min = 0;
    } else {
        tm->tm_min = timeslot_interval;
    }
    tm->tm_sec = 0;
    cur_time = mktime(tm);
    sel_time = cur_time;
}

void Epg::get_data() {
    if (use_svdrp) {
        if (!GetSvdrpData())
            return;
    } else {
        if (!file_exists(epg_conf->p_epg_data()))
            return;

        if (!read_xml_file(&channels))
            return;
    }

    cur_channel = channels.begin();
    sel_channel = cur_channel;
    sel_channel_num = 0;

    cur_time = time(0);
    sel_time = cur_time;

    int pi;
    for (pi = 0; pi < sel_channel->program.size(); pi++) {
        if (sel_channel->program[pi]->stop >= sel_time)
            break;
    }
    if (pi != sel_channel->program.size()) {
        cur_time = sel_time = sel_channel->program[pi]->start;
        cur_event = sel_channel->program[pi];
    }
}

void Epg::parse_channels(std::string _channels, std::vector<int> *_channelnumbers) {
    std::vector<int>::iterator it;

    int fc, lc, posc, posr;
    std::string range;

    posc = _channels.find(",");

    while (posc >= 0 && !_channels.empty()) {
        range = _channels.substr(0, posc);
        posr = range.find("-");
        if (posr >= 0) {
            if (posr == 0) {
                fc = 1;
            } else {
                sscanf(_channels.substr(0, posr).c_str(), "%d", &fc);
            }
            sscanf(_channels.substr(posr+1, posc).c_str(), "%d", &lc);
        } else {
            sscanf(_channels.substr(0, posc).c_str(), "%d", &fc);
            lc = fc;
        }
        for (int i = fc; i <= lc; i++) {
            if (_channelnumbers->empty()) {
                _channelnumbers->push_back(i);
            } else {
                for (it = _channelnumbers->begin(); *it < i && it != _channelnumbers->end(); it++) {
                }
                if (it == _channelnumbers->end() && *it != i) {
                    _channelnumbers->push_back(i);
                } else if (*it > i) {
                    _channelnumbers->insert(it, i);
                }
            }
        }
        _channels.erase(0, posc + 1);

        posc = _channels.find(",");
    }
    if (!_channels.empty()) {
        range = _channels;
        posr= range.find("-");
        if (posr >= 0) {
            if (posr == 0) {
                fc = 1;
            } else {
                sscanf(_channels.substr(0, posr).c_str(), "%d", &fc);
            }
            sscanf(_channels.substr(posr + 1, posc).c_str(), "%d", &lc);
        } else {
            sscanf(_channels.c_str(), "%d", &fc);
            lc = fc;
        }
        for (int i = fc; i <= lc; i++) {
            if (_channelnumbers->empty()) {
                _channelnumbers->push_back(i);
            } else {
                for (it = _channelnumbers->begin(); *it < i && it != _channelnumbers->end(); it++) {
                }
                if (it == _channelnumbers->end() && *it != i) {
                    _channelnumbers->push_back(i);
                } else if (*it > i) {
                    _channelnumbers->insert(it, i);
                }
            }
        }
    }
}

Epg::Epg() {
#ifdef use_nls
    // gettext
    setlocale(LC_ALL, ""); /* set from the environment variables */
    bindtextdomain("mms-epg", mms_prefix"/share/locale");
    bind_textdomain_codeset("mms-epg", nl_langinfo(CODESET));
#endif

    epg_conf = S_EPGConfig::get_instance();

    // Resolution-Independent-Stuff
    border_left = 25;
    border_right = 25;
    border_top = 10;
    spacing = 10;
    border_bottom = global->getBottomHeight() + spacing;

    weekdays[0] = dgettext("mms-epg", "Monday");
    weekdays[1] = dgettext("mms-epg", "Tuesday");
    weekdays[2] = dgettext("mms-epg", "Wednesday");
    weekdays[3] = dgettext("mms-epg", "Thursday");
    weekdays[4] = dgettext("mms-epg", "Friday");
    weekdays[5] = dgettext("mms-epg", "Saturday");
    weekdays[6] = dgettext("mms-epg", "Sunday");

    header_font         = "Vera/" + conv::itos(resolution_dependant_font_size(28, conf->p_v_res()));
    title_font          = "Vera/" + conv::itos(resolution_dependant_font_size(22, conf->p_v_res()));
    subtitle_font       = "Vera/" + conv::itos(resolution_dependant_font_size(18, conf->p_v_res()));
    normal_font         = "Vera/" + conv::itos(resolution_dependant_font_size(16, conf->p_v_res()));
    annotation_font     = "Vera/" + conv::itos(resolution_dependant_font_size(12, conf->p_v_res()));
    grid_font           = "Vera/" + conv::itos(resolution_dependant_font_size(14, conf->p_v_res()));
    search_font         = "Vera/" + conv::itos(resolution_dependant_font_size(22, conf->p_v_res()));
    search_select_font  = "Vera/" + conv::itos(resolution_dependant_font_size(16, conf->p_v_res()));
    weekday_font        = "Vera/" + conv::itos(resolution_dependant_font_size(12, conf->p_v_res()));

    header_size         = string_format::calculate_string_size("abcltuwHPMjJg", header_font);
    title_size          = string_format::calculate_string_size("abcltuwHPMjJg", title_font);
    subtitle_size       = string_format::calculate_string_size("abcltuwHPMjJg", subtitle_font);
    normal_size         = string_format::calculate_string_size("abcltuwHPMjJg", normal_font);
    annotation_size     = string_format::calculate_string_size("abcltuwHPMjJg", annotation_font);
    grid_size           = string_format::calculate_string_size("abcltuwHPMjJg", grid_font);
    search_size         = string_format::calculate_string_size("abcltuwHPMjJg", search_font);
    search_select_size  = string_format::calculate_string_size("abcltuwHPMjJg", search_select_font);

    channel_size = string_format::calculate_string_size("abcltuwHPMjJgmi", grid_font);

    header_box_size = static_cast<int>(header_size.second * 0.75);
    search_box_size = static_cast<int>(search_size.second * 0.75);

    int max_x;

    total_wd_x = 0;

    for (int wd = 0; wd < WEEKDAYS; wd++) {
        weekday_size[wd] = string_format::calculate_string_size(weekdays[wd], weekday_font);
        max_x = weekday_size[wd].first > max_x ? weekday_size[wd].first : max_x;
        weekday_size[wd].first += spacing;
        total_wd_x = total_wd_x + weekday_size[wd].first + weekday_size[wd].second + spacing;
    }

    max_x = 0;

    if (total_wd_x + border_left + spacing + border_right + spacing > conf->p_h_res()) {
        total_wd_x = 0;
        for (int wd = 0; wd < WEEKDAYS; wd++) {
            weekdays[wd] = weekdays[wd].substr(0, 2);
            weekday_size[wd] = string_format::calculate_string_size(weekdays[wd], weekday_font);
            max_x = weekday_size[wd].first > max_x ? weekday_size[wd].first : max_x;
            weekday_size[wd].first += spacing;
            total_wd_x = total_wd_x + weekday_size[wd].first + weekday_size[wd].second + spacing;
        }
    }

    timezone = epg_conf->p_epg_timezone();
    
    timeslot_interval = (epg_conf->p_timeslot_interval() > 5 ? epg_conf->p_timeslot_interval() : 30);

    display_channel_name = epg_conf->p_display_channel_name();
    display_channel_logo = epg_conf->p_display_channel_logo();

    timeslots = epg_conf->p_timeslots();

    grid_header_x = int((conf->p_h_res() * 0.75) + spacing/2);
    grid_x = grid_header_x;

    scale_factor = (timeslot_interval * SECONDS_PER_MINUTE * timeslots) / (double(conf->p_h_res() * 0.75) - (spacing / 2) - border_right);

    max_logo_size_x = 0;

    use_svdrp = false;
    lastrefresh = 0;
    pipe_is_nl = epg_conf->p_svdrp_pipe_is_nl();

    if (epg_conf->p_use_svdrp()) {
        svdrp_channelnumbers = new std::vector<int>;

        // Timers = new std::vector<SvdrpTimerEvent *>;
        Recordings = new std::vector<Recording>;

        svdrp = new Svdrp(epg_conf->p_svdrp_host(), epg_conf->p_svdrp_port());
        if (svdrp) {
            parse_channels(epg_conf->p_svdrp_channels(), svdrp_channelnumbers);
            timer_lead = epg_conf->p_svdrp_timer_lead();
            if (timer_lead <= 1) {
                timer_lead = 1;
            }
            timer_trailer = epg_conf->p_svdrp_timer_trailer();
            if (timer_trailer <= 1) {
                timer_trailer = 1;
            }
            use_svdrp = true;

            if (epg_conf->p_svdrp_convert()) {
                svdrp->SetLocale(epg_conf->p_svdrp_locale());
            }
        }
    }

    update_running = false;
    reload_needed = true;
    loaded_correctly = false;

    epg_updater = new EpgUpdate();
    rec_on_off = false;
}

Epg::~Epg() {
    ende = true;

    while (update_running)
        mmsUsleep(200*1000);

    delete epg_updater;
}

bool Epg::valid_program() {
    bool found_match = false;

    time_t sel_start_time = (*program_iter)->start;

    vector<EpgEvent *>::iterator piter;
    for (piter = sel_channel->program.begin(), program_iter_end = sel_channel->program.end(); piter != program_iter_end; ++piter)
        if ((*piter)->stop > sel_start_time && (*piter)->stop > cur_time) {
            found_match = true;
            break;
        }

    program_iter = piter;
    cur_event = *program_iter;

    return found_match;
}

void Epg::find_next_program() {
    vector<EpgEvent *>::iterator end = sel_channel->program.end();

    --end;

    vector<EpgEvent *>::iterator piter;
    for (piter = sel_channel->program.begin(), program_iter_end = sel_channel->program.end(); piter != program_iter_end; piter++) {
        if ((*piter)->start >= sel_time) {
            if ((*piter)->start == sel_time && piter != end) {

                vector<EpgEvent *>::iterator last_program_iter = piter;
                ++piter;

                // fix for strange programs with no length
                if ((*last_program_iter)->start == (*last_program_iter)->stop || (*piter)->start == (*piter)->stop && piter != end) {
                    ++piter;
                }
            }
            break;
        }
    }

    if (piter == program_iter_end) { // Epg-Info Ends too early
        if (sel_channel->program.size() > 0)
            piter--;
    }

    program_iter = piter;
    cur_event = *program_iter;

    sel_time = (*program_iter)->start;

    int time = timeslots*timeslot_interval*SECONDS_PER_MINUTE;

    while (sel_time >= cur_time + time)
        cur_time += timeslot_interval * SECONDS_PER_MINUTE;
}

void Epg::find_valid_program() {
    if (!valid_program()) {
        find_next_program();
        return;
    }

    sel_time = (*program_iter)->start;

    int time = timeslots*timeslot_interval*SECONDS_PER_MINUTE;

    while (sel_time >= cur_time + time)
        cur_time += timeslot_interval*SECONDS_PER_MINUTE;
}

bool Epg::find_logos() {
    bool logos = true;

    std::vector<Channel>::iterator ch;

    foreach(Channel& channel, channels) {
        string logo_file = epg_conf->p_icon_path() + "/" + channel.display_name + ".xpm";
        if (file_exists(logo_file)) {
            channel.logo = logo_file;
            grid_header_x = int((conf->p_h_res() * 0.75) + spacing/2);
            render->image_mut.enterMutex();
            render->device->wait_for_not_drawing();
            render->create_scaled_image_wrapper_upscaled(logo_file, int(conf->p_h_res() * 0.25 - spacing/2), grid_size.second);

            PFObj*l = new PFObj(logo_file, 1, 1, int(conf->p_h_res() * 0.25 - spacing/2), grid_size.second, false, 2);

            if (l) {
                if (l->w > max_logo_size_x)
                    max_logo_size_x = l->w;

                delete l;
            }
            render->image_mut.leaveMutex();

        } else {
            logos = false;
            channel.logo = "";
            print_info(dgettext("mms-epg", "logo not found:") + logo_file, "EPG");
        }
    }
    return logos;
}

bool Epg::check_tv_data() {
    // check if the TV.xml file is new enough
    time_t max = 0;

    if (!use_svdrp) {
        foreach(Channel& channel, channels) {

            string::size_type dot_pos = channel.id.find(".");

            if (dot_pos == string::npos) {
                dot_pos = channel.id.size();
                print_critical(dgettext("mms-epg", "Error in channel id, please report this problem"), "EPG");
            }
        }
    }

    foreach(Channel& channel, channels) {
        if (channel.program.size()> 0) {
            vector<EpgEvent *>::iterator cha = channel.program.end();
            --cha;
            if (cha != channel.program.begin())
                --cha;

            max = std::max((*cha)->stop, max);
        }
    }

    if (max < time(0)) {
        Print pdialog(dgettext("mms-epg", "Your TV Data is too old or inaccessible"), Print::SCREEN);

        return false;
    } else
        return true;
}

// commands
void Epg::print_description() {
    S_BusyIndicator::get_instance()->idle();

    (*program_iter)->PrintDescription(render, themes, conf->p_h_res(), conf->p_v_res());
}

bool Epg::toggle_timer() {
    if (cur_event != NULL) {
        return toggle_timer(cur_event);
    } else {
        Print pdialog(Print::SCREEN);
        pdialog.add_line(dgettext("mms-epg", "Timer is not assigned to Epg-Event - "));

        pdialog.add_line(dgettext("mms-epg", "modification is not possible!"));
        pdialog.add_line("");
        pdialog.print();
    }
    return false;
}

bool Epg::delete_timer() {
    if (curtimer != NULL) {
        ExtraMenu em(dgettext("mms-epg", "Delete Timer?"));
        em.add_item(ExtraMenuItem(dgettext("mms-epg", "Yes"), "", NULL));
        em.add_item(ExtraMenuItem(dgettext("mms-epg", "No"), "", NULL));
        int ret = em.mainloop();

        if (ret == 0) {
            return delete_timer(curtimer);
        }
    }
    return false;
}

bool Epg::toggle_timer(EpgEvent *ev) {

    S_BusyIndicator::get_instance()->idle();

    if (use_svdrp) {
        if (!svdrp->Connect()) {
            return false;
        }
        svdrp->ClearTimer(&Timers);
        if (!svdrp->GetTimer(&Timers)) {
            svdrp->Disconnect();
            return false;
        }
        if (!svdrp->AssignTimersAndEpgEvents(&channels, &Timers)) {
            svdrp->Disconnect();
            return false;
        }
        svdrp->Disconnect();

        if (ev->ToggleTimer(render, themes, conf->p_h_res(), conf->p_v_res())) {
            SvdrpTimerEvent *Timer = ((SvdrpEpgEvent *)ev)->Timer();
            if (Timer->Number() < 0) {
                return delete_timer(Timer);
            } else if (Timer->Number() == 0) {
                return create_timer(Timer);
                delete Timer;
                ((SvdrpEpgEvent *)ev)->Cleartimer();
            } else {
                return modify_timer(Timer);
            }
        }
    }
    return false;
}

bool Epg::switch_to_channel() {
    bool result;

    if (use_svdrp) {
        svdrp_mutex.enterMutex();
        if (!svdrp->Connect()) {
            svdrp_mutex.leaveMutex();
            return false;
        }

        result = svdrp->SwitchtoChannel(sel_channel->number);

        svdrp->Disconnect();
        svdrp_mutex.leaveMutex();
        if (result) {
            run::exclusive_external_program(epg_conf->p_svdrp_tv_path() + ' ' + epg_conf->p_svdrp_tv_opts());
        }

        return result;
    }
    return false;
}

bool Epg::modify_timer(SvdrpTimerEvent *Ste) {
    bool result = false;

    svdrp_mutex.enterMutex();
    if (!svdrp->Connect()) {
        svdrp_mutex.leaveMutex();
        return false;
    }

    result = svdrp->ModifyTimer(Ste->Number(), TIMER_ACTIVE, Ste->Channel(), Ste->Days(), Ste->start, Ste->stop, 50, 50,
            Ste->title, "<created by mms>");

    if (result) {

        svdrp->ClearTimer(&Timers);

        result = svdrp->GetTimer(&Timers);

        if (result) {
            result = svdrp->AssignTimersAndEpgEvents(&channels, &Timers);
        }
    }

    svdrp->Disconnect();
    svdrp_mutex.leaveMutex();

    if (result) {
        DialogWaitPrint pdialog(2000);

        pdialog.add_line(dgettext("mms-epg", "Timer created !"));
        pdialog.print();
    } else {
        Print pdialog(Print::SCREEN);
        pdialog.add_line(dgettext("mms-epg", "Timer not created !"));
        pdialog.add_line(svdrp->Message());
        pdialog.print();
    }

    return result;
}

bool Epg::create_timer(SvdrpTimerEvent *Ste) {
    bool result = false;

    svdrp_mutex.enterMutex();
    if (!svdrp->Connect()) {
        svdrp_mutex.leaveMutex();
        return false;
    }

    result = svdrp->NewTimer(TIMER_ACTIVE, Ste->Channel(), Ste->Days(),
    Ste->start, Ste->stop, 50, 50,
    Ste->title, "<created by mms>");

    if (result) {

        svdrp->ClearTimer(&Timers);

        result = svdrp->GetTimer(&Timers);

        if (result) {
            result = svdrp->AssignTimersAndEpgEvents(&channels, &Timers);
        }
    }

    svdrp->Disconnect();
    svdrp_mutex.leaveMutex();

    if (result) {
        DialogWaitPrint pdialog(2000);
        pdialog.add_line(dgettext("mms-epg", "Timer created !"));
        pdialog.print();
    } else {
        Print pdialog(Print::SCREEN);
        pdialog.add_line(dgettext("mms-epg", "Timer not created !"));
        pdialog.add_line(svdrp->Message());
        pdialog.print();
    }

    return result;
}

bool Epg::delete_timer(SvdrpTimerEvent *ste) {
    bool result = true;

    if (ste->Number() < 0) {
        ste->SetNumber(ste->Number() * -1);
    }

    if (ste->IsRecording()) {
        svdrp_mutex.enterMutex();
        if (!svdrp->Connect()) {
            svdrp_mutex.leaveMutex();
            return false;
        }
        ExtraMenu em(dgettext("mms-epg", "Timer is recording - really delete?"));
        em.add_item(ExtraMenuItem(dgettext("mms-epg", "Yes"), "", NULL));
        em.add_item(ExtraMenuItem(dgettext("mms-epg", "No"), "", NULL));
        int ret = em.mainloop();

        if (ret != 0) {
            svdrp->Disconnect();
            svdrp_mutex.leaveMutex();
            return true;
        }
        if (!svdrp->ModifyTimerOnOff(ste->Number(), false)) {
            result = false;
        }
        svdrp->Disconnect();
        svdrp_mutex.leaveMutex();
    }

    svdrp_mutex.enterMutex();
    if (!svdrp->Connect()) {
        svdrp_mutex.leaveMutex();
        return false;
    }

    if (result) {
        result = svdrp->DeleteTimer(ste->Number());

        if (result) {

            svdrp->ClearTimer(&Timers);

            result = svdrp->GetTimer(&Timers);

            if (result) {
                result = svdrp->AssignTimersAndEpgEvents(&channels, &Timers);
            }
        }

        svdrp->Disconnect();
        svdrp_mutex.leaveMutex();
    }
    svdrp_mutex.leaveMutex();

    if (result) {
        DialogWaitPrint pdialog(2000);
        pdialog.add_line(dgettext("mms-epg", "Timer deleted !"));
        pdialog.print();
    } else {
        Print pdialog(Print::SCREEN);
        pdialog.add_line(dgettext("mms-epg", "Timer not deleted !"));
        pdialog.add_line("");
        pdialog.add_line(svdrp->Message());
        pdialog.print();
    }
    return result;
}

void Epg::search_func() {

    search_position = 0;

    BusyIndicator *busy_indicator = S_BusyIndicator::get_instance();

    ScreenUpdater *screen_updater = S_ScreenUpdater::get_instance();

    search_mode = true;
    search_help_offset = -1;

    std::pair<int, int> search_text_size = string_format::calculate_string_size("abcltuwHPMjJg", search_font);
    int search_size = static_cast<int>(search_text_size.second * 0.75);

    if (list_contains(conf->p_input(), string("lirc")) || list_contains(conf->p_input(), string("evdev")))
        search_top_size += search_size;

    list<string> empty;
    empty.push_back("");

    screen_updater->trigger.add(TriggerElement("search marker", empty, boost::bind(&Epg::print_marked_search_letter, this),
            boost::bind( &Epg::clean_up_search_print, this)));

    screen_updater->timer.add(TimeElement("search marker", boost::bind( &Epg::check_search_letter, this), boost::bind(
            &Epg::print_marked_search_letter, this)));

    std::vector<Channel>::iterator ci;
    std::vector<EpgEvent *>::iterator ei;

    std::vector<EpgEvent *>::iterator si;

    search_mutex.enterMutex();

    input_master->epg_search<EpgEvent *>(all_events, &search_position, boost::bind(&Epg::search_print, this, _1), boost::bind(
            &Epg::search_compare, this, _1), boost::bind( &Epg::search_get_title, this, _1), search_mode, search_str,
            lowercase_search_str, offset, boost::bind(&Epg::search_action, this, _1), busy_indicator);

    search_mutex.leaveMutex();

    screen_updater->trigger.del("search marker");
    screen_updater->timer.del("search marker");

    force_search_print_cleanup();

    if (list_contains(conf->p_input(), string("lirc")) || list_contains(conf->p_input(), string("evdev")))
        search_top_size -= search_size;

}

void Epg::search_touch_action(const vector<EpgEvent *>& events, int pos) {
    if (events.size() > 0) {
        clean_up_search_print();

        offset = pos;
        print_description(&(*(events[pos])));

    }
}

void Epg::search_action(const vector<EpgEvent *>& events) {
    int pos;

    if (events.size() > 0) {
        clean_up_search_print();

        pos = offset % events.size();
        print_description(&(*(events[pos])));

    }
}

void Epg::search_print(const vector<EpgEvent *>& events) {
    int y= border_top;
    int pos = 0;
    int cursor, start, end;

    if (!all_logos) {
        display_channel_name = true;
    }

    if (display_channel_logo) {
        logo_x = border_left;
        channel_x = logo_x + max_logo_size_x + spacing;
    } else {
        logo_x = 0;
        channel_x = border_left;
        display_channel_name = true;
    }

    if (display_channel_name == false) { // This only happens when all Channel-Logos are present and display_channel_name is set to false
        grid_header_x = border_left + max_logo_size_x + spacing;
        grid_x = grid_header_x;
    } else {
        grid_header_x = int((conf->p_h_res() * 0.25) + spacing/2);
        grid_x = grid_header_x;
    }

    render->prepare_new_image();

#if 0
    render->current.add(new PObj(themes->epg_background, 0, 0, 0, SCALE_FULL));
#else
    render->current.add(new BgRObj(0));
#endif

    PObj *back = new PFObj(themes->startmenu_epg, border_left, y, header_box_size, header_box_size, 1, true);

    render->current.add(back);

    if (themes->show_header) {
        TObj *t = new TObj(dgettext("mms-epg", "EPG"), header_font,
                border_left + header_box_size + spacing,
                y + (header_box_size - header_size.second) / 2,
                themes->epg_header_font1, themes->epg_header_font2,
                themes->epg_header_font3, 1);

        render->current.add(t);

        S_Touch::get_instance()->register_area(TouchArea(rect(t->x, t->y, t->w, t->h), t->layer, global->toggle_playlist));
    }

    y += header_size.second * 0.75;

    grid_header_y = border_top + header_size.second + spacing + annotation_size.second;
    grid_spacing = int(3 * conf->p_v_res() / 405); // for adjustment of vertical spacing on higher resolutions
    grid_y = grid_header_y + normal_size.second + spacing;

    grid_header_y += search_size.second;
    grid_y += search_size.second;

    render->current.add(new RObj(border_left, y, conf->p_h_res() - border_left - border_right, search_size.second,
            themes->general_search_rectangle_color1,
            themes->general_search_rectangle_color2,
            themes->general_search_rectangle_color3,
            themes->general_search_rectangle_color4, 2));

    render->current.add(new PFObj(themes->general_search, border_left + spacing, y + (search_size.second - search_box_size)/2, search_box_size, search_box_size, 3, true));

    int max_x = conf->p_h_res() - border_left - border_right - header_box_size - 2 * spacing;

    string sw = search_str;

    string s = gettext("Search");
    s.append(" : ");

    int x_size = string_format::calculate_string_width(s, search_font);

    string_format::format_to_size(sw, search_font, max_x - x_size, true);

    render->current.add(new TObj(s + sw, search_font,
            border_left + search_box_size + 2 * spacing,
            y, themes->search_font1, themes->search_font2, themes->search_font3, 3));

    y += (search_size.second * 2) + spacing;

    y_start = grid_y;

    rows = (conf->p_v_res() - (y + border_bottom)) / (grid_size.second + grid_spacing);

    if (events.size() > 0) {
        pos = offset % events.size();
    } else {
        pos = 0;
    }

    if (events.size() > rows) {
        if (pos <= (rows/2)) {
            cursor = pos;
            start = 0;
        } else if (pos > (events.size() - (rows / 2))) {
            cursor = rows - (events.size() - pos);
            start = events.size() - rows;
        } else {
            cursor = (rows / 2);
            start = pos - (rows / 2);
        }
    } else {
        start = 0;
        cursor = pos;
    }
    end = start + rows;

    char time_str[30];

    string counts;

    counts = string_format::str_printf("%d/%d", pos + 1, events.size());

    x_size = string_format::calculate_string_width(counts, normal_font);

    int time_pos_from_right = 15;

    if (conf->p_busy_indicator())
        time_pos_from_right = 32;

    if (events.size() > 0) {
        render->current.add(new TObj(counts, normal_font, conf->p_h_res()-(x_size + time_pos_from_right), border_top + (header_box_size - normal_size.second) / 2,
                themes->epg_font1, themes->epg_font2,
                themes->epg_font3, 2));
    }

    int time_size = string_format::calculate_string_width("31.12.1999 99:99 - 99:99", grid_font);

    if (events.size() > 0) {
        int i;
        for (i = 0; (i < rows) && ((i + start) < events.size()); i++) {
            std::string time;
            print_channel(y + (i* (grid_size.second + grid_spacing)), events[i + start]->basechannel);

            strftime(time_str, 20, "%x %H:%M", localtime( &(events[i + start]->start)));
            time.assign(time_str);
            time.append(" - ");
            strftime(time_str, 20, "%H:%M", localtime( &(events[i + start]->stop)));
            time.append(time_str);

            if (events[i + start]->HasTimer()) {
                render->current.add(new TObj(time, grid_font, grid_x, y + (i * (grid_size.second + grid_spacing)),
                        themes->epg_timer_marked1, themes->epg_timer_marked2, themes->epg_timer_marked3, 3));

                string_format::format_to_size(events[i + start]->title, grid_font, conf->p_h_res() - (grid_x + time_size + spacing)
                        - border_right, true);

                render->current.add(new TObj(events[i + start]->title, grid_font, grid_x + time_size + spacing, y + (i * (grid_size.second + grid_spacing)),
                        themes->epg_timer_marked1, themes->epg_timer_marked2, themes->epg_timer_marked3, 3));
            } else {
                render->current.add(new TObj(time, grid_font, grid_x, y + (i * (grid_size.second + grid_spacing)),
                        themes->epg_font1, themes->epg_font2, themes->epg_font3, 3));

                string_format::format_to_size(events[i + start]->title, grid_font, conf->p_h_res() - (grid_x + time_size + spacing)
                        - border_right, true);

                render->current.add(new TObj(events[i + start]->title, grid_font, grid_x + time_size + spacing, y + (i * (grid_size.second + grid_spacing)),
                        themes->epg_font1, themes->epg_font2, themes->epg_font3, 3));
            }

            S_Touch::get_instance()->register_area(TouchArea(rect(grid_x, y + (i * (grid_size.second + grid_spacing)), conf->p_h_res() - (grid_x
                    + border_right), grid_size.second + grid_spacing), 3, boost::bind( &Epg::search_touch_action, this, events, i
                    + start) ) );
        }
        render->current.add(new PFObj(themes->general_marked, logo_x - 10, y + cursor * (grid_size.second + grid_spacing) - (grid_spacing / 2), conf->p_h_res() - border_right - logo_x + 20, grid_size.second + grid_spacing, 2, true));
    }
    render->draw_and_release("epg-search");
}

void Epg::print_marked_search_letter() {
    if (!list_contains(conf->p_input(), string("lirc")) && !list_contains(conf->p_input(), string("evdev")))
        return;

    if (search_mode) {

        search_need_cleanup = true;

        render->wait_and_aquire();

        int y_offset = static_cast<int>(search_size.second + search_top_size);

        if (overlay_search.elements.size() > 0)
            overlay_search.partial_cleanup();

        int marked_thickness = 28;
        int marked_height = search_select_size.second;
        int magic_offset = -8;
        if (conf->p_h_res() <= 800) {
            marked_thickness = 22;
            magic_offset = -10;
        }

        magic_offset = 0;

        if (time(0)-conf->p_last_key() <= 2 && search_help_offset != -1)
            overlay_search.add(new RObj(vector_lookup(letters, search_help_offset)+magic_offset, y_offset+5,
                    marked_thickness, marked_height, 160, 150, 90, 255, 4));

        string keys = global->get_valid_keys();
        string spaced_keys = "";

        int start_pos = border_left;

        for (int i = 0; i < keys.size(); ++i) {
            if (keys[i] != '|')
                spaced_keys += (keys.substr(i, 1) + " ");

            if (keys[i] == '|' || i == keys.size()-1) {

                spaced_keys = spaced_keys.substr(0, spaced_keys.size()-1);

                int letters_width = string_format::calculate_string_width(spaced_keys, search_select_font);

                overlay_search.add(new RObj(start_pos-4, y_offset+5, letters_width, marked_height,
                        20, 30, 20, 255, 3));

                overlay_search.add(new TObj(spaced_keys, search_select_font,
                        start_pos, y_offset, themes->audio_font1,
                        themes->audio_font2, themes->audio_font3, 5));

                spaced_keys = "";
                start_pos = start_pos + 5 + letters_width;
            }
        }

        render->draw_and_release("search marker", true);
    }
}

bool Epg::search_compare(const EpgEvent *e) {
    std::string ltitle, ldesc;
    int pos;

    if (lowercase_search_str.empty()) {
        return false;
    }

    ltitle = string_format::lowercase(e->title);
    ldesc = string_format::lowercase(e->desc);

    pos = ltitle.find(lowercase_search_str);

    if (pos >= 0) {
        return true;
    }
    return false;
}

std::string Epg::search_get_title(const EpgEvent *e) {
    return e->title;
}

void Epg::exit() {
    input_master->restore_map();

    exit_loop = true;

    FadeOut();
}

bool Epg::move_element_up() {
    if (sel_channel != channels.begin()) {
        --sel_channel;

        if (sel_channel_num > 0) {
            --sel_channel_num;
        } else {
            --cur_channel;
        }

        return true;
    } else
        return false;
}

bool Epg::move_element_down() {
    if (sel_channel != (--channels.end())) {
        ++sel_channel;

        if (sel_channel_num < (rows - 1)) {
            ++sel_channel_num;
        } else {
            ++cur_channel;
        }

        return true;
    } else
        return false;
}

string Epg::mainloop() {
    Input input;

    input_master->save_map();
    input_master->set_map("epg");

    ScreenUpdater *screen_updater = S_ScreenUpdater::get_instance();

    // disable screensaver
    screen_updater->timer.deactivate("audio_fullscreen");

    screen_updater->timer.add(TimeElement("epg", boost::bind( &Epg::check_epg_print, this), boost::bind(&Epg::epg_print, this)));

    bool update_needed = true;

    BusyIndicator *busy_indicator = S_BusyIndicator::get_instance();
    Shutdown *sd = S_Shutdown::get_instance();

    refresh_mutex.enterMutex();
    program_iter = channels.begin()->program.begin();
    cur_event = *program_iter;

    refresh_mutex.leaveMutex();

    animation = true;

    while (!exit_loop) {
        if (update_needed) {
            refresh_mutex.enterMutex();
            print();
            animation = false;
            refresh_mutex.leaveMutex();
        }

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
        if (input.command == "back" && input.mode == "general") {
            exit();
        } else if (input.command == "startmenu") {
            exit();
        } else if (input.command == "next") {
            refresh_mutex.enterMutex();
            if (move_element_down())
                find_valid_program();
            refresh_mutex.leaveMutex();
        } else if (input.command == "prev") {
            refresh_mutex.enterMutex();
            if (move_element_up())
                find_valid_program();
            refresh_mutex.leaveMutex();
        } else if (input.command == "page_up") {
            refresh_mutex.enterMutex();

            bool changed_channel = false;

            for (int i = 0; i < conf->p_jump(); ++i) {
                if (!move_element_up())
                    break;
                else
                    changed_channel = true;
            }

            if (changed_channel)
                find_valid_program();

            refresh_mutex.leaveMutex();

        } else if (input.command == "page_down") {
            refresh_mutex.enterMutex();

            bool changed_channel = false;

            for (int i = 0; i < conf->p_jump(); ++i) {
                if (!move_element_down())
                    break;
                else
                    changed_channel = true;
            }

            if (changed_channel)
                find_valid_program();

            refresh_mutex.leaveMutex();

        } else if (input.command == "action") {
            refresh_mutex.enterMutex();

            screen_updater->timer.deactivate("epg");
            print_description();
            screen_updater->timer.activate("epg");

            refresh_mutex.leaveMutex();

        } else if (input.command == "list_timer") {
            screen_updater->timer.del("epg");
            show_timers();
            screen_updater->timer.add(TimeElement("epg", boost::bind( &Epg::check_epg_print, this), boost::bind(&Epg::epg_print,
                    this)));
            screen_updater->timer.activate("epg");
        } else if (input.command == "right") {
            refresh_mutex.enterMutex();

            find_next_program();

            refresh_mutex.leaveMutex();

        } else if (input.command == "left") {
            refresh_mutex.enterMutex();

            vector<EpgEvent *>::iterator old_program_iter = program_iter;
            vector<EpgEvent *>::iterator piter;

            for (piter = sel_channel->program.begin(); piter != sel_channel->program.end(); ++piter) {
                if ((*piter)->start > sel_time) {
                    if (piter != sel_channel->program.begin())
                        --piter;
                    if (piter != sel_channel->program.begin())
                        --piter;
                    if ((*old_program_iter)->stop == (*piter)->stop && (*old_program_iter)->start == (*piter)->start)
                        if (piter != sel_channel->program.begin())
                            --piter;

                    break;
                }
            }

            program_iter = piter;
            cur_event = *program_iter;

            if (program_iter == sel_channel->program.end() && program_iter != sel_channel->program.begin()) {
                --program_iter;
            }
            if (program_iter == sel_channel->program.end() && program_iter != sel_channel->program.begin()) {
                --program_iter;
            }

            sel_time = (*program_iter)->start;
            cur_event = *program_iter;

            while (sel_time < cur_time)
                cur_time -= timeslot_interval*SECONDS_PER_MINUTE;

            refresh_mutex.leaveMutex();

        } else if (input.command == "second_action") {
            screen_updater->timer.del("epg");
            ExtraMenu em;

            em.add_item(ExtraMenuItem(dgettext("mms-epg", "Print description"), input_master->find_shortcut("action"), boost::bind(
                    &Epg::print_description, this)));

            em.add_item(ExtraMenuItem(dgettext("mms-epg", "Reload Data"), input_master->find_shortcut("reload"), boost::bind(
                    &Epg::get_data, this)));
            if (use_svdrp) {
                em.add_item(ExtraMenuItem(dgettext("mms-epg", "Toggle Timer"), input_master->find_shortcut("toggle_timer"),
                        boost::bind( &Epg::toggle_timer, this)));

                em.add_item(ExtraMenuItem(dgettext("mms-epg", "Switch to Channel"),
                        input_master->find_shortcut("switch_to_channel"), boost::bind( &Epg::switch_to_channel, this)));

                em.add_item(ExtraMenuItem(dgettext("mms-epg", "Show Timer"), input_master->find_shortcut("list_timer"),
                        boost::bind( &Epg::show_timers, this)));
            }

            em.add_item(ExtraMenuItem(gettext("Search"), input_master->find_shortcut("search"), boost::bind( &Epg::search_func,
                    this)));

            em.add_item(ExtraMenuItem(dgettext("mms-epg", "Return to startmenu"), input_master->find_shortcut("back"), boost::bind(
                    &Epg::exit, this)));

            foreach (ExtraMenuItem& item, global->menu_items)
            em.add_persistent_item(item);

            em.mainloop();
            screen_updater->timer.add(TimeElement("epg", boost::bind( &Epg::check_epg_print, this), boost::bind(&Epg::epg_print,
                    this)));
        } else if (input.command == "search") {
            screen_updater->timer.deactivate("epg");
            search_func();
            screen_updater->timer.activate("epg");
        } else if (input.command == "toggle_timer") {
            refresh_mutex.enterMutex();
            screen_updater->timer.deactivate("epg");

            toggle_timer(cur_event);
            screen_updater->timer.activate("epg");

            refresh_mutex.leaveMutex();

        } else if (input.command == "reload") {
            get_data();
        } else if (input.command == "switch_to_channel") {
            switch_to_channel();
        } else
            update_needed = !global->check_commands(input);
    }

    input_master->restore_map();

    screen_updater->timer.del("epg");

    exit_loop = false;

    screen_updater->timer.activate("audio_fullscreen");

    refresh_mutex.leaveMutex();

    return "";
}

void Epg::print_timelines(double diff) {
    if (diff > 0 && diff < timeslots)
        render->current.add(new RObj(int(grid_x + diff * timeslot_interval * SECONDS_PER_MINUTE / scale_factor), grid_y - spacing, 2, rows * (grid_size.second + grid_spacing) + spacing * 2,
                themes->epg_time_bar1, themes->epg_time_bar2,
                themes->epg_time_bar3, themes->epg_time_bar4, 1));

    render->current.add(new RObj(grid_x, grid_y - spacing, 2, rows * (grid_size.second + grid_spacing) + spacing * 2,
            themes->epg_bar1, themes->epg_bar2,
            themes->epg_bar3, themes->epg_bar4, 1));

    for (int x = 0; x < timeslots; ++x) {
        time_t tmp_time = cur_time + x*timeslot_interval*SECONDS_PER_MINUTE;
        char time_str[30];
        strftime(time_str, 30, "%H:%M", localtime(&tmp_time));

        std::pair<int, int> time_size = string_format::calculate_string_size(time_str, grid_font);
        render->current.add(new TObj(time_str, grid_font,
                static_cast<int>(grid_x - (time_size.first / 2) + (x * timeslot_interval * SECONDS_PER_MINUTE)/scale_factor), grid_header_y ,
                themes->epg_font1, themes->epg_font2,
                themes->epg_font3, 2));
        render->current.add(new RObj(static_cast<int>(grid_x + ((x+1) * timeslot_interval * SECONDS_PER_MINUTE) / scale_factor), grid_y - spacing, 2, rows * (grid_size.second + grid_spacing) + spacing * 2,
                themes->epg_bar1, themes->epg_bar2, themes->epg_bar3,
                themes->epg_bar4, 1));
    }
}

void Epg::print_channel(int y, BaseChannel *cur_chan) {
    if (display_channel_logo) {
        if (!cur_chan->logo.empty()) {
            render->create_scaled_image_wrapper_upscaled(cur_chan->logo, max_logo_size_x, grid_size.second);

            render->current.add(new PFObj(cur_chan->logo, logo_x, y, max_logo_size_x, grid_size.second, false, 3));
        }
    }
    if (display_channel_name) {
        string channel_name = cur_chan->display_name;
        string_format::format_to_size(channel_name, grid_font, grid_x - channel_x - spacing/2, false);
        render->current.add(new TObj(channel_name, grid_font, channel_x, y,
                themes->epg_font1, themes->epg_font2,
                themes->epg_font3, 3));
    }
}

void Epg::print_channel(int y, std::vector<Channel>::iterator cur_chan) {
    if (display_channel_logo) {
        if (!cur_chan->logo.empty()) {
            render->create_scaled_image_wrapper_upscaled(cur_chan->logo, max_logo_size_x, grid_size.second);

            render->current.add(new PFObj(cur_chan->logo, logo_x, grid_y + y * (grid_size.second + grid_spacing), max_logo_size_x, grid_size.second, false, 3));
        }
    }
    if (display_channel_name) {
        string channel_name = cur_chan->display_name;
        string_format::format_to_size(channel_name, grid_font, grid_x - channel_x - spacing/2, false);
        render->current.add(new TObj(channel_name, grid_font, channel_x, grid_y + y * (grid_size.second + grid_spacing) ,
                themes->epg_font1, themes->epg_font2,
                themes->epg_font3, 3));
    }
}

void Epg::BeginAnimation() {
    render->device->animation_section_begin();
    curlayer = render->device->get_current_layer();
    render->device->animation_section_end();
}

void Epg::EndAnimation() {
    render->device->animation_section_begin();
    render->device->switch_to_layer(curlayer+1);
    render->device->reset_layout_attribs_nowait();
    render->device->animation_zoom(0.90, 0.90, 1, 1, 40, curlayer+1);
    render->device->animation_fade(0, 1, 50, curlayer+1);
    render->device->animation_fade(1, 0, 60, curlayer);
    render->device->animation_section_end();
}

void Epg::FadeOut() {
    render->device->animation_section_begin();
    render->device->switch_to_layer(curlayer);
    render->device->reset_layout_attribs_nowait();
    render->device->animation_fade(0, 1, 80, curlayer);
    render->device->animation_zoom(1, 1, 4, 4, 40, curlayer+1);
    render->device->animation_fade(1, 0, 40, curlayer+1);
    render->device->animation_section_end();
}

void Epg::print() {
    int y = border_top;

    if (!all_logos) {
        display_channel_name = true;
    }

    if (display_channel_logo) {
        logo_x = border_left;
        channel_x = logo_x + max_logo_size_x + spacing;
    } else {
        logo_x = 0;
        channel_x = border_left;
        display_channel_name = true;
    }

    if (display_channel_name == false) { // This only happens when all Channel-Logos are present and display_channel_name is set to false
        grid_header_x = border_left + max_logo_size_x + spacing;
        grid_x = grid_header_x;
    } else {
        grid_header_x = int((conf->p_h_res() * 0.25) + spacing/2);
        grid_x = grid_header_x;
    }

    scale_factor = (timeslot_interval * SECONDS_PER_MINUTE * timeslots) / (double(conf->p_h_res() - grid_x - border_right));

    render->prepare_new_image();

    if (animation) {
        BeginAnimation();
    }

#if 0
    render->current.add(new PObj(themes->epg_background, 0, 0, 0, SCALE_FULL));
#else
    render->current.add(new BgRObj(0));
#endif

    PObj *back = new PFObj(themes->startmenu_epg, border_left, y, header_box_size, header_box_size, 1, true);

    render->current.add(back);

    S_Touch::get_instance()->register_area(TouchArea(rect(back->x, back->y, back->w, back->h), back->layer, boost::bind( &Epg::exit, this)));

    if (themes->show_header) {
        TObj *t = new TObj(dgettext("mms-epg", "EPG"), header_font, border_left + header_box_size + spacing, y + (header_box_size - header_size.second) / 2,
                themes->epg_header_font1, themes->epg_header_font2,
                themes->epg_header_font3, 1);

        render->current.add(t);

        S_Touch::get_instance()->register_area(TouchArea(rect(t->x, t->y, t->w, t->h), t->layer, global->toggle_playlist));
    }

    char time_str[30];
    time_t tmp_time = time(0);
    strftime(time_str, 30, "%x - %R", localtime(&tmp_time));

    int x_size = string_format::calculate_string_width(time_str, normal_font);

    int time_pos_from_right = 15;

    if (conf->p_busy_indicator())
        time_pos_from_right = 32;

    render->current.add(new TObj(time_str, normal_font, conf->p_h_res()-(x_size + time_pos_from_right), y +(header_box_size - normal_size.second) / 2,
            themes->epg_font1, themes->epg_font2,
            themes->epg_font3, 2));

    y += header_size.second + spacing;

    grid_header_y = border_top + header_size.second + spacing + annotation_size.second;
    grid_spacing = int(3 * conf->p_v_res() / 405); // for adjustment of vertical spacing on higher resolutions
    grid_y = grid_header_y + normal_size.second + spacing;

    y_start = grid_y;

    rows = (conf->p_v_res() - (grid_y + border_bottom + normal_size.second + 2 * spacing)) / (grid_size.second + grid_spacing);

    strftime(time_str, 30, "%x - %R", localtime(&cur_time));

    x_size = string_format::calculate_string_width(time_str, annotation_font);

    render->current.add(new TObj(time_str, annotation_font, (conf->p_h_res()-x_size)/2, y,
            themes->epg_font1, themes->epg_font2,
            themes->epg_font3, 2));

    time_t temp_time = time(0);

    double diff = 0;
    diff += (temp_time-cur_time)/(timeslot_interval*SECONDS_PER_MINUTE);
    diff += ((temp_time-cur_time)/SECONDS_PER_MINUTE-diff*timeslot_interval) /timeslot_interval;

    print_timelines(diff);

    if ((grid_x + timeslots * timeslot_interval * SECONDS_PER_MINUTE / scale_factor + border_right) < conf->p_h_res()) {
        tmp_time = cur_time + timeslots*timeslot_interval*SECONDS_PER_MINUTE;
        strftime(time_str, 30, "%H:%M", localtime(&tmp_time));
        render->current.add(new TObj(time_str, grid_font,
                int(grid_x - timeslots*timeslot_interval*SECONDS_PER_MINUTE/scale_factor),
                grid_header_y,
                themes->epg_font1, themes->epg_font2, themes->epg_font3, 2));
    }

    std::ostringstream footer;
    int x, w;
    bool found_match = false;

    vector<Channel>::iterator cur_chan = cur_channel;

    for (int y = 0; y < std::min(rows, int(channels.size())); ++y) {
        print_channel(y, cur_chan);

        foreach (EpgEvent* program, cur_chan->program) {
            if (program->stop> cur_time &&
                    program->start < (cur_time + timeslots*timeslot_interval*SECONDS_PER_MINUTE)) {

                if (program->start < cur_time) {
                    x = grid_x + 2;
                    w = int((program->stop - cur_time)/scale_factor);
                } else {
                    x = int(grid_x + 2 + (program->start - cur_time)/scale_factor);
                    w = int((program->stop - program->start)/scale_factor);
                }

                if (w < 14) // very short shows
                w = 14;

                if (x + w> grid_x + (timeslots*timeslot_interval*SECONDS_PER_MINUTE)/scale_factor) {
                    // programs ending at the end of screen should just be cut off, others
                    // should cross the line a little to indicate that they end later.
                    w -= (x + w) - int(grid_x+(timeslots*timeslot_interval*SECONDS_PER_MINUTE)/scale_factor);
                    if (program->stop> cur_time+timeslots*timeslot_interval*SECONDS_PER_MINUTE)
                    w += 15;
                }

                if (cur_chan == sel_channel && program->start <= sel_time &&
                        program->stop> sel_time) {
                    if (!found_match) {

                        found_match = true;

                        RObj *r = program->Print(render, themes, x, grid_y + y * (grid_size.second + grid_spacing), w, grid_size.second, true, grid_font);

                        footer.str("");
                        strftime(time_str, 30, "%H:%M", localtime(&program->start));
                        footer << time_str << " - ";
                        strftime(time_str, 30, "%H:%M", localtime(&program->stop));
                        footer << time_str << dgettext("mms-epg", " on ") << cur_chan->display_name;

                        string f = footer.str() + "   " + program->title;

                        int f_size = f.size();
                        string_format::format_to_size(f, grid_font, conf->p_h_res() - border_left - border_right, false);

                        if (f.size() != f_size)
                        f += "...";

                        render->current.add(new TObj(f, grid_font, border_left, grid_y + rows * (grid_size.second + grid_spacing) + (2 * spacing), themes->epg_font1,
                                        themes->epg_font2, themes->epg_font3, 4));

                        S_Touch::get_instance()->register_area(TouchArea(rect(r->x, r->y, r->w, r->h), r->layer,
                                        boost::bind(&Epg::print_description, this, program)));
                    }
                } else {

                    RObj *r = program->Print(render, themes, x, grid_y + y * (grid_size.second + grid_spacing), w, grid_size.second, false, grid_font);

                    S_Touch::get_instance()->register_area(TouchArea(rect(r->x, r->y, r->w, r->h), r->layer,
                                    boost::bind(&Epg::print_description, this, program)));

                }
            }
        }
        ++cur_chan;
    }

    if (animation) {
        EndAnimation();
    }

    render->draw_and_release("epg");
}

void Epg::print_description(EpgEvent *p) {
    SvdrpEpgEvent *se;
    XmlEpgEvent *xe;

    se = (SvdrpEpgEvent *)p;
    xe = (XmlEpgEvent *)p;

    if (use_svdrp) {
        svdrp_mutex.enterMutex();
        if (svdrp->Connect()) {
            svdrp->ClearTimer(&Timers);
            if (svdrp->GetTimer(&Timers)) {
                svdrp->AssignTimersAndEpgEvents(&channels, &Timers);
            }
            svdrp->Disconnect();
        }
        svdrp_mutex.leaveMutex();

        animation = true;

        se->PrintDescription(render, themes, conf->p_h_res(), conf->p_v_res());

        animation = false;

    } else {
        animation = true;

        xe->PrintDescription(render, themes, conf->p_h_res(), conf->p_v_res());

        animation = false;

    }
}

void Epg::epg_print() {
    if (conf->p_idle_time() != 0) {
        // update the idle timer
        int idle = time(0)-conf->p_last_key();
        if (idle > SECONDS_PER_MINUTE*conf->p_idle_time() && ((*program_iter)->stop > time(0) && (*program_iter)->start < (time(0) + timeslots*timeslot_interval *SECONDS_PER_MINUTE)))
            update_time();
    }

    if (use_svdrp) {
        svdrp_mutex.enterMutex();
        if (svdrp->Connect()) {
            svdrp->ClearTimer(&Timers);
            if (svdrp->GetTimer(&Timers)) {
                svdrp->AssignTimersAndEpgEvents(&channels, &Timers);
            }
            svdrp->Disconnect();
        }
        svdrp_mutex.leaveMutex();
    }

    print();
}

int Epg::check_epg_print() {
    return SECONDS_PER_MINUTE*1000; // ms
}

int Epg::check_timers_print() {
    rec_on_off = !rec_on_off;
    return 800; // ms
}

void Epg::update_svdrp_data() {
    if (!ende && svdrp && !update_running) {
        update_running = true;
        DialogWaitPrint pdialog(dgettext("mms-epg", "Rereading TV Data"), 1000);
        epg_updater->start();
    }
}

void Epg::update_epg_datafile() {
    if (!ende && !epg_conf->p_epg_update_script().empty() && file_exists(epg_conf->p_epg_update_script()) && !update_running) {
        update_running = true;
        epg_updater->start();
    } else {
        epg_script_found = false;
        print_warning(dgettext("mms-epg", "Could not find epg update program"), "EPG");
    }
}

void Epg::TimeStamp(const std::string message) {
    char timestr[256];

    time_t now = time(NULL);
    struct tm *nowinfo = localtime(&now);

    strftime(timestr, 255, "%d.%m.%Y %H:%M:%S", nowinfo);

    printf("%s - Timestamp (%s)\n", timestr, message.c_str());
}

bool Epg::read_xml_file(vector<Channel> *chan) {
    std::vector<Channel>::iterator ch;
    std::vector<EpgEvent *> new_program, old_program;
    std::vector<EpgEvent *>::iterator e;

    TiXmlElement* root_element;
    TiXmlElement* element;

    refresh_mutex.enterMutex();

    if (!file_exists(epg_conf->p_epg_data())) {
        refresh_mutex.leaveMutex();
        return false;
    }

    TiXmlDocument doc(epg_conf->p_epg_data().c_str());

    doc.LoadFile();
    root_element = doc.RootElement();

    if (root_element == 0) {
        refresh_mutex.leaveMutex();
        return false;
    }

    if (channels.empty()) {

        channels.clear();

        int channel_number = 1;

        for (element = root_element->FirstChildElement("channel"); element != 0; element = element->NextSiblingElement("channel")) {
            Channel temp = XMLChannel(channel_number++, element);
            chan->push_back(temp);
        }

        for (ch = channels.begin(); ch != channels.end(); ch++) {
            if (ch->program.empty()) {
                time_t now = time(NULL);
                XmlEpgEvent *ev = new XmlEpgEvent(&(*ch), now, 7 * 24*60*SECONDS_PER_MINUTE, dgettext("mms-epg", "No Epg available"));
                ch->program.push_back(ev);
            }
        }

        cur_channel = channels.begin();
        sel_channel = cur_channel;
        sel_channel_num = 0;

        loaded_correctly = true;
        reload_needed = false;

        all_logos = find_logos();

        refresh_mutex.leaveMutex();
    } else {
        for (ch = channels.begin(); ch != channels.end(); ch++) {
            ch->program.clear();
            time_t now = time(NULL);
            XmlEpgEvent *ev = new XmlEpgEvent(&(*ch), now, 7 * 24*60*SECONDS_PER_MINUTE, dgettext("mms-epg", "No Epg available"));
            ch->program.push_back(ev);
        }

        cur_channel = channels.begin();
        sel_channel = cur_channel;
        sel_channel_num = 0;

        loaded_correctly = true;
        reload_needed = false;

        refresh_mutex.leaveMutex();
    }

    string last_id = "";
    time_t min, max;

    search_mutex.enterMutex();
    all_events.clear();

    for (element = root_element->FirstChildElement("programme"); element != 0; element = element->NextSiblingElement("programme")) {
        std::string channel_id = element->Attribute("channel");
        for (ch = channels.begin(); (ch != channels.end() && channel_id != ch->id); ch++) {
        }

        if (ch != channels.end()) { // Found matching Channel !!!

            XmlEpgEvent *p = new XmlEpgEvent(&(*ch), element, timezone);

            refresh_mutex.enterMutex();
            new_program = ch->program;
            old_program = ch->program;

            // Sort into vector
            e = new_program.begin();

            if ((*e)->title == dgettext("mms-epg", "No Epg available")) {
                new_program.clear();
            }

            for (e = new_program.begin(); e != new_program.end() && p->start > (*e)->start; e++) {
            }
            if (e == new_program.end()) {
                if (new_program.size() > 0) {
                    --e;
                    if ((*e)->stop == 0) {
                        (*e)->stop = p->start;
                    }
                }
                new_program.push_back(p);
            } else if (e == new_program.begin()) {
                std::vector<EpgEvent *>::iterator t = new_program.insert(e, p);
                e = ++t;
                if ((*e)->stop == 0) {
                    (*e)->stop = p->start;
                }
            }
            if (min == 0)
                min = p->start;

            if (min > p->start)
                min = p->start;

            if (max == 0)
                max = (p->stop == 0 ? p->start : p->stop);

            if (max < p->stop)
                max = p->stop;

            if (ch == chan->begin()) {
                cur_time = time(0);
                sel_time = cur_time;

                for (program_iter = sel_channel->program.begin(), program_iter_end = sel_channel->program.end(); program_iter
                        != program_iter_end; ++program_iter)
                    if ((*program_iter)->stop >= sel_time)
                        break;

                if (program_iter != program_iter_end) {
                    cur_time = sel_time = (*program_iter)->start;
                    cur_event = *program_iter;
                }
            }
        }
        ch->program = new_program;

        old_program.clear();
        refresh_mutex.leaveMutex();
    }

    if (channels.size() == 0) {
        return false;
    }

    for (ch = channels.begin(); ch != channels.end(); ch++) {
        std::vector<EpgEvent *>::iterator si, ei;
        for (ei = ch->program.begin(); ei != ch->program.end(); ei++) {
            if (all_events.empty()) {
                all_events.push_back(*ei);
            } else {
                for (si = all_events.begin(); si != all_events.end() && (*ei)->start > (*si)->start; si++) {
                }
                if (si == all_events.end()) {
                    all_events.push_back(*ei);
                } else {
                    all_events.insert(si, *ei);
                }
            }
        }
    }

    search_mutex.leaveMutex();

    return true;
}

bool Epg::GetSvdrpData() {
    std::vector<Channel>::iterator ch;
    std::vector<EpgEvent *> new_program, old_program;

    refresh_mutex.enterMutex();

    svdrp_mutex.enterMutex();
    if (!svdrp->Connect()) {
        refresh_mutex.leaveMutex();
        svdrp_mutex.leaveMutex();
        return false;
    }

    if (channels.empty()) {

        channels.clear();
        // Elemente noch löschen !!!!
        Timers.clear();

        if (!svdrp->ListChannels(svdrp_channelnumbers, &channels)) {
            svdrp->Disconnect();
            refresh_mutex.leaveMutex();
            svdrp_mutex.leaveMutex();
            return false;
        }

        for (ch = channels.begin(); ch != channels.end(); ch++) {
            if (ch->program.empty()) {
                time_t now = time(NULL);
                SvdrpEpgEvent *ev = new SvdrpEpgEvent(&(*ch), 1, ch->id, now, 7 * 24*60*SECONDS_PER_MINUTE, 0, 0);
                ev->title = dgettext("mms-epg", "No Epg available");

                ch->program.push_back(ev);
            }
        }

        cur_channel = channels.begin();
        sel_channel = cur_channel;
        sel_channel_num = 0;

        loaded_correctly = true;
        reload_needed = false;

        all_logos = find_logos();

        refresh_mutex.leaveMutex();
    } else {
        refresh_mutex.leaveMutex();
    }

    search_mutex.enterMutex();
    all_events.clear();
    for (ch = channels.begin(); ch != channels.end(); ch++) {
        new_program = svdrp->GetEpgEvents(&(*ch), ch->number);

        if (!new_program.empty()) {
            refresh_mutex.enterMutex();
            old_program = ch->program;
            ch->program = new_program;

            old_program.clear();

            if (ch == channels.begin()) {
                cur_time = time(0);
                sel_time = cur_time;

                for (program_iter = sel_channel->program.begin(), program_iter_end = sel_channel->program.end(); program_iter
                        != program_iter_end; ++program_iter)
                    if ((*program_iter)->stop >= sel_time)
                        break;

                if (program_iter != program_iter_end) {
                    cur_time = sel_time = (*program_iter)->start;
                    cur_event = *program_iter;
                }
            }

            std::vector<EpgEvent *>::iterator si, ei;
            for (ei = ch->program.begin(); ei != ch->program.end(); ei++) {
                if (all_events.empty()) {
                    all_events.push_back(*ei);
                } else {
                    for (si = all_events.begin(); si != all_events.end() && (*ei)->start > (*si)->start; si++) {
                    }
                    if (si == all_events.end()) {
                        all_events.push_back(*ei);
                    } else {
                        all_events.insert(si, *ei);
                    }
                }
            }
            refresh_mutex.leaveMutex();
        }
    }
    search_mutex.leaveMutex();

    svdrp->ClearTimer(&Timers);
    if (!svdrp->GetTimer(&Timers)) {
        svdrp->Disconnect();
        svdrp_mutex.leaveMutex();
        return false;
    }

    if (!svdrp->AssignTimersAndEpgEvents(&channels, &Timers)) {
        svdrp->Disconnect();
        svdrp_mutex.leaveMutex();
        return false;
    }
    svdrp->Disconnect();

    svdrp_mutex.leaveMutex();

    return true;
}

void EpgUpdate::run() {
    EPGConfig *epg_conf = S_EPGConfig::get_instance();

    Epg *epg = get_class<Epg>(dgettext("mms-epg", "Electronic Program Guide"));

    if (!epg) {
        return;
    }
    if (epg->use_svdrp) {
        if (epg->svdrp) {
            if (epg->GetSvdrpData()) {
                DialogWaitPrint pdialog(2000);
                pdialog.add_line(dgettext("mms-epg", "EPG TV listings reloaded from Svdrp-Host"));
                pdialog.print();
                epg->reload_needed = false;
            } else {
                Print pdialog(Print::SCREEN);
                pdialog.add_line(dgettext("mms-epg", "EPG TV listings couldn't be loaded from Svdrp-Host"));
                pdialog.add_line("");
                pdialog.print();
                epg->reload_needed = true;
            }
            epg->update_running = false;
        }
    } else {
        run::external_program(epg_conf->p_epg_update_script());

        DialogWaitPrint pdialog(2000);
        pdialog.add_line(dgettext("mms-epg", "EPG TV listings file reloaded"));
        pdialog.add_line("");
        pdialog.add_line(dgettext("mms-epg", "The new data file will be used"));
        pdialog.add_line(dgettext("mms-epg", "the next time you enter EPG"));
        pdialog.print();

        epg->reload_needed = false;
        epg->update_running = false;
    }
}

int Epg::check_epg_datafile() {
    time_t now = time(0);

    if (!epg_conf->p_epg_data().empty()) {
        if ((now - lastrefresh > (interval * 60)) && !update_running) {
            struct stat s;

            stat(epg_conf->p_epg_data().c_str(), &s);

            time_t diff = s.st_mtime - lastrefresh;

            if (diff > 0) {
                lastrefresh = now;
                return 0;
            }
        }
        return (epg_conf->p_interval() >= 5) ? (epg_conf->p_interval() * 60 * 1000) : (5 * 60 * 1000); // ms
    } else
        return 24*60*60*1000; // just wait a long time
}

int Epg::check_svdrp_data() {
    time_t now = time(0);

    if (svdrp) {
        if ((now - lastrefresh > (interval * 60)) && !update_running) {
            lastrefresh = now;
            return 0; // update now
        }
        return (epg_conf->p_interval() >= 5) ? (epg_conf->p_interval() * 60 * 1000) : (5 * 60 * 1000); // ms
    } else {
        return 24*60*60*1000; // Wait a day
    }
}

void Epg::startup_updater() {
    ScreenUpdater *screen_updater = S_ScreenUpdater::get_instance();

    if (use_svdrp) {
        screen_updater->timer.add(TimeElement("epg_data_update", boost::bind( &Epg::check_svdrp_data, this), boost::bind(
                &Epg::update_svdrp_data, this)));
    } else {
        screen_updater->timer.add(TimeElement("epg_data_update", boost::bind( &Epg::check_epg_datafile, this), boost::bind(
                &Epg::update_epg_datafile, this)));

    }
}

void Epg::reenter() {
    if (!loaded_correctly) {
        get_data();
        loaded_correctly = true;
    }

    if (!check_tv_data()) {

        reload_needed = true;
        return;
    }
}

std::string Epg::show_timers() {
    fprintf(stderr, "Yes - show Timer\n");
    Input input;

    input_master->save_map();
    input_master->set_map("epg");

    ScreenUpdater *screen_updater = S_ScreenUpdater::get_instance();

    // disable screensaver
    screen_updater->timer.deactivate("audio_fullscreen");

    screen_updater->timer.add(TimeElement("epg", boost::bind( &Epg::check_timers_print, this),
            boost::bind(&Epg::print_timers, this)));

    bool update_needed = true;

    BusyIndicator *busy_indicator = S_BusyIndicator::get_instance();
    Shutdown *sd = S_Shutdown::get_instance();

    start = 0;
    cursor = 0;

    if (use_svdrp) {
        svdrp_mutex.enterMutex();
        if (!svdrp->Connect()) {
            svdrp_mutex.leaveMutex();
            return false;
        }
        svdrp->ClearTimer(&Timers);
        if (!svdrp->GetTimer(&Timers)) {
            svdrp->Disconnect();
            svdrp_mutex.leaveMutex();
            return false;
        }
        if (!svdrp->AssignTimersAndEpgEvents(&channels, &Timers)) {
            svdrp->Disconnect();
            svdrp_mutex.leaveMutex();
            return false;
        }
        svdrp->Disconnect();
        svdrp_mutex.leaveMutex();
    }

    animation = true;

    while (!exit_loop) {
        if (update_needed) {

            refresh_mutex.enterMutex();
            print_timers();
            animation = false;
            refresh_mutex.leaveMutex();
        }
        if ((int)Timers.size() > 0) {
            curtimer = Timers[start + cursor];
        } else {
            curtimer = NULL;
        }

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

        if (input.command == "back" && input.mode == "general") {
            exit();
        } else if (input.command == "next") {
            if (cursor < (rows - 1)) {
                if ((cursor + 1) < (int)Timers.size()) {
                    cursor++;
                }
            } else {
                if ((start + rows) < (int)Timers.size()) {
                    start++;
                }
            }
        } else if (input.command == "prev") {
            if (cursor > 0) {
                cursor--;
            } else {
                if (start > 0) {
                    start--;
                }
            }
        } else if ((input.command == "action" || input.command == "toggle_timer") && curtimer != NULL) {
            if (curtimer->epgevents.size() > 0) {
                cur_event = curtimer->epgevents[0];
                screen_updater->timer.deactivate("epg");
                if (toggle_timer(curtimer->epgevents[0])) {
                    start = 0;
                    cursor = 0;
                }
                update_needed = true;
                screen_updater->timer.activate("epg");
            } else {
                Print pdialog(Print::SCREEN);
                pdialog.add_line(dgettext("mms-epg", "Timer is not assigned to Epg-Event - "));

                pdialog.add_line(dgettext("mms-epg", "modification is not possible!"));
                pdialog.add_line("");
                pdialog.print();
            }
        } else if (input.command == "second_action") {
            if (curtimer->epgevents.size() > 0) {
                cur_event = curtimer->epgevents[0];
            } else {
                cur_event = NULL;
            }

            screen_updater->timer.del("epg");
            // screen_updater->timer.deactivate("epg");
            ExtraMenu em;

            em.add_item(ExtraMenuItem(dgettext("mms-epg", "Toggle Timer"), input_master->find_shortcut("toggle_timer"),
                    boost::bind(&Epg::toggle_timer, this)));

            em.add_item(ExtraMenuItem(dgettext("mms-epg", "Toggle Timer"), input_master->find_shortcut("action"), boost::bind(
                    &Epg::toggle_timer, this)));

            em.add_item(ExtraMenuItem(dgettext("mms-epg", "Delete Timer"), input_master->find_shortcut("delete_timer"),
                    boost::bind(&Epg::delete_timer, this)));

            em.add_item(ExtraMenuItem(dgettext("mms-epg", "Return to startmenu"), input_master->find_shortcut("back"), boost::bind(
                    &Epg::exit, this)));

            foreach (ExtraMenuItem& item, global->menu_items)
            em.add_persistent_item(item);

            em.mainloop();

            screen_updater->timer.add(TimeElement("epg", boost::bind( &Epg::check_timers_print, this), boost::bind(
                    &Epg::print_timers, this)));

        } else if (input.command == "delete_timer") {
            screen_updater->timer.deactivate("epg");

            if (delete_timer()) {
                start = 0;
                cursor = 0;
                update_needed = true;
            }

            screen_updater->timer.activate("epg");

        } else if (input.command == "reload") {
        } else {
            update_needed = !global->check_commands(input);
        }
    }
    input_master->restore_map();

    exit_loop = false;

    screen_updater->timer.del("epg");
    screen_updater->timer.activate("audio_fullscreen");

    refresh_mutex.leaveMutex();

    return "";
}

void Epg::print_timers() {
    std::vector<SvdrpTimerEvent>::iterator ti;
    int i;

    refresh_mutex.enterMutex();
    int y = border_top;

    if (!all_logos) {
        display_channel_name = true;
    }

    if (display_channel_logo) {
        logo_x = border_left;
        channel_x = logo_x + max_logo_size_x + spacing;
    } else {
        logo_x = 0;
        channel_x = border_left;
        display_channel_name = true;
    }

    if (display_channel_name == false) { // This only happens when all Channel-Logos are present and display_channel_name is set to false
        grid_header_x = border_left + max_logo_size_x + spacing;
        grid_x = grid_header_x;
    } else {
        grid_header_x = int((conf->p_h_res() * 0.25) + spacing/2);
        grid_x = grid_header_x;
    }

    scale_factor = (timeslot_interval * SECONDS_PER_MINUTE * timeslots) / (double(conf->p_h_res() - grid_x - border_right));

    render->prepare_new_image();

    if (animation) {
        BeginAnimation();
    }

#if 0
    render->current.add(new PObj(themes->epg_background, 0, 0, 0, SCALE_FULL));
#else
    render->current.add(new BgRObj(0));
#endif

    PObj *back = new PFObj(themes->startmenu_timer, border_left, y, header_box_size, header_box_size, 1, true);

    render->current.add(back);

    S_Touch::get_instance()->register_area(TouchArea(rect(back->x, back->y, back->w, back->h), back->layer, boost::bind( &Epg::exit, this)));

    if (themes->show_header) {
        TObj *t = new TObj(dgettext("mms-epg", "EPG - Show Timer"), header_font, border_left + header_box_size + spacing, y + (header_box_size - header_size.second) / 2,
                themes->epg_header_font1, themes->epg_header_font2,
                themes->epg_header_font3, 1);

        render->current.add(t);

        S_Touch::get_instance()->register_area(TouchArea(rect(t->x, t->y, t->w, t->h), t->layer, global->toggle_playlist));
    }

    char time_str[30];
    time_t tmp_time = time(0);
    strftime(time_str, 30, "%x - %R", localtime(&tmp_time));

    int x_size = string_format::calculate_string_width(time_str, normal_font);

    int time_pos_from_right = 15;

    if (conf->p_busy_indicator())
        time_pos_from_right = 32;

    render->current.add(new TObj(time_str, normal_font, conf->p_h_res()-(x_size + time_pos_from_right), y +(header_box_size - normal_size.second) / 2,
            themes->epg_font1, themes->epg_font2,
            themes->epg_font3, 2));

    y += header_size.second + spacing;

    grid_header_y = border_top + header_size.second + spacing + annotation_size.second;
    grid_spacing = int(3 * conf->p_v_res() / 405); // for adjustment of vertical spacing on higher resolutions
    grid_y = y;

    y_start = grid_y;

    rows = (int)(conf->p_v_res() - (grid_y + border_bottom)) / (grid_size.second + grid_spacing);

    int y_off;

    for (i = 0; (i < rows) && ((i + start) < Timers.size()); i++) {
        SvdrpTimerEvent *ste;

        ste = Timers[i + start];

        y_off = (i * (grid_size.second + grid_spacing));

        if (ste->epgevents.size() > 0) {
            SvdrpEpgEvent *sev = (SvdrpEpgEvent *)ste->epgevents[0];
            print_channel(grid_y + y_off, sev->basechannel);
        } else {
            BaseChannel *bc = (BaseChannel *)(&channels[ste->Channel() - 1]);
            print_channel(grid_y + y_off, bc);
        }

        int recording_x = grid_x;
        int date_x = recording_x + spacing/2 + grid_size.second;
        int date_size = string_format::calculate_string_width("31.12.1999", grid_font) > string_format::calculate_string_width(
                "WWWWWWW", grid_font) ? string_format::calculate_string_width("31.12.1999", grid_font)
                : string_format::calculate_string_width("WWWWWW", grid_font);
        int time_x = date_x + spacing/2 + date_size;
        int time_size = string_format::calculate_string_width("23:59 - 23:59", grid_font);
        int title_x = time_x + spacing/2 + time_size;

        std::string date;
        std::string time;

        time = "";
        strftime(time_str, 20, "%H:%M", localtime( &(ste->start)));
        time.append(time_str);
        time.append(" - ");
        strftime(time_str, 20, "%H:%M", localtime( &(ste->stop)));
        time.append(time_str);

        if (ste->IsRecording() && rec_on_off) {
            render->current.add(new PFObj(themes->epg_recording, recording_x, grid_y + y_off + (int)(grid_size.second*0.25)/2, (int)grid_size.second *0.75, (int)grid_size.second * 0.75, 3, true));
        }

        if (ste->IsRepetitive()) {
            date = ste->Days();
            for (int wd = 0; wd < WEEKDAYS; wd++) {
                int x_off = string_format::calculate_string_width("W", grid_font) - string_format::calculate_string_width(
                        date.substr(wd, 1), grid_font);

                render->current.add(new TObj(date.substr(wd, 1), grid_font, date_x + x_off / 2 + (wd * date_size/WEEKDAYS), grid_y + y_off,
                        themes->epg_font1, themes->epg_font2, themes->epg_font3, 3));
            }
        } else {
            strftime(time_str, 20, "%x", localtime( &(ste->start)));
            date.assign(time_str);
            render->current.add(new TObj(date, grid_font, date_x, grid_y + y_off,
                    themes->epg_font1, themes->epg_font2, themes->epg_font3, 3));
        }
        render->current.add(new TObj(time, grid_font, time_x, grid_y + y_off,
                themes->epg_font1, themes->epg_font2, themes->epg_font3, 3));

        string_format::format_to_size(ste->title, grid_font, conf->p_h_res() - title_x - border_right, true);

        if (ste->epgevents.size() > 0) {
            render->current.add(new TObj(ste->title, grid_font, title_x, grid_y + y_off,
                    themes->epg_font1, themes->epg_font2, themes->epg_font3, 3));
        } else {
            render->current.add(new TObj(ste->title, grid_font, title_x, grid_y + y_off,
                    themes->epg_header_font1, themes->epg_header_font2, themes->epg_header_font3, 3));
        }
    }
    render->current.add(new PFObj(themes->general_marked, logo_x - 10, grid_y + cursor * (grid_size.second + grid_spacing) - (grid_spacing / 2), conf->p_h_res() - border_right - logo_x + 20, grid_size.second + grid_spacing, 2, true));

    if (animation) {
        EndAnimation();
    }

    render->draw_and_release("epg");

    refresh_mutex.leaveMutex();
}
