#include "plugin.hpp"
#include "rtdout.hpp"

class RTDPlugin : public OutputPlugin
{
public:
  RTDPlugin() { device = new RTDdev; }
  std::string plugin_name() const { return "rtdout"; }
};

MMS_PLUGIN(RTDPlugin)
