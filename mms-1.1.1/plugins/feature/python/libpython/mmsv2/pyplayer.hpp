#ifndef PYPLAYER_HPP
#define PYPLAYER_HPP

#include <Python.h>
#include "structmember.h"
#include "audioplayer.hpp"
#include "videoplayer.hpp"
#include "player.hpp"
#include "pyplaylist.hpp"
#include "playlistthread.hpp"

#ifdef __cplusplus
extern "C" {
#endif

namespace pymms {

namespace python {

typedef struct {
  PyObject_HEAD
  PlayList* pyPlayList;
  pymms::player::PythonPlayer* pPlayer;
  PlayListThread* pThread;
  bool bRepeat;
  bool bPlayer;
  PyObject* pCallback;
} Player;

extern PyTypeObject Player_Type;

}

}

#ifdef __cplusplus
}
#endif

#endif
