
/* if DLOG is defined, dprintf() is defined as well */

//#define DLOG


#include "config.h"

#include "common.hpp"

#include "config.hpp"
#include "themes.hpp"
#include "simplefile.hpp"
#include "updater.hpp"
#include "print.hpp"
#include "renderer.hpp"
#include "plugins.hpp"
#include "theme.hpp"

#ifdef use_inotify
#include "notify.hpp"
#endif

#include <iostream>
#include <sstream>
#include <algorithm>
#include <cmath>

#include <unistd.h>
#ifdef use_inotify
#include "notify.hpp"
#endif

// file exists
#include <sys/stat.h>

#include "gettext.hpp"

// conversion
#ifdef use_iconv
#include <iconv.h>
#include <langinfo.h>
#endif
#include <errno.h>
#include <cassert>
#include <csignal>
#include <sys/wait.h>
#include "mms_signals.hpp"
#include <linux/unistd.h> /* for syscall(__NR_gettid) */
using std::string;
using std::list;

// check if the current file matches one of the types defined
// in the configuration file pairs
MyPair check_type(const string& s, const list<MyPair>& temp)
{
  string testtype;

  foreach (MyPair pair, temp) {
    testtype = string_format::lowercase(s.substr(s.size() - (pair.first.size() +1)));
    if (testtype == "." + pair.first)
      return pair;
  }
  return emptyMyPair;
}

int conv::atoi(const string& s)
{
  return ::atoi(s.c_str());
}

string conv::itos(int i)
{
  std::stringstream s;
  s << i;
  return s.str();
}

float conv::atof(const string& s)
{
  return ::atof(s.c_str());
}

string conv::ftos(float f)
{
  std::stringstream s;
  s << f;
  if (int(f) == f)
    s << ".0";
  return s.str();
}

bool conv::stob(const string& s)
{
  return !(s == "0" || s == "f" || s == "n" ||
	   s == "false" || s == "no" || s == "off" ||
	   s == gettext("false") || s == gettext("no") || s == gettext("off"));
}

string conv::btos(bool b)
{
  return (b) ? "true" : "false";
}

string string_format::unique_folder_name(const string& folder)
{
  string final = remove_doubles(folder);
  if (final[final.size()-1] != '/')
    final += '/';
  return final;
}

int string_format::wrap_strings(string& s, const string& font, int maxwidth,
				std::vector<std::string>& v )
{
  v.clear();
  //I am not sure it is the fastest way to do it....

  if (s.empty()) return 0;
  int start = 0;
  int count = 0;
  int lastspace = 0;
  std::string subs;
  s = trim(ValidateU8(s));
  while (count < s.size()){

    if (calculate_string_width(s.substr(start, count-start), font) > maxwidth){
      if (!lastspace) { //we don't have a space to use as wrap point
	if (--count < 1 || start == count) { //we roll-back 1 char... if we don't have at least 1
	  //char we exit...
	  lastspace = 0;
	  break;
	}
	subs = s.substr(start, count - start);
	v.push_back(trim(subs));

	start = count;
      }
      else {
	subs = s.substr(start,  lastspace - start);
	v.push_back(trim(subs));
	start = lastspace;
	lastspace = 0;
      }

    }

    else if (s[count] == ' '){
      lastspace = count;
    }

    else if (s[count] == '\n'){
      lastspace = 0;
      subs = s.substr(start, count-start);
      v.push_back(trim(subs));
      start = count + 1;
    }

    count++;
  }

  if (start < (count -1) ){
    subs = s.substr(start, count - start );
    v.push_back(trim(subs));
  }

  return v.size();

}

void string_format::format_to_size(string& s, const string& font, int size, bool dots,
				   bool long_string)
{
  
  s = ValidateU8(s);

  if(size <= 0){
    s = "";
    return;
  }

  int x = string_format::calculate_string_width(s, font);

  if (x > size) {
    string test = s;
    int chars = int(size/(double(x)/s.size()));

    if (chars == 0) {
      s = "";
      return;
    }

    assert(chars > 0);

    if (dots)
      pretty_print_string(test, chars);
    else
      test = test.substr(0, chars);

    x = calculate_string_width(test, font);

    while (x < int(size*0.97)) {

      if (chars == s.size())
	break;

      test = s;
      ++chars;

      if (dots)
	pretty_print_string(test, chars);
      else
	test = test.substr(0, chars);

      x = calculate_string_width(test, font);
    }

    while (x > int(size*1.03)) {

      if (chars == 0) {
	test = "";
	break;
      }

      test = s;
      --chars;

      if (dots)
	pretty_print_string(test, chars);
      else
	test = test.substr(0, chars);

      x = calculate_string_width(test, font);
    }

    if (long_string)
      s = test.substr(0, test.size()-2); // utf-8 fix
    else
      s = test;
  } else {
    if (dots)
      pretty_print_string(s, s.size());
  }
}

void string_format::pretty_print_string(string& s, int maxlen)
{
  int dif = s.length() - maxlen;
  if (dif > 0)
    s.replace((s.length()>>1)-(dif>>1)-1, dif+3, "...", 3);

  for (string::size_type i=0; (i=s.rfind("_")) != string::npos; s.replace(i,1," "))
    ;
}

string string_format::pretty_print_string_copy(const string& s, int maxlen)
{
  string result = s;
  pretty_print_string(result, maxlen);
  return result;
}

string string_format::pretty_pad_string(const string& s, int maxlen, const char& padchar)
{
  string result = ValidateU8(s);

  if (s.size() > maxlen)
    result = string_format::pretty_print_string_copy(s, maxlen);
  else if (s.size() < maxlen) {
    int numpads = int((maxlen-s.size())/2);
    string padstring = string(numpads, padchar);
    string restofline = string(maxlen-s.size()-numpads, padchar);

    result = padstring + s + restofline;
  }

  return result;
}

