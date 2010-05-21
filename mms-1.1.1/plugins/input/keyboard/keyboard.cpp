#include "keyboard.hpp"

#include "global.hpp"

#ifdef use_sdl
#include "common.hpp"
#endif

using std::list;
using std::string;

static const char* convert_table[] = {
 ""     ,"" ,"" ,"" ,"" ,"" ,"" ,"" ,"" ,"TAB","" ,"" ,"" ,"ENTER","" ,"" ,
 ""     ,"" ,"" ,"" ,"" ,"" ,"" ,"" ,"" ,""   ,"" ,"" ,"" ,""     ,"" ,"" ,
 "SPACE","" ,"" ,"" ,"" ,"" ,"" ,"" ,"" ,""   ,"" ,"" ,"" ,""     ,"" ,"" ,
 "0"    ,"1","2","3","4","5","6","7","8","9"  ,":",";","<","="    ,">","?",
 "@"    ,"A","B","C","D","E","F","G","H","I"  ,"J","K","L","M"    ,"N","O",
 "P"    ,"Q","R","S","T","U","V","W","X","Y"  ,"Z","[","" ,"]"    ,"^","_",
 ""     ,"a","b","c","d","e","f","g","h","i"  ,"j","k","l","m"    ,"n","o",
 "p"    ,"q","r","s","t","u","v","w","x","y"  ,"z","{","" ,"}"    ,"~","" ,
 ""     ,"" ,"" ,"" ,"" ,"" ,"" ,"" ,"" ,""   ,"" ,"" ,"" ,""     ,"" ,"" ,
 ""     ,"" ,"" ,"" ,"" ,"" ,"" ,"" ,"" ,""   ,"" ,"" ,"" ,""     ,"" ,"" ,
 ""     ,"" ,"" ,"" ,"" ,"" ,"" ,"" ,"" ,""   ,"" ,"" ,"" ,""     ,"" ,"" ,
 ""     ,"" ,"" ,"" ,"" ,"" ,"" ,"" ,"" ,""   ,"" ,"" ,"" ,""     ,"" ,"" ,
 ""     ,"" ,"" ,"" ,"" ,"" ,"" ,"" ,"" ,""   ,"" ,"" ,"" ,""     ,"" ,"" ,
 ""     ,"" ,"" ,"" ,"" ,"" ,"" ,"" ,"" ,""   ,"" ,"" ,"" ,""     ,"" ,"" ,
 ""     ,"" ,"" ,"" ,"" ,"" ,"" ,"" ,"" ,""   ,"" ,"" ,"" ,""     ,"" ,"" ,
 ""     ,"" ,"" ,"" ,"" ,"" ,"" ,"" ,"" ,""   ,"" ,"" ,"" ,""     ,"" ,"" ,

 "BACKSPACE","DEL" ,"INS" ,"HOME","END","PGUP","PGDOWN","ESC",""  ,""  ,""   ,""   ,""   ,""   ,""   ,"",
 "RIGHT"    ,"LEFT","DOWN","UP"  ,""   ,""    ,""      ,""   ,""  ,""  ,""   ,""   ,""   ,""   ,""   ,"",
 ""         ,""    ,""    ,""    ,""   ,""    ,""      ,""   ,""  ,""  ,""   ,""   ,""   ,""   ,""   ,"",
 ""         ,""    ,""    ,""    ,""   ,""    ,""      ,""   ,""  ,""  ,""   ,""   ,""   ,""   ,""   ,"",
 ""         ,"F1"  ,"F2"  ,"F3"  ,"F4" ,"F5"  ,"F6"    ,"F7" ,"F8","F9","F10","F11","F12","F13","F14","F15"};

Keyboard::Keyboard()
  : halfdelay_time(100), tty_mode(0)
#ifdef use_sdl
  , next_time(0), this_time(0)
  , start_scroll(0), last_valid_x(0), last_valid_y(0)
#endif
{
  running = true;
}

Keyboard::~Keyboard()
{
  running = false;
  terminate(); /* thread */
  normal();
}

bool Keyboard::init()
{
#ifdef use_sdl
  ren = S_Render::get_instance();
#endif
  return true;
}

string convert_int_to_string(int i)
{
  if (i >= 0 && i < 337) {
    string value = convert_table[i];
    return value;
  } else {
    char kbuf[10] = {0};
    snprintf(kbuf, sizeof(kbuf), "#%d", i);
    string keycode = string_format::convert(kbuf);
    return keycode;
 }
  return "";
}

#ifdef use_sdl
static bool suspended = false;

int Keyboard::EventFilter(const SDL_Event *event){
  if (suspended)
    return 0;
  else
    return 1;
}

int Keyboard::SDLWaitEvent(SDL_Event *event){
  int ret;
  while (running){
    ren->device->sdlpump();
    if ((ret = SDL_PeepEvents(event, 1, SDL_GETEVENT, SDL_ALLEVENTS)) < 0)
      return 0;
    if (ret == 1)
      return ret;
    if (suspended)
      mmsSleep(1);
    else
      mmsUsleep(25*1000);
  }
  return 0;
}

