#ifndef PYTHONPLAYER
#define PYTHONPLAYER

#include "playlist.hpp"
#include <string>

namespace pymms {

namespace player {

class PythonPlayer
{
  public:
    PythonPlayer() {}
    virtual ~PythonPlayer() {}
    virtual void play(std::string strPath, std::string strTitle) = 0;
    virtual void stop() = 0;
    virtual void pause() = 0;
    virtual void mute() = 0;
    virtual void volup() = 0;
    virtual void voldown() = 0;
    virtual void ff() = 0;
    virtual void fb() = 0;
    virtual int isPlaying() = 0;
};

}

}

#endif
