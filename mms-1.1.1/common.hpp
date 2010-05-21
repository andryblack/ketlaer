#ifndef COMMON_HPP
#define COMMON_HPP

#include "config.h"

#include "boost.hpp"

#include <string>
#include <list>
#include <vector>
#include <fcntl.h>
#include <errno.h>
#include "regextools.h"
//mypair
typedef std::pair<std::string, std::string> MyPair;
const MyPair emptyMyPair = MyPair("","");

MyPair check_type(const std::string& s, const std::list<MyPair>& temp);

namespace conv {
  int atoi(const std::string& s);
  std::string itos(int i);
  float atof(const std::string& s);
  std::string ftos(float f);
  bool stob(const std::string& s);
  std::string btos(bool b);
}

template<typename T>
bool list_contains(const std::list<T>& list, const T& element)
{
  foreach (const T& e, list)
    if (e == element)
      return true;

  return false;
}

template<typename T>
bool list_only_contains(const std::list<T>& list, const T& element)
{
  return list.size() == 1 && list.front() == element;
}

namespace string_format {

  std::string unique_folder_name(const std::string& folder);

  void format_to_size(std::string& s, const std::string& font, int size, bool dots, bool long_string = false);

  // if a string is longer than maxlen, where maxlen is in number of chars,
  // pps will transform the string into: the start, ..., the end of the string
  void pretty_print_string(std::string& s, int maxlen);

  std::string pretty_print_string_copy(const std::string& s, int maxlen);

  std::string pretty_pad_string(const std::string& s, int maxlen, const char& padchar);

  // escapes ' in a string
  std::string escape_db_string(const std::string& input);
  std::string escape_string(const std::string& input);

  // convertes // to /, useful for path prettyprinting
  std::string remove_doubles(const std::string& doubles);

  std::string lowercase(std::string s);

  // like lowercase, but it is not locale depndent
  // it accepts an UTF-8 string and returns an UTF-8 string
  std::string lowercase_utf(std::string s);

  /* convert an uri parameter according to the RFC 2396 specs. Do not use with the full URL, but
   * only with paramaters passed to a web server in an URL string */
  std::string sanitizeurl(const std::string &text);

  // trims spaces, tabs and newline off the start and end of a string
  std::string trim(std::string s);

  int calculate_string_width(const std::string& text, const std::string& font);
  // returns a pair of w, h
  std::pair<int, int> calculate_string_size(const std::string& text, const std::string& font);

  // please note that get_line must not contain a newline
  std::string get_line(const std::string& text, int &pos, int width, const std::string& font);

  // return the difference in the number of chars between two strings
  int difference(const std::string& lhs, const std::string& rhs);

  // returns the string with as many letters in common from input strings
  // beginning from the start of the string and ending when the first letter
  // doesn't match
  std::string greatest_common_substring(const std::string& lhs, const std::string& rhs);

  // convert a string in a given locale to the system locale format
  std::string convert_to_locale(const std::string& in, const std::string from_locale = "UTF-8");

  // convert a string from current locale to another one (default is UTF-8)
  std::string convert_from_locale(const std::string& in, const std::string to_locale = "UTF-8"); 

  // convert a string format locale
  std::string convert_locale(const std::string& in, const std::string from_locale, 
      const std::string to_locale = "UTF-8");

  // convert a filename or mp3 idtags to UTF-8 according to convert_locale and convert_mp3_locale parameters
  // in Config.
  std::string convert(const std::string& in, bool mp3_locale = false);

  // convert an HTML string to UTF-8
  std::string ConvertHTMLToAnsi(const std::string& strHTML, const std::string& fromlocale = "UTF-8");

  // parses an UTF-8 string and replaces invalid characters with '?'
  std::string ValidateU8(const std::string& str);

  // a printf like routine that returns a std::string
  std::string str_printf(const char *fmt, ...);

  //wrap a line of text  and returns a vector of strings, each one being a line whose size
  //fits in maxwidth
  int wrap_strings(std::string& s, const std::string& font, int maxwidth, std::vector<std::string>& v);
  
  /* replaces mblen() for utf-8 strings. This does not depend on LC_CTYPE */
  int utflen(const char *s, size_t n);
}

namespace time_helper {
  struct timespec compute_interval(unsigned int millisecs);
  struct timespec compute_interval(const struct timespec &ts, unsigned int millisecs);
}

unsigned int mmsSleep(unsigned int secs);