string string_format::escape_db_string(const string& input)
{
  string result;
  int size = input.size();

  for (int i = 0; i < size; ++i){
    switch(input[i]){
      case '\'':
	result +="''";
	break;
      case '%':
	result +="%%";
	break;
      default:
	result +=input[i];
    }
  }
  return result;
}

string string_format::escape_string(const string& input)
{
  string result = "";

  const char chars[] =", []()|&;<>'\"\\$";
  for (size_t t = 0; t < input.size(); ++t) {
      if (strchr(chars, input[t]) != NULL)
	result += '\\';
      result += input[t];
  }

  return result;
}

string string_format::sanitizeurl(const string &text){
/* Apply the percent encoding as per RCF 3986
 * Considering some web sites are not RCF 3986 compliant, 
 * before calling this routine you must ensure you're passing
 * a string formatted with the proper locale.
 * No check is performed here.
 */

  string str = text;
  string str2;
  /* ASCII characters we must percent encode as per RCF 3986
   * are:
   * reserved    = gen-delims / sub-delims
   * gen-delims  = ":" / "/" / "?" / "#" / "[" / "]" / "@"
   * sub-delims  = "!" / "$" / "&" / "'" / "(" / ")"
   *               / "*" / "+" / "," / ";" / "="
   * We also encode " and %.
   */

  const char reservedchars[] =":/?#[]@!$&'()*+,;=%\"";
  for (int t = 0; t < str.size(); t++){
    uint8_t c = str[t];
    if (c >= 0x80 || strchr(reservedchars, c) != NULL)
      str2 += string_format::str_printf("%%%1X", c);
    else if (c == 0x20)
      str2 +="+";
    else
      str2 += str[t];
  }
  return str2;
}

string string_format::remove_doubles(const string& doubles)
{
  string temp = doubles;

  string::size_type pos = temp.rfind("//");

  while (pos != string::npos) {
    temp.replace(pos, 2, "/");
    pos = temp.rfind("//");
  }

  return temp;
}

string string_format::lowercase(string s) {
  transform(s.begin(), s.end(), s.begin(), tolower);
  return s;
}

string string_format::lowercase_utf(string s){

  string str = "";
  bool wc = false;
  for (size_t t = 0; t < s.size(); t++){
    unsigned char c = static_cast<unsigned char>(s[t]);
    if (c >= 0x80){
      if (c >= 0xc0 && c <= 0xdf){
	wc = true;
      }
      else if (c<= 0xbf && wc){
	wc = false;
	c |= 0x20;
      }
      str+=c;
    }
    else
      str += tolower(s[t]);
  }
  return str;
}


string string_format::trim(string s)
{
  if (s.empty())
    return s;

  s = regex_tools::regex_replace(s, "", "[ \r\n\t]+$", true, false);
  return regex_tools::regex_replace(s, "", "^[ \r\n\t]+", true, false);
}


int string_format::calculate_string_width(const string& text, const string& font)
{
  int text_w = 0;

  if (text.size() > 0)
    S_Render::get_instance()->font_query_text_size(text + " ", font, &text_w, NULL);

  // during rendering we add black borders
  text_w += 2;

  return text_w;
}


std::string string_format::str_printf(const char *fmt, ...){
  string str;
  char *p, *np;
  va_list ap;
  int  n;
  size_t size, nextsize;
  size = 32;

  if (( p =static_cast<char*> (malloc(size))) == NULL){
    fprintf(stderr, "%s, Out of memory error\n", __func__); //warn user no matter what
    return "";
  }
  while (true){
    va_start(ap, fmt);
    n = vsnprintf (p, size, fmt, ap);
    va_end(ap);
    if (n == -1){// buffer is too small but we don't know how much we should allocate;
      nextsize = size*2;
    }

    else if (static_cast<unsigned int>(n) == size){ //buffer is too small because eol couldn't fit
      nextsize = size+3;
    }

    else if (static_cast<unsigned int>(n) == size -1){  //This is ambiguous. It may mean that the output string
      nextsize = size*2;                                      //was truncated to fit the buffer
    }

    else if (static_cast<unsigned int>(n) > size) {     //this is telling us how much the buffer should be
      nextsize = static_cast<unsigned int>(n) +3;  //3 bytes more just to stay on the safe side
    }

    else {//string fits in the buffer!!!!!!!
      break;
    }
    if ((np = static_cast<char*>(realloc (p, nextsize))) == NULL) {
      free(p);
      fprintf(stderr, "%s: Out of memory error\n",__func__);
      return "";
    } else {
      size = nextsize;
      p = np;
    }
  }
  str = p;
  free(p);
  return str;
}

std::pair<int, int> string_format::calculate_string_size(const string& text, const string& font)
{
  int text_w = 0;
  int text_h = 0;
  if (text.size() > 0)
    S_Render::get_instance()->font_query_text_size(text + " ", font, &text_w, &text_h);
  // during rendering we add black borders
  text_w += 2;
  text_h += 2;

  return std::make_pair(text_w, text_h);
}

int string_format::utflen(const char *s, size_t n){

  if (s[0] == 0)
    return 0;

  unsigned char z = static_cast<unsigned char>(s[0]);

  if (z < 0x80)
    return (n < 1) ? -1: 1;

  else if (z >= 0xf0)
    return (n < 4) ? -1: 4;

  else if (z >= 0xe0)
    return (n < 3) ? -1: 3;

  else if (z >= 0xc0) 
    return (n < 2) ? -1: 2;
  
  else /* 80-BF means Second, third, or fourth byte of a multi-byte sequence */
    return -1;

}


