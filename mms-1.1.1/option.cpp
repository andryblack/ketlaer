#include "option.hpp"

#include "global.hpp"

using std::list;
using std::vector;
using std::string;

Option::Option(bool t, const string& n, const string& e_n, int p,
	       const vector<string>& v, const vector<string>& e_v, callback_func cb)
  : callback(cb), type(t), name(n), pos(p), english_name(e_n)
{
  if (v.size() != e_v.size()) { // buggy code
    print_critical(gettext("The size of the translated and the english option lists does not match: ") + e_n, "OPTION");
    exit(0);
  }

  values = v;
  english_values = e_v;
}

