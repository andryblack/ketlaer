#ifndef GUIMESSAGE_HPP
#define GUIMESSAGE_HPP

#include <string>

namespace pymms {

namespace gui {

enum MESSAGE
{ 
   GUI_MSG_SETFOCUS,      // set focus to control param1=up/down/left/right
   GUI_MSG_LOSTFOCUS,     // control lost focus
   GUI_MSG_VISIBLE,       // set control visible
   GUI_MSG_ISVISIBLE,	  // ask control if visible
   GUI_MSG_HIDDEN,        // set control hidden
   GUI_MSG_LABEL_ADD,     // for controls supporting more then 1 label
   GUI_MSG_LABEL_RESET,   // clear all labels of a control
   GUI_MSG_LABEL_SET,     // set the label of a control
   GUI_MSG_ITEM_SELECTED, // ask control 2 return the selected item
   GUI_MSG_CLICKED,	  // control clicked
   GUI_MSG_TOUCH,	  // control touched (mouse click)
   GUI_MSG_ACTION	  // control route action
};

class GUIMessage
{
   private:
      MESSAGE m_msgId;
      int m_iSenderId, m_iControlId, m_iParam1, m_iParam2;
      void* m_pVoid;
      std::string m_strLabel;
      std::string m_strAction;

   public:
      GUIMessage(MESSAGE msgId, int iSenderId, int iControlId, 
		  int iParam1=0, int iParam2=0, void* pVoid=NULL);
      virtual ~GUIMessage(void);
      int getControlId() const;
      MESSAGE getMessage() const;
      void* getVoid() const;
      int getParam1() const;
      int getParam2() const;
      int getSenderId() const;
      void setParam1(int iParam1);
      void setParam2(int iParam2);
      void setVoid(void* pVoid);
      void setLabel(const std::string& strLabel);
      std::string getLabel() const;
      void setAction(const std::string& strAction);
      std::string getAction() const;
};

}

}

#endif
