#include "weather.hpp"
#include "weather_config.hpp"

#include "plugin.hpp"
#include "themes.hpp"
#include "config.hpp"
#include "common-feature.hpp"
#include "startmenu.hpp"

#include "gettext.hpp"

void weather()
{
  get_class<Weather>(dgettext("mms-weather", "Weather forecast"))->mainloop();
}

class WeatherPlugin : public FeaturePlugin
{
public:
  WeatherPlugin() {
    Themes *themes = S_Themes::get_instance();

    WeatherConfig *weather_conf = S_WeatherConfig::get_instance();
    Config *conf = S_Config::get_instance();

    weather_conf->parse_configuration_file(conf->p_homedir());

    module = new Weather();

    features.push_back(startmenu_item(dgettext("mms-weather", "See weather forecast"), "weather", themes->startmenu_weather, 0, &weather));
  }

  std::string plugin_name() const { return dgettext("mms-weather", "Weather forecast"); }
  int priority() const { return 3; }
  unsigned long capability() const { return 0; }
};

MMS_PLUGIN(WeatherPlugin)

