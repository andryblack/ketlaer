#include "guilistitem.hpp"

using std::string;

void pymms::gui::GUIListItem::setLabel(const string& strLabel)
{
  m_strLabel = strLabel;
}

string pymms::gui::GUIListItem::getLabel() const
{
  return m_strLabel;
}

void pymms::gui::GUIListItem::setLabel2(const string& strLabel2)
{
  m_strLabel2 = strLabel2;
}

string pymms::gui::GUIListItem::getLabel2() const
{
  return m_strLabel2;
}

void pymms::gui::GUIListItem::setThumbImage(const string& strThumbImage)
{
  m_strThumbImage = strThumbImage;
}

string pymms::gui::GUIListItem::getThumbImage() const
{
  return m_strThumbImage;
}

void pymms::gui::GUIListItem::setIconImage(const string& strIconImage)
{
  m_strIconImage = strIconImage;
}

string pymms::gui::GUIListItem::getIconImage() const
{
  return m_strIconImage;
}

void pymms::gui::GUIListItem::setFont(const string& strFont)
{
  m_strFont = strFont;
}

void pymms::gui::GUIListItem::setRgb(const string& strRgb)
{
  m_strRgb = strRgb;
}

string pymms::gui::GUIListItem::getFont() const
{
  return m_strFont;
}

string pymms::gui::GUIListItem::getRgb() const
{
  return m_strRgb;
}
