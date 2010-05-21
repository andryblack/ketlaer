#include "plugin.hpp"
#include "lirc.hpp"

class LircPlugin : public InputPlugin
{
public:
  LircPlugin() { device = new Lirc; }
  std::string plugin_name() const { return "lirc"; }
};

MMS_PLUGIN(LircPlugin)
