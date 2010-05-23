#ifndef LIRC_CONFIG_HPP
#define LIRC_CONFIG_HPP

#include "config.h"

#ifdef CONFIG_HEADER_PLUGIN_H
#undef CONFIG_HEADER_PLUGIN_H
#endif

/* Name of the definition file */

#ifdef PARAMETER_DEFINES
#undef PARAMETER_DEFINES
#endif
 
#define PARAMETER_DEFINES "lirc_config_parameters"

/* Option to read "[name]" lines */
#undef USE_SECTION
 
/* Option to save */
#undef USE_WRITER

#include "config_header_plugin.hpp"

#endif