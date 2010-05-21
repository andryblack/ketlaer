#include "genericplayer.hpp"
#include "simple_movie.hpp"
#include "common-feature.hpp"
#include "movie_config.hpp"

#ifdef use_nls
#include <langinfo.h>
#include <locale.h>
#endif

#include <fstream>

using std::string;
using std::vector;

goptstruct::goptstruct (char s_c, bool t, const string& n, const string& e_n, int p,
			const vector<string>& v, const vector<string>& e_v)
{
  shortcut = s_c;
  realopt = new Option (t,n,e_n,p,v,e_v);
}

GenericPlayer::GenericPlayer()
  : MoviePlayer(true, true, true, true), opt_file("genericplayer.ops")
{
#ifdef use_nls
  // gettext
  setlocale(LC_ALL, ""); /* set from the environment variables */
  bindtextdomain("mms-movie-genericplayer", mms_prefix"/share/locale");
  bind_textdomain_codeset("mms-movie-genericplayer", nl_langinfo(CODESET));
#endif

  genericp_conf = S_GenericPlayerConfig::get_instance();

  movie_p = !genericp_conf->p_genericplayer_movie_path().empty();
  vcd_p = !genericp_conf->p_genericplayer_dvd_path().empty();
  dvd_p = !genericp_conf->p_genericplayer_vcd_path().empty();

  initialized = load_opt_file();
  opts.set_header("GenericPlayer");
  opts.set_save_name("GenericPlayerOptions");
  opts.load();
}

GenericPlayer::~GenericPlayer()
{
  opts.save();
  
  if (initialized && (gopt_p.size()> 0))
    foreach (goptstruct& opt, gopt_p)
      delete opt.realopt;

  gopt_p.clear();
}

bool GenericPlayer::load_opt_file()
{
  //This routine loads the user-defined list of options
  //Options are saved in a text file
  //Format of the text file is the following:
  //lines that start with a '#' ora a ' ' are considered comments and are skipped
  //lines that start with a '!' are considered header options and have the following format:
  //!<localizable name>|<internal name>
  //example:
  //!DVD Options|Options DVD
  //the second one is passed through the dgettext("mms-movie-genericplayer", ) API
  //
  //lines that start with a '&' are option entries and must have the following format:
  //&<localizable name>|<internal name>|<shortcut identifier>
  //example:
  //&Aspect ratio|Aspect ratio|g
  //
  //Option entries are followed by option values with this format:
  //=<localizable name>|<Name for substitution>
  //a list of values must be terminated with a line with a single '$' character

  string _str = conf->p_homedir();

  _str+=opt_file;
  if (!file_exists(_str)){
    _str="/etc/mms/";
    _str+=opt_file;
    if (!file_exists(_str)){
      print_warning(dgettext("mms-movie-genericplayer", "Cannot open options file!"), "Genericplayer");
      return false;
    }
  }
  //We try to open the option file
  std::ifstream ffile;
  ffile.open(_str.c_str());
  if (ffile.fail()){
    print_warning(dgettext("mms-movie-genericplayer", "Cannot open options file!"), "Genericplayer");
    return false;
  }
  vector<string> val1;
  vector<string> val2;
  string _name;
  string _e_name;
  char shortcut;
  bool _have_entry = false;

  while(!ffile.eof() && !ffile.fail()) {
    std::getline(ffile, _str);
    if (_str.empty() || _str[0]==' ' || _str[0] =='#')
      continue;
    if (_str[0] == '!'){
      if (_have_entry){
	print_warning(dgettext("mms-movie-genericplayer", "Option file malformed, list of entries not correctly terminated"), "Genericplayer");
      }
      val1.clear();
      val2.clear();
      size_t pos = _str.find("|");
      if (pos == string::npos || pos <2){
	print_warning(dgettext("mms-movie-genericplayer", "Option file malformed, skipping line"), "Genericplayer");
	continue;
      }
      else if (pos==(_str.size()-1)) /* '|' is the last character in the line */
	_str += " "; /* we add a trailing space */

      goptstruct *tmp_p = new goptstruct('\0', true, dgettext("mms-movie-genericplayer", _str.substr(1, pos-1).c_str()), _str.substr(pos+1),0,val1,val2);
      opts.add_opt(tmp_p->realopt);

      gopt_p.push_back(*tmp_p);
    }

    else if (_str[0] == '&'){
      if (_have_entry)
	print_warning(dgettext("mms-movie-genericplayer", "Option file malformed, list of entries not correctly terminated"), "Genericplayer");
      val1.clear();
      val2.clear();
      string tmpstr;
      size_t pos = _str.find("|");
      if (pos == string::npos || pos <2 || pos==(_str.size()-1)){
	print_warning(dgettext("mms-movie-genericplayer", "Option file malformed, skipping line"), "Genericplayer");
	continue;
      }
      _name = _str.substr(1, pos-1);
      tmpstr = _str.substr(pos+1);
      pos = tmpstr.find("|");
      if (pos == string::npos || pos ==0 || pos==(_str.size()-1)){
	print_warning(dgettext("mms-movie-genericplayer", "Option file malformed, skipping line"), "Genericplayer");
	continue;
      }
      _e_name = tmpstr.substr(0, pos);
      shortcut = tmpstr[pos+1];
      _have_entry = true;
    }

    else if (_str[0] == '='){
      if (!_have_entry)
	{
	  print_warning(dgettext("mms-movie-genericplayer", "Option file malformed, option value without option entry"), "Genericplayer");
	  continue;
	}
      size_t pos = _str.find("|");
      if (pos == string::npos || pos <2 || pos==(_str.size()-1)){
	print_warning(dgettext("mms-movie-genericplayer", "Option file malformed, skipping line"), "Genericplayer");
	continue;
      }
      val1.push_back(dgettext("mms-movie-genericplayer", _str.substr(1, pos-1).c_str()));
      val2.push_back(_str.substr(pos+1));
    }

    else if (_str[0] == '$'){
      if (!_have_entry)
	{
	  print_warning(dgettext("mms-movie-genericplayer", "Option file malformed, got '$' line without previous option entry"), "Genericplayer");
	  continue;
	}
      goptstruct *tmp_p = new goptstruct(shortcut, false, dgettext("mms-movie-genericplayer", _name.c_str()), _e_name, 0,val1,val2);
      _have_entry= false;
      opts.add_opt(tmp_p->realopt);
      gopt_p.push_back(*tmp_p);
    }
    else{
      print_warning(dgettext("mms-movie-genericplayer", "Option file malformed, skipping line"), "Genericplayer");
    }
  }
  ffile.close();
  return (gopt_p.size()> 0);
}

