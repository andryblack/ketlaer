#ifndef PYLISTITEM_HPP
#define PYLISTITEM_HPP

#include <Python.h>
#include "structmember.h"
#include "guilistitem.hpp"

using namespace pymms::gui;

#define ListItem_Check(op) PyObject_TypeCheck(op, &ListItem_Type)

#ifdef __cplusplus
extern "C" {
#endif

namespace pymms {

namespace python {

typedef struct {
  PyObject_HEAD
  GUIListItem *pListItem;
} ListItem;

extern PyTypeObject ListItem_Type;

}

}

#ifdef __cplusplus
}
#endif

#endif
