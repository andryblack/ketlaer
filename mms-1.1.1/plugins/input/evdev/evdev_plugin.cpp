#include "plugin.hpp"
#include "evdev.hpp"

class EvdevPlugin : public InputPlugin
{
public:
  EvdevPlugin() { device = new Evdev; }
  std::string plugin_name() const { return "evdev"; }
};

MMS_PLUGIN(EvdevPlugin)
