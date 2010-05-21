#include "config.h"

#include "evdev.hpp"

#include "gettext.hpp"
#include "config.hpp"
#include "print.hpp"
#include "common.hpp"

#include "global.hpp"

#include <sstream>

#include <stdio.h>

#include <signal.h>
#include <fcntl.h>
#include <unistd.h>

#include <string.h>

#include <linux/input.h>
#include <error.h>

#include <iostream>

#ifdef use_nls
#include <langinfo.h>
#include <locale.h>
#endif

using std::string;

#define TRIGGER 16

static const char *events[EV_MAX + 1] = {
 "Reset", "Key", "Relative", "Absolute", "", "", "", "",
  "", "", "", "", "", "",  "", "",
  "", "LED", "Sound", "", "Repeat", "ForceFeedback", "", "ForceFeedbackStatus"};

static const char *keys[KEY_MAX + 1] = {
#if !defined alternate_evdev_map
  "Reserved", "Esc", "1", "2", "3", "4", "5", "6",
  "7", "8", "9", "0", "-", "=", "Backspace", "Tab",
  "Q", "W", "E", "R", "T", "Y", "U", "I",
  "O", "P", "LeftBrace", "RightBrace", "Enter", "LeftControl", "A", "S",
  "D", "F", "G", "H", "J", "K", "L", "Semicolon", "Apostrophe",
  "Grave", "LeftShift", "BackSlash", "Z", "X", "C", "V", "B",
   "N", "M", "Comma", "Dot", "Slash", "RightShift", "KPAsterisk", "LeftAlt",
   "Space", "CapsLock", "F1", "F2", "F3", "F4", "F5", "F6",
   "F7", "F8", "F9", "F10", "NumLock", "ScrollLock", "KP7", "KP8",
   "KP9", "KPMinus", "KP4", "KP5", "KP6", "KPPlus", "KP1", "KP2",
   "KP3", "KP0", "KPDot", "103rd", "F13", "102nd", "F11", "F12",
   "F14", "F15", "F16", "F17", "F18", "F19", "F20", "KPEnter",
   "RightCtrl", "KPSlash", "SysRq", "RightAlt", "LineFeed", "Home", "Up", "PageUp",
   "Left", "Right", "End", "Down", "PageDown", "Insert", "Delete", "Macro",
   "Mute", "VolumeDown", "VolumeUp", "Power", "KPEqual", "KPPlusMinus", "Pause", "F21",
   "F22", "F23", "F24", "KPComma", "LeftMeta", "RightMeta", "Compose", "Stop",
   "Again", "Props", "Undo", "Front", "Copy", "Open", "Paste", "Find",
   "Cut", "Help", "Menu", "Calc", "Setup", "Sleep", "WakeUp", "File",
   "SendFile", "DeleteFile", "X-fer", "Prog1", "Prog2", "WWW", "MSDOS", "Coffee",
   "Direction", "CycleWindows", "Mail", "Bookmarks", "Computer", "Back", "Forward", "CloseCD", "EjectCD",
   "EjectCloseCD", "NextSong", "PlayPause", "PreviousSong", "StopCD", "Record", "Rewind", "Phone",
   "ISOKey", "Config", "HomePage", "Refresh", "Exit", "Move", "Edit", "ScrollUp",
   "ScrollDown", "KPLeftParenthesis", "KPRightParenthesis", "International1", "International2", "International3", "International4", "International5",
   "International6", "International7", "International8", "International9", "Language1", "Language2", "Language3", "Language4",
   "Language5", "Language6", "Language7", "Language8", "Language9", "",  "PlayCD", "PauseCD",
   "Prog3", "Prog4", "Suspend", "Close", "", "", "", "",
   "", "", "", "", "", "", "", "",
   "", "", "", "", "", "", "", "",
   "", "", "", "", "", "", "", "",
   "", "", "", "", "", "", "", "",
   "", "", "", "", "", "", "", "",
   "", "", "", "", "", "", "Btn0", "Btn1",
   "Btn2", "Btn3", "Btn4", "Btn5", "Btn6", "Btn7", "Btn8",
   "Btn9", "", "", "", "", "", "", "LeftBtn",
   "RightBtn", "MiddleBtn", "BackwardBtn", "ForwardBtn", "TopWindowBtn", "FastForwardBtn", "FastBackBtn", "",
   "", "", "", "", "", "", "", "Trigger",
   "ThumbBtn", "ThumbBtn2", "TopBtn", "TopBtn2", "PinkieBtn", "BaseBtn", "BaseBtn2", "BaseBtn3",
   "BaseBtn4", "BaseBtn5", "BaseBtn6", "", "", "", "BtnDead", "BtnA",
   "BtnB", "BtnC", "BtnX", "BtnY", "BtnZ", "BtnTL", "BtnTR", "BtnTL2",
   "BtnTR2", "BtnSelect", "BtnStart", "BtnMode", "BtnThumbL", "BtnThumbR", "",
   "ToolPen", "ToolRubber", "ToolBrush", "ToolPencil", "ToolAirbrush", "ToolFinger", "ToolMouse", "ToolLens", "", "",
   "Touch", "Stylus", "Stylus2", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
   "Ok", "Select", "Goto", "Clear", "Power2", "Option", "Info", "Time", "Vendor",
   "Archive", "Program", "Channel", "Favorites", "EPG", "PVR", "MHP", "Language",
   "Title", "Subtitle", "Angle", "Zoom", "Mode", "Keyboard", "Screen", "PC", "TV",
   "TV2", "VCR", "VCR2", "Sat", "Sat2", "CD", "Tape", "Radio", "Tuner", "Player", 
   "Text", "DVD", "Aux", "MP3", "Audio", "Video", "Directory", "List", "Memo",
   "Calendar", "Red", "Green", "Yellow", "Blue", "ChannelUp", "ChannelDown", 
   "First", "Last", "AB", "Play", "Restart", "Slow", "Shuffle", "FastForward", 
   "Previous", "Next", "Digits", "Teen", "Twen", "Break" };
