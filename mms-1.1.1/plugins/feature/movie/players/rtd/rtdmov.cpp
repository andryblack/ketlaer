#include "rtdmov.hpp"
#include "simple_movie.hpp"
#include "common-feature.hpp"
#include "movie_config.hpp"

#ifdef use_nls
#include <langinfo.h>
#include <locale.h>
#endif

#include <fstream>

using std::string;

RtdMov::RtdMov()
  : MoviePlayer(true, true, true, true)
{
#ifdef use_nls
  // gettext
  setlocale(LC_ALL, ""); /* set from the environment variables */
  bindtextdomain("mms-rtdmov", mms_prefix"/share/locale");
  bind_textdomain_codeset("mms-rtdmov", nl_langinfo(CODESET));
#endif
}

RtdMov::~RtdMov()
{
}

void RtdMov::play_movie(const string& paths, bool window)
{
}

void RtdMov::play_disc(const string& play_cmd)
{
}

void RtdMov::play_vcd(const string& device)
{
}

void RtdMov::play_dvd(const string& device)
{
}
