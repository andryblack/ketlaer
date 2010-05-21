#include <Python.h>
#include "structmember.h"
#include "pylistitem.hpp"
#include "pyutil.hpp"

using std::string;

#ifdef __cplusplus
extern "C" {
#endif

namespace pymms {

namespace python {

static PyObject* ListItem_New(PyTypeObject* type, PyObject* args, PyObject* kwds)
{
  ListItem* self;

  self = (ListItem*)type->tp_alloc(type, 0);
  if (!self) 
    return NULL;

  self->pListItem = new GUIListItem();

  return (PyObject*)self;
}

static PyObject* ListItem_setLabel(ListItem* self, PyObject* args, PyObject* kwds)
{
  PyObject* pObject = NULL;

  static char* kwlist[] = {"text", NULL};
    
  if (!PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &pObject))	
    return NULL;

  string strUtf8;
  if(pObject && Py_wCharToChar(strUtf8, pObject))
    self->pListItem->setLabel(strUtf8);

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* ListItem_getLabel(ListItem* self, PyObject* args)
{
  return Py_BuildValue("s", self->pListItem->getLabel().c_str());
}

static PyObject* ListItem_setLabel2(ListItem* self, PyObject* args, PyObject* kwds)
{
  PyObject* pObject = NULL;

  static char* kwlist[] = {"text", NULL};

  if (!PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &pObject))
    return NULL;

  string strUtf8;
  if(pObject && Py_wCharToChar(strUtf8, pObject))
    self->pListItem->setLabel2(strUtf8);

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* ListItem_getLabel2(ListItem* self, PyObject* args)
{
  return Py_BuildValue("s", self->pListItem->getLabel2().c_str());
}

static PyObject* ListItem_setThumbImage(ListItem* self, PyObject* args, PyObject* kwds)
{
  char* strThumbImage = NULL;

  static char* kwlist[] = {"thumbimage", NULL};

  if (!PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &strThumbImage))
    return NULL;

  if(strThumbImage)
    self->pListItem->setThumbImage(strThumbImage);

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* ListItem_setIconImage(ListItem* self, PyObject* args, PyObject* kwds)
{
  char* strIconImage = NULL;

  static char* kwlist[] = {"iconimage", NULL};

  if (!PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &strIconImage))
    return NULL;

  if(strIconImage)
    self->pListItem->setIconImage(strIconImage);

  Py_INCREF(Py_None);
  return Py_None;
}

static void ListItem_Dealloc(ListItem* self)
{
  delete self->pListItem;
  self->ob_type->tp_free((PyObject*)self);
}

static PyMethodDef ListItem_methods[] = {
  {"setLabel", (PyCFunction)ListItem_setLabel, METH_VARARGS | METH_KEYWORDS, 0},
  {"getLabel", (PyCFunction)ListItem_getLabel, METH_VARARGS, 0},
  {"setLabel2", (PyCFunction)ListItem_setLabel2, METH_VARARGS | METH_KEYWORDS, 0},
  {"getLabel2", (PyCFunction)ListItem_getLabel2, METH_VARARGS, 0},
  {"setThumbImage", (PyCFunction)ListItem_setThumbImage, METH_VARARGS | METH_KEYWORDS, 0},
  {"setIconImage", (PyCFunction)ListItem_setIconImage, METH_VARARGS | METH_KEYWORDS, 0},
  {NULL, NULL, 0, NULL}
};

PyTypeObject ListItem_Type = {
	PyObject_HEAD_INIT(NULL)
	0,                         /*ob_size*/
	"mmsv2gui.ListItem",    /*tp_name*/
	sizeof(ListItem),      /*tp_basicsize*/
	0,                         /*tp_itemsize*/
	(destructor)ListItem_Dealloc,/*tp_dealloc*/
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
	ListItem_methods,      /* tp_methods */
	0,                         /* tp_members */
	0,                         /* tp_getset */
	0,             /* tp_base */
	0,                         /* tp_dict */
	0,                         /* tp_descr_get */
	0,                         /* tp_descr_set */
	0,                         /* tp_dictoffset */
	0,                         /* tp_init */
	0,                         /* tp_alloc */
	ListItem_New,          /* tp_new */
	0,		       /* tp_free */
};

}

}

#ifdef __cplusplus
}
#endif