#else
   "Reserved",
   "Esc", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "-", "=", "Backspace", "Tab",
   "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P", "{", "}", "Enter", "LeftControl",
   "A", "S", "D", "F", "G", "H", "J", "K", "L", ";", "'", "`",
   "LeftShift", "\\", "Z", "X", "C", "V", "B", "N", "M", ",", ".", "/", "RightShift", "*", "LeftAlt", " ",
   "CapsLock", "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", "NumLock", "ScrollLock",
              "7", "8", "9", "-",
              "4", "5", "6", "+",
              "1", "2", "3",
              "0", ".", "103rd",
   "F13", "102nd", "F11", "F12", "F14", "F15", "F16", "F17", "F18", "F19", "F20", "KPEnter",
   "RightCtrl", "/", "SysRq", "RightAlt", "LineFeed",
              "Home", "Up", "PageUp",
              "Left", "Right", "End",
              "Down", "PageDown",
              "Insert", "Delete", "Macro",
 "Mute", "VolumeDown", "VolumeUp", "Power",

                "=", "KPPlusMinus",
 "Pause", "F21", "F22", "F23", "F24", ",",
   "LeftMeta", "RightMeta", "Compose",
   "Stop", "Again", "Props",
   "Undo", "Front", "Copy", "Open", "Paste", "Find", "Cut", "Help", "Menu",
   "Calc", "Setup", "Sleep", "WakeUp", "File", "SendFile", "DeleteFile",
   "X-fer", "Prog1", "Prog2", "WWW", "MSDOS", "Coffee", "Direction",
   "CycleWindows", "Mail", "Bookmarks", "Computer",
   "Back", "Forward", "CloseCD", "EjectCD", "EjectCloseCD",
   "NextSong", "PlayPause", "PreviousSong", "StopCD",
   "Record", "Rewind",
   "Phone", "ISOKey",
   "Config", "HomePage", "Refresh", "Exit", "Move", "Edit", "ScrollUp",
   "ScrollDown", "(", ")",
   "International1", "International2", "International3", "International4", "International5",
   "International6", "International7", "International8", "International9",
   "Language1", "Language2", "Language3", "Language4", "Language5", "Language6", "Language7", "Language8", "Language9", "",
   "PlayCD", "PauseCD",
   "Prog3", "Prog4", "Suspend", "Close", "", "", "", "",
   "", "", "", "", "", "", "", "",
   "", "", "", "", "", "", "", "",
   "", "", "", "", "", "", "", "",
   "", "", "", "", "", "", "", "",
   "", "", "", "", "", "", "", "",
   "", "", "", "", "", "",

