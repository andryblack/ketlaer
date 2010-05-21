#ifndef PYCONTROL_HPP
#define PYCONTROL_HPP

#include <Python.h>
#include "structmember.h"
#include "guicontrol.hpp"
#include "guilabelcontrol.hpp"
#include "guiimagecontrol.hpp"
#include "guibuttoncontrol.hpp"
#include "guilistcontrol.hpp"
#include "guitextboxcontrol.hpp"
#include "guirectanglecontrol.hpp"
#include "pylistitem.hpp"
#include "guitextfieldcontrol.hpp"
#include "guiprogresscontrol.hpp"
#include "guiimagelistcontrol.hpp"
#include <vector>

using namespace pymms::gui;

#define Control_Check(op) PyObject_TypeCheck(op, &Control_Type)
#define LabelControl_Check(op) PyObject_TypeCheck(op, &LabelControl_Type)
#define ImageControl_Check(op) PyObject_TypeCheck(op, &ImageControl_Type)
#define ButtonControl_Check(op) PyObject_TypeCheck(op, &ButtonControl_Type)
#define ListControl_Check(op) PyObject_TypeCheck(op, &ListControl_Type)
#define TextBoxControl_Check(op) PyObject_TypeCheck(op, &TextBoxControl_Type)
#define RectangleControl_Check(op) PyObject_TypeCheck(op, &RectangleControl_Type)
#define TextFieldControl_Check(op) PyObject_TypeCheck(op, &TextFieldControl_Type)
#define ProgressControl_Check(op) PyObject_TypeCheck(op, &ProgressControl_Type)
#define ImageListControl_Check(op) PyObject_TypeCheck(op, &ImageListControl_Type)

#ifdef __cplusplus
extern "C" {
#endif

namespace pymms {

namespace python {

typedef struct {
  PyObject_HEAD
  GUIControl* pControl;
  bool isInitialized;
} Control;

typedef struct {
  Control pyControl;
} ImageControl;

typedef struct {
  Control pyControl;
} LabelControl;

typedef struct {
  Control pyControl;
} ButtonControl;

typedef struct {
  Control pyControl;
  std::vector<ListItem*> vecItems;
} ListControl;

typedef struct {
  Control pyControl;
} TextBoxControl;

typedef struct {
  Control pyControl;
} RectangleControl;

typedef struct {
  Control pyControl;
} TextFieldControl;

typedef struct {
  Control pyControl;
} ProgressControl;

typedef struct {
  Control pyControl;
  std::vector<ListItem*> vecItems;
} ImageListControl;

extern PyTypeObject Control_Type;
extern PyTypeObject ImageControl_Type;
extern PyTypeObject LabelControl_Type;
extern PyTypeObject ButtonControl_Type;
extern PyTypeObject ListControl_Type;
extern PyTypeObject TextBoxControl_Type;
extern PyTypeObject RectangleControl_Type;
extern PyTypeObject TextFieldControl_Type;
extern PyTypeObject ProgressControl_Type;
extern PyTypeObject ImageListControl_Type;

}

}

#ifdef __cplusplus
}
#endif

#endif