string string_format::get_line(const string& text, int &pos, int width, const string& font)
{
  assert(text.find('\n') == string::npos);
  bool padding = false;

  string desc = pos != 0 ? text.substr(pos) : text;
  string last_desc = desc;

  string_format::format_to_size(desc, font, width, false, true);

  // check the end of the string
  if (desc.size() != last_desc.size() && desc.size() < last_desc.size()
      && last_desc.size() >= 2) {
    char next_char = last_desc[desc.size()];
    char last_char = last_desc[desc.size()-1];
    if (next_char == ' ' || next_char == ',' || next_char == '.') {
      desc += next_char;
      if (last_desc[desc.size()] == ' ')
	desc += last_desc.substr(desc.size(), 1);
    } else if (last_char != ' ' && last_char != ',' && last_char != '.') {

      // if last word was small to not try to split it
      // or if you just get one char of a new word then don't try to
      // break the word onto multiple lines
      int first_space = desc.rfind(" ");

      if (first_space != string::npos) {

	string rest_of_string = last_desc.substr(desc.size());

	int last_space = rest_of_string.find(" ");
	int last_dot = rest_of_string.find(".");

	if (last_dot != string::npos) {
	  if (last_space == string::npos)
	    last_space = last_dot;
	  else
	    if (last_space > last_dot)
	      last_space = last_dot;
	} else if (last_space == string::npos)
	  last_space = 0;

	int last_word_size = desc.size() - (first_space+1) + last_space;

	if (last_word_size < 4 || first_space + 2 >= desc.size())
	  desc = last_desc.substr(0, first_space+1);
	else {
	  desc += "-";
	  padding = true;
	}

      }
    }
  }

  int size = desc.size();
  if (padding)
    --size;

  while (utflen(text.substr(pos+size).c_str(), 4) == -1) {

    // increase string size
    ++size;

    desc = text.substr(pos, size);
    if (padding)
      desc += "-";
  }

  pos += size;

  return desc;
}

int string_format::difference(const string& lhs, const string& rhs)
{
  int min_size = std::min(lhs.size(), rhs.size());

  int difference = 0;

  for (int i = 0; i < min_size; ++i)
    if (lhs[i] != rhs[i])
      ++difference;

  return difference + std::abs(int(lhs.size()-rhs.size()));
}

string string_format::greatest_common_substring(const string& lhs, const string& rhs)
{
  const int shortest = std::min(lhs.size(), rhs.size());

  string result = "";

  for (int i = 0; i < shortest; ++i)
    if (lhs[i] == rhs[i])
      result += lhs[i];
    else
      break;

  return result;
}

string string_format::convert_from_locale(const string& in, const string to_locale){

#if 0 /* how do I check nl_langinfo() is supported? */
    	char *syslocale = setlocale(LC_CTYPE, NULL);
   if (syslocale == NULL)
     return in;

   std::string str = syslocale;
   size_t t = str.find(".");
   if (t == string::npos || ++t >= str.size())
     return in;
   
   str = str.substr(t, str.find_last_not_of("@") - t + 1);
#endif

  char *syslocale = nl_langinfo(CODESET);
  if (syslocale == NULL)
    return in;
  std::string str = syslocale;
  return convert_locale(in, str, to_locale);
}


string string_format::convert_to_locale(const string& in, const string from_locale)
{
#if 0 /* how do I check nl_langinfo() is supported? */
  char *syslocale = setlocale(LC_CTYPE, NULL);
  if (syslocale == NULL)
    return in;

  std::string str = syslocale;
  size_t t = str.find(".");
  if (t == string::npos || ++t >= str.size())
    return in;

  str = str.substr(t, str.find_last_not_of("@") - t + 1);
#endif

  char *syslocale = nl_langinfo(CODESET);
  if (syslocale == NULL)
    return in;
  std::string str = syslocale;
  return convert_locale(in, from_locale, str);
}

string string_format::convert_locale(const string& in, const string from_locale, const string to_locale)
{
#ifdef use_iconv

  if (from_locale == to_locale || in.size() == 0)
    return in;
  const char *fromencoding = from_locale.c_str();
  const char *toencoding = to_locale.c_str(); /* to_locale defaults to "UTF-8", see common.hpp */
  size_t size = in.size()*2; /* initial output buffer is twice the size of the original string */
  int error = 0;

  iconv_t conversion = iconv_open(toencoding, fromencoding);
  if (conversion == reinterpret_cast<iconv_t>(-1)){
    error = errno;
    fprintf(stderr, "%s: %s\n", gettext("Failed to open iconv"), s_strerror(error).c_str());
    return in;
  }

  char *outbuf, *newbuf;
  if (( outbuf =static_cast<char*>(malloc(size))) == NULL){
    fprintf(stderr, "%s, Out of memory error\n", __func__); //warn user no matter what
    iconv_close(conversion);
    return "";
  }

  size_t outsize;
  
  while(true){
    outsize = size;
    char *inbuf = const_cast<char*>(in.c_str());
    size_t insize = in.size();
    char * outbuf_t = outbuf;
    if (iconv(conversion, &inbuf, &insize, &outbuf_t, &outsize) == -1){
      error = errno;
      if (error != E2BIG){
	iconv_close(conversion);
	free(outbuf);
	fprintf(stderr, "%s ---> %s\n* %s\n", from_locale.c_str(), to_locale.c_str(), in.c_str());
	fprintf(stderr, "%s: %s\n", gettext("String conversion failed"), s_strerror(error).c_str());
	return in;
      }
      else{ /* outbuf is not large enough */
	size = size + 1024;
	if (( newbuf = static_cast<char*>(realloc(outbuf,size))) == NULL){
	  free(outbuf);
	  fprintf(stderr, "%s, Out of memory error\n", __func__); //warn user no matter what
	  iconv_close(conversion);
	  return "";
	}
	outbuf = newbuf;
	iconv(conversion, NULL, NULL, &outbuf, &outsize); /* reset inconv state */
      }
    }
    else
      /* conversion succedeed */
      break;
  }
  string str(outbuf, size - outsize);
  free(outbuf);
  iconv_close(conversion);
  /* fprintf(stderr, "%s ---> %s\n* %s\n* %s\n", from_locale.c_str(), to_locale.c_str(), in.c_str(), str.c_str()); */
  return str;
#else
  return in;
#endif

}

