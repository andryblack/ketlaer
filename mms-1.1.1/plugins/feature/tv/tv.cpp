#define _XOPEN_SOURCE /* glibc2 needs this */

#include "config.h"

#include "tv.hpp"

#include "startmenu.hpp"
#include "updater.hpp"
#include "common-feature.hpp"
#include "tv_config.hpp"

#ifdef use_nls
#include <langinfo.h>
#include <locale.h>
#endif

Tv::Tv()
{
#ifdef use_nls
  // gettext
  setlocale(LC_ALL, ""); /* set from the environment variables */
  bindtextdomain("mms-tv", mms_prefix"/share/locale");
  bind_textdomain_codeset("mms-tv", nl_langinfo(CODESET));
#endif
}

std::string Tv::mainloop()
{
  TVConfig *tv_conf = S_TVConfig::get_instance();

  run::exclusive_external_program(tv_conf->p_tv_path() + ' ' + tv_conf->p_tv_opts());

  // make sure the screensaver doesn't kick in right after tv finishes
  conf->s_last_key(time(0));
  
  return "";
}

