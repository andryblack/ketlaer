#include "config.h"

#include "collection.hpp"

#include "plugin.hpp"

#include "themes.hpp"
#include "config.hpp"

#ifdef use_graphical_movie
void movie_db()
{
  MovieConfig *movie_conf = S_MovieConfig::get_instance();

  if (movie_conf->p_movie_db()) {
    MovieCollection *mc = get_class<MovieCollection>(dgettext("mms-movie", "Video Collection"));
    if (!mc->loaded_correctly)
      mc->read_dirs();
    mc->mainloop();
  }
}
#endif

class MovieCollectionPlugin : public FeaturePlugin
{
public:
  MovieCollectionPlugin()
  {
#ifdef use_graphical_movie
    MovieConfig *movie_conf = S_MovieConfig::get_instance();

    if (movie_conf->p_movie_db()) {

      module = new MovieCollection();

      Themes *themes = S_Themes::get_instance();

      features.push_back(startmenu_item(dgettext("mms-movie", "View video collection"), "movie_db",
					themes->startmenu_movie_dir, 0,
					&movie_db));
    }
#endif
  }

  std::string plugin_name() const { return dgettext("mms-movie", "Video Collection"); }
  int priority() const { return 2; }
  unsigned long capability() const { return 0; }
};

MMS_PLUGIN(MovieCollectionPlugin)
