#include "plugin.hpp"
#include "epg.hpp"
#include "themes.hpp"
#include "print.hpp"
#include "cd.hpp"
#include "config.hpp"
#include "common-feature.hpp"

void view_epg() {
    Epg *epg = get_class<Epg>(dgettext("mms-epg", "Electronic Program Guide"));

    if (!epg->use_svdrp) {
        if (!epg->loaded_correctly) {
            epg->get_data();
            epg->loaded_correctly = true;
        }

        if (epg->reload_needed) {
            DialogWaitPrint pdialog(dgettext("mms-epg", "Rereading TV Data"), 1000);
            epg->get_data();
        }

        if (!epg->check_tv_data()) {
            epg->reload_needed = true;
        } else {
            epg->reload_needed = false;
            epg->mainloop();
        }
    } else {
        if (!epg->check_tv_data()) {
            epg->reload_needed = true;
            epg->update_svdrp_data();
        }
        if (epg->loaded_correctly) {
            epg->mainloop();
        }
    }
}

void show_timers() {
    fprintf(stderr, "Show Timers\n");

    Epg *epg = get_class<Epg>(dgettext("mms-epg", "Electronic Program Guide"));

    if (epg->use_svdrp) {
        if (!epg->check_tv_data()) {
            epg->reload_needed = true;
            epg->update_svdrp_data();
        }
        if (epg->loaded_correctly) {
            epg->show_timers();
        }
    }
}

class EpgPlugin : public FeaturePlugin {
public:
    bool plugin_post_init() {
        S_InputMaster::get_instance()->parse_keys("epg", "4");

        Epg *epg = (Epg *)module;
        EPGConfig *epg_conf = S_EPGConfig::get_instance();

        if (epg) {
            if (epg->use_svdrp && !epg->update_running) {
                epg->update_running = true;
                epg->epg_updater->start();
            }
        } else if (!epg_conf->p_epg_update_script().empty() && file_exists(epg_conf->p_epg_update_script()) && !epg->update_running) {
            epg->update_running = true;
            epg->epg_updater->start();
        }
        return true;
    }

    EpgPlugin() {
        Config *conf = S_Config::get_instance();
        EPGConfig *epg_conf = S_EPGConfig::get_instance();

        epg_conf->parse_configuration_file(conf->p_homedir());

        module = new Epg();

        Themes *themes = S_Themes::get_instance();
        features.push_back(startmenu_item(dgettext("mms-epg", "View Electronic Program Guide"), "epg", themes->startmenu_epg, 0,
                &view_epg));

        if (((Epg *)module)->use_svdrp) {
            features.push_back(startmenu_item(dgettext("mms-epg", "Show Timer"), "epg-timers", themes->startmenu_timer, 1,
                    &show_timers));
        }
    }
    std::string plugin_name() const {
        return dgettext("mms-epg", "Electronic Program Guide");
    }
    int priority() const {
        return 4;
    }
    unsigned long capability() const {
        return 0;
    }
};

MMS_PLUGIN(EpgPlugin)
