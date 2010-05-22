#include "plugin.hpp"
#include "rtd.hpp"

class RtdPlugin : public InputPlugin
{
public:
  RtdPlugin() { device = new Rtd; }
  std::string plugin_name() const { return "rtd"; }
};

MMS_PLUGIN(RtdPlugin)
