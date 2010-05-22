#include "rtd.hpp"
#include "qtkeys.h"

#include "config.h"

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

#ifdef use_nls
#include <langinfo.h>
#include <locale.h>
#endif

Rtd::Rtd()
{
#ifdef use_nls
  // gettext
  setlocale(LC_ALL, ""); /* set from the environment variables */
  bindtextdomain("mms-rtd", mms_prefix"/share/locale");
  bind_textdomain_codeset("mms-rtd", nl_langinfo(CODESET));
#endif
}

bool Rtd::init()
{
  return true;
}

Rtd::~Rtd()
{
  terminate(); /* thread */
}

void Rtd::suspend()
{
}

void Rtd::wake_up()
{
}

void Rtd::run()
{
  InputMaster *input_master = S_InputMaster::get_instance();
  Input input;

  while (true) {
    sleep(1);
    //input_master->add_input(input, "rtd");
  }
}
