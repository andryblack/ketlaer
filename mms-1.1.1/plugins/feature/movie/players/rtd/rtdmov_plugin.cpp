#include "plugin.hpp"
#include "rtdmov.hpp"

class RtdMovPlugin : public MoviePlayerPlugin
{
public:

  RtdMovPlugin() 
  {
    player = new RtdMov();
  }

  std::string plugin_name() const { return "rtdmov"; }
};

MMS_PLUGIN(RtdMovPlugin)
