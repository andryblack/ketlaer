#ifndef PLUGIN_HPP
#define PLUGIN_HPP

#include "../config.h"

#include "module.hpp"
#include "input.hpp"
#include "renderer.hpp"
#include "startmenu.hpp"

#include "plugins/feature/audio/players/audio_player.hpp"
#include "plugins/feature/movie/players/movie_player.hpp"
#include "printer.hpp"

#include <list>

using std::cerr;
using std::endl;

#define MMS_PLUGINS_DIR mms_prefix"/lib/mms/plugins"

#define MMS_PLUGIN(classtype) extern "C" classtype *construct() { \
  return new classtype(); \
} \
extern "C" void destroy(classtype* c) { \
  delete c; \
}

enum ptype { IN_PLUGIN, OUT_PLUGIN, FEATURE_PLUGIN, AUDIO_PLAYER_PLUGIN, MOVIE_PLAYER_PLUGIN, PRINTER_PLUGIN, WRONG_PLUGIN };
typedef enum ptype plugin_type;


// Generic Plugin class
// For internal use. Do not use it directly, use one of the childs class
// InputPlugin, OutputPlugin, FeaturePlugin instead

class Plugin
{
public:
  // This is not only the plugin name, but the start menu entry too
  virtual std::string plugin_name() const { return gettext("Unnamed"); }

public:

  void *handle;			// Set by mms
  void *(*destructor) (Plugin*);   // Set by mms
  std ::string filename;	// Set by mms

  // The destructor may uninit everything
  virtual ~Plugin() {}

  // This is the type specifier from the childs class
  virtual plugin_type type() const { return WRONG_PLUGIN; }

  virtual bool plugin_post_init()  { return true;}

};

// Class used to define a dynamic Input device

class InputPlugin : public Plugin
{
public:
  plugin_type type() const { return IN_PLUGIN; }
  InputDevice *device;
  virtual ~InputPlugin() { delete device; }
};


// Class used to define a dynamic Render device

class PrinterPlugin : public Plugin
{
public:
  plugin_type type() const { return PRINTER_PLUGIN; }
  Printer *printer;
};

// Class used to define a dynamic Render device

class OutputPlugin : public Plugin
{
public:
  plugin_type type() const { return OUT_PLUGIN; }
  bool loaded() { return loaded_ok; }
  void was_loaded_ok() { loaded_ok = true; }
  RenderDevice *device;
  OutputPlugin() : loaded_ok(false) {}
  virtual ~OutputPlugin() { delete device; }
private:
  bool loaded_ok;
};


// This class is used to add a feature to the start menu
// Use "plugin_name" to specify the named line the plugin will show unselected
// if it does not exist, it will be created, otherwhise the "priority" will
// be used to sort the list. (not implemented)

class FeaturePlugin : public Plugin
{
public:
  // The plugin name is also the start menu entry
  virtual ~FeaturePlugin() {
    if (module != 0){
      delete module;
      module = 0; 
    }
  
  }

  FeaturePlugin()
    : module(0)
  {}

  virtual int priority() const { return -1; }

  plugin_type type() const { return FEATURE_PLUGIN; }
  Module *module;

  const std::list<startmenu_item> items() const { return features; }
protected:
  std::list<startmenu_item> features;
};

// Class used to define a dynamic audio player device

class AudioPlayerPlugin : public Plugin
{
public:
  plugin_type type() const { return AUDIO_PLAYER_PLUGIN; }
  AudioPlayer *player;
  virtual ~AudioPlayerPlugin() { delete player; }
};

// Class used to define a dynamic movie player device

class MoviePlayerPlugin : public Plugin
{
public:
  plugin_type type() const { return MOVIE_PLAYER_PLUGIN; }
  MoviePlayer *player;
  virtual ~MoviePlayerPlugin() { delete player; }
};

#endif
