#include "tv.hpp"
#include "tv_config.hpp"

#include "plugin.hpp"
#include "themes.hpp"
#include "config.hpp"
#include "common-feature.hpp"
#include "startmenu.hpp"

#include "gettext.hpp"

void tv()
{
  get_class<Tv>(dgettext("mms-tv", "TV"))->mainloop();
}

class TVPlugin : public FeaturePlugin
{
public:
  TVPlugin() {
    Themes *themes = S_Themes::get_instance();

    TVConfig *tv_conf = S_TVConfig::get_instance();
    Config *conf = S_Config::get_instance();

    tv_conf->parse_configuration_file(conf->p_homedir());

    module = new Tv();

    features.push_back(startmenu_item(dgettext("mms-tv", "Watch TV"), "tv", themes->startmenu_tv, 0, &tv));
  }

  std::string plugin_name() const { return dgettext("mms-tv", "TV"); }
  int priority() const { return 3; }
  unsigned long capability() const { return 0; }
};

MMS_PLUGIN(TVPlugin)
