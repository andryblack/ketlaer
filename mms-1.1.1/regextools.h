#ifndef REGEXTOOLS_H_
#define REGEXTOOLS_H_
#include <string>
#include <list>
#include <vector>
#include <fcntl.h>
#include <errno.h>


namespace regex_tools{
/* replace text in str to pattern according to the regular expression regex */
std::string regex_replace(const std::string& str, const std::string& newtext, const std::string& regexp,
    bool global = true, bool ignorecase = false);

/* parses a given string using the provided regular expression and returns a std::vector<string>
 * containing all (substring or main, according to bool submatches)  matches */
std::vector<std::string> regex_matches(const std::string& str, const std::string& regex, bool ignorecase = false,
    bool submatches = true);

/* parses a given string using the provided regular expression and returns the position of the first match in string 
 * it returns std::pair<-1, -1> if there's no match */
std::pair<size_t, size_t> regex_str_pos(const std::string& str, const std::string& regex, bool ignorecase = false);
 

/* check if the regular expression matches anything in string */
bool regex_str_match(const std::string& str, const std::string& regex, bool ignorecase = false);

/* extract a substring from str according to the regex expression 
 * example: str = "viva la pizza margherita";
 * extract_substr(str, "viva la (.+?) margherita", true) will return "pizza" */
std::string extract_substr(const std::string &str, const std::string &regex, bool ignorecase);


}
#endif /*REGEXTOOLS_H_*/


