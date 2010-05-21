#include "guicontrolfactory.hpp"
#include "guibuttoncontrol.hpp"
#include "guilistcontrol.hpp"
#include "guiimagecontrol.hpp"
#include "guilabelcontrol.hpp"
#include "guibuttoncontrol.hpp"
#include "guitextboxcontrol.hpp" 
#include "guirectanglecontrol.hpp"
#include "guitextfieldcontrol.hpp"
#include "guiprogresscontrol.hpp"
#include "guiimagelistcontrol.hpp"

using std::string;

pymms::gui::GUIControlFactory::GUIControlFactory()
{
}

pymms::gui::GUIControlFactory::~GUIControlFactory()
{
}

bool pymms::gui::GUIControlFactory::getInt(const TiXmlNode* pRootNode, const char* strTag, int& iIntValue)
{
   TiXmlNode* pNode=pRootNode->FirstChild(strTag );
   if (!pNode) 
      return false;
	
   iIntValue = atoi(pNode->FirstChild()->Value());

   return true;
}

bool pymms::gui::GUIControlFactory::getBoolean(const TiXmlNode* pRootNode, const char* strTag, bool& bBoolValue)
{
   TiXmlNode* pNode=pRootNode->FirstChild(strTag );
   if (!pNode) 
      return false;

   string strEnabled = pNode->FirstChild()->Value();
   if (strEnabled=="false") 
      bBoolValue=false;
   else bBoolValue=true;
	
   return true;
}

bool pymms::gui::GUIControlFactory::getString(const TiXmlNode* pRootNode, const char* strTag, string& strStringValue)
{
   TiXmlNode* pNode=pRootNode->FirstChild(strTag );
   if (!pNode) 
      return false;
	
   pNode = pNode->FirstChild();
   if (pNode != NULL) 
      strStringValue=pNode->Value();
   else strStringValue = "";
	
   return true;
}

