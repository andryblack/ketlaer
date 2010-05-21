#include "guiutil.hpp"
#include "config.hpp"
#include "input.hpp"
#include "common.hpp"
#include "remote.hpp"

using std::string;
using std::pair;

static const string lircFullLayout[] = {"yz_0", "ab1", "cd2", "ef3", "ghi4",
                           "jkl5", "mno6", "pqr7", "stu8", "vwx9"};

static const string lircMobileLayout[] = {"0", "1", "abc2", "def3", "ghi4", "jkl5",
                           "mnop6", "qrst7", "uvw8", "xyz_9"};

static const char* lircKeys[] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", 0};
static const char* searchCommands[] = {"prev", "next", "right", "left", "second_action", "erase", "action", "back", 0};
static const string specialChars = " *_@%&?";

pymms::gui::GUIInputUtil::GUIInputUtil()
{
  m_tOld = std::time(0);
  m_tNew = std::time(0);
  m_iCount1 = 0;
  m_iCount2 = 0;
  m_strLastInput = "";
}

pair<bool,bool> pymms::gui::GUIInputUtil::convertInput(string& strInput)
{
  m_tNew = std::time(0);
  time_t tDiff = m_tNew - m_tOld;
  m_tOld = m_tNew;

  pair<bool,bool> bConvert;
  bConvert.first = false;
  bConvert.second = false;

  if( (list_contains(S_Config::get_instance()->p_input(), string("lirc"))) && isLircSearchKey(strInput) )
  {
    if((strInput == m_strLastInput) && (tDiff < 2))
      m_iCount1++;
    else
      m_iCount1 = 0;

    char cInput;
    if(S_RemoteConfig::get_instance()->p_remote_layout() == "full_layout")
      cInput = lircFullLayout[conv::atoi(strInput)][m_iCount1 % lircFullLayout[conv::atoi(strInput)].length()];
    else
      cInput = lircMobileLayout[conv::atoi(strInput)][m_iCount1 % lircMobileLayout[conv::atoi(strInput)].length()];

    m_strLastInput = strInput;
    strInput = "";
    strInput += cInput;

    if(m_iCount1)
    {
      bConvert.first = true;
      bConvert.second = true;
    }
    else
    {
      bConvert.first = true;
      bConvert.second = false;
    }
  }
  else if( (list_contains(S_Config::get_instance()->p_input(), string("keyboard"))) && strInput.length() == 1 )
  {
    bConvert.first = true;
    bConvert.second = false;

    m_strLastInput = strInput;
  }
  else if(strInput == "second_action")
  {
    if((m_strLastInput == strInput) && (tDiff < 2))
      m_iCount2++;
    else
      m_iCount2 = 0;

    m_strLastInput = strInput;
    strInput = "";
    strInput += specialChars[m_iCount2 % specialChars.length()];

    if(m_iCount2)
    {
      bConvert.first = true;
      bConvert.second = true;
    }
    else
    {
      bConvert.first = true;
      bConvert.second = false;
    }
  }
    
  return bConvert;
}

void pymms::gui::GUIInputUtil::inputHandled(const string& strInput)
{
  m_strLastInput = strInput;
}

bool pymms::gui::GUIInputUtil::isSearchCommand(const std::string& strCommand)
{
    for(int i = 0; searchCommands[i]; i++)
      if(searchCommands[i] == strCommand)
        return true;

    return false;
}

bool pymms::gui::GUIInputUtil::isLircSearchKey(const std::string& strKey)
{
    for(int i = 0; lircKeys[i]; i++)
      if(lircKeys[i] == strKey)
        return true;

    return false;
}
