#include "remote.hpp"

#include "remote_config.hpp"

#include "global.hpp"

using std::vector;
using std::string;

Remote::Remote()
  : last_keypress(0)
{
  remote_conf = S_RemoteConfig::get_instance();
  remote_conf->parse_configuration_file(S_Config::get_instance()->p_homedir());

  std::string all_chars;

  if (remote_conf->p_remote_layout() == "mobile")
    all_chars = "1|abc2|def3|ghi4|jkl5|mno6|pqrs7|tuv8|wxyz9| 0";
  else
    all_chars = "ab1|cd2|ef3|ghi4|jkl5|mno6|pqr7|stu8|vwx9|yz 0";

  S_Global::get_instance()->set_valid_keys(all_chars);
}

vector<vector<string> > Remote::valid_keys()
{
  string all_chars;

  if (remote_conf->p_remote_layout() == "mobile")
    all_chars = "1|abc2|def3|ghi4|jkl5|mno6|pqrs7|tuv8|wxyz9| 0";
  else
    all_chars = "ab1|cd2|ef3|ghi4|jkl5|mno6|pqr7|stu8|vwx9|yz 0";

  return parse_string(all_chars);
}

vector<vector<string> > Remote::parse_string(string all_chars)
{
  vector<vector<string> > all_keys;

  string::size_type seperator_pos = 0;

  do {
    seperator_pos = all_chars.find('|');

    string chars = all_chars.substr(0, seperator_pos);

    vector<string> tmp;

    for (int i = 0; i < chars.size(); ++i)
      tmp.push_back(chars.substr(i, 1));

    all_keys.push_back(tmp);

    if (seperator_pos != string::npos)
      all_chars = all_chars.substr(seperator_pos+1);

  } while (seperator_pos != string::npos);

  return all_keys;
}
