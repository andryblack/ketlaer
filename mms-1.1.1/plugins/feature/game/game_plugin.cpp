#include "config.h"

#include "plugin.hpp"

#include "game.hpp"

#include "common-feature.hpp"
#include "themes.hpp"
#include "print.hpp"
#include "cd.hpp"
#include "config.hpp"

void no_games_error()
{
  Print pdialog(Print::SCREEN);
  pdialog.add_line(dgettext("mms-game", "Could not find any games"));
  pdialog.add_line("");
  pdialog.add_line(dgettext("mms-game", "Please specify a correct path "));
  pdialog.add_line(dgettext("mms-game", "in the configuration file"));
  pdialog.print();
}

void game_fromhd()
{
  Game *game = get_class<Game>(dgettext("mms-game", "Game"));

  if (!game->loaded_correctly) {
    game->read_dirs();
    game->loaded_correctly = true;
  }

  if (game->reload_needed) {
    DialogWaitPrint pdialog(dgettext("mms-game", "Reloading directories"), 1000);
    game->read_dirs();
  }

  if (game->files_size() == 0) {
    no_games_error();
    game->reload_needed = true;
  } else {
    game->reload_needed = false;
    game->mainloop();
  }
}

void game_cd()
{
  Cd *cd = S_Cd::get_instance();

  if (cd->open()) {

    Cd::cdstatus cdstatus = cd->check_cddrive();

    if (cdstatus == Cd::OK)
      get_class<Game>(dgettext("mms-game", "Game"))->determine_media();
  }
}

class GamePlugin : public FeaturePlugin
{
public:
  GamePlugin() {
    Config *conf = S_Config::get_instance();
    GameConfig *game_conf = S_GameConfig::get_instance();

    game_conf->parse_configuration_file(conf->p_homedir());

    module = new Game();

    Themes *themes = S_Themes::get_instance();

    std::string text;

    text = dgettext("mms-game", "Play games from harddrive");

    // add game button
    if (!conf->p_media())
      text = dgettext("mms-game", "Play games");

    features.push_back(startmenu_item(text, "game_hd", themes->startmenu_game_dir, 0, &game_fromhd));
    if (conf->p_media()) {

      text = dgettext("mms-game", "Play PSX or roms from cd/dvd");

      features.push_back(startmenu_item(text, "game_disc", themes->startmenu_game_cd, 1, &game_cd));
    }
  }

  std::string plugin_name() const { return dgettext("mms-game", "Game"); }
  int priority() const { return 2; }
  unsigned long capability() const { return 0; }
};

MMS_PLUGIN(GamePlugin)
