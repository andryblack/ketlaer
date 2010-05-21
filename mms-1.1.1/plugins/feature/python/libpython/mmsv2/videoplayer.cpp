#include "videoplayer.hpp"
#include "config.h"
#include "plugins.hpp"
#include "movie.hpp"
#include <vector>
#include <iostream>

using std::vector;
using std::string;

pthread_mutex_t pymms::player::PythonVideoPlayer::singleton_mutex = PTHREAD_MUTEX_INITIALIZER;

pymms::player::PythonVideoPlayer::PythonVideoPlayer()
{
  #ifdef c_movie
  m_pPlayer = 0;
  MoviePlayerPlugin *pMoviePlayerPlugin = 0;
  Plugins *pPlugins = S_Plugins::get_instance();
  Movie* pMovie = get_class<Movie>(dgettext("mms-movie", "Video"));

  if(pPlugins && pMovie)
    pMoviePlayerPlugin = pPlugins->find(pPlugins->movie_players, pMovie->get_opts()->movie_player());
  else
    std::cerr << "Video support detected, but something seems wrong with the movie plugin" << std::endl;

  if(pMoviePlayerPlugin)
    m_pPlayer = pMoviePlayerPlugin->player;
  else
    std::cerr << "Video support detected, but no video player!" << std::endl;
  #else
  std::cerr << "No video support detected" std::endl;
  #endif
}

void pymms::player::PythonVideoPlayer::play(string strPath, string strTitle)
{
  play(strPath, false);
}
    
void pymms::player::PythonVideoPlayer::play(string strPath, bool bWindow)
{
  #ifdef c_movie
  if(m_pPlayer)
  {
    DialogWaitPrint pdialog(dgettext("mms-movie", "Starting playback..."), 1000);

    S_BusyIndicator::get_instance()->disable();

    while (!pdialog.cleaned()) {
      // wait for dialog to clean up
      usleep(100*1000);
    }

    m_pPlayer->play_movie("\"" + strPath + "\"", bWindow);

    if (!m_pPlayer->wants_exclusivity())
      S_Render::get_instance()->complete_redraw();

    S_BusyIndicator::get_instance()->enable();
  }
  #endif
}

void pymms::player::PythonVideoPlayer::pause()
{
  #ifdef c_movie
  if(m_pPlayer)
  {
    m_pPlayer->pause();
  }
  #endif
}

void pymms::player::PythonVideoPlayer::stop()
{
  #ifdef c_movie
  if(m_pPlayer)
  {
    m_pPlayer->stop();
  }
  #endif
}

void pymms::player::PythonVideoPlayer::ff()
{
  #ifdef c_movie
  if(m_pPlayer)
  {
    m_pPlayer->ff();
  }
  #endif
}

void pymms::player::PythonVideoPlayer::fb()
{
  #ifdef c_movie
  if(m_pPlayer)
  {
    m_pPlayer->fb();
  }
  #endif
}

int pymms::player::PythonVideoPlayer::isPlaying()
{
  #ifdef c_movie
  if(m_pPlayer)
  {
    if(m_pPlayer->playing())
      return 1;
    else
      return 0;
  }
  #endif
}
