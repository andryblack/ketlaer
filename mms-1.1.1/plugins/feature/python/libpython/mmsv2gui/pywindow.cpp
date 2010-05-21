#include "pywindow.hpp"
#include "pyutil.hpp"
#include "guiwindowmanager.hpp"
#include <iostream>
#include <typeinfo>

using namespace pymms::gui;

#ifdef __cplusplus
extern "C" {
#endif

namespace pymms {

namespace python {

static PyObject* Window_New(PyTypeObject* type, PyObject* args, PyObject* kwds)
{
  Window* self;

  self = (Window*)type->tp_alloc(type, 0);
  if (!self) 
    return NULL;

  self->isInitialized = false;
  self->iControlId = 10001;
  self->bIsOpen = false;

  return (PyObject*)self;
}

static int Window_init(Window* self, PyObject* args, PyObject* kwds)
{
  if(!self->isInitialized)
  {
    char* strXMLFile = NULL;

    static char *kwlist[] = {"xmlfile", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &strXMLFile))
      return -1;

    int iWindowId = 1001;
    GUIWindowManager* pWindowManager = S_WindowManager::get_instance();
    while(pWindowManager->getWindow(iWindowId))
      iWindowId++;

    self->pWindow = new GUIPYWindow();
    self->pWindow->setId(iWindowId);
    self->pWindow->setCallbackWindow((PyObject*)self);

    pWindowManager->registerWindow(self->pWindow);

    if(strXMLFile != NULL)
      self->pWindow->load(strXMLFile);

    self->isInitialized = true;
  }

  return 0;
}

static void Window_Dealloc(Window* self)
{
  if(self->pWindow)
    S_WindowManager::get_instance()->unregisterWindow(self->pWindow);

  for (std::vector<Control*>::iterator i = self->vecControls.begin(),
         end = self->vecControls.end(); i != end; i++) {

    if(self->pWindow)
      self->pWindow->removeControl((*i)->pControl->getId());

    Py_DECREF(*i);
  }

  self->vecControls.clear();

  if(self->pWindow)
  {
    self->pWindow->clear();
    delete self->pWindow;
  }

  self->ob_type->tp_free((PyObject*)self);
}

static PyObject* Window_close(Window* self, PyObject* args)
{
  if(!self->isInitialized) 
  { 
    PyErr_SetString(PyExc_AssertionError, "Window.__init__() not called");
    return NULL;
  }

  self->bIsOpen = false;

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* Window_onAction(Window* self, PyObject* args)
{
  if(!self->isInitialized) 
  { 
    PyErr_SetString(PyExc_AssertionError, "Window.__init__() not called");
    return NULL;
  }

  char* strAction;

  if (!PyArg_ParseTuple(args, "s", &strAction))
    return NULL;

  if(!strcmp(strAction, "back"))
    Window_close(self, args);

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* Window_onClick(Window* self, PyObject* args)
{
  if(!self->isInitialized) 
  { 
    PyErr_SetString(PyExc_AssertionError, "Window.__init__() not called");
    return NULL;
  }

  int iControl;

  if (!PyArg_ParseTuple(args, "i", &iControl))
    return NULL;

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* Window_onFocus(Window* self, PyObject* args)
{
  if(!self->isInitialized) 
  { 
    PyErr_SetString(PyExc_AssertionError, "Window.__init__() not called");
    return NULL;
  }

  int iControl;

  if (!PyArg_ParseTuple(args, "i", &iControl))
    return NULL;

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* Window_show(Window* self, PyObject* args)
{
  if(!self->isInitialized) 
  { 
    PyErr_SetString(PyExc_AssertionError, "Window.__init__() not called");
    return NULL;
  }

  S_WindowManager::get_instance()->setActiveWindow(self->pWindow);

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* Window_doModal(Window* self, PyObject* args)
{
  if(!self->isInitialized) 
  { 
    PyErr_SetString(PyExc_AssertionError, "Window.__init__() not called");
    return NULL;
  }

  if(!self->bIsOpen)
  {
    Py_PulseReturnEvent();

    Window_show(self, NULL);

    self->bIsOpen = true;
    while(self->bIsOpen)
    {  
      Py_BEGIN_ALLOW_THREADS
      Py_WaitForActionEvent();
      Py_END_ALLOW_THREADS

      // only call Py_MakePendingCalls from a python thread
      Py_MakePendingCalls();
    }
  }

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* Window_addControl(Window* self, PyObject* args)
{
  if(!self->isInitialized) 
  { 
    PyErr_SetString(PyExc_AssertionError, "Window.__init__() not called");
    return NULL;
  }

  Control* pControl;

  if (!PyArg_ParseTuple(args, "O", &pControl)) 
    return NULL;

  if(!Control_Check(pControl))
  {
    PyErr_SetString(PyExc_TypeError, "Object should be of type Control");
    return NULL;
  }

  if(!pControl->pControl->getId())
  {
    pControl->pControl->setId(++(self->iControlId));
    self->pWindow->addControl(pControl->pControl);
  }
  else
  {
    PyErr_SetString(PyExc_ReferenceError, "Control is already used");
    return NULL;
  }

  Py_INCREF(pControl);  
  self->vecControls.push_back(pControl);

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* Window_setFocus(Window* self, PyObject* args)
{
  if(!self->isInitialized)
  {
    PyErr_SetString(PyExc_AssertionError, "Window.__init__() not called");
    return NULL;
  }

  Control* pControl;

  if (!PyArg_ParseTuple(args, "O", &pControl)) 
    return NULL;

  if(!Control_Check(pControl))
  {
    PyErr_SetString(PyExc_TypeError, "Object should be of type Control");
    return NULL;
  }

  if(!self->pWindow->getControl(pControl->pControl->getId()))
  {
    PyErr_SetString(PyExc_RuntimeError, "Control does not exist in window");
    return NULL;
  }

  self->pWindow->setFocus(pControl->pControl->getId());

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* Window_setFocusId(Window* self, PyObject* args)
{
  if(!self->isInitialized) 
  { 
    PyErr_SetString(PyExc_AssertionError, "Window.__init__() not called");
    return NULL;
  }

  int iControl = -1;

  if (!PyArg_ParseTuple(args, "i", &iControl))
    return NULL;

  if(!self->pWindow->getControl(iControl))
  {
    PyErr_SetString(PyExc_RuntimeError, "Control does not exist in window");
    return NULL;
  }

  self->pWindow->setFocus(iControl);

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* Window_getFocus(Window* self, PyObject* args)
{
  if(!self->isInitialized) 
  { 
    PyErr_SetString(PyExc_AssertionError, "Window.__init__() not called");
    return NULL;
  }

  int iControl = -1;
  Control* pControl = 0;

  if((iControl = self->pWindow->getFocus()) == -1)
  {
    PyErr_SetString(PyExc_RuntimeError, "No control in this window has focus");
    return NULL;
  }

  for (std::vector<Control*>::iterator i = self->vecControls.begin(),
      end = self->vecControls.end(); i != end; i++) {
    if((*i)->pControl->getId() == iControl)
    {
      pControl = *i;
      break;
    }
  }

  if(!pControl)
  {
    GUIControl* pGuiControl = self->pWindow->getControl(iControl);
    if(pGuiControl)
    {
      switch(pGuiControl->getType())
      {
        case BUTTONCONTROL:
          pControl = (Control*)ButtonControl_Type.tp_alloc(&ButtonControl_Type, 0);
	  pControl->pControl = pGuiControl;
          pControl->isInitialized = true;
          break;

        case LABELCONTROL:
          pControl = (Control*)LabelControl_Type.tp_alloc(&LabelControl_Type, 0);
          pControl->pControl = pGuiControl;
          pControl->isInitialized = true;
          break;

	case IMAGECONTROL:
          pControl = (Control*)ImageControl_Type.tp_alloc(&ImageControl_Type, 0);
          pControl->pControl = pGuiControl;
          pControl->isInitialized = true;
          break;

        case LISTCONTROL:
          pControl = (Control*)ListControl_Type.tp_alloc(&ListControl_Type, 0);
          pControl->pControl = pGuiControl;
          pControl->isInitialized = true;
          break;

        case TEXTBOXCONTROL:
          pControl = (Control*)TextBoxControl_Type.tp_alloc(&TextBoxControl_Type, 0);
          pControl->pControl = pGuiControl;
	  pControl->isInitialized = true;
          break;

        case RECTANGLECONTROL:
          pControl = (Control*)RectangleControl_Type.tp_alloc(&RectangleControl_Type, 0);
          pControl->pControl = pGuiControl;
	  pControl->isInitialized = true;
          break;

        case TEXTFIELDCONTROL:
          pControl = (Control*)TextFieldControl_Type.tp_alloc(&TextFieldControl_Type, 0);
          pControl->pControl = pGuiControl;
	  pControl->isInitialized = true;
          break;

        case PROGRESSCONTROL:
          pControl = (Control*)ProgressControl_Type.tp_alloc(&ProgressControl_Type, 0);
          pControl->pControl = pGuiControl;
	  pControl->isInitialized = true;
          break;

	case IMAGELISTCONTROL:
          pControl = (Control*)ImageListControl_Type.tp_alloc(&ImageListControl_Type, 0);
          pControl->pControl = pGuiControl;
	  pControl->isInitialized = true;
          break;

	default:
	  PyErr_SetString(PyExc_RuntimeError, "This error should not happen :-)");
          return NULL;	  
      }

      self->vecControls.push_back(pControl);
    }
    else
    {
      PyErr_SetString(PyExc_RuntimeError, "This error should not happen :-)");
      return NULL;
    }
  }

  Py_INCREF(pControl);

  return (PyObject*)pControl;
}

static PyObject* Window_getFocusId(Window* self, PyObject* args)
{
  if(!self->isInitialized) 
  { 
    PyErr_SetString(PyExc_AssertionError, "Window.__init__() not called");
    return NULL;
  }

  int iControl = -1;
  Control* pControl = 0;

  if((iControl = self->pWindow->getFocus()) == -1)
  {
    PyErr_SetString(PyExc_RuntimeError, "No control in this window has focus");
    return NULL;
  }

  for (std::vector<Control*>::iterator i = self->vecControls.begin(),
      end = self->vecControls.end(); i != end; i++) {
    if((*i)->pControl->getId() == iControl)
    {
      pControl = *i;
      break;
    }
  }

  if(!pControl)
  {
    GUIControl* pGuiControl = self->pWindow->getControl(iControl);
    if(pGuiControl)
    {
      switch(pGuiControl->getType())
      {
        case BUTTONCONTROL:
          pControl = (Control*)ButtonControl_Type.tp_alloc(&ButtonControl_Type, 0);
          pControl->pControl = pGuiControl;
          pControl->isInitialized = true;
          break;

        case LABELCONTROL:
          pControl = (Control*)LabelControl_Type.tp_alloc(&LabelControl_Type, 0);
          pControl->pControl = pGuiControl;
	  pControl->isInitialized = true;
          break;

        case IMAGECONTROL:
          pControl = (Control*)ImageControl_Type.tp_alloc(&ImageControl_Type, 0);
          pControl->pControl = pGuiControl;
          pControl->isInitialized = true;
          break;

        case LISTCONTROL:
          pControl = (Control*)ListControl_Type.tp_alloc(&ListControl_Type, 0);
          pControl->pControl = pGuiControl;
	  pControl->isInitialized = true;
          break;

        case TEXTBOXCONTROL:
          pControl = (Control*)TextBoxControl_Type.tp_alloc(&TextBoxControl_Type, 0);
          pControl->pControl = pGuiControl;
	  pControl->isInitialized = true;
          break;

        case RECTANGLECONTROL:
          pControl = (Control*)RectangleControl_Type.tp_alloc(&RectangleControl_Type, 0);
          pControl->pControl = pGuiControl;
	  pControl->isInitialized = true;
          break;

        case TEXTFIELDCONTROL:
          pControl = (Control*)TextFieldControl_Type.tp_alloc(&TextFieldControl_Type, 0);
          pControl->pControl = pGuiControl;
	  pControl->isInitialized = true;
          break;

        case PROGRESSCONTROL:
          pControl = (Control*)ProgressControl_Type.tp_alloc(&ProgressControl_Type, 0);
          pControl->pControl = pGuiControl;
	  pControl->isInitialized = true;
          break;

	case IMAGELISTCONTROL:
          pControl = (Control*)ImageListControl_Type.tp_alloc(&ImageListControl_Type, 0);
          pControl->pControl = pGuiControl;
	  pControl->isInitialized = true;
          break;

        default:
          PyErr_SetString(PyExc_RuntimeError, "This error should not happen :-)");
          return NULL;
      }

      self->vecControls.push_back(pControl);
    }
    else
    {
      PyErr_SetString(PyExc_RuntimeError, "This error should not happen :-)");
      return NULL;
    }
  }

  return PyLong_FromLong(iControl);
}

static PyObject* Window_getControl(Window* self, PyObject* args)
{
  if(!self->isInitialized) 
  { 
    PyErr_SetString(PyExc_AssertionError, "Window.__init__() not called");
    return NULL;
  }

  int iControl = -1;
  Control* pControl = 0;

  if (!PyArg_ParseTuple(args, "i", &iControl))
    return NULL;

  for (std::vector<Control*>::iterator i = self->vecControls.begin(),
      end = self->vecControls.end(); i != end; i++) {
    if((*i)->pControl->getId() == iControl)
    {
      pControl = *i;
      break;
    }
  }

  if(!pControl)
  {
    GUIControl* pGuiControl = self->pWindow->getControl(iControl);
    if(pGuiControl)
    {
      switch(pGuiControl->getType())
      {
        case BUTTONCONTROL:
          pControl = (Control*)ButtonControl_Type.tp_alloc(&ButtonControl_Type, 0);
          pControl->pControl = pGuiControl;
          pControl->isInitialized = true;
          break;

        case LABELCONTROL:
          pControl = (Control*)LabelControl_Type.tp_alloc(&LabelControl_Type, 0);
          pControl->pControl = pGuiControl;
	  pControl->isInitialized = true;
          break;

        case IMAGECONTROL:
          pControl = (Control*)ImageControl_Type.tp_alloc(&ImageControl_Type, 0);
          pControl->pControl = pGuiControl;
	  pControl->isInitialized = true;
          break;

        case LISTCONTROL:
          pControl = (Control*)ListControl_Type.tp_alloc(&ListControl_Type, 0);
          pControl->pControl = pGuiControl;
	  pControl->isInitialized = true;
          break;

        case TEXTBOXCONTROL:
          pControl = (Control*)TextBoxControl_Type.tp_alloc(&TextBoxControl_Type, 0);
          pControl->pControl = pGuiControl;
	  pControl->isInitialized = true;
          break;

        case RECTANGLECONTROL:
          pControl = (Control*)RectangleControl_Type.tp_alloc(&RectangleControl_Type, 0);
          pControl->pControl = pGuiControl;
	  pControl->isInitialized = true;
          break;

        case TEXTFIELDCONTROL:
          pControl = (Control*)TextFieldControl_Type.tp_alloc(&TextFieldControl_Type, 0);
          pControl->pControl = pGuiControl;
	  pControl->isInitialized = true;
          break;

	case PROGRESSCONTROL:
          pControl = (Control*)ProgressControl_Type.tp_alloc(&ProgressControl_Type, 0);
          pControl->pControl = pGuiControl;
	  pControl->isInitialized = true;
          break;

	case IMAGELISTCONTROL:
          pControl = (Control*)ImageListControl_Type.tp_alloc(&ImageListControl_Type, 0);
          pControl->pControl = pGuiControl;
	  pControl->isInitialized = true;
          break;

        default:
          PyErr_SetString(PyExc_RuntimeError, "This error should not happen :-)");
          return NULL;
      }

      self->vecControls.push_back(pControl);
    }
    else
    {
      PyErr_SetString(PyExc_RuntimeError, "No control in this window have that id");
      return NULL;
    }
  }

  Py_INCREF(pControl);

  return (PyObject*)pControl;
}

static PyObject* Window_getWidth(Window* self, PyObject* args)
{
  if(!self->isInitialized) 
  { 
    PyErr_SetString(PyExc_AssertionError, "Window.__init__() not called");
    return NULL;
  }
 
  return PyLong_FromLong(self->pWindow->getWidth());
}

static PyObject* Window_getHeight(Window* self, PyObject* args)
{
  if(!self->isInitialized) 
  { 
    PyErr_SetString(PyExc_AssertionError, "Window.__init__() not called");
    return NULL;
  }

  return PyLong_FromLong(self->pWindow->getHeight());
}

static PyObject* Window_removeControl(Window* self, PyObject* args)
{
  if(!self->isInitialized) 
  { 
    PyErr_SetString(PyExc_AssertionError, "Window.__init__() not called");
    return NULL;
  }
 
  Control* pControl;

  if (!PyArg_ParseTuple(args, "O", &pControl)) 
    return NULL;

  // type checking, object should be of type Control
  if(!Control_Check(pControl))
  {
    PyErr_SetString(PyExc_TypeError, "Object should be of type Control");
    return NULL;
  }

  if(!self->pWindow->removeControl(pControl->pControl->getId()))
  {
    PyErr_SetString(PyExc_RuntimeError, "Control does not exist in window");
    return NULL;
  }

  for (std::vector<Control*>::iterator i = self->vecControls.begin(),
      end = self->vecControls.end(); i != end; i++) {
    if((*i) == pControl)
    {
      self->vecControls.erase(i);
      break;
    }
  }

  Py_DECREF(pControl);

  Py_INCREF(Py_None);
  return Py_None;
}

static PyMethodDef Window_methods[] = {
  {"onAction", (PyCFunction)Window_onAction, METH_VARARGS, 0},
  {"onClick", (PyCFunction)Window_onClick, METH_VARARGS, 0},
  {"onFocus", (PyCFunction)Window_onFocus, METH_VARARGS, 0},
  {"doModal", (PyCFunction)Window_doModal, METH_VARARGS, 0},
  {"close", (PyCFunction)Window_close, METH_VARARGS, 0},
  {"addControl", (PyCFunction)Window_addControl, METH_VARARGS, 0},
  {"setFocus", (PyCFunction)Window_setFocus, METH_VARARGS, 0},
  {"setFocusId", (PyCFunction)Window_setFocusId, METH_VARARGS, 0},
  {"getFocus", (PyCFunction)Window_getFocus, METH_VARARGS, 0},
  {"getFocusId", (PyCFunction)Window_getFocusId, METH_VARARGS, 0},
  {"getControl", (PyCFunction)Window_getControl, METH_VARARGS, 0},
  {"getHeight", (PyCFunction)Window_getHeight, METH_VARARGS, 0},
  {"getWidth", (PyCFunction)Window_getWidth, METH_VARARGS, 0},
  {"removeControl", (PyCFunction)Window_removeControl, METH_VARARGS, 0},
  {"show", (PyCFunction)Window_show, METH_VARARGS, 0},
  {NULL, NULL, 0, NULL}
};

PyTypeObject Window_Type = {
	PyObject_HEAD_INIT(NULL)
	0,                         /*ob_size*/
	"mmsv2gui.Window",         /*tp_name*/
	sizeof(Window),            /*tp_basicsize*/
	0,                         /*tp_itemsize*/
	(destructor)Window_Dealloc,/*tp_dealloc*/
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
	0,      /* tp_doc */
	0,		                     /* tp_traverse */
	0,		                     /* tp_clear */
	0,		                     /* tp_richcompare */
	0,		                     /* tp_weaklistoffset */
	0,		                     /* tp_iter */
	0,		                     /* tp_iternext */
	Window_methods,            /* tp_methods */
	0,                         /* tp_members */
	0,                         /* tp_getset */
	0,                         /* tp_base */
	0,                         /* tp_dict */
	0,                         /* tp_descr_get */
	0,                         /* tp_descr_set */
	0,                         /* tp_dictoffset */
	(initproc)Window_init,                         /* tp_init */
	0,                         /* tp_alloc */
	Window_New,                /* tp_new */
	0,			   /* tp_free */
};

}

}

#ifdef __cplusplus
}
#endif


