// This file is part of Configuration Files Definition Tool

// Generates the class definition without any include

#undef PARAM_CLASS
#include "config_macros.hpp"
#include PARAMETER_DEFINES

PARAM_CLASS(PARAMETER_CLASS)
{
PARAM_BASE( PARAMETER_CLASS )
private:
#undef PRIVATE_PAR
#include "config_macros.hpp"
#include PARAMETER_DEFINES
public:
#undef PUBLIC_PAR
#include "config_macros.hpp"
#include PARAMETER_DEFINES
};

typedef Singleton<PARAMETER_CLASS> S_PARAMETER_CLASS;

