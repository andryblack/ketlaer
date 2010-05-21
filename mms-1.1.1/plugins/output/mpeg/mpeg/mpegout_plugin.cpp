#include "plugin.hpp"
#include "mpeg_out.hpp"

class MpegOutPlugin : public OutputPlugin
{
public:
  MpegOutPlugin() { device = new MpegOut; }
  std::string plugin_name() const { return "mpeg"; }
};

MMS_PLUGIN(MpegOutPlugin)
