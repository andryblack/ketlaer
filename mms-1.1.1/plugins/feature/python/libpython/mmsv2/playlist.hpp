#ifndef PYTHONPLAYLIST_HPP
#define PYTHONPLAYLIST_HPP

#include <iostream>
#include <fstream>
#include <vector>
#include <list>

namespace pymms {

namespace player {

class PythonPlayList
{
  private:
    std::vector<std::pair<std::string, std::string> > m_playListItems;
    int m_iCurrentItem;

    void trim(std::string& s);
    void trim(std::pair<std::string, std::string>& p);
    std::string getTitle(const std::string& strPath);

  public:
    PythonPlayList();
    virtual ~PythonPlayList() {}
    std::pair<std::string, std::string> get(int iItem = -1);
    void set(int iItem);
    void add(std::pair<std::string, std::string> item);
    void remove(int iItem);
    void next();
    void prev();
    int size();
    int getCurrentPos();
    void clear();
    void load(std::string strFile);  
    void copy(PythonPlayList& playList);
    bool empty();
    bool isLastTrack();
    std::vector<std::pair<std::string, std::string> > toList();
};

}

}

#endif
