#ifndef PYAUDIOPLAYER_HPP
#define PYAUDIOPLAYER_HPP

#include <Python.h>
#include "structmember.h"
#include "audioplayer.hpp"

#ifdef __cplusplus
extern "C" {
#endif

namespace pymms {

namespace python {

typedef struct {
  PyObject_HEAD
  pymms::player::PythonAudioPlayer *audioplayer;
} PyAPlayer;

extern PyTypeObject PyAPlayer_Type;

}

}

#ifdef __cplusplus
}
#endif

#endif
