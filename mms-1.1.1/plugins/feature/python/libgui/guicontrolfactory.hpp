#ifndef GUICONTROLFACTORY_HPP
#define GUICONTROLFACTORY_HPP

#include <string>
#include <cc++/thread.h>
#include "renderer.hpp"
#include "guicontrol.hpp"
#include "tinyxml.h"

namespace pymms {

namespace gui {

#include "tinyxml/tinyxml.h"

class GUIControlFactory
{
   public:
      GUIControlFactory();
      virtual ~GUIControlFactory();
      GUIControl* create(const TiXmlNode* pControlNode);
   private:
      bool getInt(const TiXmlNode* pRootNode, const char* strTag, int& iIntValue);
      bool getBoolean(const TiXmlNode* pRootNode, const char* strTag, bool& bBoolValue);
      bool getString(const TiXmlNode* pRootNode, const char* strTag, std::string& strStringValue);
};

    
}

}

#endif
