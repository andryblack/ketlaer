#include "pyaction.hpp"

#ifdef __cplusplus
extern "C" {
#endif

namespace pymms {

namespace python {


static PyObject* Action_New(PyTypeObject* type, PyObject* args, PyObject* kwds)
{
  Action* self;

  self = (Action*)type->tp_alloc(type, 0);
  if (!self)
    return NULL;

  return (PyObject*)self;
}

static PyObject* Action_run(Action* self, PyObject* args)
{
  Py_INCREF(Py_None);
  return Py_None;
}

static void Action_Dealloc(Action* self)
{
  self->ob_type->tp_free((PyObject*)self);
}

static PyMethodDef Action_methods[] = {
  {"run", (PyCFunction)Action_run, METH_VARARGS, 0},
  {NULL, NULL, 0, NULL}
};

PyTypeObject Action_Type = {
	PyObject_HEAD_INIT(NULL)
	0,                         /*ob_size*/
	"mmsv2gui.Action",         /*tp_name*/
	sizeof(Action),           /*tp_basicsize*/
	0,                         /*tp_itemsize*/
	(destructor)Action_Dealloc, /*tp_dealloc*/
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
	Action_methods,           /* tp_methods */
	0,                         /* tp_members */
	0,                         /* tp_getset */
	0,                         /* tp_base */
	0,                         /* tp_dict */
	0,                         /* tp_descr_get */
	0,                         /* tp_descr_set */
	0,                         /* tp_dictoffset */
	0,                         /* tp_init */
	0,                         /* tp_alloc */
	Action_New,               /* tp_new */
	0,			  /* tp_free */
};

}

}

#ifdef __cplusplus
}
#endif

