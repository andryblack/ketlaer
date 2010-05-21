#ifndef GETTEXT_HPP
#define GETTEXT_HPP

#include "config.h"

// gettext
#ifdef use_nls
#include <libintl.h>
#include <langinfo.h>
#else
#define gettext(String) (String)
#endif

#endif