/* Mouse */
   "Btn0", "Btn1", "Btn2", "Btn3", "Btn4", "Btn5", "Btn6", "Btn7", "Btn8", "Btn9", "", "", "", "", "", "",
   "LeftBtn",  "RightBtn", "MiddleBtn", "BackwardBtn", "ForwardBtn", "TopWindowBtn", "FastForwardBtn", "FastBackBtn",
   "", "", "", "", "", "", "", "",

/* JoyStick */
   "Trigger", "ThumbBtn", "ThumbBtn2", "TopBtn", "TopBtn2", "PinkieBtn",
   "BaseBtn", "BaseBtn2", "BaseBtn3",  "BaseBtn4", "BaseBtn5", "BaseBtn6",
   "", "", "", "BtnDead", "BtnA", "BtnB", "BtnC", "BtnX", "BtnY", "BtnZ",
   "BtnTL", "BtnTR", "BtnTL2", "BtnTR2", "BtnSelect", "BtnStart", "BtnMode",
   "BtnThumbL", "BtnThumbR", "",

/* Tablet -photo */
   "ToolPen", "ToolRubber", "ToolBrush", "ToolPencil", "ToolAirbrush", "ToolFinger", "ToolMouse", "ToolLens", "", "",

/* Printer - Scanner */
   "Touch", "Stylus", "Stylus2", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
   "Ok", "Select", "Goto", "Clear", "Power2", "Option", "Info", "Time", "Vendor",  "Archive",

/* Remote */
   "Program", "Channel", "Favorites", "EPG", "PVR", "MHP",
   "Language", "Title", "Subtitle", "Angle", "Zoom", "Mode",
   "Keyboard", "Screen", "PC", "TV", "TV2", "VCR", "VCR2",
   "Sat", "Sat2", "CD", "Tape", "Radio", "Tuner", "Player",
   "Text", "DVD", "Aux", "MP3", "Audio", "Video",
   "Directory", "List", "Memo", "Calendar",
   "Red", "Green", "Yellow", "Blue", "ChannelUp", "ChannelDown",
   "First", "Last", "AB", "Play", "Restart", "Slow",
   "Shuffle", "FastForward", "Previous", "Next", "Digits", "Teen",
   "Twen", "Break" };
#endif


// static const char *absval[5] = {
//  "Value", "Min  ", "Max  ", "Fuzz ", "Flat " };

static const char *relatives[REL_MAX + 1] = {
  "X", "Y", "Z", "", "", "", "HWheel", "Dial",
  "Wheel" };

static const char *absolutes[ABS_MAX + 1] = {
  "X", "Y", "Z", "Rx", "Ry", "Rz", "Throttle", "Rudder",
  "Wheel", "Gas", "Brake", "", "", "", "", "",
  "Hat0X", "Hat0Y", "Hat1X", "Hat1Y", "Hat2X", "Hat2Y", "Hat3X", "Hat 3Y",
  "Pressure", "Distance", "XTilt", "YTilt"};

static const char *leds[LED_MAX + 1] = {
   "NumLock", "CapsLock", "ScrollLock", "Compose", "Kana", "Sleep", "Suspend", "Mute" };

static const char *repeats[REP_MAX + 1] = {
  "Delay", "Period" };

static const char *sounds[SND_MAX + 1] = {
   "Bell", "Click" };



static const char *direction[6] = {
   "Left", "Right", "Up", "Down", "Front" , "Rear"};


static const char **names[EV_MAX + 1] = {
  events, keys, relatives, absolutes, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  leds, sounds, NULL, repeats, NULL, NULL, NULL };


static const int sizes[EV_MAX + 1] = {
  EV_MAX, KEY_MAX, REL_MAX, ABS_MAX, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  LED_MAX, SND_MAX, 0, REP_MAX, 0, 0, 0 };

static int relvalues[REL_MAX + 1] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
static unsigned int absvalues[ABS_MAX + 1] = {
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0};


#define BITS_PER_LONG (sizeof(long) * 8)
#define NBITS(x) ((((x)-1)/BITS_PER_LONG)+1)
#define OFF(x)  ((x)%BITS_PER_LONG)
#define BIT(x)  (1UL<<OFF(x))
#define LONG(x) ((x)/BITS_PER_LONG)
#define test_bit(bit, array)	((array[LONG(bit)] >> OFF(bit)) & 1)

