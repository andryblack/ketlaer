//#define DLOG
#include "common.hpp"
#include "config.hpp"
#include "playlist_parser.hpp"
#include "wget_wrapper.hpp"
#include "fs++/general.h"


using std::string;
using std::vector;
namespace playlist_parser_private {
  bool is_downloadable(const string &plist_path){
    return (!plist_path.empty() &&
    (strncasecmp(plist_path.c_str(), "http://", 7) == 0 || strncasecmp(plist_path.c_str(), "ftp://", 6) == 0));
  }

  bool dump_file_to_string (const string& filename, string * buf_str){
    if (filesystem::getFileSize(filename) > 8 * 1024 * 1024) /* if the playlist is > 8MB, give up */
    return false;

    FILE *stream;
    if ( (stream = fopen(filename.c_str(), "r" )) == NULL )
      return false; /* can't open file */

  char linebuf[1025];

  *buf_str = "";
  int ret;

  while (true){
    ret = fread(linebuf, sizeof(char), 1024, stream);
    linebuf[ret] = 0;
    *buf_str += linebuf;
    if (ret < 1024){
      if (feof(stream))
        break;
      else if (ferror(stream)){
//        fprintf(stderr, OBJOUTPUT("Read error: %s \n"), s_strerror(errno).c_str());
        fclose(stream);
        return false;
      }
    }
  }
  fclose(stream);
  return true;
  }

  bool parse_pls(const string& playlist, vector<string> * urls){
    dprintf("\n\n\n=============================================================\n%s", playlist.c_str());
    if (!regex_tools::regex_str_match(playlist, "(\\[playlist\\])", true) ||
      !regex_tools::regex_str_match(playlist, "(Version=2)", true)){
      dprintf("File doesnt look like a PLS playlist\n");
      return false; /* this is an illegal pls playlist */
    }

    urls->clear();
    *urls = regex_tools::regex_matches(playlist, "^file[0-9]+=(.+?)$", true, true);
    return (!urls->empty());
  }
};
using namespace playlist_parser_private;

vector<string> PlaylistParser::resolve_playlist(const string& plist_path){
  vector<string> ret_vec;
  dprintf("Processing playlist: %s\n", plist_path.c_str());

  if (plist_path.empty()){
    ret_vec.push_back("");
    return ret_vec;
  }
  string playlist_buf;

  /* fetch file extension */
  string ext = regex_tools::extract_substr(plist_path, "\\.(.{2,4})$", false);

  if (ext.empty() || strcasecmp(ext.c_str(), "pls") != 0){
    dprintf("Playlist extension not supported: %s\n", ext.c_str());
    goto bail_out;
  }

  if (filesystem::isFile(plist_path)){
    if (!dump_file_to_string(plist_path, &playlist_buf)){
      dprintf("Couldn't open playlist: %s\n", plist_path.c_str());
      goto bail_out;
    }
  }
  else if (is_downloadable(plist_path)){
    if(!WgetWrapper::download(plist_path, playlist_buf)){
      dprintf("Couldn't download playlist: %s\n", plist_path.c_str());
      goto bail_out;
    }
  }
  else {
    dprintf("Couldn't open nor download playlist: %s\n", plist_path.c_str());
    goto bail_out;
  }

  if (!parse_pls(playlist_buf, &ret_vec))
    goto bail_out;

  return ret_vec;
bail_out:
  ret_vec.push_back(plist_path);
  return ret_vec;

}

