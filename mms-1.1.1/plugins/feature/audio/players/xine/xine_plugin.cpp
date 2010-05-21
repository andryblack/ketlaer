#include "plugin.hpp"
#include "xine.hpp"

class XineAudioPlayerPlugin : public AudioPlayerPlugin
{
public:

  XineAudioPlayerPlugin() 
  {
    player = new Xine();
  }

  std::string plugin_name() const { return "Xine Audio"; }
};

MMS_PLUGIN(XineAudioPlayerPlugin)