pymms::gui::GUIControl* pymms::gui::GUIControlFactory::create(const TiXmlNode* pControlNode)
{
   string strType;
   getString(pControlNode, "type", strType);

   int iId = 0;
   int iPosX = 0,iPosY = 0;
   int iWidth = 0, iHeight = 0;
   int iLeft = -1, iRight = -1, iPrev = -1, iNext = -1;
   int iLayer = 0;
   string strTexture = "";
   bool	bVisible = true;
   string strLabel = "", strText = "";
   string strFont = "Vera";
   string strTextColor = "0xffffff";
   int iTextOffset = 0;
   string strAlign = "left";
   string  strTextureFocus = "", strTextureNoFocus = "";
   string strTextColorFocus = "0xffffff";
   int iAlpha = 0;
   string strRectangleColor = "0xffffff";
   int iItemHeight = 30, iItemSpace = 0;
   int iRowHeight = 30, iRowSpace = 0;
   int iItemWidth = 30, iTextHeight = 20, iItemSpaceX = 10, iItemSpaceY = 1;
   int iRouteControl = -1;
   bool bListCenterX = false, bListCenterY = false;   
	
   /////////////////////////////////////////////////////////////////////////////
   // Read control properties from XML
   //

   if (!getInt(pControlNode, "id", iId))
   {
      return NULL; // NO id????
   }
  
   getInt(pControlNode, "posx", iPosX);
   getInt(pControlNode, "posy", iPosY);
   getInt(pControlNode, "width", iWidth);
   getInt(pControlNode, "height", iHeight);
   getInt(pControlNode, "onleft", iLeft);
   getInt(pControlNode, "onright", iRight);
   getInt(pControlNode, "onprev", iPrev);
   getInt(pControlNode, "onnext", iNext);
   getInt(pControlNode, "layer", iLayer);
   getInt(pControlNode, "routecontrol", iRouteControl);
   getString(pControlNode, "texture", strTexture);
   getBoolean(pControlNode, "visible", bVisible);
   getString(pControlNode, "label", strLabel);
   getString(pControlNode, "text", strText);
   getString(pControlNode, "font", strFont);
   getString(pControlNode, "textcolor", strTextColor);
   getInt(pControlNode, "textoffset", iTextOffset);
   getString(pControlNode, "align", strAlign);
   getString(pControlNode, "texturefocus", strTextureFocus);
   getString(pControlNode, "texturenofocus", strTextureNoFocus);
   getString(pControlNode, "textcolorfocus", strTextColorFocus);
   getInt(pControlNode, "alpha", iAlpha);
   getString(pControlNode, "rectanglecolor", strRectangleColor);
   getInt(pControlNode, "itemheight", iItemHeight);
   getInt(pControlNode, "itemspace", iItemSpace);
   getInt(pControlNode, "rowheight", iRowHeight);
   getInt(pControlNode, "rowspace", iRowSpace);
   getInt(pControlNode, "itemwidth", iItemWidth);
   getInt(pControlNode, "textheight", iTextHeight);
   getInt(pControlNode, "itemspacex", iItemSpaceX);
   getInt(pControlNode, "itemspacey", iItemSpaceY);
   getBoolean(pControlNode, "listcenterx", bListCenterX);
   getBoolean(pControlNode, "listcentery", bListCenterY);
 
   ////////////////////////////////////////////////////////////////////////////
   // Instantiate a new control using the properties gathered above
   //

   GUIControl* pControl = NULL;

   if(strType=="label")
   {
      pControl = new GUILabelControl(iPosX, iPosY, iWidth, iHeight, strLabel, 
				      iLayer, strFont, strTextColor, iTextOffset,
				      strAlign);
   }
   else if(strType=="button")
   {
      pControl = new GUIButtonControl(iPosX, iPosY, iWidth, iHeight, strLabel,
                                      iLayer, strTextureFocus, strTextureNoFocus,
				      strFont, strTextColor, strTextColorFocus, 
				      iTextOffset, strAlign);
   }
   else if(strType=="list")
   {
      pControl = new GUIListControl(iPosX, iPosY, iWidth, iHeight, iLayer, 
				    strTextureFocus, strTextureNoFocus,
                                    strFont, strTextColor, strTextColorFocus,
                                    iTextOffset, strAlign, iItemHeight, iItemSpace);
   }
   else if(strType=="textbox")
   {
      pControl = new GUITextBoxControl(iPosX, iPosY, iWidth, iHeight, iLayer,
                                    strFont, strTextColor, iRowHeight, iRowSpace);
   }
   else if(strType=="image")
   {
      pControl = new GUIImageControl(iPosX, iPosY, iWidth, iHeight, 
				    strTexture, iLayer);
   }
   else if(strType=="rectangle")
   {
      pControl = new GUIRectangleControl(iPosX, iPosY, iWidth, iHeight, iLayer,
                                    iAlpha, strRectangleColor);
   }
   else if(strType=="textfield")
   {
      pControl = new GUITextFieldControl(iPosX, iPosY, iWidth, iHeight, strText,
                                      iLayer, strFont, strTextColor, iTextOffset,
                                      strAlign);
   }
   else if(strType=="progress")
   {
      pControl = new GUIProgressControl(iPosX, iPosY, iWidth, iHeight,
                                      iLayer, strText, strFont, strTextColor,
                                      strAlign, strRectangleColor, iAlpha, iTextOffset);
   }
   else if(strType=="imagelist")
   {
      pControl = new GUIImageListControl(iPosX, iPosY, iWidth, iHeight, iLayer,
                                        strFont, strTextColor, strTextColorFocus, strAlign,
                                        iItemHeight, iItemWidth, iTextHeight, iItemSpaceX,
                                        iItemSpaceY, bListCenterX, bListCenterY);
   }

   if(pControl != NULL)
   {
      pControl->setId(iId);
      pControl->setVisible(bVisible);
      pControl->controlLeft(iLeft);
      pControl->controlRight(iRight);
      pControl->controlPrev(iPrev);
      pControl->controlNext(iNext);
      pControl->setRouteControl(iRouteControl);
   }

   return pControl;
}
