#ifndef GUIUTIL_HPP
#define GUIUTIL_HPP

#include <string>
#include <ctime>

namespace pymms {

namespace gui {

class GUIInputUtil
{
  private:
    std::string m_strLastInput;
    std::time_t m_tOld;
    std::time_t m_tNew;
    int m_iCount1;
    int m_iCount2;

  public:
    GUIInputUtil();
    std::pair<bool,bool> convertInput(std::string& strInput);
    void inputHandled(const std::string& strInput);
    static bool isSearchCommand(const std::string& strCommand);
    static bool isLircSearchKey(const std::string& strKey);
};

}

}

#endif
