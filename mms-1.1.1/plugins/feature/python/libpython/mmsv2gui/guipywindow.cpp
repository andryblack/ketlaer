#include "guipywindow.hpp"
#include "guiwindowmanager.hpp"
#include "themes.hpp"

using std::string;

PyMMSAction::PyMMSAction()
{
  pCallback = NULL;
}

PyMMSActionEvent::PyMMSActionEvent() 
{
  iType = 0;
  iControlId = -1;
  strAction = "";
}

GUIPYWindow::GUIPYWindow()
{
  m_pCallbackWindow = NULL;
}

GUIPYWindow::~GUIPYWindow()
{}

void GUIPYWindow::setCallbackWindow(PyObject* pObject)
{
  m_pCallbackWindow = pObject;
}

bool GUIPYWindow::onMessage(GUIMessage& message)
{
  bool bRetVal = GUIWindow::onMessage(message);

  switch(message.getMessage())
  {
    case GUI_MSG_CLICKED:
    {
       if(m_pCallbackWindow && m_bActive)
       {
	 PyMMSActionEvent* pAction = new PyMMSActionEvent;
	
         pAction->pCallback = m_pCallbackWindow;
         pAction->iControlId = message.getSenderId();
	 pAction->iType = 1;

         Py_AddPendingCall(Py_MMS_Event_OnClick, pAction);
       }

       break;
    }

    case GUI_MSG_SETFOCUS:
    {
       if(m_pCallbackWindow && m_bActive)
       {
         PyMMSActionEvent* pAction = new PyMMSActionEvent;

         pAction->pCallback = m_pCallbackWindow;
         pAction->iControlId = message.getControlId();
         pAction->iType = 1;

         Py_AddPendingCall(Py_MMS_Event_OnFocus, pAction);
       }

       break;
    }
  }

  return bRetVal;
}

bool GUIPYWindow::onAction(const string& strAction)
{
  GUIWindow::onAction(strAction);

  if(m_pCallbackWindow && m_bActive)
  {
    PyMMSActionEvent* pAction = new PyMMSActionEvent;

    pAction->pCallback = m_pCallbackWindow;
    pAction->strAction = strAction;
    pAction->iType = 0;
    pAction->iControlId = -1;

    Py_SetWaiting();
    Py_AddPendingCall(Py_MMS_Event_OnAction, pAction);
    Py_PulseActionEvent();

    Py_WaitForReturnEvent();
  }

  return true;
}

/*
 * called from python library!
 */
int Py_MMS_Event_OnAction(void* pArg)
{
  if (pArg != NULL)
  {
    PyMMSActionEvent* pAction = (PyMMSActionEvent*)pArg;

    PyObject* pRet = PyObject_CallMethod(pAction->pCallback, "onAction", "(s)", pAction->strAction.c_str());

    S_WindowManager::get_instance()->render();

    if(pRet) 
      Py_DECREF(pRet);

    delete pAction;
  }

  Py_PulseReturnEvent();

  return 0;
}

int Py_MMS_Event_OnClick(void* pArg)
{
  if (pArg != NULL)
  {
    PyMMSActionEvent* pAction = (PyMMSActionEvent*)pArg;
    PyObject *pRet = PyObject_CallMethod(pAction->pCallback, "onClick", "(i)", pAction->iControlId);
    
    if (pRet)
      Py_DECREF(pRet);

    delete pAction;
  }

  return 0;
}

int Py_MMS_Event_OnFocus(void* pArg)
{
  if (pArg != NULL)
  {
    PyMMSActionEvent* pAction = (PyMMSActionEvent*)pArg;
    PyObject* pRet = PyObject_CallMethod(pAction->pCallback, "onFocus", "(i)", pAction->iControlId);
    
    if (pRet)
      Py_DECREF(pRet);

    delete pAction;
  }

  return 0;
}
