#include "config.h"

#include "config.hpp"
#include "common.hpp"
#include "startmenu.hpp"
#include "updater.hpp"
#include "theme.hpp"
#include "print.hpp"

#include "plugins.hpp"

#include "renderer.hpp"
#include "input.hpp"

#include "boost.hpp"

#include "SSaverobj.h"

#ifdef use_inotify
#include "notify.hpp"
#endif

#include <unistd.h>
#include <sstream>

#include <iostream>

// --version
#include <getopt.h>

#include "gettext.hpp"

#include "version.h"

#ifdef use_nls
#include <langinfo.h>
#include <locale.h>
#endif
#include <pwd.h>
#include <grp.h>
#include "mms_signals.hpp"


using std::vector;
using std::string;
using string_format::str_printf;

//apparently some systems don't have setenv()
void mms_setenv(const std::string & name, const std::string & value)
{
#if defined(HAVE_SETENV)
  if (setenv(name.c_str(), value.c_str(), 1) < 0) {
#else
  std::string str = name + '=' + value;
  if(putenv(str.c_str()) != 0) {
#endif
    print_critical(gettext("Critical: couldn't set environment variable ") + name);
    print_critical(gettext("Error: ") + s_strerror());
    exit(1);
  }
}

//given a gid, returns name of the group
std::string get_name_from_gid(gid_t gid) {
  std::string str="";
  struct group *my_group = getgrgid(gid);
  if (my_group != NULL)
    str = my_group->gr_name;

  return str;
}

void start()
{
  //Print the info and wait for input
  for (string output = ""; output != "quit";)
    output = S_Startmenu::get_instance()->mainloop();
}

void print_options_header()
{
  Print print(gettext("MMS options:") + string("\n"), Print::INFO);
}

void print_options_footer()
{
  Print print(Print::INFO);
  print.add_line("");
  print.add_line(gettext("<relative> : filename relative to config directory"));
  print.add_line(gettext("<absolute> : full path filename"));
  print.print();
}

void print_output_options()
{
  Plugins *plugins = S_Plugins::get_instance();
  string output = gettext(" -o : output device [");

  foreach (OutputPlugin *plugin, plugins->op_data) {
    output += plugin->plugin_name();
    if (plugin != plugins->op_data.back())
      output += ",";
  }

  output += "]";

  Print print(output, Print::INFO);
}

void print_input_options()
{
  Plugins *plugins = S_Plugins::get_instance();
  string output = gettext(" -i : input device [");

  foreach (InputPlugin *plugin, plugins->ip_data) {
    output += plugin->plugin_name();
    if (plugin != plugins->ip_data.back())
      output += ",";
  }

  output += "]";

  Print print(output, Print::INFO);
}

void print_misc_options()
{
  string output = str_printf("%s\n", gettext(" -h : show this help and exit")) +
    str_printf("%s\n",gettext(" -d : daemonize (run in background)")) +
    str_printf("%s\n", gettext(" -c <absolute> : use alternative config directory")) +
    str_printf("%s\n", gettext(" -p <absolute> : use alternative personal directory (db, playlist)")) +
    gettext(" -f : fullscreen\n") +
    gettext(" -u <username> : run mms as username") +
    gettext("\n -U <username> : run mms as username and reset most environment variables") +
    gettext("\n -e : dump the list of currently set environment variables; useful for setting up and/or debug mms") +
    str_printf("\n%s", gettext(" -v|V : show version and exit"));

  Print print (output, Print::INFO);
}

void print_replay_options()
{
  string output = string("\n") + gettext(" -r <relative> : use replay file as input (requires replay as input device)") + string("\n") +
    gettext(" -b : benchmark MMS by replaying as fast as possible (requires replay as input device) ") + string("\n") +
    gettext(" -w <relative> : write replay to specified file") + string("\n");

  Print print (output, Print::INFO);
}

static struct option longopts[] =
{
  { "conf", 1, 0, 'c' },
  { "personal", 1, 0, 'p' },
  { "daemon", 0, 0, 'd' },
  { "fullscreen", 0, 0, 'f' },
  { "replay-read", 1, 0, 'r' },
  { "benchmark", 0, 0, 'b' },
  { "replay-write", 1, 0, 'w' },
  { "version", 0, 0, 'v' },
  { "help", 0, 0, 'h' },
  { "user", 1, 0, 'u' },
  { "dumpenv", 1, 0, 'e' },
  {0, 0, 0, 0}
};

void set_config(Config *conf, string config)
{
  if (config[config.size()-1] != '/')
    config += '/';

  conf->s_homedir(config);
}

void set_personal(Config *conf, string personal)
{
  if (personal[personal.size()-1] != '/')
    personal += '/';

  conf->s_var_data_dir(personal);
}

bool amiroot()
{
  int euid = geteuid();
  int uid = getuid();
  return (uid == 0 || euid == 0);
}

bool runasuser(const std::string & user, bool resetenv) //we try to drop privileges in a safe manner
{
  //we reset environment variables the best we can
  //so first we save the current ones we want to keep after the reset

  std::string _PATH = s_getenv("PATH"); //I don't like this, but I see no other way
  std::string _LD_LIBRARY_PATH = s_getenv("LD_LIBRARY_PATH");
  std::string _PYTHONPATH = s_getenv("PYTHONPATH"); //for the python plugin
  std::string _PYTHONSTARTUP = s_getenv("PYTHONSTARTUP"); //for the python plugin
  std::string _DISPLAY = s_getenv("DISPLAY"); //obviously X access should be set properly via xauth and/or other means
  std::string _XAUTHORITY = s_getenv("XAUTHORITY"); //this allows user to pass the correct env variable via cmd line

  //locale related settings
  std::string _CHARSET = s_getenv("CHARSET");
  std::string _KEYBOARD = s_getenv("KEYBOARD");
  std::string _KEYTABLE = s_getenv("KEYTABLE");
  std::string _LANG = s_getenv("LANG");
  std::string _LANGUAGE = s_getenv("LANGUAGE");
  std::string _LC_ADDRESS = s_getenv("LC_ADDRESS");
  std::string _LC_COLLATE = s_getenv("LC_COLLATE");
  std::string _LC_CTYPE = s_getenv("LC_CTYPE");
  std::string _LC_IDENTIFICATION = s_getenv("LC_IDENTIFICATION");
  std::string _LC_MEASUREMENT = s_getenv("LC_MEASUREMENT");
  std::string _LC_MESSAGES = s_getenv("LC_MESSAGES");
  std::string _LC_MONETARY = s_getenv("LC_MONETARY");
  std::string _LC_NAME = s_getenv("LC_NAME");
  std::string _LC_NUMERIC = s_getenv("LC_NUMERIC");
  std::string _LC_PAPER = s_getenv("LC_PAPER");
  std::string _LC_SOURCED = s_getenv("LC_SOURCED");
  std::string _LC_TELEPHONE = s_getenv("LC_TELEPHONE");
  std::string _LC_TIME = s_getenv("LC_TIME");
  std::string _BACKSPACE = s_getenv("BACKSPACE");
  std::string _NLSPATH = s_getenv("NLSPATH");

  std::string _IFS = " \t\n"; // internal field separator. We force a reset of this one to prevent
  // the old IFS exploit trick.
  std::string _TZ  = s_getenv("TZ");  //timezone

  //these are set according to user settings in the password file or otherwise
  std::string _HOME = "";
  std::string _SHELL = "";
  std::string _TMP = "";
  std::string _USER = user;

  //gotta find the uid of the username specified by user....

  passwd *pwd = getpwnam(user.c_str());
  if (pwd == NULL){ //we didn't find user "user"
    print_critical(gettext("Critical: couldn't find user: ") + user);
    return false;
  }

  _HOME = pwd->pw_dir;
  _SHELL = pwd->pw_shell;
  int uid = pwd->pw_uid;
  int gid = pwd->pw_gid;

  //here comes the juicy part....
  //
  // we set first group, then user.
  //
  int old_egid = getegid();
  if (setregid(gid, gid) < 0){
    print_critical(gettext("Critical: couldn't set group privileges!"));
    print_critical(gettext("Error: ") + s_strerror());
    return false;
  }

  if (getgid() != gid || getegid() != gid){
    print_critical(gettext("Critical: couldn't set group privileges!"));
    return false;
  }

  //we also set supplementary groups...
  //this may fail if user has no CAP_SETGID capability... usually any user != root
  if (initgroups(user.c_str(), gid) < 0){
    print_critical(gettext("Warning: failed to set supplementary groups!"));
    print_critical(gettext("Error: ") +  s_strerror());
  }

  int old_euid = geteuid();
  if (setreuid(uid, uid) < 0){
    print_critical(gettext("Critical: couldn't set user privileges!"));
    print_critical(gettext("Error: ") +  s_strerror());
    return false;
  }
  if (getuid() != uid || geteuid() != uid){
    print_critical(gettext("Critical: couldn't set user privileges!"));
    return false;
  }

  //can we regain privileges?
  if (old_euid != uid && (setreuid(-1, old_euid) == 0)){
    print_critical(gettext("Critical: couldn't set user privileges permanently!"));
    return false;
  }

  if (old_egid != gid && uid !=0 && (setregid(-1, old_egid) == 0)){
    print_critical(gettext("Critical: couldn't set group privileges permanently!"));
    return false;
  }

  if (old_euid != uid){ // let's handle env variables

    if (resetenv){
#ifdef HAVE_CLEARENV
      clearenv();
#else
      environ = NULL;
#endif
      mms_setenv("PATH", _PATH);
      mms_setenv("LD_LIBRARY_PATH", _LD_LIBRARY_PATH);
      mms_setenv("PYTHONPATH", _PYTHONPATH);
      mms_setenv("PYTHONSTARTUP", _PYTHONSTARTUP);
      mms_setenv("DISPLAY", _DISPLAY);
      mms_setenv("CHARSET", _CHARSET);
      mms_setenv("KEYBOARD", _KEYBOARD);
      mms_setenv("KEYTABLE", _KEYTABLE);
      mms_setenv("LANG", _LANG);
      mms_setenv("LANGUAGE", _LANGUAGE);
      mms_setenv("PATH", _PATH);
      mms_setenv("LC_ADDRESS", _LC_ADDRESS);
      mms_setenv("LC_COLLATE", _LC_COLLATE);
      mms_setenv("LC_CTYPE", _LC_CTYPE);
      mms_setenv("LC_IDENTIFICATION", _LC_IDENTIFICATION);
      mms_setenv("LC_MEASUREMENT", _LC_MEASUREMENT);
      mms_setenv("LC_MESSAGES", _LC_MESSAGES);
      mms_setenv("LC_MONETARY", _LC_MONETARY);
      mms_setenv("LC_NAME", _LC_NAME);
      mms_setenv("LC_NUMERIC", _LC_NUMERIC);
      mms_setenv("LC_PAPER", _LC_PAPER);
      mms_setenv("LC_SOURCED", _LC_SOURCED);
      mms_setenv("LC_TELEPHONE", _LC_TELEPHONE);
      mms_setenv("LC_TIME", _LC_TIME);
      mms_setenv("BACKSPACE", _BACKSPACE);
      mms_setenv("NLSPATH", _NLSPATH);
      mms_setenv("IFS", _IFS);
      mms_setenv("TZ", _TZ);
      mms_setenv("XAUTHORITY", _XAUTHORITY);
    }

    mms_setenv("HOME", _HOME);
    mms_setenv("SHELL", _SHELL);
    mms_setenv("LOGNAME", user);
    mms_setenv("USER", user);
    std::string _TMPDIR = _HOME + "/tmp";
    if (file_exists(_TMPDIR)){
      mms_setenv("TMPDIR", _TMPDIR);
      mms_setenv("TMP", _TMPDIR);
    }
    else{
      mms_setenv("TMPDIR", "/tmp");
      mms_setenv("TMP", "/tmp");
    }
  }

  printf ("Running mms as user %s (uid=%d)\n",  user.c_str(), uid);

  print_critical(gettext("Full list of active groups for user ")  + user);
  fprintf(stderr, "  - gid=%d (%s) - main/default group\n", gid, get_name_from_gid(gid).c_str());

  //we fetch all groups user belongs to:
  long max_groups = sysconf(_SC_NGROUPS_MAX);
  if (max_groups > 0){
    // alloc space for the groups list
    gid_t *groups = static_cast<gid_t*>( malloc(max_groups*sizeof(gid_t)));
    if (groups == NULL){ //Out of memory????????!!!!!!!
      print_critical(gettext("Critical: out of memory!"));
      exit(1);
    }
    int num_groups;

    if ((num_groups = getgroups(max_groups, groups)) < 0){
      print_critical(gettext("Warning: couldn't fetch a list of secondary groups for user ")+ user);
      print_critical(gettext("Error: ") +  s_strerror());
    }
    else {
      for (int t = 0; t < num_groups; t++)
	if (gid != groups[t])
	  fprintf(stderr, "  - gid=%d (%s)\n", groups[t], get_name_from_gid(groups[t]).c_str());
    }
    free(groups);
  }

  if (uid == 0){ //Print a big warning
    printf("\n\n\nYou have chosen to execute mms with superuser privileges.\n");
    printf("This is highly discouraged and it constitutes a security risk.\n");
    printf("I hope you know what you're doing.\n\n\n");
  }
  return true;
}

int main(int argc,char* argv[])
{
  setsid();
  S_mmsSignals::get_instance()->init();

  Config *conf = S_Config::get_instance();

  if (!conf->p_autostart().empty())
    S_Global::get_instance()->autostarting = true;

  // overwriting variables
  string homedir = "", var_data = "";
  bool benchmark = false, fullscreen = false;
  string replay_read = "", replay_write = "", outdev = "", user = "";
  bool resetenv = false, dumpenv = false;
  std::list<string> indevs;

#ifdef use_nls
  // gettext
  setlocale(LC_ALL, ""); /* set from the environment variables */
  bindtextdomain("mms", mms_prefix"/share/locale");
  bind_textdomain_codeset("mms", nl_langinfo(CODESET));
  textdomain("mms");
#endif

  int c;

  while ((c = getopt_long(argc, argv, "abc:dfhlno:p:u:U:w:r:i:t:O:vVe",
			  longopts, NULL)) != -1)
    switch (c) {

    case 'b':
      benchmark = true;
      break;
    case 'c': // config
      homedir = optarg;
      set_config(conf, homedir);
      break;
    case 'p': // personal
      var_data = optarg;
      set_personal(conf, var_data);
      break;
    case 'd': // daemonize
      switch (fork()) {
      case 0: break; // my child will go on
      case -1:
	fprintf(stderr,"Error forking\n");
	return -1;
      default:
	_exit(0);
      }
      if (setsid() < 0) // we won't get SIGHUP after this
	return -1;
      switch (fork()) {
      case 0: break; // my child will go on
      case -1: // error
	fprintf(stderr,"Error forking\n");
	return -1;
      default: _exit(0); // the parent process exits
      }
      break;
    case 'f':
      fullscreen = true;
      break;
    case 'h':
      conf->s_debug_level(0);
      S_Plugins::get_instance()->scan_input_output();
      conf->s_debug_level(2);
      print_options_header();
      print_misc_options();
      print_replay_options();
      print_input_options();
      print_output_options();
      print_options_footer();
      exit(0);
      break;
    case 'r':
      replay_read = optarg;
      break;
    case 'w':
      replay_write = optarg;
      break;
    case 'o':
      if (!strcmp(optarg,"h") || !strcmp(optarg,"help")) {
	conf->s_debug_level(0);
	S_Plugins::get_instance()->scan_input_output();
	conf->s_debug_level(2);
	print_options_header();
	print_output_options();
	exit(0);
      }
      outdev = optarg;
      break;
    case 'i':
      {
	if (!strcmp(optarg,"h") || !strcmp(optarg,"help")) {
	  print_options_header();
	  print_input_options();
	  exit(0);
	}

	string opt_input = optarg;

	int comma = opt_input.find(',');

	while (comma != string::npos) {
	  indevs.push_back(opt_input.substr(0, comma));
	  opt_input = opt_input.substr(comma+1);
	  comma = opt_input.find(',');
	}

	indevs.push_back(opt_input);
      }
      break;
    case 'u': 		/* run as user */
      user = optarg;
      resetenv = false;
      break;
    case 'U':
      user = optarg;
      resetenv = true;
      break;
    case 'e':
      dumpenv = true;
      break;
    case 'v':		/* version */
    case 'V':		/* version */
      {
	print_info(string("My Media System: 1.1.1 ") + VERSION);
	print_info(str_printf("Configuration: %s", MMS_CONFIG_OPTIONS));
      }
      exit(0);
    }


  if (amiroot() && user =="") {
    print_critical(gettext("\nIt appears mms is running as superuser."));
    print_critical(gettext("Not only is this totally unnecessary, but it also constitutes a security risk "));
    print_critical(gettext("and thus it is strongly discouraged!\n"));
    print_critical(gettext("If you really really want to give mms superuser privileges, run it like this:"));
    print_critical(gettext("mms -u root [other options]\n"));
    exit(1);
  }

  /* Make sure only one instance of MMS is running at the same time */
  int otherinstance = -1;

  for (int t = 0; t < 5; t++){
    otherinstance = filemutex::lock();
    if (!otherinstance){
      break;
    }
    mmsUsleep(500*1000);
  }

  if (otherinstance < 0){
    print_critical(gettext("Another instance of MMS or an instance of movie/pic/audio library is already running.\n"));
    exit(1);
  }

  else if(otherinstance > 0){
    fprintf(stderr, gettext("File lock is currently owned by another process (pid %d). Aborting\n"), otherinstance);
    exit(1);
  }

  if (user != "")
    if(!runasuser(user, resetenv))
      exit(1);

  if (dumpenv){
    print_info(gettext("Full list of environment variables:\n"));
    char ** envp;
    for (envp = environ; *envp; envp++){
      fprintf(stderr,"  %s\n", *envp);
    }
    fprintf(stderr,"\n");
  }

  if (var_data == "" ) { // options files, databases, etc. go into $HOME/.mms or /var/lib/mms
    if (!havemydir("", &var_data)){ //we have no place where
      // we can save option files, databases, etc, so we exit
      print_critical(gettext("Unable to find or create HOME directory"));
      exit(1);
    }
    else {
      set_personal(conf, var_data);
    }
  }

  if (homedir == ""){  //configuration files
    if (!(homedir = s_getenv("HOME")).empty() && dir_exists(homedir + "/.mms"))
      set_config(conf, filesystem::FExpand(homedir) + "/.mms");
    else
      set_config(conf, filesystem::FExpand(mms_config)+ "/"); // hardcoded default config
  }

  // now pass the configuration file using the correct path
  conf->parse_configuration_file();

  if ((conf->p_cdpath().empty() || conf->p_cdname().empty()) && conf->p_media()) {
    Print print1(gettext("CD/DVD not configured properly and media enabled in Config, exiting"), Print::INFO);
    exit(0);
  }

  if (conf->p_temp_image_loc() == "") {
    string cache_dir;
    recurse_mkdir(conf->p_var_data_dir(), "cache/", &cache_dir);
    conf->s_temp_image_loc(cache_dir);
  }

  // set options from configuration line to overwrite config
  if (benchmark)
    conf->s_benchmark(true);

  if (fullscreen)
    conf->s_fullscreen(true);

  if (!replay_read.empty())
    conf->s_replay_read(replay_read);

  if (!replay_write.empty())
    conf->s_replay_write(replay_write);

  if (!outdev.empty())
    conf->s_outdev(outdev);

  if (indevs.size() != 0)
    conf->s_input(indevs);

  Print print1(gettext("Using ") + conf->p_homedir() + gettext(" as primary configuration directory"), Print::INFO);
  Print print2(gettext("Using ") + std::string(mms_config) + gettext(" as fallback configuration directory"), Print::INFO);
  Print print3(gettext("Using ") + conf->p_var_data_dir() + gettext(" as personal data directory"), Print::INFO);
  Print print4(gettext("Using ") + conf->p_temp_image_loc() + gettext(" as cache location"), Print::INFO);

  // init the theme engine before plugins, so that plugins can use themes
  S_Theme::get_instance();

  Plugins *plugins = S_Plugins::get_instance();
  plugins->scan();

  // Init the i/o classes
  // First init renderer and InputMaster, because I/O device plugins need them.

  Render *render = S_Render::get_instance();

  // make sure the temporary directory is available
  string temp_loc = conf->p_temp_image_loc();
  if (temp_loc[temp_loc.size()-1] != '/')
    temp_loc += '/';

  abs_recurse_mkdir(temp_loc, NULL);
  render->set_theme();

  print_debug("Detecting input and output plugins");

  // Derefer fail to report both input & output plugins pre-errors.
  int init_error = 0;

  if (plugins->ip_data.size() == 0) {
    print_critical(gettext("No input plugin found"));
    init_error++;
  }

  if (plugins->op_data.size() == 0) {
    print_critical(gettext("No output device specified! Use the -o option."));
    init_error++;
  }

  if (init_error){
    clean_up(false);
    mmsSleep(5000); /* wait to be killed */
  }

  print_debug("At least one input and one output plugin were loaded... fine");
  print_debug("Detecting specified output device");

  // Try to detect specified plugin (config or overwritten by command line)
  foreach (OutputPlugin *plugin, plugins->op_data)
    if (plugin->plugin_name() == conf->p_outdev()) {
      print_debug("Specified device found: " + plugin->plugin_name());
      plugin->device->init();
      render->device=plugin->device;
      plugin->was_loaded_ok();
      S_SSaverobj::get_instance()->Init();
      print_debug("Init ok device ok");
      break;
    }

  //  if no one found report and fail
  if (!render->device) {
    if (plugins->op_data.size() > 0) {
      OutputPlugin *out_dev = plugins->op_data.front();
      print_debug("No render device found, using first: " + out_dev->plugin_name());
      out_dev->device->init();
      conf->s_outdev(out_dev->plugin_name());
      render->device=out_dev->device;
      out_dev->was_loaded_ok();
    } else {
      init_error++;
      print_critical("No output plugin can init");
    }
  }

  print_debug("Detecting specified input device");

  InputMaster *im = S_InputMaster::get_instance();

  bool input_device_added = false;

  foreach (const string& name, conf->p_input()) {
    foreach (InputPlugin *plugin, plugins->ip_data) {
      if (name == plugin->plugin_name()) {
	print_debug("Specified device found: " + plugin->plugin_name());
	if (plugin->device->init()) {
	  im->add_input_device(plugin->device);
	  if (!input_device_added)
	    im->set_default_device(plugin->plugin_name());
	  input_device_added = true;
	  print_debug("init ok");
	} else
	  print_warning("init failed");
      }
    }
  }

  // else use first one (but the one already tested!) that init
  if (!input_device_added) {
    print_warning("Input plugin not found - Trying first one");
    foreach (const string& name, conf->p_input()) {
      foreach (InputPlugin *plugin, plugins->ip_data) {
	if (name == plugin->plugin_name()) {
	  print_warning("Device found " + plugin->plugin_name());
	  if (plugin->device->init()) {
	    print_warning("init ok");
	    im->add_input_device(plugin->device);
	    if (!input_device_added)
	      im->set_default_device(plugin->plugin_name());
	    std::list<string> working_input;
	    working_input.push_back(plugin->plugin_name());
	    conf->s_input(working_input);
	    input_device_added = true;
	    break;
	  } else
	    print_warning("init failed");
	}
      }
    }
  }

  if (!input_device_added) {
    print_critical("No input plugin can init");
    init_error++;
  }

  if (init_error)
    return 1;

  im->input_devices_loaded();
  print_debug("Input and Output plugin init ok");

  print_debug("Plugins post-init");
  foreach (OutputPlugin* plugin, plugins->op_data) {
    if (plugin->plugin_name() == conf->p_outdev())
      plugin->plugin_post_init();
  }

  foreach (InputPlugin* plugin, plugins->ip_data) {
    if (list_contains(conf->p_input(), plugin->plugin_name()))
      plugin->plugin_post_init();
  }

  foreach (FeaturePlugin* plugin, plugins->fp_data) {
    plugin->plugin_post_init();
  }

  foreach (AudioPlayerPlugin* plugin, plugins->audio_players) {
    plugin->plugin_post_init();
  }

  foreach (MoviePlayerPlugin* plugin, plugins->movie_players) {
    plugin->plugin_post_init();
  }

  print_debug("init input device");
  im->start_devices();
  print_debug("init render device");
  render->device->start();


  Global *global = S_Global::get_instance();

  if (global->lcd_output_possible()) {
    global->lcd_add_output(string_format::pretty_pad_string("My Media System",global->lcd_pixels(),' '));
    global->lcd_print();
  }


  print_debug("Start mms Startmenu mainloop");

  start();

  S_Global::get_instance()->clean_up();

  clean_up(true);

  mmsSleep(5000); /* wait to be killed */

  exit(0);
}
