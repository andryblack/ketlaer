#ifndef PYVIDEOPLAYER_HPP
#define PYVIDEOPLAYER_HPP

#include <Python.h>
#include "structmember.h"
#include "videoplayer.hpp"

#ifdef __cplusplus
extern "C" {
#endif

namespace pymms {

namespace python {

typedef struct {
  PyObject_HEAD
  pymms::player::PythonVideoPlayer *movieplayer;
} PyVPlayer;

extern PyTypeObject PyVPlayer_Type;

}

}

#ifdef __cplusplus
}
#endif

#endif
