#include "plugin.hpp"
#include "replay_device.hpp"

class ReplayDevicePlugin : public InputPlugin
{
public:
  ReplayDevicePlugin() { device = new ReplayDevice; }
  std::string plugin_name() const { return "replay"; }
};

MMS_PLUGIN(ReplayDevicePlugin)
