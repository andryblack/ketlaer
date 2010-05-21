#ifndef GUILISTITEM
#define GUILISTITEM

#include <string>

namespace pymms {

namespace gui {

class GUIListItem
{
  private: 
    std::string m_strLabel, m_strLabel2, m_strThumbImage, m_strIconImage;
    std::string m_strFont, m_strRgb;

  public:
    GUIListItem() {m_strLabel = m_strLabel2 = m_strThumbImage = m_strIconImage = m_strFont = m_strRgb = "";}
    ~GUIListItem() {}
    void setLabel(const std::string& strLabel);
    std::string getLabel() const;
    void setLabel2(const std::string& strLabel2);
    std::string getLabel2() const;
    void setThumbImage(const std::string& strThumbImage);
    std::string getThumbImage() const;
    void setIconImage(const std::string& strIconImage);
    std::string getIconImage() const;
    void setFont(const std::string& strFont);
    std::string getFont() const;
    void setRgb(const std::string& strRgb);
    std::string getRgb() const;
    
};

}

}

#endif
