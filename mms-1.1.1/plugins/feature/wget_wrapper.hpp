#ifndef WGET_WRAPPER_HPP
#define WGET_WRAPPER_HPP

#include <string>

namespace WgetWrapper
{
  /* inits the wget process and returns the file descriptor
   * of the read-end pipe of wget (< 1 == error) */
  int set_download(const std::string& url);


  /* downloads a web page to a temporary file and returns its
   * stream pointer. This file is automatically deleted once
   * you're done with it (you must use fclose()) or when mms exits.
   * (error == NULL) */
  FILE * download_to_temp(const std::string& url);


  /* downloads a web page and copies
   * it to a string. It returns false for error */
//  bool download_to_str(const std::string& url, std::string& contents); 
  
  
  /* downloads a web page into a file */
  
  void download_to_file(const std::string& url, const std::string& file);
  bool download(const std::string& url, std::string& contents);

}

#endif
