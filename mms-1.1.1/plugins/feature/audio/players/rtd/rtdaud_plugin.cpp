#include "plugin.hpp"
#include "rtdaud.hpp"

class RtdAudioPlayerPlugin : public AudioPlayerPlugin
{
public:

  RtdAudioPlayerPlugin() 
  {
    player = new RtdAud();
  }

  std::string plugin_name() const { return "rtdaud"; }
};

MMS_PLUGIN(RtdAudioPlayerPlugin)
