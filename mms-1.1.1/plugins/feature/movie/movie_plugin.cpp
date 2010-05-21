#include "config.h"

#include "plugin.hpp"

#include "simple_movie.hpp"

#ifdef use_graphical_movie
#include "graphical_movie.hpp"
#endif

#include "themes.hpp"
#include "print.hpp"
#include "cd.hpp"
#include "config.hpp"

void no_movies_error()
{
  Print pdialog(Print::SCREEN);
  pdialog.add_line(dgettext("mms-movie", "Could not find any videos"));
  pdialog.add_line("");
  pdialog.add_line(dgettext("mms-movie", "Please specify a correct path "));
  pdialog.add_line(dgettext("mms-movie", "in the configuration file"));
  pdialog.print();
}

void movie_fromhd()
{
  Movie *mov = get_class<Movie>(dgettext("mms-movie", "Video"));

  if (!mov->loaded_correctly) {
    mov->read_dirs();
    mov->loaded_correctly = true;
  }

  if (mov->reload_needed) {
    DialogWaitPrint pdialog(dgettext("mms-movie", "Reloading directories"), 1000);
    mov->read_dirs();
  }

  if (mov->files_size() == 0) {
    no_movies_error();
    mov->reload_needed = true;
  } else {
    mov->reload_needed = false;
    mov->mainloop();
  }
}

void movie_cd()
{
  Cd *cd = S_Cd::get_instance();

  if (cd->open()) {

    Cd::cdstatus cdstatus = cd->check_cddrive();

    if (cdstatus == Cd::OK)
      get_class<Movie>(dgettext("mms-movie", "Video"))->determine_media();
  }
}

class MoviePlugin : public FeaturePlugin
{
public:
  bool plugin_post_init(){
    S_InputMaster::get_instance()->parse_keys("movie", "5");
    return true;
  }

  MoviePlugin() {
    Config *conf = S_Config::get_instance();
    MovieConfig *movie_conf = S_MovieConfig::get_instance();

    movie_conf->parse_configuration_file(conf->p_homedir());

#ifdef use_graphical_movie
    if (movie_conf->p_graphical_movie_mode())
      module = new GraphicalMovie();
    else
#endif
      module = new SimpleMovie();

    (static_cast<Movie *>(module))->set_folders();

#ifdef use_graphical_movie
    if (movie_conf->p_graphical_movie_mode())
      (static_cast<GraphicalMovie *>(module))->check_db_consistency();
#endif

    Themes *themes = S_Themes::get_instance();

    std::string text;

    text = dgettext("mms-movie", "Play videos from harddrive");

    // add movie button
    if (!conf->p_media())
      text = dgettext("mms-movie", "Play videos");

    features.push_back(startmenu_item(text, "movie_hd", themes->startmenu_movie_dir, 0, &movie_fromhd));

    if (conf->p_media()) {
      text = dgettext("mms-movie", "Play a dvd/vcd/svcd");
      features.push_back(startmenu_item(text, "movie_disc", themes->startmenu_movie_cd, 1, &movie_cd));
    }
  }

  ~MoviePlugin()
  {
    (static_cast<Movie *>(module))->save_runtime_settings();
    delete module;
    module = 0;
  }

  std::string plugin_name() const { return dgettext("mms-movie", "Video"); }
  int priority() const { return 2; }
  unsigned long capability() const { return 0; }
};

MMS_PLUGIN(MoviePlugin)
