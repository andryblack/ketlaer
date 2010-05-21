#include "plugin.hpp"
#include "rtd.hpp"

class RtdAudioPlayerPlugin : public AudioPlayerPlugin
{
public:

  RtdAudioPlayerPlugin() 
  {
    player = new Rtd();
  }

  std::string plugin_name() const { return "rtd"; }
};

MMS_PLUGIN(RtdAudioPlayerPlugin)
