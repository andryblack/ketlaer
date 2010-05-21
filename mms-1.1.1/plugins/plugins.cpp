#include "config.h"

#include "plugins.hpp"

#include "libfspp.hpp"

#include "boost.hpp"

#include <csignal>
#include <cstdio>
#include <dlfcn.h>
#include <sys/stat.h>

#include <fstream>
#include <iostream>

using std::vector;
using std::string;

pthread_mutex_t Plugins::singleton_mutex = PTHREAD_MUTEX_INITIALIZER;

void Plugins::_destroy(Plugin * plugin){
  ppnames.push_back(plugin->filename);
  if (conf->p_debug_level() > 2)
    fprintf(stderr,"Destroying plugin '%s'\n", plugin->filename.c_str());
  plugin->destructor(plugin);
  return;
}

bool Plugins::_dlclose(const string& pname){
  if (conf->p_debug_level() > 2){
    fprintf(stderr,"Unloading plugin '%s'\n", pname.c_str());
  }
  //fetch its handle and, of course, make sure it is still loaded
  void * phandle = dlopen(pname.c_str(), RTLD_NOW | RTLD_GLOBAL | RTLD_NOLOAD); //this doesn't actually load the library
  if (phandle == NULL){
    fprintf(stderr, "Warning: library '%s' was supposed to be already loaded, but apparently it isn't\n", pname.c_str());
  }
  else if (dlclose(phandle) != 0) {
    fprintf(stderr, "dlclose() error '%s' for plugin '%s'\n", dlerror(), pname.c_str());
    return false;
  }
  return true;
}

void Plugins::delete_plugins()
{
  conf = S_Config::get_instance();
  ppnames.clear();
  foreach (FeaturePlugin* plugin, fp_data) {
    _destroy(plugin);
  }
  foreach (AudioPlayerPlugin* plugin, audio_players) {
    _destroy(plugin);
  }
  foreach (MoviePlayerPlugin* plugin, movie_players) {
    _destroy(plugin);
  }
  foreach (PrinterPlugin* plugin, printers) {
    _destroy(plugin);
  }
  foreach (InputPlugin* plugin, ip_data) {
    _destroy(plugin);
  }
  foreach (OutputPlugin* plugin, op_data) {
    if (plugin->loaded()) {
      plugin->device->deinit();
    }
    _destroy(plugin);
  }
  foreach (string pn, ppnames) {
    _dlclose(pn); // TODO: uncomment this when the callback stuff is properly fixed
  }
  ppnames.clear();
}

Plugin *Plugins::add_plugin(const string& filename)
{
  print_debug("Plugins::add_plugin() " + filename);

  string base_filename = basename(filename.c_str());

  print_debug("Plugins name is " + base_filename);

  // search in all types if already loaded plugin

  // input plugin ?
  foreach (InputPlugin* plugin, ip_data)
    if (base_filename == basename(plugin->filename.c_str()))
      return NULL;

  print_debug("Not an already loaded input plugin");

  // output plugin ?
  foreach (OutputPlugin* plugin, op_data)
    if (base_filename == basename(plugin->filename.c_str()))
      return NULL;

  print_debug("Not an already loaded output plugin");

  // feature plugin ?
  foreach (FeaturePlugin* plugin, fp_data)
    if (base_filename == basename(plugin->filename.c_str()))
      return NULL;

  print_debug("Not an already loaded feature plugin");

  // audio player plugin ?
  foreach (AudioPlayerPlugin* plugin, audio_players)
    if (base_filename == basename(plugin->filename.c_str()))
      return NULL;

  print_debug("Not an already loaded audio player plugin");

  // movie player plugin ?
  foreach (MoviePlayerPlugin* plugin, movie_players)
    if (base_filename == basename(plugin->filename.c_str()))
      return NULL;

  print_debug("Not an already loaded movie player plugin");

  // printer plugin ?
  foreach (PrinterPlugin* plugin, printers)
    if (base_filename == basename(plugin->filename.c_str()))
      return NULL;

  print_debug("Not an already loaded print plugin");

  print_debug("Not already loaded, thus loading plugin");

  Plugin *p = load(filename);
  return p;
}

Plugin* Plugins::load(const string& filename)
{
  void *h = NULL;
  void *(*constructor) (void);
  void *(*destructor) (Plugin*);

  string base_filename = basename(filename.c_str());
  h = dlopen(filename.c_str(), RTLD_NOW | RTLD_GLOBAL | RTLD_NOLOAD);
  if (h == NULL)
    h = dlopen(filename.c_str(), RTLD_NOW | RTLD_GLOBAL);
  else 
    print_warning("Plugin loaded elsewhere: " + base_filename);

  if (h){
    print_debug("Plugin loaded");

    // FIXME oldstyle cast - ISO C++ forbid it directly - howto do this now ??
    if ((constructor = (void *(*)())dlsym(h, "construct")) && ((destructor = (void *(*)(Plugin*))dlsym(h, "destroy"))))
      {
	Plugin *p = static_cast<Plugin *>(constructor());
	if (p) {
	  p->handle = h;
	  p->destructor = destructor;
	  p->filename = filename;
	  switch (p->type()) {
	  case IN_PLUGIN:
	  case OUT_PLUGIN:
	  case FEATURE_PLUGIN:
	  case AUDIO_PLAYER_PLUGIN:
	  case MOVIE_PLAYER_PLUGIN:
	  case PRINTER_PLUGIN:
	    return p;
	    break;
	  default:
	    print_warning("Unknown plugin type, removed: " + base_filename);
	    dlclose(h);
	    break;
	  }
	} else{
	  print_warning("Plugin failed to pre-init: " + base_filename);
	  dlclose(h);
	}
      } else{
	print_warning("Does not look like a MMS plugin");
	dlclose(h);
      }
  } else
    print_critical(string("dlopen failed with error: ") + dlerror());
  sleep(2);

  return NULL;
}

