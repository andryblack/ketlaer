#include <Python.h>
#include "structmember.h"
#include "pycontrol.hpp"

using namespace pymms::gui;

#ifdef __cplusplus
extern "C" {
#endif

namespace pymms {

namespace python {

static PyObject* ListControl_New(PyTypeObject* type, PyObject* args, PyObject* kwds)
{
  ListControl* self;
  
  self = (ListControl*)Control_Type.tp_new(type, args, kwds);
  if (!self) 
    return NULL;
    
  return (PyObject*)self;
}

static int ListControl_init(ListControl* self, PyObject* args, PyObject* kwds)
{
  if(!self->pyControl.isInitialized)
  {
    int iPosX = 0, iPosY = 0, iWidth = 0, iHeight = 0;
    int iLayer = 4, iTextOffset = 0, iItemHeight = 30, iItemSpace = 0;
    char* strFocus = "";
    char* strNoFocus = "";
    char* strFont = "Vera";
    char* strRgb = "0xffffff";
    char* strRgbFocus = "0xffffff";
    char* strAlignment = "left";

    static char *kwlist[] = {"x", "y", "width", "height",
                           "font", "rgb", "rgbfocus", "focus", "nofocus",
                           "textoffset", "alignment", "itemheight", "itemspace", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "iiii|sssssisii", kwlist, &iPosX, &iPosY,
                                   &iWidth, &iHeight, &strFont, &strRgb, &strRgbFocus,
                                   &strFocus, &strNoFocus, &iTextOffset, &strAlignment, &iItemHeight,
                                   &iItemSpace))
      return -1;

    self->pyControl.pControl = new GUIListControl(iPosX, iPosY, iWidth, iHeight, iLayer, strFocus,
                                        strNoFocus, strFont, strRgb, strRgbFocus, iTextOffset,
                                        strAlignment, iItemHeight, iItemSpace);

    self->pyControl.isInitialized = true;
  }

  return 0;
}

static PyObject* ListControl_setLabel(ListControl* self, PyObject* args, PyObject* kwds)
{
  if(!self->pyControl.isInitialized)
  {
    PyErr_SetString(PyExc_AssertionError, "Base class __init__() not called");
    return NULL;
  }

  char* strFont = NULL;
  char* strRgb = NULL;
  char* strRgbFocus = NULL;

  static char *kwlist[] = {"font", "rgb", "rgbfocus", NULL};
    
  if (!PyArg_ParseTupleAndKeywords(args, kwds, "|sss", kwlist, &strFont, &strRgb, &strRgbFocus))	
    return NULL;

  if(strFont)
    ((GUIListControl*)self->pyControl.pControl)->setFont(strFont);
  if(strRgb)
    ((GUIListControl*)self->pyControl.pControl)->setRgb(strRgb);
  if(strRgbFocus)
    ((GUIListControl*)self->pyControl.pControl)->setRgbFocus(strRgbFocus);

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* ListControl_setLabelAt(ListControl* self, PyObject* args, PyObject* kwds)
{
  if(!self->pyControl.isInitialized)
  {
    PyErr_SetString(PyExc_AssertionError, "Base class __init__() not called");
    return NULL;
  }

  char* strFont = NULL;
  char* strRgb = NULL;
  int iPos = 0;

  static char *kwlist[] = {"font", "rgb", "pos", NULL};

  if (!PyArg_ParseTupleAndKeywords(args, kwds, "|ssi", kwlist, &strFont, &strRgb, &iPos))
    return NULL;

  GUIListItem* pGuiItem;

  if(!(pGuiItem = ((GUIListControl*)self->pyControl.pControl)->getItemAt(iPos)))
  {
    PyErr_SetString(PyExc_RuntimeError, "No list item at given position");
    return NULL;
  }

  if(strFont)
    pGuiItem->setFont(strFont);
  if(strRgb)
    pGuiItem->setRgb(strRgb);

  ((GUIListControl*)self->pyControl.pControl)->m_iPosExtra = iPos;

  Py_INCREF(Py_None);
  return Py_None;
}

static void ListControl_Dealloc(ListControl* self)
{
  for (std::vector<ListItem*>::iterator i = self->vecItems.begin(),
         end = self->vecItems.end(); i != end; i++) {

    Py_DECREF(*i);
  }

  self->vecItems.clear();
  if(self->pyControl.pControl)
    ((GUIListControl*)self->pyControl.pControl)->clear();

  Control_Type.tp_dealloc((PyObject*)self);
}

static PyObject* ListControl_addItem(ListControl* self, PyObject* args)
{
  if(!self->pyControl.isInitialized)
  {
    PyErr_SetString(PyExc_AssertionError, "Base class __init__() not called");
    return NULL;
  }

  ListItem* pItem;

  if (!PyArg_ParseTuple(args, "O", &pItem))
    return NULL;

  if(!ListItem_Check(pItem))
  {
    PyErr_SetString(PyExc_TypeError, "Object should be of type ListItem");
    return NULL;
  }

  if(!((GUIListControl*)self->pyControl.pControl)->addItem(pItem->pListItem))
  {
    PyErr_SetString(PyExc_ReferenceError, "Item is already in list");
    return NULL;
  }

  Py_INCREF(pItem);

  self->vecItems.push_back(pItem);

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* ListControl_setItemHeight(ListControl* self, PyObject* args)
{
  if(!self->pyControl.isInitialized)
  {
    PyErr_SetString(PyExc_AssertionError, "Base class __init__() not called");
    return NULL;
  }

  int iItemHeight = 0;

  if (!PyArg_ParseTuple(args, "i", &iItemHeight))
    return NULL;

  ((GUIListControl*)self->pyControl.pControl)->setItemHeight(iItemHeight);

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* ListControl_setItemSpace(ListControl* self, PyObject* args)
{
  if(!self->pyControl.isInitialized)
  {
    PyErr_SetString(PyExc_AssertionError, "Base class __init__() not called");
    return NULL;
  }

  int iItemSpace = 0;

  if (!PyArg_ParseTuple(args, "i", &iItemSpace))
    return NULL;

  ((GUIListControl*)self->pyControl.pControl)->setItemSpace(iItemSpace);

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* ListControl_clear(ListControl* self, PyObject* args)
{
  if(!self->pyControl.isInitialized)
  {
    PyErr_SetString(PyExc_AssertionError, "Base class __init__() not called");
    return NULL;
  }

  for (std::vector<ListItem*>::iterator i = self->vecItems.begin(),
         end = self->vecItems.end(); i != end; i++) {

    Py_DECREF(*i);
  }

  self->vecItems.clear();
  ((GUIListControl*)self->pyControl.pControl)->clear();

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* ListControl_size(ListControl* self, PyObject* args)
{
  if(!self->pyControl.isInitialized)
  {
    PyErr_SetString(PyExc_AssertionError, "Base class __init__() not called");
    return NULL;
  }

  return PyLong_FromLong(((GUIListControl*)self->pyControl.pControl)->size());
}

static PyObject* ListControl_getSelectedPosition(ListControl* self, PyObject* args)
{
  if(!self->pyControl.isInitialized)
  {
    PyErr_SetString(PyExc_AssertionError, "Base class __init__() not called");
    return NULL;
  }

  return PyLong_FromLong(((GUIListControl*)self->pyControl.pControl)->getSelectedPosition());
}

static PyObject* ListControl_getSelectedItem(ListControl* self, PyObject* args)
{
  if(!self->pyControl.isInitialized)
  {
    PyErr_SetString(PyExc_AssertionError, "Base class __init__() not called");
    return NULL;
  }

  const GUIListItem* pGuiItem;
  ListItem* pItem = 0;

  if(!(pGuiItem = ((GUIListControl*)self->pyControl.pControl)->getSelectedItem()))
  {
    Py_INCREF(Py_None);
    return Py_None;
  }

  for (std::vector<ListItem*>::iterator i = self->vecItems.begin(),
      end = self->vecItems.end(); i != end; i++) {
    if((*i)->pListItem == pGuiItem)
    {
      pItem = *i;
      break;
    }
  }

  if(!pItem)
  {
    PyErr_SetString(PyExc_RuntimeError, "This error should not happen :-)");
    return NULL;
  }

  Py_INCREF(pItem);

  return (PyObject*)pItem;
}

static PyObject* ListControl_getItemAt(ListControl* self, PyObject* args)
{
  if(!self->pyControl.isInitialized)
  {
    PyErr_SetString(PyExc_AssertionError, "Base class __init__() not called");
    return NULL;
  }

  int iPos = 0;

  if (!PyArg_ParseTuple(args, "i", &iPos))
    return NULL;

  const GUIListItem* pGuiItem;
  ListItem* pItem = 0;

  if(!(pGuiItem = ((GUIListControl*)self->pyControl.pControl)->getItemAt(iPos)))
  {
    Py_INCREF(Py_None);
    return Py_None;
  }

  for (std::vector<ListItem*>::iterator i = self->vecItems.begin(),
      end = self->vecItems.end(); i != end; i++) {
    if((*i)->pListItem == pGuiItem)
    {
      pItem = *i;
      break;
    }
  }

  if(!pItem)
  {
    PyErr_SetString(PyExc_RuntimeError, "This error should not happen :-)");
    return NULL;
  }

  Py_INCREF(pItem);

  return (PyObject*)pItem;
}

static PyMethodDef ListControl_methods[] = {
  {"setLabel", (PyCFunction)ListControl_setLabel, METH_VARARGS | METH_KEYWORDS, 0},
  {"setLabelAt", (PyCFunction)ListControl_setLabelAt, METH_VARARGS | METH_KEYWORDS, 0},
  {"addItem", (PyCFunction)ListControl_addItem, METH_VARARGS, 0},
  {"clear", (PyCFunction)ListControl_clear, METH_VARARGS, 0},
  {"size", (PyCFunction)ListControl_size, METH_VARARGS, 0},
  {"getSelectedItem", (PyCFunction)ListControl_getSelectedItem, METH_VARARGS, 0},
  {"getItemAt", (PyCFunction)ListControl_getItemAt, METH_VARARGS, 0},
  {"getSelectedPosition", (PyCFunction)ListControl_getSelectedPosition, METH_VARARGS, 0},
  {"setItemHeight", (PyCFunction)ListControl_setItemHeight, METH_VARARGS, 0},
  {"setItemSpace", (PyCFunction)ListControl_setItemSpace, METH_VARARGS, 0},
  {NULL, NULL, 0, NULL}
};

PyTypeObject ListControl_Type = {
	PyObject_HEAD_INIT(NULL)
	0,                         /*ob_size*/
	"mmsv2gui.ListControl",    /*tp_name*/
	sizeof(ListControl),      /*tp_basicsize*/
	0,                         /*tp_itemsize*/
	(destructor)ListControl_Dealloc,/*tp_dealloc*/
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
	ListControl_methods,      /* tp_methods */
	0,                         /* tp_members */
	0,                         /* tp_getset */
	0,             /* tp_base */
	0,                         /* tp_dict */
	0,                         /* tp_descr_get */
	0,                         /* tp_descr_set */
	0,                         /* tp_dictoffset */
	(initproc)ListControl_init,                         /* tp_init */
	0,                         /* tp_alloc */
	ListControl_New,          /* tp_new */
	0,			  /* tp_free */
};

}

}

#ifdef __cplusplus
}
#endif
