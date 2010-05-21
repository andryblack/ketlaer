#include <Python.h>
#include "structmember.h"
#include "pycontrol.hpp"
#include "pyutil.hpp"

using std::string;
using namespace pymms::gui;

#ifdef __cplusplus
extern "C" {
#endif

namespace pymms {

namespace python {

static PyObject* TextFieldControl_New(PyTypeObject* type, PyObject* args, PyObject* kwds)
{
  TextFieldControl* self;
  
  self = (TextFieldControl*)Control_Type.tp_new(type, args, kwds);
  if (!self) 
    return NULL;
    
  return (PyObject*)self;
}

static int TextFieldControl_init(TextFieldControl* self, PyObject* args, PyObject* kwds)
{
  if(!self->pyControl.isInitialized)
  {
    int iPosX = 0, iPosY = 0, iWidth = 0, iHeight = 0, iLayer = 4, iTextOffset = 0;
    PyObject* pObject = NULL;
    char* strFont = "Vera";
    char* strRgb = "0xffffff";
    char* strAlignment = "left";

    static char *kwlist[] = {"x", "y", "width", "height",
                           "text", "font", "rgb", "textoffset", "alignment", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "iiii|Ossis", kwlist, &iPosX, &iPosY,
                                   &iWidth, &iHeight, &pObject, &strFont, &strRgb, &iTextOffset, &strAlignment))
      return -1;

    string strUtf8;
    if(pObject)
      Py_wCharToChar(strUtf8, pObject);

    self->pyControl.pControl = new GUITextFieldControl(iPosX, iPosY, iWidth, iHeight, strUtf8, iLayer,
                                              strFont, strRgb, iTextOffset, strAlignment);

    self->pyControl.isInitialized = true;
  }

  return 0;
}

static PyObject* TextFieldControl_setText(TextFieldControl* self, PyObject* args, PyObject* kwds)
{
  if(!self->pyControl.isInitialized)
  {
    PyErr_SetString(PyExc_AssertionError, "Base class __init__() not called");
    return NULL;
  }

  PyObject* pObject = NULL;
  char* strFont = NULL;
  char* strRgb = NULL;

  static char *kwlist[] = {"text", "font", "rgb", NULL};
    
  if (!PyArg_ParseTupleAndKeywords(args, kwds, "|Oss", kwlist, &pObject, &strFont, &strRgb))	
    return NULL;

  string strUtf8;
  if(pObject && Py_wCharToChar(strUtf8, pObject))
    ((GUITextFieldControl*)self->pyControl.pControl)->setText(strUtf8);
  if(strFont)
    ((GUITextFieldControl*)self->pyControl.pControl)->setFont(strFont);
  if(strRgb)
    ((GUITextFieldControl*)self->pyControl.pControl)->setRgb(strRgb);

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* TextFieldControl_getText(TextFieldControl* self, PyObject* args)
{
  if(!self->pyControl.isInitialized)
  {
    PyErr_SetString(PyExc_AssertionError, "Base class __init__() not called");
    return NULL;
  }

  return Py_BuildValue("s", ((GUITextFieldControl*)self->pyControl.pControl)->getText().c_str());
}

static PyObject* TextFieldControl_setEditable(TextFieldControl* self, PyObject* args)
{
  if(!self->pyControl.isInitialized)
  {
    PyErr_SetString(PyExc_AssertionError, "Base class __init__() not called");
    return NULL;
  }

  bool bEditable;

  if (!PyArg_ParseTuple(args, "b", &bEditable))
    return NULL;

  if (self->pyControl.pControl)
    ((GUITextFieldControl*)self->pyControl.pControl)->setEditable(bEditable);

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* TextFieldControl_getEditable(TextFieldControl* self, PyObject* args)
{
  if(!self->pyControl.isInitialized)
  {
    PyErr_SetString(PyExc_AssertionError, "Base class __init__() not called");
    return NULL;
  }

  return Py_BuildValue("b", ((GUITextFieldControl*)self->pyControl.pControl)->getEditable());
}

static PyMethodDef TextFieldControl_methods[] = {
  {"setText", (PyCFunction)TextFieldControl_setText, METH_VARARGS | METH_KEYWORDS, 0},
  {"getText", (PyCFunction)TextFieldControl_getText, METH_VARARGS, 0},
  {"getEditable", (PyCFunction)TextFieldControl_getEditable, METH_VARARGS, 0},
  {"setEditable", (PyCFunction)TextFieldControl_setEditable, METH_VARARGS, 0},
  {NULL, NULL, 0, NULL}
};

PyTypeObject TextFieldControl_Type = {
	PyObject_HEAD_INIT(NULL)
	0,                         /*ob_size*/
	"mmsv2gui.TextFieldControl",    /*tp_name*/
	sizeof(TextFieldControl),      /*tp_basicsize*/
	0,                         /*tp_itemsize*/
	0,			/*tp_dealloc*/
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
	TextFieldControl_methods,      /* tp_methods */
	0,                         /* tp_members */
	0,                         /* tp_getset */
	0,             /* tp_base */
	0,                         /* tp_dict */
	0,                         /* tp_descr_get */
	0,                         /* tp_descr_set */
	0,                         /* tp_dictoffset */
	(initproc)TextFieldControl_init,                         /* tp_init */
	0,                         /* tp_alloc */
	TextFieldControl_New,          /* tp_new */
	0,			       /* tp_free */
};

}

}

#ifdef __cplusplus
}
#endif
