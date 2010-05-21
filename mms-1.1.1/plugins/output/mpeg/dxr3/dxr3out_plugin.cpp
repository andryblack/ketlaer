#include "plugin.hpp"
#include "dxr3out.hpp"

class Dxr3Plugin : public OutputPlugin
{
public:
  Dxr3Plugin() { device = new Dxr3; }
  std::string plugin_name() const { return "dxr3"; }
};

MMS_PLUGIN(Dxr3Plugin)
