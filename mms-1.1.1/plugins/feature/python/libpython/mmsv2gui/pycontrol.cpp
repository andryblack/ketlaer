#include "pycontrol.hpp"

#ifdef __cplusplus
extern "C" {
#endif

namespace pymms {

namespace python {


static PyObject* Control_New(PyTypeObject* type, PyObject* args, PyObject* kwds)
{
  Control* self;

  self = (Control*)type->tp_alloc(type, 0);
  if (!self)
    return NULL;

  self->isInitialized = false;
  self->pControl = NULL;

  return (PyObject*)self;
}

static void Control_Dealloc(Control* self)
{
  if(self->pControl)
    delete self->pControl;
  
  self->ob_type->tp_free((PyObject*)self);
}

static PyObject* Control_setVisible(Control* self, PyObject* args)
{
  if(!self->isInitialized)
  {
    PyErr_SetString(PyExc_AssertionError, "Base class __init__() not called");
    return NULL;
  }

  bool bVisible;

  if (!PyArg_ParseTuple(args, "b", &bVisible)) 
    return NULL;

  if (self->pControl) 	
    self->pControl->setVisible(bVisible);

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* Control_getVisible(Control* self, PyObject* args)
{
  if(!self->isInitialized)
  {
    PyErr_SetString(PyExc_AssertionError, "Base class __init__() not called");
    return NULL;
  }

  return Py_BuildValue("b", self->pControl->getVisible());
}

static PyObject* Control_setHeight(Control* self, PyObject* args)
{
  if(!self->isInitialized)
  {
    PyErr_SetString(PyExc_AssertionError, "Base class __init__() not called");
    return NULL;
  }

  int iHeight;

  if (!PyArg_ParseTuple(args, "i", &iHeight))
    return NULL;

  if (self->pControl)
    self->pControl->setHeight(iHeight);

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* Control_setWidth(Control* self, PyObject* args)
{
  if(!self->isInitialized)
  {
    PyErr_SetString(PyExc_AssertionError, "Base class __init__() not called");
    return NULL;
  }

  int iWidth;

  if (!PyArg_ParseTuple(args, "i", &iWidth))
    return NULL;

  if (self->pControl)
    self->pControl->setWidth(iWidth);

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* Control_setLayer(Control* self, PyObject* args)
{
  if(!self->isInitialized)
  {
    PyErr_SetString(PyExc_AssertionError, "Base class __init__() not called");
    return NULL;
  }

  int iLayer;

  if (!PyArg_ParseTuple(args, "i", &iLayer))
    return NULL;

  if (self->pControl)
    self->pControl->setLayer(iLayer);

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* Control_setRouteControl(Control* self, PyObject* args)
{
  if(!self->isInitialized)
  {
    PyErr_SetString(PyExc_AssertionError, "Base class __init__() not called");
    return NULL;
  }

  int iRouteControl;

  if (!PyArg_ParseTuple(args, "i", &iRouteControl))
    return NULL;

  if (self->pControl)
    self->pControl->setRouteControl(iRouteControl);

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* Control_setPosition(Control* self, PyObject* args)
{
  if(!self->isInitialized)
  {
    PyErr_SetString(PyExc_AssertionError, "Base class __init__() not called");
    return NULL;
  }

  int iPosX, iPosY;

  if (!PyArg_ParseTuple(args, "ii", &iPosX, &iPosY))
    return NULL;

  if (self->pControl)
    self->pControl->setPosition(iPosX, iPosY);

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* Control_controlLeft(Control* self, PyObject* args)
{
  if(!self->isInitialized)
  {
    PyErr_SetString(PyExc_AssertionError, "Base class __init__() not called");
    return NULL;
  }

  Control *pControl;

  if (!PyArg_ParseTuple(args, "O", &pControl))
    return NULL;

  if (!Control_Check(pControl))
  {
    PyErr_SetString(PyExc_TypeError, "Object should be of type Control");
    return NULL;
  }

  if(pControl->pControl->getId() == 0)
  {
    PyErr_SetString(PyExc_ReferenceError, "Control has to be added to a window first");
    return NULL;
  }

  if (self->pControl)
    self->pControl->controlLeft(pControl->pControl);

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* Control_controlRight(Control* self, PyObject* args)
{
  if(!self->isInitialized)
  {
    PyErr_SetString(PyExc_AssertionError, "Base class __init__() not called");
    return NULL;
  }

  Control *pControl;

  if (!PyArg_ParseTuple(args, "O", &pControl))
    return NULL;

  if (!Control_Check(pControl))
  {
    PyErr_SetString(PyExc_TypeError, "Object should be of type Control");
    return NULL;
  }

  if(pControl->pControl->getId() == 0)
  {
    PyErr_SetString(PyExc_ReferenceError, "Control has to be added to a window first");
    return NULL;
  }

  if (self->pControl)
    self->pControl->controlRight(pControl->pControl);

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* Control_controlPrev(Control* self, PyObject* args)
{
  if(!self->isInitialized)
  {
    PyErr_SetString(PyExc_AssertionError, "Base class __init__() not called");
    return NULL;
  }

  Control *pControl;

  if (!PyArg_ParseTuple(args, "O", &pControl))
    return NULL;

  if (!Control_Check(pControl))
  {
    PyErr_SetString(PyExc_TypeError, "Object should be of type Control");
    return NULL;
  }

  if(pControl->pControl->getId() == 0)
  {
    PyErr_SetString(PyExc_ReferenceError, "Control has to be added to a window first");
    return NULL;
  }

  if (self->pControl)
    self->pControl->controlPrev(pControl->pControl);

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* Control_controlNext(Control* self, PyObject* args)
{
  if(!self->isInitialized)
  {
    PyErr_SetString(PyExc_AssertionError, "Base class __init__() not called");
    return NULL;
  }

  Control *pControl;

  if (!PyArg_ParseTuple(args, "O", &pControl))
    return NULL;

  if (!Control_Check(pControl))
  {
    PyErr_SetString(PyExc_TypeError, "Object should be of type Control");
    return NULL;
  }

  if(pControl->pControl->getId() == 0)
  {
    PyErr_SetString(PyExc_ReferenceError, "Control has to be added to a window first");
    return NULL;
  }

  if (self->pControl)
    self->pControl->controlNext(pControl->pControl);

  Py_INCREF(Py_None);
  return Py_None;
}


static PyMethodDef Control_methods[] = {
  {"getVisible", (PyCFunction)Control_getVisible, METH_VARARGS, 0},
  {"setVisible", (PyCFunction)Control_setVisible, METH_VARARGS, 0},
  {"controlLeft", (PyCFunction)Control_controlLeft, METH_VARARGS, 0},
  {"controlRight", (PyCFunction)Control_controlRight, METH_VARARGS, 0},
  {"controlNext", (PyCFunction)Control_controlNext, METH_VARARGS, 0},
  {"controlPrev", (PyCFunction)Control_controlPrev, METH_VARARGS, 0},
  {"setHeight", (PyCFunction)Control_setHeight, METH_VARARGS, 0},
  {"setWidth", (PyCFunction)Control_setWidth, METH_VARARGS, 0},
  {"setLayer", (PyCFunction)Control_setLayer, METH_VARARGS, 0},
  {"setRouteControl", (PyCFunction)Control_setRouteControl, METH_VARARGS, 0},
  {"setPosition", (PyCFunction)Control_setPosition, METH_VARARGS, 0},
  {NULL, NULL, 0, NULL}
};

PyTypeObject Control_Type = {
	PyObject_HEAD_INIT(NULL)
	0,                         /*ob_size*/
	"mmsv2gui.Control",         /*tp_name*/
	sizeof(Control),           /*tp_basicsize*/
	0,                         /*tp_itemsize*/
	(destructor)Control_Dealloc,/*tp_dealloc*/
	0,                         /*tp_print*/
	0,                         /*tp_getattr*/
	0,                         /*tp_setattr*/
	0,           /*tp_compare*/
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
	0,            /* tp_doc */
	0,		                     /* tp_traverse */
	0,		                     /* tp_clear */
	0,		                     /* tp_richcompare */
	0,		                     /* tp_weaklistoffset */
	0,		                     /* tp_iter */
	0,		                     /* tp_iternext */
	Control_methods,           /* tp_methods */
	0,                         /* tp_members */
	0,                         /* tp_getset */
	0,                         /* tp_base */
	0,                         /* tp_dict */
	0,                         /* tp_descr_get */
	0,                         /* tp_descr_set */
	0,                         /* tp_dictoffset */
	0,                         /* tp_init */
	0,                         /* tp_alloc */
	Control_New,               /* tp_new */
	0,		           /* tp_free */
};

}

}

#ifdef __cplusplus
}
#endif

