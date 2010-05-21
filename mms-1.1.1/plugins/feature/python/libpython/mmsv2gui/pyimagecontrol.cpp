#include <Python.h>
#include "structmember.h"
#include "pycontrol.hpp"

using namespace pymms::gui;

#ifdef __cplusplus
extern "C" {
#endif

namespace pymms {

namespace python {

static PyObject* ImageControl_New(PyTypeObject* type, PyObject* args, PyObject* kwds)
{
  ImageControl* self;

  self = (ImageControl*)Control_Type.tp_new(type, args, kwds);
  if (!self) 
    return NULL;
  
  return (PyObject*)self;
}

static int ImageControl_init(ImageControl* self, PyObject* args, PyObject* kwds)
{
  if(!self->pyControl.isInitialized)
  {
    int iPosX = 0, iPosY = 0, iWidth = 0, iHeight = 0, iLayer = 1;
    char* strPath = "";
  
    static char *kwlist[] = {"x", "y", "width", "height", "path", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "iiii|s", kwlist, &iPosX, &iPosY,
                                   &iWidth, &iHeight, &strPath))
      return -1;
  
    Config* pConfig = S_Config::get_instance();
    if( (iWidth * iHeight) < (pConfig->p_h_res() * pConfig->p_v_res()) )
      iLayer = 3;
  
    self->pyControl.pControl = new GUIImageControl(iPosX, iPosY, iWidth, iHeight, strPath, iLayer);
    
    self->pyControl.isInitialized = true;
  }
  
  return 0;
}

static PyObject* ImageControl_setPath(ImageControl* self, PyObject* args, PyObject* kwds)
{
  if(!self->pyControl.isInitialized)
  {
    PyErr_SetString(PyExc_AssertionError, "Base class __init__() not called");
    return NULL;
  }

  char* strPath = NULL;

  static char *kwlist[] = {"path", NULL};

  if (!PyArg_ParseTupleAndKeywords(args, kwds, "s", kwlist, &strPath))
    return NULL;

  if(strPath)
    ((GUIImageControl*)self->pyControl.pControl)->setPath(strPath);

  Py_INCREF(Py_None);
  return Py_None;
}

static PyMethodDef ImageControl_methods[] = {
  {"setPath", (PyCFunction)ImageControl_setPath, METH_VARARGS | METH_KEYWORDS, 0},
  {NULL, NULL, 0, NULL}
};

PyTypeObject ImageControl_Type = {
	PyObject_HEAD_INIT(NULL)
	0,                         /*ob_size*/
	"mmsv2gui.ImageControl",    /*tp_name*/
	sizeof(ImageControl),      /*tp_basicsize*/
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
	ImageControl_methods,      /* tp_methods */
	0,                         /* tp_members */
	0,                         /* tp_getset */
	0,             /* tp_base */
	0,                         /* tp_dict */
	0,                         /* tp_descr_get */
	0,                         /* tp_descr_set */
	0,                         /* tp_dictoffset */
	(initproc)ImageControl_init,                         /* tp_init */
	0,                         /* tp_alloc */
	ImageControl_New,          /* tp_new */
	0,			   /* tp_free */
};

}

}

#ifdef __cplusplus
}
#endif
