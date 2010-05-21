#include "playlist.hpp"
#include "libfspp.hpp"
#include "common.hpp"
#include "config.hpp"

using std::string;
using std::pair;
using std::vector;
using std::list;

pymms::player::PythonPlayList::PythonPlayList()
{
  m_iCurrentItem = 0;
}
    
pair<string, string> pymms::player::PythonPlayList::get(int iItem)
{
  if(!m_playListItems.empty())
  {
    if( (iItem > -1) && (iItem < m_playListItems.size()) ) 
      return m_playListItems[iItem];
    else
      return m_playListItems[m_iCurrentItem];
  }
  else
    return emptyMyPair;
}

void pymms::player::PythonPlayList::set(int iItem)
{
  if(!m_playListItems.empty())
  {
    if( (iItem > -1) && (iItem < m_playListItems.size()) )
      m_iCurrentItem = iItem;
  }
}

void pymms::player::PythonPlayList::add(pair<string, string> item)
{
  trim(item);

  if(item.second.empty())
    item.second = getTitle(item.first);

  m_playListItems.push_back(item);
}

void pymms::player::PythonPlayList::remove(int iItem)
{
  if( (iItem > -1) && (iItem < m_playListItems.size()) )
  {
    vector<pair<string, string> >::iterator iter = m_playListItems.begin();

    for(int i = 0; i < iItem; i++)
      iter++;

    m_playListItems.erase(iter);
  
    if(iItem == m_playListItems.size())
      m_iCurrentItem--;
  }  
}
    
void pymms::player::PythonPlayList::next()
{
  if(!m_playListItems.empty())
  {
    if( m_iCurrentItem < (m_playListItems.size() - 1) )
      m_iCurrentItem++;
    else
      m_iCurrentItem = 0;
  }
}
    
void pymms::player::PythonPlayList::prev()
{
  if(!m_playListItems.empty())
  {
    if(m_iCurrentItem > 0)
      m_iCurrentItem--;
    else
      m_iCurrentItem = m_playListItems.size() - 1;
  }
}

int pymms::player::PythonPlayList::size()
{
  return m_playListItems.size();
}

void pymms::player::PythonPlayList::trim(string& s)
{
  int iLength = s.length();

  while(iLength-- > 0 )
    if(s[iLength] > 47)
      break;                   

  s.resize(++iLength);
}

void pymms::player::PythonPlayList::trim(pair<string, string>& p)
{
  trim(p.first);
  trim(p.second);
}

string pymms::player::PythonPlayList::getTitle(const string& strPath) 
{
  Config* pConf = S_Config::get_instance();
  string strTitle = "";

  if(file_exists(strPath) && !isDirectory(strPath))
  {
    strTitle = strPath.substr(0, ( (strPath.rfind(".") != string::npos)?strPath.rfind("."):strPath.size()-1 )); 
    string::size_type pos = strTitle.rfind("/");

    if (pos != string::npos)
      strTitle = strTitle.substr(pos + 1);

    if (pConf->p_convert())
      strTitle = string_format::convert(strTitle);
  }

  return strTitle;
}

void pymms::player::PythonPlayList::clear()
{
  if(!m_playListItems.empty())
  {
    m_iCurrentItem = 0;
    m_playListItems.clear();
  }
}
    
void pymms::player::PythonPlayList::load(string strFile)
{
  string strLine, strPath, strTitle;
  std::ifstream ifReader;

  if(file_exists(strFile) && !isDirectory(strFile))
  {
    ifReader.open(strFile.c_str());
    if(!ifReader.fail())
    {
      while(std::getline(ifReader, strLine))
      {
        if(strLine.find("File", 0) != string::npos)
        {
          pair<string, string> item;

	  trim(strLine);
          item.first = strLine.substr(strLine.find("=", 0) + 1);
          m_playListItems.push_back(item);
        }

        if(strLine.find("Title", 0) != string::npos)
        {
	  trim(strLine);
	  if(!m_playListItems.empty())
            m_playListItems.back().second = strLine.substr(strLine.find("=", 0) + 1);
        }    
      }
    }

    ifReader.close();

    if(m_playListItems.empty())
    {
      ifReader.open(strFile.c_str());
      if(!ifReader.fail())
      {
        while(getline(ifReader, strLine)) 
        {
	  trim(strLine);

          string::size_type pos_comma = strLine.find(",");

          if(pos_comma != string::npos) 
          {
            int pos_semi = strLine.rfind(";");

            if (pos_semi != string::npos) 
            {
              strPath = strLine.substr(pos_comma+1, pos_semi-(pos_comma+1));
              strTitle = strLine.substr(pos_semi+1);
            }
            else
            {
	      strPath = strLine.substr(pos_comma+1);
	      strTitle = getTitle(strPath);
            }

	    pair<string, string> item;
            item.first = strPath;
	    item.second = strTitle;

	    m_playListItems.push_back(item);
          }
        }
      }

      ifReader.close();
    }
  }
}

void pymms::player::PythonPlayList::copy(PythonPlayList& playList)
{
  for(int i = 0; i < playList.size(); i++)
    m_playListItems.push_back(playList.m_playListItems[i]);
}

bool pymms::player::PythonPlayList::empty()
{
  return m_playListItems.empty();
}

bool pymms::player::PythonPlayList::isLastTrack()
{
  if(empty() || (m_iCurrentItem == (m_playListItems.size() - 1)))
    return true;
  else
    return false;
}

int pymms::player::PythonPlayList::getCurrentPos()
{
  return m_iCurrentItem;  
}

std::vector<std::pair<std::string, std::string> > pymms::player::PythonPlayList::toList()
{
  return m_playListItems;
}
