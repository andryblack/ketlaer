#ifndef LASTFM_HPP
#define LASTFM_HPP

#include <vector>
#include <string>

#include "cpp_sqlitepp.hpp"
#include <cc++/thread.h>

class LastFM
{
public:
  static void end_of_song(const std::string& artist, const std::string& album,
			  const std::string& title, int length, int timeplayed);

  // it would be a good idea to check if genres already exists before calling
  // this function as its rather slow
  static std::vector<std::string> lookup_and_insert_genres(const std::string& artist, 
							   SQLDatabase& db, ost::Mutex& db_mutex);
  class Track
  {
  public:
    std::string artist;
    std::string title;
  };

  static std::vector<Track> lookup_similar_tracks(const std::string& artist, const std::string& track);
  static std::vector<Track> lookup_loved_tracks(const std::string& lover);
  static std::vector<Track> lookup_top_tracks(const std::string& username);

private:
  
  static std::vector<Track> extract_tracks(std::string& content);

  static std::vector<std::string> lookup_genres(const std::string& artist);
  static void insert_genres(const std::vector<std::string>& genres, const std::string& artist,
			    SQLDatabase& db, ost::Mutex& db_mutex);

  static std::string domain;
  static std::string key;
};

#endif
