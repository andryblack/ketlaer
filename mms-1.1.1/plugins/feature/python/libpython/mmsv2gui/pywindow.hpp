#ifndef PYWONDOW_HPP
#define PYWINDOW_HPP

#include <Python.h>
#include "structmember.h"
#include "guipywindow.hpp"
#include "pycontrol.hpp"

#ifdef __cplusplus
extern "C" {
#endif

namespace pymms {

namespace python {

typedef struct {
  PyObject_HEAD
  GUIPYWindow* pWindow;
  std::vector<Control*> vecControls;
  bool bIsOpen;
  int iControlId;
  bool isInitialized;
} Window;

extern PyTypeObject Window_Type;

}

}

#ifdef __cplusplus
}
#endif

#endif
