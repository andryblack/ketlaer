#ifndef PYPLAYLIST_HPP
#define PYPLAYLIST_HPP

#include <Python.h>
#include "structmember.h"
#include "playlist.hpp"

using namespace pymms::player;

#define PlayList_Check(op) PyObject_TypeCheck(op, &PlayList_Type)

#ifdef __cplusplus
extern "C" {
#endif

namespace pymms {

namespace python {

typedef struct {
  PyObject_HEAD
  PythonPlayList *pPlayList;
} PlayList;

extern PyTypeObject PlayList_Type;

}

}

#ifdef __cplusplus
}
#endif

#endif