string string_format::convert(const string& in, bool mp3_locale)
{
  std::string inp;

  Config *conf = S_Config::get_instance();

  if (!conf->p_convert())
    return ValidateU8(convert_from_locale(in));

  string from = conf->p_convert_locale();
  if (mp3_locale && conf->p_convert_mp3_locale()!=""){
    from = conf->p_convert_mp3_locale();
  }

  inp = convert_locale(in, from);
   return ValidateU8(inp);
}

string string_format::ConvertHTMLToAnsi(const string& strHTML, const string &fromlocale)
{
  /* fromlocale defaults to "UTF-8" */
  std::pair<size_t, size_t> ppair;
  std::string str = strHTML;
  size_t pos = 0;

  while(true){
    //ppair = regex_tools::regex_str_pos(str.substr(pos), "(&.{1,8};)", false);
    ppair = regex_tools::regex_str_pos(str.substr(pos), "(&.+?;)", false);
    if (ppair.second == static_cast<size_t>(-1))
      break;
    std::string sub = str.substr(pos+ppair.first, ppair.second);
    
    /* Not all the entity names are recognized, only the most common ones */
    if (sub == "&amp;")
      str.replace(pos+ppair.first, ppair.second, "&");
    else if (sub == "&nbsp;")
      str.replace(pos+ppair.first, ppair.second, " ");
    else if (sub == "&lt;")
      str.replace(pos+ppair.first, ppair.second, "<");
    else if (sub == "&gt;")
      str.replace(pos+ppair.first, ppair.second, ">");
    else if (sub == "&quot;")
      str.replace(pos+ppair.first, ppair.second, "\"");
    else{
      string str2 = regex_tools::extract_substr(sub,"&#([0-9]{1,5});", false);
      if (str2.size() > 0){
	uint32_t num = atoi(str2.c_str());
	if (num > 65535){ /* illegal entity value, skip */
	  str.replace(pos+ppair.first, ppair.second, "");
	}
	char chars[3];
	if (num > 255) { /* Two byte entity num */
	  chars[0] = static_cast<char>(num / 256);
	  chars[1] = static_cast<char>(num & 0xff);
	  chars[2] = 0x00;
	  std::string strchars = chars;
	  str.replace(pos+ppair.first, ppair.second, chars);
	  pos +=2;
	  continue;
	}
	else{
	  chars[0] = static_cast<char>(num & 0xff);
	  chars[1] = 0x00;
	  str.replace(pos+ppair.first, ppair.second, chars);
	}
      }
      else
	str.replace(pos+ppair.first, ppair.second, "");
    }
    pos++;
  }
 return convert_locale(str, fromlocale, "UTF-8");
}

std::string string_format::ValidateU8(const std::string& str){

  string str2, str3;
  size_t multi;

  for (size_t t = 0; t < str.size(); t++){
    unsigned char c = str[t];
    if (c < 0x80){
      str2 += str[t];
      continue;
    }

    else if ((c >= 0x80 && c < 0xc2) || c > 0xf4){ /* misplaced second, third, or fourth byte of a multi-byte sequence,
								     or illegal character */
      str2 +="?";
      continue;
    }

    else if (c >= 0xc2 && c < 0xe0) /* start of 2-byte sequence */
      multi = 2;

    else if (c >= 0xe0 && c < 0xf0) /* start of 3-byte sequence */
      multi = 3;

    else
      multi = 4;

    str3 = c;
    t++;

    for (; (--multi > 0 && t < str.size()); t++){
      c = str[t];
      if (c >= 0x80 && c < 0xc0)
	str3 += str[t];
      else if (c < 0x80){ /* unexpected end of multi-byte sequence */
	break;
      }

      else {/* illegal character */
	break;
      }
    }

    t--;

    if (multi == 0)
      str2 += str3;
    else
      str2 += "?";
  } 
  return str2;
}

int round_to_int(double number)
{
  return int(number + 0.5);
}

int seconds_to_minutes(int time)
{
  return static_cast<int>(floor(time/60));
}

int remaining_seconds(int time)
{
  return (time%60);
}

bool file_exists(const string& filename)
{
  struct stat64 file_stat;
  return stat64(filename.c_str(), &file_stat) == 0;
}

time_t modification_time(const string& filename)
{
  struct stat64 file_stat;
  stat64(filename.c_str(), &file_stat);
  return file_stat.st_mtime;
}

bool dir_exists(const string& dir)
{
 struct stat64 _stat;

 if ((stat64(dir.c_str(), &_stat)) == -1)
   return false; //directory not present

 else if (!S_ISDIR(_stat.st_mode))
   return false; // it's not a directory

 return true;
}

void create_if_not_exists(const string& dir)
{
  if (!file_exists(dir)) {

    string::size_type loc = dir.find("/", 0); // root

    while ((loc = dir.find("/", loc+1))) {

      if (!file_exists(dir.substr(0, loc+1)))
	mkdir(dir.substr(0, loc+1).c_str(), 0777);

      if (loc == string::npos)
	break;
    }
  }
}

