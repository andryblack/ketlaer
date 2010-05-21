#include "plugin.hpp"
#include "lcd.hpp"

class LCDPlugin : public PrinterPlugin
{
public:
  LCDPlugin() { printer = S_Lcd::get_instance(); }
  std::string plugin_name() const { return "lcd"; }
};

MMS_PLUGIN(LCDPlugin)
