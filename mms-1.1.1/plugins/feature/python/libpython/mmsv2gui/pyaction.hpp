#ifndef PYACTION_HPP
#define PYACTION_HPP

#include <Python.h>
#include "structmember.h"

#define Action_Check(op) PyObject_TypeCheck(op, &Action_Type)

#ifdef __cplusplus
extern "C" {
#endif

namespace pymms {

namespace python {

typedef struct {
  PyObject_HEAD
} Action;

extern PyTypeObject Action_Type;

}

}

#ifdef __cplusplus
}
#endif

#endif
