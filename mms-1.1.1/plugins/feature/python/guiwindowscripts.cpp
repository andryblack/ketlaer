#include "Python.h"
#include <stdio.h>
#include <unistd.h>
#include "libfspp.hpp"
#include "config.hpp"
#include "global.hpp"
#include "guiwindowscripts.hpp"
#include "mmspython.hpp"
#include "guiwindowmanager.hpp"
#include "python_config.hpp"

#define GUILISTCONTROL 10
#define GUIIMAGECONTROL 2
#define GUILABELCONTROL 3

using std::string;
using std::vector;

GUIWindowScripts::GUIWindowScripts()
{}

GUIWindowScripts::~GUIWindowScripts()
{
  clear();
}

void GUIWindowScripts::clear()
{
  GUIWindow::clear();

  m_vecFileItems.clear();
  m_vecHistoryDirs.clear();
}

void GUIWindowScripts::readDir(const string& strArgv)
{
  Config* pConf = S_Config::get_instance();
  PythonConfig* pPythonConf = S_PythonConfig::get_instance();
  FileItem* pItem = NULL;

  GUIMessage resetMsg(GUI_MSG_LABEL_RESET, getId(), GUILISTCONTROL);
  onMessage(resetMsg);

  for (vector<FileItem*>::iterator i = m_vecFileItems.begin(),
          end = m_vecFileItems.end(); i != end; i++) {
      delete (*i);
  }  
  m_vecFileItems.clear();
  
  for(file_iterator<file_t, default_order> i (strArgv); i != i.end (); i.advance(false))
  {
    pItem = NULL;
    bool bScriptDir = false;
    string strPath = filesystem::FExpand(i->getName());

    if(isDirectory(strPath))
    {
      Simplefile s;
      string strThumbPath;
      string strIconPath;

      for(file_iterator<file_t, default_order> j (strPath); j != j.end (); j.advance(false))
      {
	string strName = filesystem::FExpand(j->getName());
	string strRealName = strName.substr(strPath[strPath.size()-1] == '/' ?
                                  strPath.size() : strPath.size() + 1);
	const MyPair fileType = check_type(strName, pConf->p_filetypes_p());

	if( !isDirectory(strName) && ((strRealName == "main.py") || (strRealName == "default.py")) )
 	{
	  s.name = strPath.substr(strArgv[strArgv.size()-1] == '/' ?
                                  strArgv.size() : strArgv.size() + 1);
	  if (pConf->p_convert())
            s.name = string_format::convert(s.name);
	  s.path = strName;
          s.type = "py";

	  bScriptDir = true;
	}

	if( !isDirectory(strName) && (fileType != emptyMyPair) && (strRealName == ("thumb." + fileType.second)) )
	  strThumbPath = strName;

        if( !isDirectory(strName) && (fileType != emptyMyPair) && (strRealName == ("icon." + fileType.second)) )
          strIconPath = strName;
      }

      if(bScriptDir)
      {
        pItem = new FileItem(s);
        pItem->setThumbImage((strThumbPath.empty()?"python/exec.png":strThumbPath));
	pItem->setIconImage((strIconPath.empty()?"python/exec.png":strIconPath));

        m_vecFileItems.push_back(pItem);
      }
      else
      {
        Simplefile d;
        d.name = strPath.substr(strArgv[strArgv.size()-1] == '/' ? 
				  strArgv.size() : strArgv.size() + 1);
        if (pConf->p_convert())
          d.name = string_format::convert(d.name);
        d.path = strPath;
        d.type = "dir";

        pItem = new FileItem(d);
        pItem->setThumbImage("python/dir.png");
        pItem->setIconImage("python/dir.png");

        m_vecFileItems.push_back(pItem);
      }
    } 
    else
    {
      // file
      const MyPair fileType = check_type(strPath, pPythonConf->p_filetypes_s());

      if (fileType != emptyMyPair)
      {
	Simplefile s;
	s.name = strPath.substr(strArgv[strArgv.size()-1] == '/' ?
                                  strArgv.size() : strArgv.size() + 1);
	s.name = s.name.substr(0, s.name.size()-((fileType.first).size()+1));

        if (pConf->p_convert())
          s.name = string_format::convert(s.name);
        s.path = strPath;
        s.type = "py";

        pItem = new FileItem(s);
        pItem->setThumbImage("python/exec.png");
	pItem->setIconImage("python/exec.png");

        m_vecFileItems.push_back(pItem);
      }
    }

    if(pItem)
    {
      GUIMessage addMsg(GUI_MSG_LABEL_ADD, getId(),GUILISTCONTROL, 0, 0, (void*)pItem);
      onMessage(addMsg);
    } 
  }
}

bool GUIWindowScripts::onAction(const string& strAction)
{
  GUIWindow::onAction(strAction);

  if(strAction == "back")
  {
     if(m_vecHistoryDirs.size() > 1)
     {
        m_vecHistoryDirs.erase(m_vecHistoryDirs.begin());
        readDir(m_vecHistoryDirs.front());
     }
     else
	return false;
  }

  if(strAction == "action")
  {
     if(getFocus() == GUIIMAGECONTROL)
     {
       if(m_vecHistoryDirs.size() > 1)
       {
         m_vecHistoryDirs.erase(m_vecHistoryDirs.begin());
         readDir(m_vecHistoryDirs.front());
       }
       else
        return false;
     }

     if(getFocus() == GUILABELCONTROL)
     {
       S_Global::get_instance()->toggle_playlist();
     }

     if(getFocus() == GUILISTCONTROL)
     {
       if(!m_vecFileItems.empty())
       {
         GUIMessage posMsg(GUI_MSG_ITEM_SELECTED, getId(), GUILISTCONTROL);
         onMessage(posMsg);

         if(m_vecFileItems[posMsg.getParam1()]->getFileInfo().type == "dir")
         { 
            m_vecHistoryDirs.insert(m_vecHistoryDirs.begin(), 
			   m_vecFileItems[posMsg.getParam1()]->getFileInfo().path);
            readDir(m_vecHistoryDirs.front());
         }
         else
         {
            pymms::MMSPython* pMmsPython = pymms::S_MMSPython::get_instance();
            pMmsPython->evalFile(m_vecFileItems[posMsg.getParam1()]->getFileInfo().path);
         }
       }
    }						  	
  }

  S_WindowManager::get_instance()->render();

  return true;
}

bool GUIWindowScripts::load(const string& strFileName)
{
   PythonConfig* pPythonConf = S_PythonConfig::get_instance();
   bool bLoadable = false;

   if (file_exists(pPythonConf->p_script_dir()))
      bLoadable = GUIWindow::load(strFileName);
   else
     std::cerr << "WARNING could not load: " << strFileName << std::endl;
   
   if (bLoadable)
   {
      m_vecHistoryDirs.insert(m_vecHistoryDirs.begin(), pPythonConf->p_script_dir());
      readDir(m_vecHistoryDirs.front());
   }

   return bLoadable;
}