string Keyboard::getSDLkey()
{
  SDL_KeyboardEvent event;
  if (SDLWaitEvent(reinterpret_cast<SDL_Event *>(&event))){
    if (event.type == SDL_VIDEOEXPOSE){
      ren->device->refresh_screen();
    } else if (event.type == SDL_KEYDOWN) {
      return get_key_string(event.keysym.sym);
    } else if (event.type == SDL_KEYUP && event.keysym.sym == SDLK_UNKNOWN ) {
      char kbuf[10] = {0};
      snprintf(kbuf, sizeof(kbuf), "#%d", event.keysym.scancode);
      return string_format::convert(kbuf);
    }
#ifdef use_mouse
    else {
      int x, y;
      int button = SDL_GetMouseState(&x, &y);

      if (SDL_BUTTON(button) == SDL_BUTTON_LEFT) {

	struct timeval tv;
	gettimeofday(&tv, NULL);
	this_time = (unsigned long long) tv.tv_sec*1000+tv.tv_usec/1000;

	if (this_time > start_scroll + 1000) {
	  start_scroll = this_time;
	  consecutive_scroll_ok = 0;
	} else {
	  if (consecutive_scroll_ok == 0) {
	    // since this is the first check for scroll, we need to know if
	    // it's up or down
	    if (std::abs(x - last_valid_x) < 50) {
	      if (y != last_valid_y) {
		if (y - last_valid_y > 0)
		  scroll_up = false;
		else
		  scroll_up = true;
		consecutive_scroll_ok += 1;
	      }
	    } 
	  } else {
	    if (std::abs(x - last_valid_x) < 50) {
	      if (scroll_up) {
		if (y - last_valid_y < 0)
		  consecutive_scroll_ok += 1;
	      } else {
		if (y - last_valid_y > 0)
		  consecutive_scroll_ok += 1;
	      }
	    } else
	      consecutive_scroll_ok = 0;
	  }
	}

	if (consecutive_scroll_ok >= 4) {
	  consecutive_scroll_ok = 0;
	  if (scroll_up)
	    return "PGUP";
	  else
	    return "PGDOWN";
	}

	last_valid_x = x;
	last_valid_y = y;

	if (next_time <= this_time) {

	  next_time = (unsigned long long) tv.tv_sec*1000+tv.tv_usec/1000 + 400;
	  S_Touch::get_instance()->got_input(ren->device->convert_xmouse(x), ren->device->convert_ymouse(y));
	  return "touch_input";
	}
      }
    }
#endif
  } else {
    SDL_Delay(75);
  }

  return "";
}

string Keyboard::get_key_string(SDLKey key)
{
  string input_key = "";

  char kbuf[4] = {static_cast<char>(key), 0};

  switch(key) {

  case SDLK_UNKNOWN:   input_key =  "Unknown"; break;
  case SDLK_TAB:       input_key =  "TAB"; break;
  case SDLK_RETURN:    input_key =  "ENTER"; break;
  case SDLK_SPACE:     input_key =  "SPACE"; break;
  case SDLK_BACKSPACE: input_key =  "BACKSPACE"; break;
  case SDLK_DELETE:    input_key =  "DEL"; break;
  case SDLK_INSERT:    input_key =  "INS"; break;
  case SDLK_HOME:      input_key =  "HOME"; break;
  case SDLK_END:       input_key =  "END"; break;
  case SDLK_PAGEUP:    input_key =  "PGUP"; break;
  case SDLK_PAGEDOWN:  input_key =  "PGDOWN"; break;
  case SDLK_ESCAPE:    input_key =  "ESC"; break;
  case SDLK_RIGHT:     input_key =  "RIGHT"; break;
  case SDLK_LEFT:      input_key =  "LEFT"; break;
  case SDLK_UP:        input_key =  "UP"; break;
  case SDLK_DOWN:      input_key =  "DOWN"; break;
  case SDLK_RSHIFT:    input_key =  "RSHIFT"; break;
  case SDLK_LSHIFT:    input_key =  "LSHIFT"; break;
  case SDLK_RCTRL:     input_key =  "RCTRL"; break;
  case SDLK_LCTRL:     input_key =  "LCTRL"; break;
  case SDLK_RALT:      input_key =  "RALT"; break;
  case SDLK_LALT:      input_key =  "LALT"; break;
  case SDLK_KP0:       input_key =  "KP_0"; break;
  case SDLK_KP1:       input_key =  "KP_1"; break;
  case SDLK_KP2:       input_key =  "KP_2"; break;
  case SDLK_KP3:       input_key =  "KP_3"; break;
  case SDLK_KP4:       input_key =  "KP_4"; break;
  case SDLK_KP5:       input_key =  "KP_5"; break;
  case SDLK_KP6:       input_key =  "KP_6"; break;
  case SDLK_KP7:       input_key =  "KP_7"; break;
  case SDLK_KP8:       input_key =  "KP_8"; break;
  case SDLK_KP9:       input_key =  "KP_9"; break;
  case SDLK_KP_PERIOD: input_key =  "KP_PERIOD"; break;
  case SDLK_KP_DIVIDE: input_key =  "KP_DIVIDE"; break;
  case SDLK_KP_MULTIPLY: input_key =  "KP_MULTIPLY"; break;
  case SDLK_KP_MINUS:  input_key =  "KP_MINUS"; break;
  case SDLK_KP_PLUS:   input_key =  "KP_PLUS"; break;
  case SDLK_KP_ENTER:   input_key =  "KP_ENTER"; break;
  case SDLK_KP_EQUALS:   input_key =  "KP_EQUALS"; break;
  case SDLK_NUMLOCK:   input_key =  "NUMLOCK"; break;
  case SDLK_CAPSLOCK:   input_key =  "CAPSLOCK"; break;
  case SDLK_SCROLLOCK:   input_key =  "SCROLLOCK"; break;
  case SDLK_F1:
  case SDLK_F2:
  case SDLK_F3:
  case SDLK_F4:
  case SDLK_F5:
  case SDLK_F6:
  case SDLK_F7:
  case SDLK_F8:
  case SDLK_F9:
  case SDLK_F10:
  case SDLK_F11:
  case SDLK_F12:
  case SDLK_F13:
  case SDLK_F14:
  case SDLK_F15: snprintf(kbuf, sizeof(kbuf), "F%d", key - SDLK_F1 + 1);

  default:
    input_key =  string_format::convert(kbuf);
  }

  return input_key;
}
#endif

