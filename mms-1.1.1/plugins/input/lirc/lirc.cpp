#include "lirc.hpp"

#include "config.h"

#include "lirc_config.hpp"

#include "gettext.hpp"
#include "config.hpp"
#include "common.hpp"
#include "print.hpp"

#include "global.hpp"

#include <cstdio>

#include <csignal>
#include <fcntl.h>
#include <unistd.h>

#include <iostream>

#include "lirc/lirc_client.h"

#ifdef use_nls
#include <langinfo.h>
#include <locale.h>
#endif

//#define LIRC_NO_BLOCK

// lirc
struct IR {
  char *buf;
  unsigned int code;
  unsigned int serial;
  char key[64];
  char remote[64];
  char rxkey;
};

// static mutex, initted at compile time
static pthread_mutex_t lirc_mutex = PTHREAD_MUTEX_INITIALIZER;
//

char thisprog[] = "mms\0"; /* this to remove an annoying warning at compile time due to the lirc_init() call */

Lirc::Lirc()
  : running(false), flush(false), next_time(0), this_time(0)
{
#ifdef use_nls
  // gettext
  setlocale(LC_ALL, ""); /* set from the environment variables */
  bindtextdomain("mms-lirc", mms_prefix"/share/locale");
  bind_textdomain_codeset("mms-lirc", nl_langinfo(CODESET));
#endif

  Config *conf = S_Config::get_instance();
  LircConfig *lirc_conf = S_LircConfig::get_instance();
  lirc_conf->parse_configuration_file(conf->p_homedir());

  timeinterv = lirc_conf->p_lirc_delay();
  lirc_rate = lirc_conf->p_lirc_rate();
  lirc_fast = lirc_conf->p_lirc_fast();

  running = true;
}

bool Lirc::init()
{
  if ((irsd = lirc_init(thisprog,1)) == -1) {
    print_critical(dgettext("mms-lirc", "Could not connect to lirc daemon"), "LIRC");
    return false;
  }
#ifdef LIRC_NO_BLOCK
  int flags = O_NONBLOCK | fcntl(irsd, F_GETFL);
  fcntl(irsd, F_SETFL, flags ); /* this is a bugfix, it actually sets socket to non blocking, previously it used F_GETFL */
#endif
  running = true;
  return true;
}

Lirc::~Lirc()
{
  terminate(); /* thread */
  lirc_deinit();
}

void Lirc::suspend()
{
  running = false;
/*  lirc_deinit(); */
}

void Lirc::wake_up()
{
  pthread_mutex_lock(&lirc_mutex);
/*  init(); */
  struct timeval tv;
  gettimeofday(&tv,NULL);
  next_time = static_cast<unsigned long long>(tv.tv_sec)*1000+tv.tv_usec/1000 + 200;
  flush=true;
  running = true;
  pthread_mutex_unlock(&lirc_mutex);
}

void ir_parsebuf(IR * irs)
{
  sscanf(irs->buf,"%x%x%s%s",&(irs->code),&irs->serial,irs->key,irs->remote);
}

void Lirc::reinit(){  /* loop till it resumes connection to the lirc socket */
  while(true){
    lirc_deinit(); /* let's free the fd */
     mmsSleep(2);
     if (init())
       break;
  }
  print_critical(dgettext("mms-lirc",gettext("Reconnected to the lirc socket")) , "LIRC");
}


