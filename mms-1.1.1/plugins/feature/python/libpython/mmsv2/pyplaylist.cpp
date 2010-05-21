#include <Python.h>
#include "structmember.h"
#include "pyplaylist.hpp"

using std::string;
using std::pair;
using std::vector;

#ifdef __cplusplus
extern "C" {
#endif

namespace pymms {

namespace python {

static PyObject* PlayList_New(PyTypeObject* type, PyObject* args, PyObject* kwds)
{
  PlayList* self;

  self = (PlayList*)type->tp_alloc(type, 0);
  if (!self) 
    return NULL;

  self->pPlayList = new PythonPlayList();

  return (PyObject*)self;
}

static PyObject* PlayList_load(PlayList* self, PyObject* args, PyObject* kwds)
{
  char* strPath = NULL;

  static char* kwlist[] = {"path", NULL};
    
  if (!PyArg_ParseTupleAndKeywords(args, kwds, "s|", kwlist, &strPath))	
    return NULL;

  if(strPath)
    self->pPlayList->load(strPath);

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* PlayList_clear(PlayList* self, PyObject* args)
{
  self->pPlayList->clear();

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* PlayList_remove(PlayList* self, PyObject* args, PyObject* kwds)
{
  int iItem = -1;

  static char* kwlist[] = {"pos", NULL};

  if (!PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &iItem))
    return NULL;

  self->pPlayList->remove(iItem);

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* PlayList_add(PlayList* self, PyObject* args, PyObject* kwds)
{
  char* strPath = NULL;
  char* strTitle = "";

  static char* kwlist[] = {"path", "title", NULL};

  if (!PyArg_ParseTupleAndKeywords(args, kwds, "s|s", kwlist, &strPath, &strTitle))
    return NULL;

  if(strPath)
  {
    pair<string, string> item(strPath, strTitle);
    self->pPlayList->add(item);
  }

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* PlayList_toList(PlayList* self, PyObject* args)
{
  PyObject* pPyList = PyList_New(0);
 
  if(self->pPlayList->size())
  {
    vector<pair<string, string> >  titles = self->pPlayList->toList();

    for(vector<pair<string, string> >::iterator i = titles.begin(), end = titles.end(); i != end; i++)
      PyList_Append(pPyList, (PyObject*)Py_BuildValue("(ss)", (*i).first.c_str(), (*i).second.c_str()));
  }
 
  return (PyObject*)pPyList;
}

static PyObject* PlayList_size(PlayList* self, PyObject* args)
{
  return PyLong_FromLong(self->pPlayList->size());
}

static void PlayList_Dealloc(PlayList* self)
{
  delete self->pPlayList;
  self->ob_type->tp_free((PyObject*)self);
}

static PyMethodDef PlayList_methods[] = {
  {"load", (PyCFunction)PlayList_load, METH_VARARGS | METH_KEYWORDS, 0},
  {"clear", (PyCFunction)PlayList_clear, METH_VARARGS, 0},
  {"remove", (PyCFunction)PlayList_remove, METH_VARARGS | METH_KEYWORDS, 0},
  {"add", (PyCFunction)PlayList_add, METH_VARARGS | METH_KEYWORDS, 0},
  {"toList", (PyCFunction)PlayList_toList, METH_VARARGS, 0},
  {"size", (PyCFunction)PlayList_size, METH_VARARGS, 0},
  {NULL, NULL, 0, NULL}
};

PyTypeObject PlayList_Type = {
	PyObject_HEAD_INIT(NULL)
	0,                         /*ob_size*/
	"mmsv2.PlayList",    /*tp_name*/
	sizeof(PlayList),      /*tp_basicsize*/
	0,                         /*tp_itemsize*/
	(destructor)PlayList_Dealloc,/*tp_dealloc*/
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
	PlayList_methods,      /* tp_methods */
	0,                         /* tp_members */
	0,                         /* tp_getset */
	0,             /* tp_base */
	0,                         /* tp_dict */
	0,                         /* tp_descr_get */
	0,                         /* tp_descr_set */
	0,                         /* tp_dictoffset */
	0,                         /* tp_init */
	0,                         /* tp_alloc */
	PlayList_New,          /* tp_new */
	0,                     /* tp_free */
};

}

}

#ifdef __cplusplus
}
#endif
