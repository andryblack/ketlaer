#include <Python.h>
#include "structmember.h"
#include "pycontrol.hpp"
#include "pyaction.hpp"
#include "pywindow.hpp"
#include "guiwindowmanager.hpp"
#include "renderer.hpp"

#ifdef __cplusplus
extern "C" {
#endif

namespace pymms {

namespace python {

int Py_MMS_InvokeLater(void* pArg);

static PyObject* MMSV2GUI_redraw(PyObject *self, PyObject *args)
{
  S_WindowManager::get_instance()->render();

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* MMSV2GUI_themePath(PyObject *self, PyObject *args)
{
  return Py_BuildValue("s", S_Render::get_instance()->default_path.c_str());
}

static PyObject* MMSV2GUI_invokeLater(PyObject *self, PyObject *args)
{
  PyObject* pObject = NULL;

  if (!PyArg_ParseTuple(args, "O", &pObject))
   return NULL;

  if(Action_Check(pObject))
  {
    PyMMSAction* pAction = new PyMMSAction;

    Py_INCREF(pObject);
    pAction->pCallback = pObject;

    Py_AddPendingCall(Py_MMS_InvokeLater, pAction);
    Py_PulseActionEvent();
  }

  Py_INCREF(Py_None);
  return Py_None;
}

int Py_MMS_InvokeLater(void* pArg)
{
  if (pArg != NULL)
  {
    PyMMSAction* pAction = (PyMMSAction*)pArg;

    PyObject* pRet = PyObject_CallMethod(pAction->pCallback, "run", NULL, NULL);

    S_WindowManager::get_instance()->render();

    if(pRet)
      Py_DECREF(pRet);

    if(pAction->pCallback)
      Py_DECREF(pAction->pCallback);
 
    delete pAction;
  }

  return 0;
}

static PyMethodDef module_methods[] = {
   {"redraw", (PyCFunction)MMSV2GUI_redraw, METH_VARARGS, 0},
   {"themePath", (PyCFunction)MMSV2GUI_themePath, METH_VARARGS, 0},
   {"invokeLater", (PyCFunction)MMSV2GUI_invokeLater, METH_VARARGS, 0},
   {NULL, NULL, 0, NULL}
   /*{NULL}  Sentinel */
};

#ifndef PyMODINIT_FUNC
#define PyMODINIT_FUNC void
#endif
PyMODINIT_FUNC
initmmsv2gui(void)
{
  PyObject* mmsv2guimodule;

  if (PyType_Ready(&Control_Type) < 0)
    return;

  ImageControl_Type.tp_base = &Control_Type;
  if (PyType_Ready(&ImageControl_Type) < 0)
    return;

  LabelControl_Type.tp_base = &Control_Type;
  if (PyType_Ready(&LabelControl_Type) < 0)
    return;
 
  ButtonControl_Type.tp_base = &Control_Type;
  if (PyType_Ready(&ButtonControl_Type) < 0)
    return;

  ListControl_Type.tp_base = &Control_Type;
  if (PyType_Ready(&ListControl_Type) < 0)
    return;

  TextBoxControl_Type.tp_base = &Control_Type;
  if (PyType_Ready(&TextBoxControl_Type) < 0)
    return;

  RectangleControl_Type.tp_base = &Control_Type;
  if (PyType_Ready(&RectangleControl_Type) < 0)
    return;

  TextFieldControl_Type.tp_base = &Control_Type;
  if (PyType_Ready(&TextFieldControl_Type) < 0)
    return;

  ProgressControl_Type.tp_base = &Control_Type;
  if (PyType_Ready(&ProgressControl_Type) < 0)
    return;

  ImageListControl_Type.tp_base = &Control_Type;
  if (PyType_Ready(&ImageListControl_Type) < 0)
    return;

  if (PyType_Ready(&Window_Type) < 0)
    return;

  if (PyType_Ready(&ListItem_Type) < 0)
    return;

  if (PyType_Ready(&Action_Type) < 0)
    return;
  
  mmsv2guimodule = Py_InitModule3("mmsv2gui", module_methods, "mmsv2gui objects");

  if (mmsv2guimodule == NULL)
    return;

  Py_INCREF(&Control_Type);
  Py_INCREF(&ImageControl_Type);
  Py_INCREF(&LabelControl_Type);
  Py_INCREF(&ButtonControl_Type);
  Py_INCREF(&ListControl_Type);
  Py_INCREF(&TextBoxControl_Type);
  Py_INCREF(&RectangleControl_Type);
  Py_INCREF(&TextFieldControl_Type);
  Py_INCREF(&ProgressControl_Type);
  Py_INCREF(&ImageListControl_Type);
  Py_INCREF(&Window_Type);
  Py_INCREF(&ListItem_Type);
  Py_INCREF(&Action_Type);

  PyModule_AddObject(mmsv2guimodule, "Control", (PyObject *)&Control_Type);
  PyModule_AddObject(mmsv2guimodule, "ImageControl", (PyObject *)&ImageControl_Type);
  PyModule_AddObject(mmsv2guimodule, "LabelControl", (PyObject *)&LabelControl_Type);
  PyModule_AddObject(mmsv2guimodule, "ButtonControl", (PyObject *)&ButtonControl_Type);
  PyModule_AddObject(mmsv2guimodule, "ListControl", (PyObject *)&ListControl_Type);
  PyModule_AddObject(mmsv2guimodule, "TextBoxControl", (PyObject *)&TextBoxControl_Type);
  PyModule_AddObject(mmsv2guimodule, "RectangleControl", (PyObject *)&RectangleControl_Type);
  PyModule_AddObject(mmsv2guimodule, "TextFieldControl", (PyObject *)&TextFieldControl_Type);
  PyModule_AddObject(mmsv2guimodule, "ProgressControl", (PyObject *)&ProgressControl_Type);
  PyModule_AddObject(mmsv2guimodule, "ImageListControl", (PyObject *)&ImageListControl_Type);
  PyModule_AddObject(mmsv2guimodule, "Window", (PyObject *)&Window_Type);
  PyModule_AddObject(mmsv2guimodule, "ListItem", (PyObject *)&ListItem_Type);
  PyModule_AddObject(mmsv2guimodule, "Action", (PyObject *)&Action_Type);
}

}

}

#ifdef __cplusplus
}
#endif


