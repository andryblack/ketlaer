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

static PyObject* ProgressControl_New(PyTypeObject* type, PyObject* args, PyObject* kwds)
{
  ProgressControl* self;
  
  self = (ProgressControl*)Control_Type.tp_new(type, args, kwds);
  if (!self) 
    return NULL;
    
  return (PyObject*)self;
}

static int ProgressControl_init(ProgressControl* self, PyObject* args, PyObject* kwds)
{
  if(!self->pyControl.isInitialized)
  {
    int iPosX = 0, iPosY = 0, iWidth = 0, iHeight = 0, iLayer = 4, iTextOffset = 0;
    PyObject* pObject = NULL;
    char* strFont = "Vera";
    char* strRgb = "0xffffff";
    char* strAlignment = "center";
    char* strRgbProgress = "0x00ff00";
    int iAlphaProgress = 100;

    static char *kwlist[] = {"x", "y", "width", "height",
                           "text", "font", "rgb", "aligment", "rgbprogress",
                           "alphaprogress", "textoffset", NULL};

    if(!PyArg_ParseTupleAndKeywords(args, kwds, "iiii|Ossssii", kwlist, &iPosX, &iPosY,
                                   &iWidth, &iHeight, &pObject, &strFont, &strRgb, &strAlignment,
                                   &strRgbProgress, &iAlphaProgress, &iTextOffset))
      return -1;

    string strUtf8;
    if(pObject)
      Py_wCharToChar(strUtf8, pObject);

    self->pyControl.pControl = new GUIProgressControl(iPosX, iPosY, iWidth, iHeight, iLayer, strUtf8,
                                          strFont, strRgb, strAlignment, strRgbProgress, iAlphaProgress,
                                          iTextOffset);

    self->pyControl.isInitialized = true;
  }

  return 0;
}

static PyObject* ProgressControl_setText(ProgressControl* self, PyObject* args, PyObject* kwds)
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
    
  if (!PyArg_ParseTupleAndKeywords(args, kwds, "|Oss", kwlist, &pObject, 
				   &strFont, &strRgb))	
    return NULL;

  string strUtf8;
  if(pObject && Py_wCharToChar(strUtf8, pObject))
    ((GUIProgressControl*)self->pyControl.pControl)->setText(strUtf8);
  if(strFont)
    ((GUIProgressControl*)self->pyControl.pControl)->setFont(strFont);
  if(strRgb)
    ((GUIProgressControl*)self->pyControl.pControl)->setRgb(strRgb);

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* ProgressControl_setBackground(ProgressControl* self, PyObject* args, PyObject* kwds)
{
  if(!self->pyControl.isInitialized)
  {
    PyErr_SetString(PyExc_AssertionError, "Base class __init__() not called");
    return NULL;
  }

  char* strRgbProgress = NULL;
  int iAlphaProgress = -1;

  static char *kwlist[] = {"rgb", "alpha", NULL};

  if (!PyArg_ParseTupleAndKeywords(args, kwds, "|si", kwlist, &strRgbProgress, iAlphaProgress))
    return NULL;

  if(strRgbProgress)
    ((GUIProgressControl*)self->pyControl.pControl)->setRgbProgress(strRgbProgress);
  if(iAlphaProgress != -1)
    ((GUIProgressControl*)self->pyControl.pControl)->setAlphaProgress(iAlphaProgress);

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* ProgressControl_setBounds(ProgressControl* self, PyObject* args, PyObject* kwds)
{
  if(!self->pyControl.isInitialized)
  {
    PyErr_SetString(PyExc_AssertionError, "Base class __init__() not called");
    return NULL;
  }

  int iMin = 0;
  int iMax = 100;

  static char *kwlist[] = {"min", "max", NULL};

  if (!PyArg_ParseTupleAndKeywords(args, kwds, "|ii", kwlist, &iMin, &iMax))
    return NULL;

  ((GUIProgressControl*)self->pyControl.pControl)->setBounds(iMin, iMax);

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* ProgressControl_update(ProgressControl* self, PyObject* args, PyObject* kwds)
{
  if(!self->pyControl.isInitialized)
  {
    PyErr_SetString(PyExc_AssertionError, "Base class __init__() not called");
    return NULL;
  }

  int iValue = 0;

  static char *kwlist[] = {"value", NULL};

  if (!PyArg_ParseTupleAndKeywords(args, kwds, "|i", kwlist, &iValue))
    return NULL;

  ((GUIProgressControl*)self->pyControl.pControl)->update(iValue);

  Py_INCREF(Py_None);
  return Py_None;
}

static PyMethodDef ProgressControl_methods[] = {
  {"setText", (PyCFunction)ProgressControl_setText, METH_VARARGS | METH_KEYWORDS, 0},
  {"setBackground", (PyCFunction)ProgressControl_setBackground, METH_VARARGS | METH_KEYWORDS, 0},
  {"setBounds", (PyCFunction)ProgressControl_setBounds, METH_VARARGS | METH_KEYWORDS, 0},
  {"update", (PyCFunction)ProgressControl_update, METH_VARARGS | METH_KEYWORDS, 0},
  {NULL, NULL, 0, NULL}
};

PyTypeObject ProgressControl_Type = {
	PyObject_HEAD_INIT(NULL)
	0,                         /*ob_size*/
	"mmsv2gui.ProgressControl",    /*tp_name*/
	sizeof(ProgressControl),      /*tp_basicsize*/
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
	ProgressControl_methods,      /* tp_methods */
	0,                         /* tp_members */
	0,                         /* tp_getset */
	0,             /* tp_base */
	0,                         /* tp_dict */
	0,                         /* tp_descr_get */
	0,                         /* tp_descr_set */
	0,                         /* tp_dictoffset */
	(initproc)ProgressControl_init,                         /* tp_init */
	0,                         /* tp_alloc */
	ProgressControl_New,          /* tp_new */
	0,			      /* tp_free */
};

}

}

#ifdef __cplusplus
}
#endif
