#define _XOPEN_SOURCE /* glibc2 needs this */

#include "config.h"

#include "tv.hpp"

#include "startmenu.hpp"
#include "updater.hpp"
#include "common-feature.hpp"
#include "SSaverobj.h"
#include "tv_config.hpp"

#ifdef use_nls
#include <langinfo.h>
#include <locale.h>
#endif

#include <libfe.h>

static void run_vdr_ketlaerfe(const char *opts)
{
  S_SSaverobj::get_instance()->StopCounter();
  run::aquire_exclusive_access();
  S_Render::get_instance()->device->unlock();
  run_vdr_frontend(opts);
  S_Render::get_instance()->device->lock();
  S_Render::get_instance()->complete_redraw();
  run::release_exclusive_access();
  S_SSaverobj::get_instance()->ResetCounter();
}

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

  if (tv_conf->p_tv_path() == "vdr-ketlaerfe")
    run_vdr_ketlaerfe(tv_conf->p_tv_opts().c_str());
  else
    run::exclusive_external_program(tv_conf->p_tv_path() + ' ' + tv_conf->p_tv_opts());

  // make sure the screensaver doesn't kick in right after tv finishes
  conf->s_last_key(time(0));
  
  return "";
}

