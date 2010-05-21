#include "config.h"
#include "plugin.hpp"
#include "clock.hpp"
#include "clock_config.hpp"
#include "alarm_config.hpp"
#include "common-feature.hpp"


class ClockPlugin : public FeaturePlugin
{
public:
  bool plugin_post_init(){
#ifdef c_music
    S_InputMaster::get_instance()->parse_keys("alarm", "1");
#endif
    return true;
  }

  ClockPlugin() {
#ifdef c_music
    Config *conf = S_Config::get_instance();

    ClockConfig *clock_conf = S_ClockConfig::get_instance();
    clock_conf->parse_configuration_file(conf->p_homedir());

    ClockAlarms *clock_alarms = S_ClockAlarms::get_instance();
    clock_alarms->parse_configuration_file(conf->p_homedir());
#endif

    module = new Clock();
  }

  std::string plugin_name() const { return dgettext("mms-clock", "Clock"); }
  int priority() const { return 3; }
  unsigned long capability() const { return 0; }
};

MMS_PLUGIN(ClockPlugin)
