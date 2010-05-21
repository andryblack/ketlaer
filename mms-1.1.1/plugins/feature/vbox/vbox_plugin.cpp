#include "vbox.hpp"
#include "vbox_config.hpp"

#include "plugin.hpp"
#include "themes.hpp"
#include "config.hpp"
#include "common-feature.hpp"
#include "startmenu.hpp"

#include "gettext.hpp"

void vbox()
{
  get_class<VBox>(dgettext("mms-vbox", "VBox - Client"))->mainloop();
}

class VBoxPlugin : public FeaturePlugin
{
public:
  VBoxPlugin() {
    Themes *themes = S_Themes::get_instance();

    VBoxConfig *vbox_conf = S_VBoxConfig::get_instance();
    Config *conf = S_Config::get_instance();

    vbox_conf->parse_configuration_file(conf->p_homedir());

    module = new VBox();

    features.push_back(startmenu_item(dgettext("mms-vbox", "Check VBox"), "vbox", themes->startmenu_vbox, 0, &vbox));
  }

  bool plugin_post_init() {
	  S_InputMaster::get_instance()->parse_keys("vbox", "1");
	  return true;
  }
	
  std::string plugin_name() const { return dgettext("mms-vbox", "VBox - Client"); }
  int priority() const { return 3; }
  unsigned long capability() const { return 0; }
};

MMS_PLUGIN(VBoxPlugin)

