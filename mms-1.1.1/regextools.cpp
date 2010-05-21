#include "regextools.h"
#include <cassert>
#include <pcrecpp.h>
using std::string;
using std::vector;
using std::pair;
using pcrecpp::RE;

/* after a long try and error session, I have decided to use pcre regexes
 * in the cpp flavour (coded by google) */

#define PCRE_DEF_FLAGS PCRE_MULTILINE | PCRE_DOTALL


string regex_tools::extract_substr(const string &str, const string &regex, bool ignorecase){
  pcrecpp::RE_Options opt(PCRE_DEF_FLAGS);
  opt.set_caseless(ignorecase);

  pcrecpp::RE re(regex, opt);
  std::string ret;

  if (re.PartialMatch(str, &ret))
    return ret;
  else
    return "";
}

vector<string> regex_tools::regex_matches(const string& str, const string& regex, bool ignorecase, bool submatches){
  vector<string> svec;
  pcrecpp::RE_Options opt(PCRE_DEF_FLAGS);
  opt.set_caseless(ignorecase);
  pcrecpp::RE re(regex, opt);
  
  int n = re.NumberOfCapturingGroups();
  if (n <=0)
    return svec;

  else if(n > 10){
    fprintf(stderr, "Overflow: There are too many capturing groups\n");
    return svec;
  }
  /* sigh */


  string matches[10];
  const pcrecpp::Arg *args[10];
  
  int z = 0;
  pcrecpp::Arg arg0 = &matches[z];
  args[z++] = &arg0;
  
  pcrecpp::Arg arg1 = &matches[z];
  args[z++] = &arg1;

  pcrecpp::Arg arg2 = &matches[z];
  args[z++] = &arg2;

  pcrecpp::Arg arg3 = &matches[z];
  args[z++] = &arg3;

  pcrecpp::Arg arg4 = &matches[z];
  args[z++] = &arg4;

  pcrecpp::Arg arg5 = &matches[z];
  args[z++] = &arg5;
  
  pcrecpp::Arg arg6 = &matches[z];
  args[z++] = &arg6;

  pcrecpp::Arg arg7 = &matches[z];
  args[z++] = &arg7;

  pcrecpp::Arg arg8 = &matches[z];
  args[z++] = &arg8;

  pcrecpp::Arg arg9 = &matches[z];
  args[z++] = &arg9;
 
  pcrecpp::StringPiece input(str);

  int consumed;
  
  do {
    if (re.DoMatch(input, RE::UNANCHORED, &consumed, args, n)) {
      input.remove_prefix(consumed);
      for (int t = 0; t < n; t++){
        svec.push_back(matches[t]);
      }
    }
    
    else
        break;
  } while (submatches);
  return svec;
}

bool regex_tools::regex_str_match(const string& str, const string& regex, bool ignorecase){
  
  return (regex_str_pos(str, regex, ignorecase).second != static_cast<size_t>(-1)); 

}

string regex_tools::regex_replace(const string& str, const string& newtext, const string& regex,
    bool global, bool ignorecase){
  pcrecpp::RE_Options opt(PCRE_DEF_FLAGS);
  opt.set_caseless(ignorecase);
  pcrecpp::RE re(regex, opt);
  
  string str2 = str;
  
  if (global)
    re.GlobalReplace(newtext,&str2);
  else
    re.Replace(newtext,&str2);
  return str2;
}

pair<size_t, size_t> regex_tools::regex_str_pos(const std::string& str, const std::string& regex, bool ignorecase){
  
  pair<size_t, size_t> p;
  p.first = -1;
  p.second = -1;
  pcrecpp::RE_Options opt(PCRE_DEF_FLAGS);
  opt.set_caseless(ignorecase);
  pcrecpp::RE re(regex, opt);
  string _str;
  pcrecpp::Arg arg0 = &_str;
  const pcrecpp::Arg *args[1];
  args[0] = &arg0;
  
  //pcrecpp::StringPiece input(str);
  
  int consumed;
  if (re.DoMatch(str, RE::UNANCHORED, &consumed, args, 1)  && static_cast<size_t>(consumed) >= _str.size()){
    p.first = consumed - _str.size();
    p.second = _str.size();
  }
  return p;
  
}
