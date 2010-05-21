#include <Python.h>
#include "structmember.h"
#include "pycontrol.hpp"

using namespace pymms::gui;

#ifdef __cplusplus
extern "C" {
#endif

namespace pymms {

namespace python {

static PyObject* RectangleControl_New(PyTypeObject* type, PyObject* args, PyObject* kwds)
{
  RectangleControl* self;
  
  self = (RectangleControl*)Control_Type.tp_new(type, args, kwds);
  if (!self) 
    return NULL;
    
  return (PyObject*)self;
}

static int RectangleControl_init(RectangleControl* self, PyObject* args, PyObject* kwds)
{
  if(!self->pyControl.isInitialized)
  {
    int iPosX = 0, iPosY = 0, iWidth = 0, iHeight = 0, iLayer = 2, iAlpha = 100;
    char* strRgb = "0xffffff";

    static char *kwlist[] = {"x", "y", "width", "height",
                           "alpha", "rgb", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "iiii|is", kwlist, &iPosX, &iPosY,
                                   &iWidth, &iHeight, &iAlpha, &strRgb))
      return -1;

    self->pyControl.pControl = new GUIRectangleControl(iPosX, iPosY, iWidth,
                                                iHeight, iLayer, iAlpha, strRgb);

    self->pyControl.isInitialized = true;
  }

  return 0;
}

static PyObject* RectangleControl_setColor(RectangleControl* self, PyObject* args, PyObject* kwds)
{
  if(!self->pyControl.isInitialized)
  {
    PyErr_SetString(PyExc_AssertionError, "Base class __init__() not called");
    return NULL;
  }

  char* strRgb = NULL;
  int iAlpha = 0;

  static char *kwlist[] = {"alpha", "rgb", NULL};
    
  if (!PyArg_ParseTupleAndKeywords(args, kwds, "|is", kwlist, &iAlpha, &strRgb))	
    return NULL;

  if(strRgb)
    ((GUIRectangleControl*)self->pyControl.pControl)->setRgb(strRgb);
  if(iAlpha)
    ((GUIRectangleControl*)self->pyControl.pControl)->setAlpha(iAlpha);

  Py_INCREF(Py_None);
  return Py_None;
}

static PyMethodDef RectangleControl_methods[] = {
  {"setColor", (PyCFunction)RectangleControl_setColor, METH_VARARGS | METH_KEYWORDS, 0},
  {NULL, NULL, 0, NULL}
};

PyTypeObject RectangleControl_Type = {
	PyObject_HEAD_INIT(NULL)
	0,                         /*ob_size*/
	"mmsv2gui.RectangleControl",    /*tp_name*/
	sizeof(RectangleControl),      /*tp_basicsize*/
	0,                         /*tp_itemsize*/
	0,		        /*tp_dealloc*/
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
	RectangleControl_methods,      /* tp_methods */
	0,                         /* tp_members */
	0,                         /* tp_getset */
	0,             /* tp_base */
	0,                         /* tp_dict */
	0,                         /* tp_descr_get */
	0,                         /* tp_descr_set */
	0,                         /* tp_dictoffset */
	(initproc)RectangleControl_init,                         /* tp_init */
	0,                         /* tp_alloc */
	RectangleControl_New,          /* tp_new */
	0,			       /* tp_free */
};

}

}

#ifdef __cplusplus
}
#endif