int mmsUsleep(unsigned int usecs);

enum AspectRatio { FOUR_THREE, SIXTEEN_NINE, SIXTEEN_TEN };

AspectRatio aspect_ratio(const int width, const int height);

int resolution_dependant_font_size(const int font_size_in_600, int width);
int extract_font_size(const std::string& str);

// rounds to nearest int
int round_to_int(double number);

int seconds_to_minutes(int time);
// returns time % 60 (the seconds left over)
int remaining_seconds(int time);

// can also check if a dir exists
bool file_exists(const std::string& filename);
bool dir_exists(const std::string& dir);
bool havemydir(const std::string &subdir, std::string *retpath=NULL);
bool recurse_mkdir(std::string homedir, std::string subdir, std::string *retpath);
// this function is recursive, remember the last slash
bool abs_recurse_mkdir(const std::string& dir, std::string *retpath=NULL);
void create_if_not_exists(const std::string& dir);

time_t modification_time(const std::string& filename);
namespace file_tools{
/* this function takes a dir name, parses it, check whether it contains files
 * whose extension match the ext string. It also has a maxdir parameter, i.e. it traverses
 * a maximum of maxdir directories.
 * If a directory contains a matching file or if there too many subdirs, it returns true */
  bool dir_contains_known_files(const std::string& dirpath, const std::string& ext, unsigned int maxdirs);

/* parses a list of "filetypes_p" and returns a string with all file extension to use with
 * dir_contains_known_files() */
  std::string create_ext_mask(const std::list<MyPair>& exts);

/* returns true if filename is a directory; returns false if it is not a directory or if an error occurred */
  bool is_directory(const std::string& filneame);

/* empties the internal file cache */
  void clean_cache();

/* removes 'filename' from the internal file cache */
  void remove_from_cache(std::string filename);

/* removes 'dirname' and everything inside it from cache */
  void remove_full_dir_from_cache(std::string dirname);

/* works pretty much like stat64() */
  int cache_stat(std::string path, struct stat64 *buf);

  /* all it does is call FExpand from the filesystem:: namespace in general.cc 
   * necessary for config parsing stuff. */
  std::string FExpand(const std::string& relative_path);

} /* end of namespace file_tools */

//string safe version of getenv()
std::string s_getenv(const std::string& envname);

//string safe version of strerror();
std::string s_strerror(int);
std::string s_strerror();

template<typename T>
inline T vector_lookup(const std::vector<T>& v, unsigned int pos)
{
#ifdef gcc_ver_major_3
  return v.at(pos);
#endif
#ifdef gcc_ver_major_2
  return v[pos];
#endif
}


/* use the following to exit mms if the latter has been fully initted */
/* "full_deinit == true" means the shutdown script, if it exists, is executed */
void clean_up(bool full_deinit = false);

class rect
{
public:
  int x, y, w, h;
  rect()
  {}
  rect(int x_, int y_, int w_, int h_)
    : x(x_), y(y_), w(w_), h(h_)
  {}

  int area()
  { return w*h; }

  bool operator > (const rect& r)
  { return x < r.x && y < r.y && x + w >= r.x + r.w && y + h >= r.y + r.h; }

  bool operator >= (const rect& r)
  { return x <= r.x && y <= r.y && x + w >= r.x + r.w && y + h >= r.y + r.h; }

  bool intersects(int x_, int y_)
  { return x <= x_ && x + w >= x_ && y <= y_ && y + h >= y_; }

  std::string to_string() const
  {
    return (conv::itos(x) + ":" + conv::itos(y) + ":" + conv::itos(w) + ":" + conv::itos(h));
  }
};

namespace filemutex {
#define  mutexname "/tmp/mms.lock"
  int lock();
}

/* creates the unused macro to remove a few warnings */
# define UNUSED(x) x __attribute__((unused)) 


/* use a replacer for printf( .....) and fprintf(stderr, ...) */
#if  defined( __GNUC__ ) && defined(DLOG)       
#define dprintf(fmt, args...)			\
  do {                                          \
    fprintf(stderr, fmt, ##args);		\
     } while(0)                                 
#else /* #if  defined( __GNUC__ ) && defined(DLOG) */                            
#define dprintf(fmt, args...)                   \
          do {}                                 \
while(0)                                        
#endif /* defined( __GNUC__ ) && defined(DLOG) */



#endif   /* COMMON_HPP */
