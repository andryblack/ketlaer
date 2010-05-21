#ifndef PLAYLIST_PARSER_HPP
#define PLAYLIST_PARSER_HPP

#include <string>
#include <vector>

namespace PlaylistParser {
  std::vector<std::string> resolve_playlist(const std::string& plist_path);
}

#endif //PLAYLIST_PARSER_HPP

