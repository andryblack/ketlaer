#ifndef PYTHONAUDIOPLAYER
#define PYTHONAUDIOPLAYER

#include "audio_player.hpp"
#include "playlist.hpp"
#include "player.hpp"
#include "singleton.hpp"
#include <cc++/thread.h>
#include <string>

namespace pymms {

namespace player {

class PythonAudioPlayer : public PythonPlayer
{
  protected:
    bool m_bSet;

  public:
    static pthread_mutex_t singleton_mutex;

    PythonAudioPlayer();
    virtual ~PythonAudioPlayer() {}
    void play(std::string strPath, std::string strTitle);
    void stop();
    void pause();
    void mute();
    void volup();
    void voldown();
    void ff();
    void fb();
    int isPlaying();
};

typedef Singleton<PythonAudioPlayer> S_PythonAudioPlayer;

}

}

#endif