int Lirc::read_lirc(char * buffer, int maxread){

#ifdef LIRC_NO_BLOCK
  fd_set rfds;
  int retval;
  struct timeval tv;
#endif
  size_t len = 0;

  while(true){
    len = read(irsd, buffer, maxread);
    int readerr = errno;
    if (len == 0 || readerr == EBADF){
      print_critical(dgettext("mms-lirc",gettext("Lost connection to the lirc socket: ")) + s_strerror(readerr), "LIRC");
      reinit();
      return 0;
    }
    if (len == static_cast<size_t>(-1)) {
#ifdef LIRC_NO_BLOCK
      if(readerr == EAGAIN || readerr == EWOULDBLOCK){ /* we behave and use select to wait for new input */
        do {
           tv.tv_sec = 0;  /* 1/2 second as timeout */
           tv.tv_usec = 500*1000;
           FD_ZERO(&rfds);
           FD_SET(irsd, &rfds); /* why does this trigger a "old style cast" warning? */
           retval = select(1 + irsd, &rfds, NULL, NULL, &tv);
        } while (retval == 0);
	readerr = errno;
	if (retval < 0){
	  print_critical(dgettext("mms-lirc",gettext("Error: ")) + s_strerror(readerr), "LIRC");
	  return -1;
	}
	continue;
      }
#endif
      print_critical(dgettext("mms-lirc",gettext("Error: ")) + s_strerror(readerr), "LIRC");
      return -1;
    }
    break;
  }
  return len;
}


void Lirc::run()
{
  InputMaster *input_master = S_InputMaster::get_instance();

  struct timeval tv;
  IR irdata;

  char buffer[65]; /* 64 bytes for the lirc buffer plus one padding byte to ensure input
		      string is always null terminated */
  int dump_fast = 0;
  int oldcode = -1;
  size_t len;

  Input input;
  pthread_mutex_lock(&lirc_mutex);
  gettimeofday(&tv,NULL);
  next_time = this_time = static_cast<unsigned long long>(tv.tv_sec)*1000+tv.tv_usec/1000;
	  
  while (true) {
    pthread_mutex_unlock(&lirc_mutex);
nounlock:
    len = read_lirc(buffer, 64);
    if (len == static_cast<size_t>(-1)){
      mmsUsleep(500*1000);
      goto nounlock;
    }

    if (len == 0)
      goto nounlock;
    
    pthread_mutex_lock(&lirc_mutex);
    gettimeofday(&tv,NULL);
    this_time = static_cast<unsigned long long>(tv.tv_sec)*1000+tv.tv_usec/1000;

    if(!running){
      print_debug("Discarding lirc event. Input is suspended", "LIRC");
      continue;
    }

    buffer[len] = '\0'; /* make sure input string is always null terminated */
    for (size_t i = 0; i<len; ++i) {
      if (buffer[i] == '\n') {
	buffer[i] = '\0';
	break;
      }
    }

    irdata.buf = buffer;
    ir_parsebuf(&irdata);
    if (!flush && irdata.code != oldcode && oldcode != -1){  /* if oldcode != irdata.code it's not a repeated key, but a new one
					                        regardless of the time limit */
      oldcode = irdata.code;
      dump_fast = 1;  /* reset repeat counter */
    }
    else if (next_time > this_time){
      if (flush){
	oldcode = -1;
	dump_fast = 0;
	continue;
      }
      if (lirc_rate == 0) {  /* lirc_rate == 0 means that it accepts repeated events every "timeinterv" millisecs */
	continue;
      }
      next_time = timeinterv+this_time;  /* update timer */

      if (dump_fast > 0 && (dump_fast%lirc_rate!=0 || dump_fast <= lirc_fast)){ /* checks repeat rate and if
										   we got to the fast mode limit */
	dump_fast++;
	continue;
      }
      else /* we decide to process input */
	dump_fast++;
    }
    else{ /* this is not a repeated key (timeinterval has passed) */
      dump_fast = 1;
      flush = false;
    }
    oldcode = irdata.code;
    Input temp;
    bool found_input = find_input(irdata.key, temp);

    if (irdata.serial > 0 && !temp.multiple)
      continue;

    if (accept_all) {

      Input i;

      if (found_input)
	if (temp.mode == "search" || temp.mode == "general")
	  i = temp;

      i.key = irdata.key;
      input = i;

    } else {
      if (found_input)
	input = temp;
      else
	continue;
    }
    input_master->add_input(input, "lirc");

    gettimeofday(&tv,NULL);
    next_time = static_cast<unsigned long long>(tv.tv_sec)*1000+tv.tv_usec/1000 + timeinterv;
    print_debug("got command " + input.command + " ( " + conv::itos(irdata.serial) + " ) ", "LIRC");
  }
}