Evdev::Evdev()
 : exit(false)
{
#ifdef use_nls
  // gettext
  setlocale(LC_ALL, ""); /* set from the environment variables */
  bindtextdomain("mms-evdev", mms_prefix"/share/locale");
  bind_textdomain_codeset("mms-evdev", nl_langinfo(CODESET));
#endif

  Config *conf = S_Config::get_instance();

  evdev_conf = S_EvdevConfig::get_instance();
  evdev_conf->parse_configuration_file(conf->p_homedir());
}

bool Evdev::init()
{
  if ((evsd = open(evdev_conf->p_evdev_dev().c_str(), O_RDONLY | O_NONBLOCK))<0) {
    print_critical(dgettext("mms-evdev", "Could not connect to evdev device ") + evdev_conf->p_evdev_dev(),
		   "EVDEV");
    return false;
  } else {
    print_debug(dgettext("mms-evdev", "Connected to device ") + evdev_conf->p_evdev_dev(), "EVDEV");
    return true;
  }
}

Evdev::~Evdev()
{
  exit = true;

  while (isRunning())
    usleep(50*1000);

  if (evsd)
    ::close(evsd);

  evsd = 0;
}

string ev_parsebuf(input_event *ev)
{
  string value = "";

  std::ostringstream o;
  o << "code " << ev->code << " type " << ev->type
    << " value " << static_cast<int>(ev->value);
  print_debug(o.str(), "EVDEV");

  switch (ev->type) {
  case EV_KEY :
    if ((ev->value==0) && (ev->code <= sizes[ev->type]))
      {
	value = keys[ev->code];
      }
    {
      print_debug("key code = " + value, "EVDEV");
    }
    break;
  case EV_REL :
    if (ev->code <= sizes[ev->type])
      {
	relvalues[ev->code] += ev->value;
	if (abs(relvalues[ev->code]) >= TRIGGER)
	  {
	    value = (names[ev->type])[ev->code];
	    value += direction[(ev->code%3)*2 + ((relvalues[ev->code] > 0) ? 1:0) ];
	    relvalues[ev->code] = 0;
	  }
      }
    {
      print_debug("rel code = " + value, "EVDEV");
    }
    break ;
  case EV_ABS :
    if ((ev->code <= sizes[ev->type]) && (ev->value != 127))
      {
	value = (names[ev->type])[ev->code];
	//		    value += direction[(ev->code%3)*2 + ((absvalues[ev->code] < ev->value) ? 1:0) ];
	//		    absvalues[ev->code] = ev->value;
	if (static_cast<int>(ev->value) <= 0)
	  value += direction[(ev->code%3)*2 + (static_cast<int>(ev->value) < 0) ? 1:0 ];
	else
	  {
	    value += direction[(ev->code%3)*2 + ((absvalues[ev->code] < ev->value) ? 1:0) ];
	    absvalues[ev->code] = ev->value;
	  }
      }
    {
      print_debug("abs code = " + value, "EVDEV");
    }
    break;
  default:
    {
      print_critical(dgettext("mms-evdev", "Unknown input type"), "EVDEV");
    }
  }
  return value;
}

void Evdev::suspend()
{
  ::close(evsd);
  evsd = 0;
}

void Evdev::wake_up()
{
/*  int len = 0;
  struct input_event buffer;

  do {
    len = read(evsd, &buffer, sizeof(buffer));
  } while (len != sizeof(buffer));*/
  init();

}

void Evdev::run()
{
  InputMaster *input_master = S_InputMaster::get_instance();

  struct input_event buffer;
  int len = 0;
  int pollInterval = 50 * 1000; // ms
  bool good = true;

  Input input;

  while (!exit) {
    good = true;

    // read one event from device
    if (evsd != 0)
    {
       len = read(evsd, &buffer, sizeof(buffer));
    }
    else
    {
       len = 0;
    }

    if (len > 0)
      print_debug("Read event, length " + conv::itos(len), "EVDEV");

    if (len != sizeof(buffer))
      {
	usleep(pollInterval);
	good = false;
	continue;
      }

    string input_from_device;

    if ( (input_from_device = ev_parsebuf(&buffer)).empty() )
      {
	good = false;
	continue;
      }

    print_debug("Event parsed, key=" + input_from_device +
		" accept_all=" + conv::btos(accept_all), "EVDEV");

    if (accept_all)
      {
	Input i;

	if (!find_input(input_from_device, i))
	  i.key = input_from_device;

	input = i;
      }
    else
      {
	if (!find_input(input_from_device, input))
	  good = false;
      }

    if (good)
      input_master->add_input(input, "evdev");
  }
}

