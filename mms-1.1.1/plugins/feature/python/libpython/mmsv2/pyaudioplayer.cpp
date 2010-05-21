#include <Python.h>
#include "structmember.h"
#include "pyaudioplayer.hpp"

#ifdef __cplusplus
extern "C" {
#endif

namespace pymms {

namespace python {

static PyObject* PyAPlayer_New(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
  PyAPlayer *self;
  
  self = (PyAPlayer*)type->tp_alloc(type, 0);
  if (!self) 
    return NULL;
    
  self->audioplayer = new pymms::player::PythonAudioPlayer();		

  return (PyObject*)self;
}

static PyObject* PyAPlayer_play(PyAPlayer *self, PyObject *args, PyObject *kwds)
{
  char *path = NULL;
  char *name = "";

  static char *kwlist[] = {"path", "name", NULL};
    
  if (!PyArg_ParseTupleAndKeywords(args, kwds, "|ss", kwlist, &path, &name))	
    return NULL;

  if(path)
    self->audioplayer->play(path, name);
  else
    self->audioplayer->play();

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* PyAPlayer_stop(PyAPlayer *self, PyObject *args)
{
  self->audioplayer->stop();

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* PyAPlayer_pause(PyAPlayer *self, PyObject *args)
{
  self->audioplayer->pause();

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* PyAPlayer_mute(PyAPlayer *self, PyObject *args)
{
  self->audioplayer->mute();

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* PyAPlayer_volup(PyAPlayer *self, PyObject *args)
{
  self->audioplayer->volup();
  
  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* PyAPlayer_voldown(PyAPlayer *self, PyObject *args)
{
  self->audioplayer->voldown();

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* PyAPlayer_ff(PyAPlayer *self, PyObject *args)
{
  self->audioplayer->ff();

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* PyAPlayer_fb(PyAPlayer *self, PyObject *args)
{
  self->audioplayer->fb();

  Py_INCREF(Py_None);
  return Py_None;
}

static void PyAPlayer_Dealloc(PyAPlayer* self)
{
  delete self->audioplayer;
  self->ob_type->tp_free((PyObject*)self);
}

static PyMethodDef PyAPlayer_methods[] = {
  {"play", (PyCFunction)PyAPlayer_play, METH_VARARGS | METH_KEYWORDS, 0},
  {"stop", (PyCFunction)PyAPlayer_stop, METH_VARARGS, 0},
  {"pause", (PyCFunction)PyAPlayer_pause, METH_VARARGS, 0},
  {"mute", (PyCFunction)PyAPlayer_mute, METH_VARARGS, 0},
  {"volup", (PyCFunction)PyAPlayer_volup, METH_VARARGS, 0},
  {"voldown", (PyCFunction)PyAPlayer_voldown, METH_VARARGS, 0},
  {"ff", (PyCFunction)PyAPlayer_ff, METH_VARARGS, 0},
  {"fb", (PyCFunction)PyAPlayer_fb, METH_VARARGS, 0},
  {NULL, NULL, 0, NULL}
};

PyTypeObject PyAPlayer_Type = {
	PyObject_HEAD_INIT(NULL)
	0,                         /*ob_size*/
	"mmsv2.PyAPlayer",    /*tp_name*/
	sizeof(PyAPlayer),      /*tp_basicsize*/
	0,                         /*tp_itemsize*/
	(destructor)PyAPlayer_Dealloc,/*tp_dealloc*/
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
	PyAPlayer_methods,      /* tp_methods */
	0,                         /* tp_members */
	0,                         /* tp_getset */
	0,             /* tp_base */
	0,                         /* tp_dict */
	0,                         /* tp_descr_get */
	0,                         /* tp_descr_set */
	0,                         /* tp_dictoffset */
	0,                         /* tp_init */
	0,                         /* tp_alloc */
	PyAPlayer_New,          /* tp_new */
	0,			/* tp_free */
};

}

}

#ifdef __cplusplus
}
#endif
