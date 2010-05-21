#include "lyrics_fetch_helper.hpp"

#include "wget_wrapper.hpp"


using std::string;

bool LFH_lyricwiki::fetch() 
{
  string url = "http://lyricwiki.org/api.php?fmt=xml&artist=" +
    string_format::sanitizeurl(artist) +
    "&song=" +
    string_format::sanitizeurl(title);

  std::string str_contents;

  if (!WgetWrapper::download(url, str_contents))
    return false;
  str_contents = string_format::ConvertHTMLToAnsi(regex_tools::extract_substr(str_contents, "<lyrics>(.+?)</lyrics>", true));
  if (str_contents.size() == 0 || str_contents == "Not found")
    return false;
  str_lyric = string_format::trim(string_format::ValidateU8(str_contents));
  return true;
}
