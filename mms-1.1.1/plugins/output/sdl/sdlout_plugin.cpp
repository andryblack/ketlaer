#include "plugin.hpp"
#include "sdlout.hpp"

class SDLPlugin : public OutputPlugin
{
public:
  SDLPlugin() { device = new SDLdev; }
  std::string plugin_name() const { return "sdl"; }
};

MMS_PLUGIN(SDLPlugin)
