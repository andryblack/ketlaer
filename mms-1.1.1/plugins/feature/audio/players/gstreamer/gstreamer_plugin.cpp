#include "plugin.hpp"
#include "gstreamer.hpp"

class GstAudioPlayerPlugin : public AudioPlayerPlugin
{
public:

  GstAudioPlayerPlugin() 
  {
    player = new Gst();
  }

  std::string plugin_name() const { return "Gstreamer"; }
};

MMS_PLUGIN(GstAudioPlayerPlugin)
