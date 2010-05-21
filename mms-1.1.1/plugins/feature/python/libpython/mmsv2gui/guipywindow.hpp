#ifndef GUIPYWINDOW_HPP
#define GUIPYWINDOW_HPP

#include <Python.h>
#include "structmember.h"
#include "guiwindow.hpp"
#include "pyutil.hpp"

using namespace pymms::gui;

class PyMMSAction
{
public:
  PyObject* pCallback;

  PyMMSAction();
};

class PyMMSActionEvent : public PyMMSAction
{
public:
  int iControlId;
  int iType; // 0=Action, 1=Control;
  std::string strAction;

  PyMMSActionEvent();
};

int Py_MMS_Event_OnAction(void* pArg);
int Py_MMS_Event_OnClick(void* pArg);
int Py_MMS_Event_OnFocus(void* pArg);

class GUIPYWindow : public GUIWindow
{
  private:
    std::string m_strPyAction;

  protected:
    PyObject* m_pCallbackWindow;

  public:
    GUIPYWindow();
    virtual ~GUIPYWindow();
    
    bool onAction(const std::string& strAction);
    bool onMessage(GUIMessage& message);
    void setCallbackWindow(PyObject* pObject);
};

#endif
