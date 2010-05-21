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

static PyObject* ButtonControl_New(PyTypeObject* type, PyObject* args, PyObject* kwds)
{
  ButtonControl* self;
  
  self = (ButtonControl*)Control_Type.tp_new(type, args, kwds);
  if (!self) 
    return NULL;

  return (PyObject*)self;
}

static int ButtonControl_init(ButtonControl* self, PyObject* args, PyObject* kwds)
{
  if(!self->pyControl.isInitialized)
  {
    int iPosX = 0, iPosY = 0, iWidth = 0, iHeight = 0, iLayer = 4, iTextOffset = 0;
    PyObject* pObject = NULL;
    char* strFocus = "";
    char* strNoFocus = "";
    char* strFont = "Vera";
    char* strRgb = "0xffffff";
    char* strRgbFocus = "0xffffff";
    char* strAlignment = "left";

    static char *kwlist[] = {"x", "y", "width", "height",
                           "text", "font", "rgb", "rgbfocus", "focus", "nofocus",
                           "textoffset", "alignment", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "iiii|Osssssis", kwlist, &iPosX, &iPosY,
                                   &iWidth, &iHeight, &pObject, &strFont, &strRgb, &strRgbFocus,
                                   &strFocus, &strNoFocus, &iTextOffset, &strAlignment))
      return -1;

    string strUtf8;
    if(pObject)
      Py_wCharToChar(strUtf8, pObject);

    self->pyControl.pControl = new GUIButtonControl(iPosX, iPosY, iWidth, iHeight, strUtf8, iLayer,
                                          strFocus, strNoFocus, strFont, strRgb, strRgbFocus,
                                          iTextOffset, strAlignment);

    self->pyControl.isInitialized = true;
  }

  return 0;
}

static PyObject* ButtonControl_setLabel(ButtonControl* self, PyObject* args, PyObject* kwds)
{
  if(!self->pyControl.isInitialized)
  {
    PyErr_SetString(PyExc_AssertionError, "Base class __init__() not called");
    return NULL;
  }

  PyObject* pObject = NULL;
  char* strFont = NULL;
  char* strRgb = NULL;
  char* strRgbFocus = NULL;

  static char *kwlist[] = {"text", "font", "rgb", "rgbfocus", NULL};
    
  if (!PyArg_ParseTupleAndKeywords(args, kwds, "|Osss", kwlist, &pObject, 
				   &strFont, &strRgb, &strRgbFocus))	
    return NULL;

  string strUtf8;
  if(pObject && Py_wCharToChar(strUtf8, pObject))
    ((GUIButtonControl*)self->pyControl.pControl)->setLabel(strUtf8);
  if(strFont)
    ((GUIButtonControl*)self->pyControl.pControl)->setFont(strFont);
  if(strRgb)
    ((GUIButtonControl*)self->pyControl.pControl)->setRgb(strRgb);
  if(strRgbFocus)
    ((GUIButtonControl*)self->pyControl.pControl)->setRgbFocus(strRgbFocus);

  Py_INCREF(Py_None);
  return Py_None;
}

static PyMethodDef ButtonControl_methods[] = {
  {"setLabel", (PyCFunction)ButtonControl_setLabel, METH_VARARGS | METH_KEYWORDS, 0},
  {NULL, NULL, 0, NULL}
};

PyTypeObject ButtonControl_Type = {
	PyObject_HEAD_INIT(NULL)
	0,                         /*ob_size*/
	"mmsv2gui.ButtonControl",    /*tp_name*/
	sizeof(ButtonControl),      /*tp_basicsize*/
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
	ButtonControl_methods,      /* tp_methods */
	0,                         /* tp_members */
	0,                         /* tp_getset */
	0,             /* tp_base */
	0,                         /* tp_dict */
	0,                         /* tp_descr_get */
	0,                         /* tp_descr_set */
	0,                         /* tp_dictoffset */
	(initproc)ButtonControl_init,                         /* tp_init */
	0,                         /* tp_alloc */
	ButtonControl_New,          /* tp_new */
	0,			    /* tp_free */
};

}

}

#ifdef __cplusplus
}
#endif