bool prioritize_plugins(const FeaturePlugin *fp1, const FeaturePlugin *fp2)
{
  return fp1->priority() < fp2->priority();
}

void Plugins::scan_input_output()
{
  string dir = MMS_PLUGINS_DIR;

  print_debug("Plugins scanning: " + dir);

  for (file_iterator<file_t, default_order> i (dir); i != i.end (); i.advance(false)) {

    string filename = i->getName();

    print_debug("Plugins Scan: trying " + filename);

    int p;
    struct stat64 statbuf;

    if (!stat64(filename.c_str(), &statbuf) && S_ISREG(statbuf.st_mode))
      if ((p=filename.rfind('.'))>0 && (filename.substr(p) == ".so")) {

	// hack to not load plugins we don't neeed
	if ((filename.find("lib_ainput_") == string::npos) &&
	    (filename.find("lib_output_") == string::npos))
	  continue;

        Plugin *plugin = add_plugin(filename);
        if (plugin) {
	  switch (plugin->type()) {
	  case IN_PLUGIN:{
	    InputPlugin *ip = static_cast<InputPlugin *>(plugin);
	    print_info("Input plugin: " + ip->plugin_name() + " loaded");
	    ip_data.push_back(ip);
	    break;
	  }
	  case OUT_PLUGIN:{
	    OutputPlugin *op = static_cast<OutputPlugin *>(plugin);
	    print_info("Output plugin: " + op->plugin_name() + " loaded");
	    op_data.push_back(op);
	    break;
	  }
	  default:
	    break;
	  }
	} else
	  print_warning("Error loading plugin: " + filename);
      }
  }
}

void Plugins::scan()
{
  string dir = MMS_PLUGINS_DIR;

  print_debug("Plugins scanning: " + dir);

  for (file_iterator<file_t, default_order> i (dir); i != i.end (); i.advance(false)) {

    string filename = i->getName();

    print_debug("Plugins Scan: trying " + filename);

    int p;
    struct stat64 statbuf;

    if (!stat64(filename.c_str(), &statbuf) && S_ISREG(statbuf.st_mode))
      if ((p=filename.rfind('.'))>0 && (filename.substr(p) == ".so")) {
        Plugin *plugin = add_plugin(filename);
        if (plugin) {
	  switch (plugin->type()) {
	  case IN_PLUGIN:{
	    InputPlugin *ip = static_cast<InputPlugin *>(plugin);
	    print_info("Input plugin: " + ip->plugin_name() + " loaded");
	    ip_data.push_back(ip);
	    break;
	  }
	  case OUT_PLUGIN:{
	    OutputPlugin *op = static_cast<OutputPlugin *>(plugin);
	    print_info("Output plugin: " + op->plugin_name() + " loaded");
	    op_data.push_back(op);
	    break;
	  }
	  case FEATURE_PLUGIN:{
	    FeaturePlugin *fp = static_cast<FeaturePlugin *>(plugin);
	    print_info("Feature plugin: " + fp->plugin_name() + " loaded");
	    fp_data.push_back(fp);
	    break;
	  }
	  case AUDIO_PLAYER_PLUGIN:{
	    AudioPlayerPlugin *ap = static_cast<AudioPlayerPlugin *>(plugin);
	    print_info("Audio player plugin: " + ap->plugin_name() + " loaded");
	    audio_players.push_back(ap);
	    break;
	  }
	  case MOVIE_PLAYER_PLUGIN:{
	    MoviePlayerPlugin *vp = static_cast<MoviePlayerPlugin *>(plugin);
	    print_info("Movie player plugin: " + vp->plugin_name() + " loaded");
	    movie_players.push_back(vp);
	    break;
	  }
	  case PRINTER_PLUGIN:{
	    PrinterPlugin *pp = static_cast<PrinterPlugin *>(plugin);
	    print_info("Printer plugin: " + pp->plugin_name() + " loaded");
	    printers.push_back(pp);
	    break;
	  }
	  default:
	    print_warning("Unknown plugin type: " + filename);
	  }
	} else
	  print_warning("Error loading plugin: " + filename);
      }
  }

  // sort feature plugins by priority
  std::sort(fp_data.begin(), fp_data.end(), prioritize_plugins);
}

Plugin* Plugins::find(const string& name)
{
  foreach (FeaturePlugin* plugin, fp_data)
    if (name == plugin->plugin_name())
      return plugin;
  foreach (OutputPlugin* plugin, op_data)
    if (name == plugin->plugin_name())
      return plugin;
  foreach (InputPlugin* plugin, ip_data)
    if (name == plugin->plugin_name())
      return plugin;
  foreach (AudioPlayerPlugin* plugin, audio_players)
    if (name == plugin->plugin_name())
      return plugin;
  foreach (MoviePlayerPlugin* plugin, movie_players)
    if (name == plugin->plugin_name())
      return plugin;
  foreach (PrinterPlugin* plugin, printers)
    if (name == plugin->plugin_name())
      return plugin;
  return 0;
}
