// test version - 2004/06/03
// This file is part of Configuration Files Definition Tool

// Macros to generates either the class definition or the class functions


CONFIG_INSTANCE(PARAMETER_CLASS)

#include <fstream>
#include <iostream>
#include <sstream>
#include <cstdlib>

#include "gettext.hpp"

#ifdef CONFIG_DESTRUCTOR
CONFIG_DESTRUCTOR(PARAMETER_CLASS)
#include "config_macros.hpp"
#include PARAMETER_DEFINES
CONFIG_DESTRUCTOR_DONE
#endif

#ifdef CONFIG_CONSTRUCTOR
CONFIG_CONSTRUCTOR(PARAMETER_CLASS)
#include "config_macros.hpp"
#include PARAMETER_DEFINES
CONFIG_CONSTRUCTOR_DONE
#endif

CONFIG_DEFAULT(PARAMETER_CLASS)
#undef DEFAULT_PAR
#include "config_macros.hpp"
#include PARAMETER_DEFINES
CONFIG_DEFAULT_DONE

CONFIG_PARSER_PLUGIN_TOP(PARAMETER_CLASS, STRING_PARAMETER_CLASS)
CONFIG_PARSER(PARAMETER_CLASS)
#undef CONFIG_READER
#include "config_macros.hpp"
#include PARAMETER_DEFINES
CONFIG_PARSER_DONE

#ifdef USE_WRITER
CONFIG_WRITER(PARAMETER_CLASS)
#undef CONFIG_WRITER
#include "config_macros.hpp"
#include PARAMETER_DEFINES
CONFIG_WRITER_DONE
#endif
