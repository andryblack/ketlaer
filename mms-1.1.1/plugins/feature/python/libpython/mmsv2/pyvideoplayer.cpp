#include <Python.h>
#include "structmember.h"
#include "pyvideoplayer.hpp"

#ifdef __cplusplus
extern "C" {
#endif

namespace pymms {

namespace python {

static PyObject* PyVPlayer_New(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
  PyVPlayer *self;
  
  self = (PyVPlayer*)type->tp_alloc(type, 0);
  if (!self) 
    return NULL;
    
  self->movieplayer = new pymms::player::PythonVideoPlayer();

  return (PyObject*)self;
}

static PyObject* PyVPlayer_play(PyVPlayer *self, PyObject *args, PyObject *kwds)
{
  char *path = NULL;
  bool window = false;

  static char *kwlist[] = {"path", "window", NULL};
    
  if (!PyArg_ParseTupleAndKeywords(args, kwds, "|sb", kwlist, &path, &window))	
    return NULL;

  if(path)
    self->movieplayer->play(path, window);

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* PyVPlayer_stop(PyVPlayer *self, PyObject *args)
{
  self->movieplayer->stop();

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* PyVPlayer_pause(PyVPlayer *self, PyObject *args)
{
  self->movieplayer->pause();

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* PyVPlayer_ff(PyVPlayer *self, PyObject *args)
{
  self->movieplayer->ff();

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* PyVPlayer_fb(PyVPlayer *self, PyObject *args)
{
  self->movieplayer->fb();

  Py_INCREF(Py_None);
  return Py_None;
}

static void PyVPlayer_Dealloc(PyVPlayer* self)
{
  delete self->movieplayer;
  self->ob_type->tp_free((PyObject*)self);
}

static PyMethodDef PyVPlayer_methods[] = {
  {"play", (PyCFunction)PyVPlayer_play, METH_VARARGS | METH_KEYWORDS, 0},
  {"stop", (PyCFunction)PyVPlayer_stop, METH_VARARGS, 0},
  {"pause", (PyCFunction)PyVPlayer_pause, METH_VARARGS, 0},
  {"ff", (PyCFunction)PyVPlayer_ff, METH_VARARGS, 0},
  {"fb", (PyCFunction)PyVPlayer_fb, METH_VARARGS, 0},
  {NULL, NULL, 0, NULL}
};

PyTypeObject PyVPlayer_Type = {
	PyObject_HEAD_INIT(NULL)
	0,                         /*ob_size*/
	"mmsv2.PyVPlayer",    /*tp_name*/
	sizeof(PyVPlayer),      /*tp_basicsize*/
	0,                         /*tp_itemsize*/
	(destructor)PyVPlayer_Dealloc,/*tp_dealloc*/
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
	PyVPlayer_methods,      /* tp_methods */
	0,                         /* tp_members */
	0,                         /* tp_getset */
	0,             /* tp_base */
	0,                         /* tp_dict */
	0,                         /* tp_descr_get */
	0,                         /* tp_descr_set */
	0,                         /* tp_dictoffset */
	0,                         /* tp_init */
	0,                         /* tp_alloc */
	PyVPlayer_New,          /* tp_new */
	0,		        /* tp_free */
};

}

}

#ifdef __cplusplus
}
#endif
