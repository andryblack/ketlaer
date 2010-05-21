#include "plugin.hpp"
#include "dvbout.hpp"

class DvbPlugin : public OutputPlugin
{
public:
  DvbPlugin() { device = new Dvb; }
  std::string plugin_name() const { return "dvb"; }
};

MMS_PLUGIN(DvbPlugin)
