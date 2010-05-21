#ifndef PYTHONVIDEOPLAYER
#define PYTHONVIDEOPLAYER

#include "movie_player.hpp"
#include "playlist.hpp"
#include "player.hpp"
#include "singleton.hpp"
#include <cc++/thread.h>
#include <string>

namespace pymms {

namespace player {

class PythonVideoPlayer : public PythonPlayer
{
  private:
    void play(std::string strPath, bool bWindow);

  protected:
    MoviePlayer *m_pPlayer;

  public:
    static pthread_mutex_t singleton_mutex;

    PythonVideoPlayer();
    virtual ~PythonVideoPlayer() {}
    void play(std::string strPath, std::string strTitle);
    void pause();
    void stop();
    void mute() {}
    void volup() {}
    void voldown() {}
    void ff();
    void fb();
    int isPlaying();
};

typedef Singleton<PythonVideoPlayer> S_PythonVideoPlayer;

}

}

#endif
