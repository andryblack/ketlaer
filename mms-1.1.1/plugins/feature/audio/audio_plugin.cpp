#include "plugin.hpp"
#include "audio.hpp"
#include "themes.hpp"
#include "config.hpp"
#include "print.hpp"
#include "plugins.hpp"
#include "cd.hpp"

#ifdef use_graphical_audio
#include "graphical_audio.hpp"
#endif

#include "simple_audio.hpp"

void audio_hd()
{
  Audio *audio = get_class<Audio>(dgettext("mms-audio", "Audio"));

  if (!audio->loaded_correctly) {
    audio->read_dirs();
    audio->loaded_correctly = true;
  }

  if (audio->reload_needed) {
    DialogWaitPrint pdialog(dgettext("mms-audio", "Reloading directories"), 1000);

    audio->read_dirs();
  }

  if (audio->audiolist_size() == 0) {

    Print pdialog(Print::SCREEN);
    pdialog.add_line(dgettext("mms-audio", "Could not find any music"));
    pdialog.add_line("");
    pdialog.add_line(dgettext("mms-audio", "Please specify a correct path "));
    pdialog.add_line(dgettext("mms-audio", "in the configuration file"));
    pdialog.print();

    audio->reload_needed = true;

  } else {

    audio->reload_needed = false;
    audio->loaded_correctly = true;

    audio->mainloop(Audio::ADD);
  }
}

void audio_cd()
{
  Cd *cd = S_Cd::get_instance();

  if (cd->open()) {

    Audio *audio = get_class<Audio>(dgettext("mms-audio", "Audio"));

    Cd::cdstatus cdstatus = cd->check_cddrive();

    if (cdstatus == Cd::OK)
      audio->detect_cdformat();

    cd->close();
  }
}

void audio_radio()
{
  get_class<Audio>(dgettext("mms-audio", "Audio"))->choose_radio();
}

class AudioPlugin : public FeaturePlugin
{
private:
  Config *conf;

public:

  bool plugin_post_init(){
    InputMaster *input_master = S_InputMaster::get_instance();
    input_master->parse_keys("playlist", "3");
    input_master->parse_keys("audio", "10");
    input_master->extend_default("audio", "2");
    static_cast<Audio *>(module)->add_playback_keys();
    return true;
  }

  AudioPlugin() {
    conf = S_Config::get_instance();
    AudioConfig *audio_conf = S_AudioConfig::get_instance();
    audio_conf->parse_configuration_file(conf->p_homedir());

    RadioConfig *radio_conf = S_RadioConfig::get_instance();
    radio_conf->parse_configuration_file(conf->p_homedir());

#ifdef use_graphical_audio
    if (audio_conf->p_graphical_audio_mode())
      module = new GraphicalAudio();
    else
#endif
      module = new SimpleAudio();

    static_cast<Audio *>(module)->load_radio_stations();

    Themes *themes = S_Themes::get_instance();

    std::string text;

    // add audio_hd button
    text = dgettext("mms-audio", "Play audio from harddrive");

    if (radio_conf->p_radio() && !conf->p_media())
      text = dgettext("mms-audio", "Play audio");
    features.push_back(startmenu_item(text, "audio_hd", themes->startmenu_music_dir, 0,
				      &audio_hd));

    if (conf->p_media()) {
      // add cd button
      text = dgettext("mms-audio", "Play an audio cd");
      features.push_back(startmenu_item(text, "audio_disc", themes->startmenu_music_cd, 1,
					&audio_cd));
    }

    if (radio_conf->p_radio()) {
      // add radio button
      features.push_back(startmenu_item(dgettext("mms-audio", "Play Internet Radio"), "audio_radio",
					themes->startmenu_radio, 2,
					&audio_radio));
    }
  }

  ~AudioPlugin()
  {
    (static_cast<Audio *>(module))->save_runtime_settings();
    delete module;
    module = 0;
  }

  std::string plugin_name() const { return dgettext("mms-audio", "Audio"); }

  int priority() const { return 1; }
  unsigned long capability() const { return 0; }
};

MMS_PLUGIN(AudioPlugin)
