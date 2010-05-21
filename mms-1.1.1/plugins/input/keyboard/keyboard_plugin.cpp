#include "plugin.hpp"
#include "keyboard.hpp"

class KbdPlugin : public InputPlugin
{
public:
  KbdPlugin() { device = new Keyboard; }
  std::string plugin_name() const { return "keyboard"; }
};

MMS_PLUGIN(KbdPlugin)