bool _mkdir(const string& dir)
{
  struct stat64  _stat;

  if ((stat64(dir.c_str(), &_stat)) == -1){
    //directory is not present, create it
    if (mkdir(dir.c_str(), 0700) == -1) {
      print_critical(gettext("Could not create directory ") + dir);
      return false;
    }
  }
  else if (!S_ISDIR(_stat.st_mode)) { //it's already there, but is it a directory?
    //no, it's not
    print_critical(gettext("Internal error, please report this"));
    print_critical(gettext("File is not a directory: ") + dir);

    return false;
  }

  return true;
}


std::string s_strerror(int errn){
  char * ptr = strerror(errn);
  if (ptr == NULL)
    return "Unknown";
  else return std::string(ptr);
}

std::string s_strerror(){
  return s_strerror(errno);
}


std::string s_getenv(const std::string & envname){
  char * ptr = getenv(envname.c_str());
  if (ptr == NULL)
    return "";
  else return std::string(ptr);
}

bool havemydir(const string& subdir, string *retpath)
{
  //get home dir...
  string homedir;
  if ((homedir = s_getenv("HOME")).empty())
    //falls back to /var/lib/
    homedir="/var/lib/mms/";
  else
    homedir += "/.mms/";

  return recurse_mkdir(homedir, subdir, retpath);
}

bool recurse_mkdir(string basedir, string subdir, string *retpath)
{
  if (retpath != NULL)
    retpath->clear();

  if (!_mkdir(basedir))
    return false;

  if (!subdir.empty()){
    if (subdir[subdir.size()-1] != '/')
      subdir += "/";
    if (subdir[0] == '/')
      subdir.erase(0, 1);
    size_t pos1=0;
    while ((pos1 = subdir.find("/")) != string::npos){
      //we now create the subdir(s) if necessary
      basedir += subdir.substr(0, ++pos1);
      subdir.erase(0, pos1);
      if(!_mkdir(basedir))
	return false;
    }
  }

  if(retpath != NULL)
    retpath->assign(basedir);

  return true;
}

bool abs_recurse_mkdir(const string& dir, string *retpath)
{
  return recurse_mkdir("/", dir, retpath);
}

AspectRatio aspect_ratio(const int width, const int height)
{
  if ((width/16.0)/(height/10) == 1)
    return SIXTEEN_TEN;
  else if ((width/16.0)/(height/9) == 1)
    return SIXTEEN_NINE;
  else if ((width/4.0)/(height/3) == 1)
    return FOUR_THREE;
  else {
    float ratio = (width*1.0)/height;
    if (ratio < 1.47)
      return FOUR_THREE;
    else if ((1.47 <= ratio) and (ratio < 1.69))
      return SIXTEEN_TEN;
    else
      return SIXTEEN_NINE;
  }
}

int resolution_dependant_font_size(const int font_size_in_600, int height)
{
  return round_to_int(font_size_in_600 * ((std::min(height, 1200))/(1.0*600)));
}

int extract_font_size(const std::string& str)
{
  return conv::atoi(str.substr(str.rfind('/')+1));
}

int filemutex::lock(){
  struct flock lock;
  memset(&lock, 0, sizeof (struct flock));
  lock.l_type = F_WRLCK;
  lock.l_whence = SEEK_SET;
  int pid = -1;
  int fdlock;
  if((fdlock = open(mutexname, O_WRONLY|O_CREAT, 0666)) == -1){// to prevent security issues, filemask should be 0600, 
	                                                       // but it is probably overkill in this context.
							       // MMS can be executed as many users and the lock file is
							       // system wide, but I'd consider unlikely the risk your wife DOSes
							       // your instance of MMS... after all it would be easier (and more efficient)
							       // for her to total your HTPC with a big hammer.
    print_critical(gettext("Unable to open/create the file lock!!!"), "FileMutex");
    return -1;
  }

  fchmod(fdlock, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH); // force permissions
	  								     // no need to check error code

  if(fcntl(fdlock, F_SETLK, &lock) <0) {
    //A lock is already set....investigate it
    //memset(&lock, 0, sizeof (struct flock));
    if((fcntl(fdlock, F_GETLK, &lock)< 0)|| lock.l_type== F_UNLCK){
      /* Maybe the lock has been released after the 1st fcntl() call, or 
         maybe it's just bad luck, but we don't actually care, we 
	 report a lock is in place nonetheless
      */
      close(fdlock);
      return -1;
    }
    else
    {
      pid = lock.l_pid;
      close(fdlock);
      return pid;
    }
  }

  /*
   * We succesfully placed our lock, this means we can safely assume there are no other
   * instances of MMS (or lib tools) running
   */
  return 0;
  /* We can't close the file or the lock gets released at the same time
   * File is not deleted when MMS exits, but this is harmless.
  */

}

int _nanosleep(struct timespec rem)
{
  int ret;

  while((ret=nanosleep(&rem, &rem)) == -1 && errno == EINTR)
    ; /* don't let signals interrupt the loop */

  if (!ret)
    return rem.tv_sec;
  else
    return 0;
}


unsigned int mmsSleep(unsigned int secs){

  struct timespec rem;
  rem.tv_sec = secs;
  rem.tv_nsec = 0;

  return _nanosleep(rem);
}

int mmsUsleep(unsigned int usecs){
  struct timespec rem;
  rem.tv_sec = usecs/1000000L;
  rem.tv_nsec = (usecs % 1000000L) * 1000;

  if (_nanosleep(rem) == 0)
    return 0;
  else
    return -1;
}


