#ifndef OPTION_HPP
#define OPTION_HPP

#include "boost.hpp"

#include <vector>
#include <string>
#include <list>

class Option
{
public:
  typedef boost::function<void (void)> callback_func;
  callback_func callback;
  bool type; // used for global options
  std::string name;
  int pos;
  std::vector<std::string> values;

  // for saving
  std::string english_name;
  std::vector<std::string> english_values;

  Option(bool t, const std::string& n, const std::string& e_n, int p,
	 const std::vector<std::string>& v, const std::vector<std::string>& e_v,
	 callback_func cb = 0);

  virtual ~Option(){};
};

#endif
