#if !defined DVB_CONFIG_H
#define DVB_CONFIG_H

#include "config.h"

/* Name of the definition file */

#ifdef PARAMETER_DEFINES
#undef PARAMETER_DEFINES
#endif
 
#define PARAMETER_DEFINES "dvb_config_parameters"

/* Option to read "[name]" lines */
#undef USE_SECTION
 
/* Option to save */
#undef USE_WRITER

#include "config_header_plugin.hpp"

#endif