pthread_mutex_t mmsSignals::singleton_mutex = PTHREAD_MUTEX_INITIALIZER;
mmsSignals::mmsSignals(){
  process_sigs = false;
  do_die = false;
  deinitted = false;
  sigset_t signal_set;
  sigemptyset( &signal_set );

  /* block the following signals */

  /* SIGHUP */
  sigaddset( &signal_set, SIGHUP);

  /* SIGTERM */
  sigaddset( &signal_set, SIGTERM);

  /* SIGCHLD */
  sigaddset( &signal_set, SIGCHLD);

  /* SIGUSR1 */
  sigaddset( &signal_set, SIGUSR1);

  pthread_sigmask( SIG_BLOCK, &signal_set, NULL );

  /* we don't block SIGINT 
   * otherwise if you attach a debugger, like gdb, to MMS, you would
   * encounter the following problem:
   * http://bugzilla.kernel.org/show_bug.cgi?id=9039
   *
   * But we're smarter than the kernel, so let's employ a little hack */


  /* Add a crash handler */
  struct sigaction new_action;
  new_action.sa_sigaction = (void (*)(int, siginfo_t*, void* ))(crash_handler);
  sigemptyset (&new_action.sa_mask);
  new_action.sa_flags = SA_RESETHAND | SA_SIGINFO;

  sigaction (SIGSEGV, &new_action, NULL); /* Segmentation fault */

  sigaction (SIGFPE, &new_action, NULL); /* Division by zero or other arithmetic exception */

  sigaction (SIGILL, &new_action, NULL); /* Illegal instruction */

  sigaction (SIGABRT, &new_action, NULL); /* execution aborted, like due to a failed assert() */

  /* Now the little hack we talked about
   * we add a signal handler for SIGINT.
   * Which does only one thing: raise a SIGHUP
   */

  memset(&new_action, 0, sizeof(new_action));
  new_action.sa_handler = (void (*)(int ))(sigint_handler);
  sigemptyset (&new_action.sa_mask);
  new_action.sa_flags = 0;

  sigaction (SIGINT, &new_action, NULL);

  /* launch signal thread */

  pthread_create(&thread_loop, NULL, (void *(*)(void *))(pre_run), this);
  pthread_detach(thread_loop);
}

mmsSignals::~mmsSignals(){

};

void *mmsSignals::pre_run(void * ptr){
  mmsSignals * thisobj = reinterpret_cast<mmsSignals*>(ptr);
  thisobj->run();
  return 0;
}

void mmsSignals::init(){
  process_sigs = true;
}

void mmsSignals::run(){

  sigset_t signal_set;
  int sig;

  while (!do_die){
    sigfillset(&signal_set);
    sigwait(&signal_set, &sig);
    /* fprintf(stderr, "got signal %d\n", sig); */
    if (sig == SIGCHLD){
      while (waitpid(-1, NULL, WNOHANG) > 0){}; /* garbage collector for zombies */
    }

    else if (process_sigs && (sig == SIGINT || sig == SIGTERM || sig == SIGHUP))
      break;
  }
  deinit_mms(do_die);
}


void mmsSignals::deinit_mms(bool full_deinit){

  Config *conf = S_Config::get_instance();
#ifdef use_inotify
  fprintf(stderr,"Terminating inotify...");
  S_Notify::get_instance()->stop();
  fprintf(stderr,"Done.\n");
#endif

  Render *render = S_Render::get_instance();
  fprintf(stderr, "Terminating background tasks... ");
  S_BackgroundUpdater::get_instance()->quit();
  S_ScreenUpdater::get_instance()->quit();
  fprintf(stderr, "Done.\nTerminating plugins..."); 
  if (render->device){
    render->prepare_new_image();
    render->clear();
    render->draw_and_release("exit");

    mmsSleep(1);
  }

#ifdef mms_debug
  cerr << "Lock input and one output plugin " << endl;
#endif

  Plugins *plugins = S_Plugins::get_instance();
  plugins->delete_plugins();

  // save theme
  fprintf(stderr, "Done.\n");

  S_Theme::get_instance()->save();

  fprintf(stderr, "Cleaning up child processes (if any)\n");
  /* Put us in a new group so that we can safely kill
  * all other processes spawned by mms without committing
  * a suicide  In other words we sort-of daemonize mms */

  pid_t grp = getpgid(0);

  pid_t pid = fork();

  if (pid > 0){
    fprintf(stderr,"Exiting...\n");
    deinitted = true;

    _exit(0);
  }
  else if (pid < 0){
    fprintf(stderr, "\nSomething went wrong, I couldn't fork()\n");
  }
  else { /* we're the child process!!! */

    if (setsid() < 0) {
      fprintf(stderr, "\nsetsid() failed with error %s", s_strerror(errno).c_str()); 
    } else{
      kill(-grp, SIGTERM);

      int z = 0;

      while (kill (-grp,0) == 0 && ++z < 500)
	mmsUsleep(10*1000);

      /* fprintf(stderr, "%d is sending SIGKILL to process group %d\n", getpid(), grp); */
      kill(-grp, SIGKILL);
    }
  }

  if (!conf->p_shutdown_script().empty() && full_deinit){

    system(conf->p_shutdown_script().c_str());
  }

  exit(0);
}

void mmsSignals::clean_up(bool full_deinit){
  if (full_deinit){
    do_die = true;
    kill(0, SIGUSR1); /* wakes signal thread */
  }
  else {
    kill(0, SIGHUP);
  }
}

