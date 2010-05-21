#include <Python.h>
#include "structmember.h"
#include "pyplayer.hpp"
#include "pyplaylist.hpp"

#ifdef __cplusplus
extern "C" {
#endif

namespace pymms {

namespace python {

static PyMethodDef module_methods[] = {
    0, 0, 0, 0 /* Sentinel */
};

#ifndef PyMODINIT_FUNC
#define PyMODINIT_FUNC void
#endif
PyMODINIT_FUNC
initmmsv2(void)
{
  PyObject* mmsv2module;

  if (PyType_Ready(&PlayList_Type) < 0)
    return;

  if (PyType_Ready(&Player_Type) < 0)
    return;

  mmsv2module = Py_InitModule3("mmsv2", module_methods, "mmsv2 objects");

  if (mmsv2module == NULL)
    return;

  Py_INCREF(&Player_Type);
  Py_INCREF(&PlayList_Type);

  PyModule_AddObject(mmsv2module, "Player", (PyObject *)&Player_Type);
  PyModule_AddObject(mmsv2module, "PlayList", (PyObject *)&PlayList_Type);
}

}

}

#ifdef __cplusplus
}
#endif


