#include "plugin.hpp"
#include "pictures.hpp"
#include "themes.hpp"
#include "print.hpp"
#include "cd.hpp"
#include "common-feature.hpp"

void pic_hd()
{
  Pictures *pictures = get_class<Pictures>(dgettext("mms-pictures", "Pictures"));

  if (!pictures->loaded_correctly) {
    pictures->read_dirs();
    pictures->loaded_correctly = true;
  }

  if (pictures->reload_needed) {
    DialogWaitPrint pdialog(dgettext("mms-pictures", "Reloading directories"), 1000);

    pictures->read_dirs();
  }

  if (pictures->pic_list.size() == 0) {

    Print pdialog(Print::SCREEN);
    pdialog.add_line(dgettext("mms-pictures", "Could not find any pictures"));
    pdialog.add_line("");
    pdialog.add_line(dgettext("mms-pictures", "Please specify a correct path "));
    pdialog.add_line(dgettext("mms-pictures", "in the configuration file"));
    pdialog.print();

    pictures->reload_needed = true;

  } else {
    pictures->reload_needed = false;
    pictures->mainloop();
  }
}

void pic_cd()
{
  Cd *cd = S_Cd::get_instance();

  if (cd->open()) {

    Pictures *pictures = get_class<Pictures>(dgettext("mms-pictures", "Pictures"));

    Cd::cdstatus cdstatus = cd->check_cddrive();

    if (cdstatus == Cd::OK)
      pictures->play_pictures_cd();

    cd->close();
  }
}

class PicturesPlugin : public FeaturePlugin
{
public:

  bool plugin_post_init(){
    S_InputMaster::get_instance()->parse_keys("pictures", "3");
    return true;
  }

  PicturesPlugin() {
    Themes *themes = S_Themes::get_instance();
    Config *conf = S_Config::get_instance();
    PictureConfig *pic_conf = S_PictureConfig::get_instance();

    pic_conf->parse_configuration_file(conf->p_homedir());

    module = new Pictures();

    features.push_back(startmenu_item(dgettext("mms-pictures", "View pictures from harddrive"),
				      "picture_hd", themes->startmenu_pictures_dir,
				      0, &pic_hd));

    if (conf->p_media())
      features.push_back(startmenu_item(dgettext("mms-pictures", "View pictures from cd"),
					"picture_disc", themes->startmenu_pictures_cd,
					1, &pic_cd));
  }
  std::string plugin_name() const { return dgettext("mms-pictures", "Pictures"); }
  int priority() const { return 3; }
  unsigned long capability() const { return 0; }
};

MMS_PLUGIN(PicturesPlugin)
