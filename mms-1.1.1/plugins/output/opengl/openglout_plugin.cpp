#include "plugin.hpp"
#include "opengl.hpp"

class OGLPlugin : public OutputPlugin
{
public:
  OGLPlugin() { device = new OpenGLdev; }
  std::string plugin_name() const { return "opengl"; }
};

MMS_PLUGIN(OGLPlugin)
