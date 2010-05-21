#include <Python.h>
#include "structmember.h"
#include "pycontrol.hpp"

using namespace pymms::gui;

#ifdef __cplusplus
extern "C" {
#endif

namespace pymms {

namespace python {

static PyObject* ImageListControl_New(PyTypeObject* type, PyObject* args, PyObject* kwds)
{
  ImageListControl* self;
  
  self = (ImageListControl*)Control_Type.tp_new(type, args, kwds);
  if (!self) 
    return NULL;
    
  return (PyObject*)self;
}

static int ImageListControl_init(ImageListControl* self, PyObject* args, PyObject* kwds)
{
  if(!self->pyControl.isInitialized)
  {
    int iPosX = 0, iPosY = 0, iWidth = 0, iHeight = 0;
    int iLayer = 4, iItemHeight = 30, iItemWidth = 30, iTextHeight = 20, iItemSpaceX = 10, iItemSpaceY = 1;
    bool bListCenterX = false, bListCenterY = false;
    char* strFont = "Vera";
    char* strRgb = "0x7f7f7f";
    char* strRgbFocus = "0xffffff";
    char* strTextAlignment = "left";

    static char *kwlist[] = {"x", "y", "width", "height",
                           "font", "rgb", "rgbfocus", "textalignment",
                           "itemheight", "itemwidth", "textheight",
                           "itemspacex", "itemspacey", "listcenterx", "listcentery", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "iiii|ssssiiiiibb", kwlist, &iPosX, &iPosY,
                                   &iWidth, &iHeight, &strFont, &strRgb, &strRgbFocus,
                                   &strTextAlignment, &iItemHeight, &iItemWidth, &iTextHeight,
                                   &iItemSpaceX, &iItemSpaceY, &bListCenterX, &bListCenterY))
      return -1;

    self->pyControl.pControl = new GUIImageListControl(iPosX, iPosY, iWidth, iHeight, iLayer,
                                                     strFont, strRgb, strRgbFocus, strTextAlignment,
                                                     iItemHeight, iItemWidth, iTextHeight, iItemSpaceX,
                                                     iItemSpaceY, bListCenterX, bListCenterY);
 
    self->pyControl.isInitialized = true;
  }

  return 0;
}

static PyObject* ImageListControl_setLabel(ImageListControl* self, PyObject* args, PyObject* kwds)
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
    ((GUIImageListControl*)self->pyControl.pControl)->setFont(strFont);
  if(strRgb)
    ((GUIImageListControl*)self->pyControl.pControl)->setRgb(strRgb);
  if(strRgbFocus)
    ((GUIImageListControl*)self->pyControl.pControl)->setRgbFocus(strRgbFocus);

  Py_INCREF(Py_None);
  return Py_None;
}

static void ImageListControl_Dealloc(ImageListControl* self)
{
  for (std::vector<ListItem*>::iterator i = self->vecItems.begin(),
         end = self->vecItems.end(); i != end; i++) {

    Py_DECREF(*i);
  }

  self->vecItems.clear();
  if(self->pyControl.pControl)
    ((GUIImageListControl*)self->pyControl.pControl)->clear();

  Control_Type.tp_dealloc((PyObject*)self);
}

static PyObject* ImageListControl_addItem(ImageListControl* self, PyObject* args)
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

  if(!((GUIImageListControl*)self->pyControl.pControl)->addItem(pItem->pListItem))
  {
    PyErr_SetString(PyExc_ReferenceError, "Item is already in list");
    return NULL;
  }

  Py_INCREF(pItem);

  self->vecItems.push_back(pItem);

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* ImageListControl_setItemHeight(ImageListControl* self, PyObject* args)
{
  if(!self->pyControl.isInitialized)
  {
    PyErr_SetString(PyExc_AssertionError, "Base class __init__() not called");
    return NULL;
  }

  int iItemHeight = 0;

  if (!PyArg_ParseTuple(args, "i", &iItemHeight))
    return NULL;

  ((GUIImageListControl*)self->pyControl.pControl)->setItemHeight(iItemHeight);

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* ImageListControl_setItemWidth(ImageListControl* self, PyObject* args)
{
  if(!self->pyControl.isInitialized)
  {
    PyErr_SetString(PyExc_AssertionError, "Base class __init__() not called");
    return NULL;
  }

  int iItemWidth = 0;

  if (!PyArg_ParseTuple(args, "i", &iItemWidth))
    return NULL;

  ((GUIImageListControl*)self->pyControl.pControl)->setItemWidth(iItemWidth);

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* ImageListControl_setItemSpaceX(ImageListControl* self, PyObject* args)
{
  if(!self->pyControl.isInitialized)
  {
    PyErr_SetString(PyExc_AssertionError, "Base class __init__() not called");
    return NULL;
  }

  int iItemSpaceX = 0;

  if (!PyArg_ParseTuple(args, "i", &iItemSpaceX))
    return NULL;

  ((GUIImageListControl*)self->pyControl.pControl)->setItemSpaceX(iItemSpaceX);

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* ImageListControl_setItemSpaceY(ImageListControl* self, PyObject* args)
{
  if(!self->pyControl.isInitialized)
  {
    PyErr_SetString(PyExc_AssertionError, "Base class __init__() not called");
    return NULL;
  }

  int iItemSpaceY = 0;

  if (!PyArg_ParseTuple(args, "i", &iItemSpaceY))
    return NULL;

  ((GUIImageListControl*)self->pyControl.pControl)->setItemSpaceY(iItemSpaceY);

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* ImageListControl_setTextHeight(ImageListControl* self, PyObject* args)
{
  if(!self->pyControl.isInitialized)
  {
    PyErr_SetString(PyExc_AssertionError, "Base class __init__() not called");
    return NULL;
  }

  int iTextHeight = 0;

  if (!PyArg_ParseTuple(args, "i", &iTextHeight))
    return NULL;

  ((GUIImageListControl*)self->pyControl.pControl)->setTextHeight(iTextHeight);

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* ImageListControl_clear(ImageListControl* self, PyObject* args)
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
  ((GUIImageListControl*)self->pyControl.pControl)->clear();

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* ImageListControl_size(ImageListControl* self, PyObject* args)
{
  if(!self->pyControl.isInitialized)
  {
    PyErr_SetString(PyExc_AssertionError, "Base class __init__() not called");
    return NULL;
  }

  return PyLong_FromLong(((GUIImageListControl*)self->pyControl.pControl)->size());
}

static PyObject* ImageListControl_getSelectedPosition(ImageListControl* self, PyObject* args)
{
  if(!self->pyControl.isInitialized)
  {
    PyErr_SetString(PyExc_AssertionError, "Base class __init__() not called");
    return NULL;
  }

  return PyLong_FromLong(((GUIImageListControl*)self->pyControl.pControl)->getSelectedPosition());
}

static PyObject* ImageListControl_getSelectedItem(ImageListControl* self, PyObject* args)
{
  if(!self->pyControl.isInitialized)
  {
    PyErr_SetString(PyExc_AssertionError, "Base class __init__() not called");
    return NULL;
  }

  const GUIListItem* pGuiItem;
  ListItem* pItem = 0;

  if(!(pGuiItem = ((GUIImageListControl*)self->pyControl.pControl)->getSelectedItem()))
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

static PyMethodDef ImageListControl_methods[] = {
  {"setLabel", (PyCFunction)ImageListControl_setLabel, METH_VARARGS | METH_KEYWORDS, 0},
  {"addItem", (PyCFunction)ImageListControl_addItem, METH_VARARGS, 0},
  {"clear", (PyCFunction)ImageListControl_clear, METH_VARARGS, 0},
  {"size", (PyCFunction)ImageListControl_size, METH_VARARGS, 0},
  {"getSelectedItem", (PyCFunction)ImageListControl_getSelectedItem, METH_VARARGS, 0},
  {"getSelectedPosition", (PyCFunction)ImageListControl_getSelectedPosition, METH_VARARGS, 0},
  {"setItemHeight", (PyCFunction)ImageListControl_setItemHeight, METH_VARARGS, 0},
  {"setItemWidth", (PyCFunction)ImageListControl_setItemWidth, METH_VARARGS, 0},
  {"setItemSpaceX", (PyCFunction)ImageListControl_setItemSpaceX, METH_VARARGS, 0},
  {"setItemSpaceY", (PyCFunction)ImageListControl_setItemSpaceY, METH_VARARGS, 0},
  {"setTextHeight", (PyCFunction)ImageListControl_setTextHeight, METH_VARARGS, 0},
  {NULL, NULL, 0, NULL}
};

PyTypeObject ImageListControl_Type = {
	PyObject_HEAD_INIT(NULL)
	0,                         /*ob_size*/
	"mmsv2gui.ImageListControl",    /*tp_name*/
	sizeof(ImageListControl),      /*tp_basicsize*/
	0,                         /*tp_itemsize*/
	(destructor)ImageListControl_Dealloc,/*tp_dealloc*/
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
	ImageListControl_methods,      /* tp_methods */
	0,                         /* tp_members */
	0,                         /* tp_getset */
	0,             /* tp_base */
	0,                         /* tp_dict */
	0,                         /* tp_descr_get */
	0,                         /* tp_descr_set */
	0,                         /* tp_dictoffset */
	(initproc)ImageListControl_init,                         /* tp_init */
	0,                         /* tp_alloc */
	ImageListControl_New,          /* tp_new */
	0,			       /* tp_free */
};

}

}

#ifdef __cplusplus
}
#endif