void GenericPlayer::play_movie(const string& paths, bool window)
{
  string str = parse_and_complete(genericp_conf->p_generic_movie_opts(), "", paths);

  std::cout << genericp_conf->p_genericplayer_movie_path() << ' ' << str << endl;

  run::exclusive_external_program(genericp_conf->p_genericplayer_movie_path() + ' ' + str);
}

void GenericPlayer::play_disc(const string& play_cmd)
{
  string str = parse_and_complete(genericp_conf->p_generic_movie_opts(), "");
  string command = play_cmd + genericp_conf->p_genericplayer_movie_path() + ' ' + str ;

  run::exclusive_external_program(command);
}

void GenericPlayer::play_vcd(const string& device)
{
  string str = parse_and_complete(genericp_conf->p_generic_vcd_opts(), device);
  run::exclusive_external_program(genericp_conf->p_genericplayer_vcd_path() + ' ' + str);
}

void GenericPlayer::play_dvd(const string& device)
{
  string str = parse_and_complete(genericp_conf->p_generic_dvd_opts(), device);
  run::exclusive_external_program(genericp_conf->p_genericplayer_dvd_path() + ' ' + str);

}

string GenericPlayer::find_opt_val(char s_c)
{
  string _str;

  _str=s_c;
  if (gopt_p.size()==0 )
    return _str;
  foreach (goptstruct& opt, gopt_p)
    if (opt.shortcut== s_c){
      _str = opt.realopt->english_values[opt.realopt->pos];
      break;
    }

  if(_str == " ")
    return "";
  else
    return _str;
}

string GenericPlayer::parse_and_complete(const string& options, const string& optd, const string& path)
{
  //parses option and replaces %d with optd and %i with input type. %@ is replaced with "path".
  //other %X are substituted with values provided by user through the genericplayer.ops file
  //and processed as options by mms
  //in other words i, @ and d shortcuts are reserved.
  //if options doesn't contain a %@ switch, path is appended to the returned string.
  string str="";
  char c;
  bool _bool = false;
  bool _path_included = false;
  for (size_t t = 0; t < options.size(); t++){
    c=options[t];
    if (_bool){
      _bool=false;
      if (c == 'd')
	str+=optd;
      else if (c == '@' && path.size() > 0){
	str+=path;
	_path_included = true;
      }
      else if (c == 'i') {
	string input = "kybd";
	if (list_contains(conf->p_input(), string("lirc")))
	  input = "lirc";
	str += input;
      }
      else if (c == '%')
	str+=c;
      else
	str+=find_opt_val(c);
    }
    else if (c == '%'){
      _bool=true;
    }
    else {
      str+=c;
    }
  }
  if (!_path_included && path.size() > 0)
    str+= " " + path;

  return str;
}
