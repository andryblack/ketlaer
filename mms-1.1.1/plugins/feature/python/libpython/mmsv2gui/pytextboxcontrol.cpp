#include <Python.h>
#include "structmember.h"
#include "pycontrol.hpp"
#include "pyutil.hpp"

using std::string;
using namespace pymms::gui;

#ifdef __cplusplus
extern "C" {
#endif

namespace pymms {

namespace python {

static PyObject* TextBoxControl_New(PyTypeObject* type, PyObject* args, PyObject* kwds)
{
  TextBoxControl* self;
  
  self = (TextBoxControl*)Control_Type.tp_new(type, args, kwds);
  if (!self) 
    return NULL;
    
  return (PyObject*)self;
}

static int TextBoxControl_init(TextBoxControl* self, PyObject* args, PyObject* kwds)
{
  if(!self->pyControl.isInitialized)
  {
    int iPosX = 0, iPosY = 0, iWidth = 0, iHeight = 0, iLayer = 4, iTextHeight = 30, iTextSpace = 0;
    char* strFont = "Vera";
    char* strRgb = "0xffffff";

    static char *kwlist[] = {"x", "y", "width", "height",
                           "font", "rgb", "textheight", "textspace", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "iiii|ssii", kwlist, &iPosX, &iPosY,
                                   &iWidth, &iHeight, &strFont, &strRgb, &iTextHeight, &iTextSpace))
      return -1;

    self->pyControl.pControl = new GUITextBoxControl(iPosX, iPosY, iWidth, iHeight, iLayer,
                                                  strFont, strRgb, iTextHeight, iTextSpace);

    self->pyControl.isInitialized = true;
  }

  return 0;
}

static PyObject* TextBoxControl_setText(TextBoxControl* self, PyObject* args, PyObject* kwds)
{
  if(!self->pyControl.isInitialized)
  {
    PyErr_SetString(PyExc_AssertionError, "Base class __init__() not called");
    return NULL;
  }

  PyObject* pObject = NULL;
  char* strFont = NULL;
  char* strRgb = NULL;

  static char *kwlist[] = {"text", "font", "rgb", NULL};
    
  if (!PyArg_ParseTupleAndKeywords(args, kwds, "|Oss", kwlist, &pObject, &strFont, &strRgb))	
    return NULL;

  string strUtf8;
  if(pObject && Py_wCharToChar(strUtf8, pObject))
    ((GUITextBoxControl*)self->pyControl.pControl)->setText(strUtf8);
  if(strFont)
    ((GUITextBoxControl*)self->pyControl.pControl)->setFont(strFont);
  if(strRgb)
    ((GUITextBoxControl*)self->pyControl.pControl)->setRgb(strRgb);
  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* TextBoxControl_clear(TextBoxControl* self, PyObject* args)
{
  if(!self->pyControl.isInitialized)
  {
    PyErr_SetString(PyExc_AssertionError, "Base class __init__() not called");
    return NULL;
  }

  ((GUITextBoxControl*)self->pyControl.pControl)->clear();

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* TextBoxControl_setTextHeight(TextBoxControl* self, PyObject* args)
{
  if(!self->pyControl.isInitialized)
  {
    PyErr_SetString(PyExc_AssertionError, "Base class __init__() not called");
    return NULL;
  }

  int iTextHeight = 0;

  if (!PyArg_ParseTuple(args, "i", &iTextHeight))
    return NULL;

  ((GUITextBoxControl*)self->pyControl.pControl)->setTextHeight(iTextHeight);

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* TextBoxControl_setTextSpace(TextBoxControl* self, PyObject* args)
{
  if(!self->pyControl.isInitialized)
  {
    PyErr_SetString(PyExc_AssertionError, "Base class __init__() not called");
    return NULL;
  }

  int iTextSpace = 0;

  if (!PyArg_ParseTuple(args, "i", &iTextSpace))
    return NULL;

  ((GUITextBoxControl*)self->pyControl.pControl)->setTextSpace(iTextSpace);

  Py_INCREF(Py_None);
  return Py_None;
}

static PyMethodDef TextBoxControl_methods[] = {
  {"setText", (PyCFunction)TextBoxControl_setText, METH_VARARGS | METH_KEYWORDS, 0},
  {"clear", (PyCFunction)TextBoxControl_clear, METH_VARARGS, 0},
  {"setTextHeight", (PyCFunction)TextBoxControl_setTextHeight, METH_VARARGS, 0},
  {"setTextSpace", (PyCFunction)TextBoxControl_setTextSpace, METH_VARARGS, 0},
  {NULL, NULL, 0, NULL}
};

PyTypeObject TextBoxControl_Type = {
	PyObject_HEAD_INIT(NULL)
	0,                         /*ob_size*/
	"mmsv2gui.TextBoxControl",    /*tp_name*/
	sizeof(TextBoxControl),      /*tp_basicsize*/
	0,                         /*tp_itemsize*/
	0,/*tp_dealloc*/
	0,                         /*tp_print*/
	0,                         /*tp_getattr*/
	0,                         /*tp_setattr*/
	0,                         /*tp_compare*/
	0,                         /*tp_repr*/
	0,                         /*tp_as_number*/
	0,                         /*tp_as_sequence*/
	0,                         /*tp_as_mapping*/
	0,                         /*tp_hash */
	0,                         /*tp_call*/
	0,                         /*tp_str*/
	0,                         /*tp_getattro*/
	0,                         /*tp_setattro*/
	0,                         /*tp_as_buffer*/
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
	0,       /* tp_doc */
	0,		                     /* tp_traverse */
	0,		                     /* tp_clear */
	0,		                     /* tp_richcompare */
	0,		                     /* tp_weaklistoffset */
	0,		                     /* tp_iter */
	0,		                     /* tp_iternext */
	TextBoxControl_methods,      /* tp_methods */
	0,                         /* tp_members */
	0,                         /* tp_getset */
	0,             /* tp_base */
	0,                         /* tp_dict */
	0,                         /* tp_descr_get */
	0,                         /* tp_descr_set */
	0,                         /* tp_dictoffset */
	(initproc)TextBoxControl_init,                         /* tp_init */
	0,                         /* tp_alloc */
	TextBoxControl_New,          /* tp_new */
	0,		             /* tp_free */
};

}

}

#ifdef __cplusplus
}
#endif