void Keyboard::run()
{
#ifdef use_sdl
  SDL_EnableKeyRepeat(500, 30);
  SDL_SetEventFilter(&EventFilter);

#endif
  cbreak();

  InputMaster *input_master = S_InputMaster::get_instance();

  Input input;

#ifdef use_sdl
  bool was_sdl_init = SDL_WasInit(SDL_INIT_VIDEO);
#endif

  while (running) {

    string input_from_device;
#ifdef use_sdl
    if (was_sdl_init) {
      input_from_device = getSDLkey();
    } else {
#endif
      input_from_device = convert_int_to_string(getch2(halfdelay_time));
#ifdef use_sdl
    }
#endif

    if (input_from_device == "touch_input") {
      Input i;
      i.key = input_from_device;
      input = i;
    } else if (accept_all) {

      Input i;

      Input temp;

      if (find_input(input_from_device, temp))
	if (temp.mode == "search" || temp.mode == "general")
	  i = temp;

      i.key = input_from_device;

      input = i;

    } else {
      if (!find_input(input_from_device, input))
	continue;
    }

    input_master->add_input(input, "keyboard");
  }
}

// one-char-at-a-time
void Keyboard::cbreak()
{
#ifdef use_sdl
  if(!SDL_WasInit(SDL_INIT_VIDEO)) {
#endif

  if (tty_mode == 0)
    {
      tcgetattr(0, &orig_tty);
      tty_mode = 1;
      new_tty = orig_tty;
    }

  new_tty.c_lflag &= ~(ICANON | ECHO);
  new_tty.c_cc[VMIN] = 1;
  new_tty.c_cc[VTIME] = 0;
  tcsetattr(0, TCSANOW, &new_tty);

#ifdef use_sdl
  } else /* SDL_WasInit(...) */
    SDL_EnableUNICODE(1);
#endif
}

// normal keyboard mode
void Keyboard::normal()
{
#ifdef use_sdl
  if(!SDL_WasInit(SDL_INIT_VIDEO)) {
#endif

  if (tty_mode == 1)
    {
      tcsetattr(0, TCSANOW, &orig_tty);
      new_tty = orig_tty;
    }

#ifdef use_sdl
  } else /* SDL_WasInit(...) */
    SDL_EnableUNICODE(0);
#endif
}

list<string> Keyboard::not_accepted_keys()
{
  list<string> unaccepted;
  unaccepted.push_back("DEL");
  unaccepted.push_back("INS");
  unaccepted.push_back("TAB");
  unaccepted.push_back("HOME");
  unaccepted.push_back("END");
  unaccepted.push_back("PGUP");
  unaccepted.push_back("PGDOWN");
  unaccepted.push_back("ESC");
  unaccepted.push_back("RIGHT");
  unaccepted.push_back("LEFT");
  unaccepted.push_back("DOWN");
  unaccepted.push_back("UP");
  unaccepted.push_back("ENTER");
  unaccepted.push_back("RSHIFT");
  unaccepted.push_back("LSHIFT");
  unaccepted.push_back("RCTRL");
  unaccepted.push_back("LCTRL");
  unaccepted.push_back("RALT");
  unaccepted.push_back("LALT");
  unaccepted.push_back("F1");
  unaccepted.push_back("F2");
  unaccepted.push_back("F3");
  unaccepted.push_back("F4");
  unaccepted.push_back("F5");
  unaccepted.push_back("F6");
  unaccepted.push_back("F7");
  unaccepted.push_back("F8");
  unaccepted.push_back("F9");
  unaccepted.push_back("F10");
  unaccepted.push_back("F11");
  unaccepted.push_back("F12");
  return unaccepted;
}

void Keyboard::suspend()
{
#ifdef use_sdl
  suspended = true;
#endif
}

void Keyboard::wake_up()
{
#ifdef use_sdl
  suspended = false;
#endif
}