inline void crash_code_identifier(siginfo_t *siginfo){
  fprintf(stderr, "\nFurther details: ");
  if (siginfo->si_code == SI_USER){
    fprintf(stderr, "Signal generated by process %d.\n", siginfo->si_pid);
    return;
  }
  else if (siginfo->si_code == SI_KERNEL){
    fprintf(stderr, "Signal generated by kernel.\n");
    return;
  }
#ifdef SI_TKILL
  else if (siginfo->si_code == SI_TKILL){
    fprintf(stderr, "Signal generated by tkill(), tgkill(), raise(), assert().\n");
    return;
  }
#endif
  else if(siginfo->si_code < 0){ /* SI_SIGIO, SI_ASYNCIO, SI_TIMER, etc. are not interesting */
    fprintf(stderr, "None.\n");
    return;
  }
  else if (siginfo->si_signo == SIGILL)
   switch(siginfo->si_code){
     case ILL_ILLOPC:
       fprintf(stderr, "Illegal opcode.\n");
       break;
     case ILL_ILLOPN:
       fprintf(stderr, "Illegal operand.\n");
       break;
     case ILL_ILLADR:
       fprintf(stderr, "Illegal addressing mode.\n");
       break;
     case ILL_ILLTRP:
       fprintf(stderr, "Illegal trap.\n");
       break;
     case ILL_PRVOPC:
       fprintf(stderr, "Privileged opcode exception.\n");
       break;
     case ILL_PRVREG:
       fprintf(stderr, "Privileged register exception.\n");
       break;
     case ILL_COPROC:
       fprintf(stderr, "Coprocessor error.\n");
       break;
     case ILL_BADSTK:
       fprintf(stderr, "Internal stack error.\n");
       break;
   }

  else if (siginfo->si_signo == SIGFPE)
    switch(siginfo->si_code){
      case FPE_INTDIV:
	fprintf(stderr, "Integer divide by zero.\n");
	break;
      case FPE_INTOVF:
	fprintf(stderr, "Integer overflow.\n");
	break;
      case FPE_FLTDIV:
	fprintf(stderr, "Floating point divide by zero.\n");
	break;
      case FPE_FLTOVF:
	fprintf(stderr, "Floating point overflow.\n");
	break;
      case FPE_FLTUND:
	fprintf(stderr, "Floating point underflow.\n");
	break;
      case FPE_FLTRES:
	fprintf(stderr, "Floating point inexact result.\n");
	break;
      case FPE_FLTSUB:
	fprintf(stderr, "Subscript out of range.\n");
	break;
   }
  else if (siginfo->si_signo == SIGSEGV)
    switch(siginfo->si_code){
      case SEGV_MAPERR:
	fprintf(stderr, "Address not mapped to object.\n");
	break;
      case SEGV_ACCERR:
	fprintf(stderr, "Invalid permissions for mapped object.\n");
	break;
    }
}

void mmsSignals::crash_handler(int signum, siginfo_t * siginfo, void *context){
  fprintf(stderr, "\n\n!!!! CRASH !!!!\n");
  fprintf(stderr, "This is MMS v. 1.1.1 %s, ", VERSION);
  fprintf(stderr, "build options:\n%s\n", MMS_CONFIG_OPTIONS);
  if (siginfo){
    fprintf(stderr, "--------------------------------------------------\n");
#ifdef __NR_gettid
    fprintf(stderr, "Thread id:             %lu\n",syscall(__NR_gettid)) ;
#endif
    fprintf(stderr, "Memory address:        0x%lx\n", reinterpret_cast<unsigned long>(siginfo->si_addr));
    fprintf(stderr, "Signal code:           0x%x\n", siginfo->si_code);
    fprintf(stderr, "Signal error number:   0x%x\n", siginfo->si_errno);
    fprintf(stderr, "--------------------------------------------------\n");
  }
  fprintf(stderr, "MMS has encountered an unrecoverable error ( %s ) ", strsignal(signum));
  fprintf(stderr, "and is about to end.");
  crash_code_identifier(siginfo); /* this prints further details about the signal */
  fprintf(stderr, "\nPlease post the information above along with a detailed bug-report\n");
  fprintf(stderr, "to our bug database: http://bugs.mymediasystem.org/\n");
  fprintf(stderr, "Also, unless you set your system to do otherwise, a core dump is about to be\n");
  fprintf(stderr, "created. Load it with gdb as follows:\n");
  fprintf(stderr, "  $ gdb mms core.%d\n", getpid());
  fprintf(stderr, "Then create a full backtrace as follows:\n");
  fprintf(stderr, "  (gdb) thread apply all bt\n");
  fprintf(stderr, "Copy and add the backtrace to the bug report.\n");
  fprintf(stderr, "For more information about reporting bugs for MMS, please refer to:\n");
  fprintf(stderr, "http://wiki.mymediasystem.org/wiki/index.php/Bug_reports\n\n");
  fprintf(stderr, "So Long, and Thanks for All the Fish\n\n\n");
}

void mmsSignals::sigint_handler(int ){
  kill(0, SIGHUP);
}

void clean_up(bool full_deinit){
  S_mmsSignals::get_instance()->clean_up(full_deinit);
}

/* file_tools APIs */
typedef std::map<std::string, struct stat64> t_file_cache;
typedef std::map<std::string, struct stat64>::iterator t_cache_it;


/* To protect the integrity of the file cache, we use a mutex and other stuff */
static pthread_mutex_t file_cache_mutex = PTHREAD_MUTEX_INITIALIZER;

/* this is our cache */
t_file_cache file_cache;

inline int enter_critical_section(){ 
  int oldstate;
  pthread_mutex_lock(&file_cache_mutex);
  if (pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &oldstate) != 0) /* Disable all cancellation points, 
									 we don't want the calling thread to exit while
									 mutex is locked
									 TODO: do the same to other mutexes in other parts of code? 
									 */
    return -1;
  return oldstate;
}

