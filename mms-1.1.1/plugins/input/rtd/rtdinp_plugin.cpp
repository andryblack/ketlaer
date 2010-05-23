#include "plugin.hpp"
#include "rtdinp.hpp"

class RtdInpPlugin : public InputPlugin
{
public:
  RtdInpPlugin() { device = new RtdInp; }
  std::string plugin_name() const { return "rtdinp"; }
};

MMS_PLUGIN(RtdInpPlugin)
