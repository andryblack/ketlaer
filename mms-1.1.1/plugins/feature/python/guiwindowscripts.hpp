#ifndef GUIWINDOWSCRIPTS_HPP
#define GUIWINDOWSCRIPTS_HPP

#include "simplefile.hpp"
#include "guiwindow.hpp"
#include "guilistitem.hpp"

#include <string>
#include <vector>

using namespace pymms::gui;

class FileItem : public GUIListItem
{
  private:
    Simplefile m_fileInfo;

  public:
    explicit FileItem(const Simplefile& fileInfo)
    {
      m_fileInfo = fileInfo;
      setLabel(fileInfo.name);
      setLabel2(fileInfo.type);
    }

    Simplefile getFileInfo() const
    {
      return m_fileInfo;
    }
};

class GUIWindowScripts : public GUIWindow
{
  private:
    std::vector<std::string> m_vecHistoryDirs;
    std::vector<FileItem*> m_vecFileItems;
    void readDir(const std::string& strArgv);

  public: 
    GUIWindowScripts();
    ~GUIWindowScripts();
    bool onAction(const std::string& strAction);
    bool load(const std::string& strFileName);
    void clear();
};
    
#endif