inline void exit_critical_section(int oldstate){
  int _unused; /* posix doesn't clearly state that pthread_setcancelstate(value, NULL) is valid, so we use a dummy variable */
  if (oldstate != -1)
    pthread_setcancelstate(oldstate, &_unused);
  pthread_mutex_unlock(&file_cache_mutex);
}


int file_tools::cache_stat(string path, struct stat64 *buf){
  path = filesystem::FExpand(path); /* use canonicalized paths */
  t_cache_it it;
  int oldstate = enter_critical_section();
  if((it = file_cache.find(path))!= file_cache.end()){
    *buf = it->second;
    exit_critical_section(oldstate);
    return 0;
  }

  exit_critical_section(oldstate);
  if (stat64(path.c_str(), buf) == 0){
    oldstate = enter_critical_section();
    file_cache[path] = *buf;
    exit_critical_section(oldstate);
    dprintf("(%d) Added '%s' to the file cache\n", file_cache.size(), path.c_str());
    return 0;
  }
  return -1;
}

bool file_tools::is_directory(const std::string& filename){
  struct stat64 statbuf;
  if (cache_stat (filename.c_str(), &statbuf) < 0){
    return false;
  }
  return S_ISDIR (statbuf.st_mode);
}

bool file_tools::dir_contains_known_files(const std::string& dirpath, const std::string& exts, unsigned int maxdirs){
  std::vector<string> dir_paths; /* using a vector of directory paths allows us to avoid recursion 
				    and prevents the risk of running out of file descriptors */
  dir_paths.push_back(filesystem::FExpand(dirpath));

  unsigned int t = 0;
  DIR *dir;
  struct dirent *entry;
  struct stat64 statbuf;

  while (t < maxdirs && t < dir_paths.size()){
    /* open the currently processed directory */
    std::string curdir = dir_paths[t];
    if ((dir = opendir(curdir.c_str()))== NULL){
      /* unexpected error, complain and return true */
      fprintf(stderr,"Can't open dir '%s'\n", curdir.c_str());
      return true;
    }
    while((entry = readdir(dir))){
      /* skip '.' and '..' entries */
      if ((strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0))
	continue;

      string str = curdir + "/" + entry->d_name;
      if (cache_stat (str.c_str(), &statbuf) < 0){
	int err = errno;
	fprintf(stderr,"Can't stat '%s': %s (errno %d)\n", str.c_str(), s_strerror(err).c_str(), errno);
	continue;
      }

      if (S_ISDIR (statbuf.st_mode)){ /* another dir: add it to the vector */
	 dir_paths.push_back(str);
	 if (dir_paths.size() > maxdirs){ /* we have too many directories, just end now and return true */
	   closedir(dir);
	   return true;
	 }
	 continue;
      }

      else if (S_ISREG (statbuf.st_mode)){ /* this is a regular file, check its extension */
	std::string ext_str = entry->d_name;
	size_t st = ext_str.rfind(".");
	if (st == string::npos)
	  continue;
	ext_str = ext_str.substr(st);
	if (ext_str.size() > 1){
	  ext_str += ".";
	  if ( strcasestr(exts.c_str(), ext_str.c_str()) != NULL ){
	    /* we have a matching file, return true */
	    closedir(dir);
	    return true;
	  }
	}
      }
    }
    t++;
    closedir(dir);
  }
  return (t >= maxdirs); /* if we exceeded the max number of directories return true, otherwise false, meaning 
			    there's nothing interesting in dirpath */
}

std::string file_tools::create_ext_mask(const list<MyPair>& exts){
  string str;
  foreach (MyPair pair, exts)
    str += "." + pair.first + ".";
  return str;
}

/* empties the internal file cache */
void file_tools::clean_cache(){
  int oldstate = enter_critical_section();
  file_cache.clear();
  exit_critical_section(oldstate);
}

/* removes 'filename' from the internal file cache */
void file_tools::remove_from_cache(std::string filename){
  filename = filesystem::FExpand(filename);
  int oldstate = enter_critical_section();
  if (!filename.empty()){
    file_cache.erase(filename);
    dprintf("Removed '%s' from the file cache\n", filename.c_str()); 
  }
  exit_critical_section(oldstate);
}

void file_tools::remove_full_dir_from_cache(std::string dirname){
  dirname = filesystem::FExpand(dirname);
  dprintf("remove_full_dir_from_cache() called with parameter '%s'\n", dirname.c_str());
  int oldstate = enter_critical_section();
  if (!dirname.empty()){
     file_cache.erase(dirname);
     dprintf( "Removed '%s' from the file cache\n", dirname.c_str());
     dirname +="/";

     for (t_cache_it iter = file_cache.begin(); iter != file_cache.end();)
       if (iter->first.find(dirname) == 0){
	 dprintf("Removed '%s' from the file cache\n", iter->first.c_str());
	 file_cache.erase(iter++);
       }
       else
	 iter++;
  }
  exit_critical_section(oldstate);
}

string file_tools::FExpand(const string& relative_path){
  return filesystem::FExpand(relative_path);
}

struct timespec time_helper::compute_interval(unsigned int millisecs){
  struct timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);
  uint64_t ttimer = static_cast<uint64_t>(ts.tv_sec)*1000 + ts.tv_nsec/1000000 + millisecs;
  ts.tv_sec = ttimer/1000;
  ts.tv_nsec = (ttimer%1000)*1000000;
  return ts;
}

struct timespec time_helper::compute_interval(const struct timespec &ts, unsigned int millisecs){
  struct timespec ts2;
  uint64_t ttimer = static_cast<uint64_t>(ts.tv_sec)*1000 + ts.tv_nsec/1000000 + millisecs;
  ts2.tv_sec = ttimer/1000;
  ts2.tv_nsec = (ttimer%1000)*1000000;
  return ts2;
}

