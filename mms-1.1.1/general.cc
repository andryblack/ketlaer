#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif
/* if DLOG is defined, dprintf() is defined as well */

/*
#define DLOG
*/

#include <cstdlib>

#include <fs++/general.h>

#include <sys/stat.h>
#include <unistd.h>
#include <sstream>

#include <map>
#include <string>
#include "common.hpp"

namespace filesystem{

  /* FExpand() resolves references to '/./', '/../' and extra '/' characters in
   * relative_path and returns the canonicalized absolute pathname. */

  string FExpand(const string& relative_path){
    std::string path = relative_path;
    std::string full_path;

    if (path.size() > 1 && path[0] == '~' &&  path[1] == '/')
      path = s_getenv("HOME") + path.substr(1);
    
    if (path.size() > 0 && path[0] == '/')
      full_path = path;
    
    else {
      char buf[4096];
      if (getcwd(buf, sizeof(buf)) == NULL)
	return ""; /* we give up */

      full_path = buf;
      if (path.size() > 0 && full_path.size() > 0 && full_path[full_path.size() - 1] != '/')
	full_path += '/';

      full_path += path;
    }

    if (full_path.empty())
      return ""; /* give up */

    /* Now comes the funny part. */

    std::string result;
    size_t slash1 = 0, slash2 = 0;

    for (slash2 = slash1; slash2 < full_path.size(); slash1 = slash2){
      while(full_path[++slash2] != '/' && slash2 < full_path.size());
      size_t dirlen = slash2 - slash1;
      if (dirlen == 1 ||(dirlen == 2 && full_path[slash1 + 1] == '.'))
	continue; /* skip '/.' */

      if (dirlen == 3 && full_path[slash1 + 1] == '.' && full_path[slash1 + 2] == '.'){
	if (result.size() > 0){ /* squeeze '/..' */
	  size_t last_slash = result.rfind("/");
	  if (last_slash == string::npos)
	    result = "";
	  else
	    result.erase(last_slash);
	}
	continue;
      }

      result += full_path.substr(slash1, dirlen);
    }

    return result;
  }

  bool 	isDirectory (const string& filename)
    {
      return file_tools::is_directory(filename);
    }

  bool isFile (const string& filename){
    struct stat64 statbuf ;
    if (file_tools::cache_stat(filename.c_str (), &statbuf) != 0)
      return false;

    return (S_ISREG(statbuf.st_mode));
  }


  const string 	getWorkingDirectory ()
    {
      string wd ;
      
#ifdef _GNU_SOURCE
      char * wd_cstr = get_current_dir_name () ;
      if (wd_cstr)
      {
	wd = wd_cstr ;
	free(wd_cstr) ;
      }
      else
	wd = "." ;


#else
      const int maxbufsize = 2048 ;
      char buf[maxbufsize] ;
      if (getcwd (buf, maxbufsize) != 0)
      {
	wd = buf ;
      }
      else
	wd = "." ;
      
#endif

      return wd ;
    }

  off_t getFileSize (const string& filename)
    {
      struct stat64 statbuf ;
      if (file_tools::cache_stat(filename.c_str (), &statbuf) != 0)
	return -1 ;	// could not stat file

      return statbuf.st_size ;
    }

  void 	getLine (std::istream& in, string& buf)
    {
      std::ostringstream tmp;
      tmp << in;
      buf = tmp.str();
    }

} // namespace filesystem
