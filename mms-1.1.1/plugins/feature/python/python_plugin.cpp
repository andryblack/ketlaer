#include "python.hpp"
#include "plugin.hpp"
#include "common-feature.hpp"
#include "python_config.hpp"

using std::string;

void python_exec()
{
  get_class<Python>(dgettext("mms-python", "Extras"))->mainloop();
}

class PythonPlugin : public FeaturePlugin
{
public:
  PythonPlugin()
  {
    Config* pConf = S_Config::get_instance();
    PythonConfig* pPythonConf = S_PythonConfig::get_instance();

    pPythonConf->parse_configuration_file(pConf->p_homedir());

    module = new Python();
    features.push_back(startmenu_item(dgettext("mms-python", "Run pyglets"), "python", "startmenu/python.png", 0, &python_exec));
  }

  ~PythonPlugin()
  {
    (static_cast<Python *>(module))->save_runtime_settings();
    delete module;
    module = 0;
  }

  string plugin_name() const { return dgettext("mms-python", "Extras"); }
  int priority() const { return 3; }
  unsigned long capability() const { return 0; }

};

MMS_PLUGIN(PythonPlugin)
