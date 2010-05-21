#include "guimessage.hpp"

using std::string;

pymms::gui::GUIMessage::GUIMessage(MESSAGE msgId, int iSenderId, int iControlId, 
		  	           int iParam1, int iParam2, void* pVoid)
{
   m_msgId = msgId;
   m_iSenderId = iSenderId;
   m_iControlId = iControlId;
   m_iParam1 = iParam1;
   m_iParam2 = iParam2;
   m_pVoid = pVoid;
}
      


pymms::gui::GUIMessage::~GUIMessage(void)
{}

int pymms::gui::GUIMessage::getControlId() const
{
   return m_iControlId;
}
      
pymms::gui::MESSAGE pymms::gui::GUIMessage::getMessage() const
{
   return m_msgId;
}
      
void* pymms::gui::GUIMessage::getVoid() const
{
   return m_pVoid;
}
      
int pymms::gui::GUIMessage::getParam1() const
{
   return m_iParam1;
}
      
int pymms::gui::GUIMessage::getParam2() const
{
   return m_iParam2;
}
      
int pymms::gui::GUIMessage::getSenderId() const
{
   return m_iSenderId;
}
      
void pymms::gui::GUIMessage::setParam1(int iParam1)
{
   m_iParam1 = iParam1;
}
      
void pymms::gui::GUIMessage::setParam2(int iParam2)
{
   m_iParam2 = iParam2;
}
      
void pymms::gui::GUIMessage::setVoid(void* pVoid)
{
   m_pVoid = pVoid;
}
      
void pymms::gui::GUIMessage::setLabel(const string& strLabel)
{
   m_strLabel = strLabel;
}
      
string pymms::gui::GUIMessage::getLabel() const
{
   return m_strLabel;
}

void pymms::gui::GUIMessage::setAction(const string& strAction)
{
   m_strAction = strAction;
}

string pymms::gui::GUIMessage::getAction() const
{
   return m_strAction;
}
