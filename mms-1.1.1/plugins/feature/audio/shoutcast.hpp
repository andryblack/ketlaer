#ifndef SHOUT_HPP
#define SHOUT_HPP

#include <string>
#include <list>
#include <iostream>

std::string itostr(int i, int width = 0, char fill = '0', std::_Ios_Fmtflags align = std::ios_base::right);

struct shout_info
{
  std::string id;
  std::string url;
  std::string name;
  //std::string genre;
  //std::string nowplaying;
  //std::string listeners;
  //std::string bitrate;
  //std::string mediatype;
};

typedef std::pair<std::string, std::string> Genre;
typedef std::pair<std::string, std::string> TopStation;

class Shoutdownloader
{
  public:
    static std::list<Genre> fetch_genrelist();
    static std::list<TopStation> fetch_toplist(std::string genreUrl);
    static std::pair<std::string, std::string> fetch_playlist(std::string playlistname);
};

class Shoutcast
{
  private:
    static Shoutcast *_instance;
    std::list<std::pair<std::string, std::string> > _genrelist;
  protected:
    Shoutcast();
  public:
    std::list<std::pair<std::string, std::string> > get_genrelist();
    std::list<std::pair<std::string, std::string> > get_toplist(std::string genreUrl);
    std::pair<std::string, std::string> get_playlist(std::string playlistname);
    static Shoutcast *get_instance();
};

#endif
