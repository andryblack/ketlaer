#include "audio.hpp"

#include "global_options.hpp"
#include "gettext.hpp"

using std::vector;
using std::string;

Audio::AudioOpts::AudioOpts()
{
#ifdef use_nls
  // gettext
  setlocale(LC_ALL, ""); /* set from the environment variables */
  bindtextdomain("mms-audio", mms_prefix"/share/locale");
  bind_textdomain_codeset("mms-audio", nl_langinfo(CODESET));
#endif

  AudioConfig *audio_conf = S_AudioConfig::get_instance();
  RadioConfig *radio_conf = S_RadioConfig::get_instance();

  // header
  header = "Audio Options";
  translated_header = dgettext("mms-audio", "Audio Options");
  save_name = "AudioOptions";

  // values

#ifdef use_inotify
  if (audio_conf->p_audio_dirs_no_watch().size() > 0) {
#else
  if (audio_conf->p_audio_dirs_no_watch().size() > 0 || audio_conf->p_audio_dirs().size() > 0) {
#endif
    // reload
    vector<string> none;
    reload_p = new Option(false, dgettext("mms-audio", "reload"), "reload", 0, none, none);
    val.push_back(reload_p);
  }

  // global options
  dir_order_p = go->dir_order();
  val.push_back(dir_order_p);

  if (conf->p_cdpath().size() > 1)
    val.push_back(go->cd_device);

  if (go->audio_device_audio->values.size() > 1)
    val.push_back(go->audio_device_audio);

  vector<string> repeat_values;

  repeat_values.push_back(dgettext("mms-audio", "no"));
  repeat_values.push_back(dgettext("mms-audio", "yes"));

  vector<string> english_repeat_values;

  english_repeat_values.push_back("no");
  english_repeat_values.push_back("yes");

  repeat_p = new Option(false, dgettext("mms-audio", "repeat"), "repeat", 0, repeat_values, english_repeat_values);
  val.push_back(repeat_p);

  vector<string> shutdown_values;

  shutdown_values.push_back(dgettext("mms-audio", "no"));
  shutdown_values.push_back(dgettext("mms-audio", "yes"));

  vector<string> english_shutdown_values;

  english_shutdown_values.push_back("no");
  english_shutdown_values.push_back("yes");

  shutdown_p = new Option(false, dgettext("mms-audio", "shutdown on finished"), "autoshutdown", 0, shutdown_values, english_shutdown_values);
  val.push_back(shutdown_p);

  vector<string> shuffle_values;

  shuffle_values.push_back(dgettext("mms-audio", "off"));
  shuffle_values.push_back(dgettext("mms-audio", "completely random"));
  shuffle_values.push_back(dgettext("mms-audio", "simple random"));

#ifdef use_imms
  if (audio_conf->p_graphical_audio_mode())
    shuffle_values.push_back(dgettext("mms-audio", "intelligent"));
#endif

  vector<string> english_shuffle_values;

  english_shuffle_values.push_back("off");
  english_shuffle_values.push_back("completely random");
  english_shuffle_values.push_back("simple random");
#ifdef use_imms
  if (audio_conf->p_graphical_audio_mode())
    english_shuffle_values.push_back("intelligent");
#endif

  shuffle_p = new Option(false, dgettext("mms-audio", "shuffle"), "shuffle", 0, shuffle_values, english_shuffle_values);
  val.push_back(shuffle_p);

  vector<string> lyrics_values;

  lyrics_values.push_back(dgettext("mms-audio", "no"));
  lyrics_values.push_back(dgettext("mms-audio", "yes"));

  vector<string> english_lyrics_values;

  english_lyrics_values.push_back("no");
  english_lyrics_values.push_back("yes");

  lyrics_p = new Option(false, dgettext("mms-audio", "show lyrics"), "lyrics", 0, lyrics_values, english_lyrics_values);
  val.push_back(lyrics_p);

  vector<string> time_mode_values;

  time_mode_values.push_back(dgettext("mms-audio", "from start"));
  time_mode_values.push_back(dgettext("mms-audio", "from end"));

  vector<string> english_time_mode_values;

  english_time_mode_values.push_back("from start");
  english_time_mode_values.push_back("from end");

  time_mode_p = new Option(false, dgettext("mms-audio", "time view"), "time view", 0, time_mode_values, english_time_mode_values);
  val.push_back(time_mode_p);

  if (radio_conf->p_radio()) {
    vector<string> bitrate_values;

    bitrate_values.push_back("0");
    bitrate_values.push_back("24");
    bitrate_values.push_back("48");
    bitrate_values.push_back("64");
    bitrate_values.push_back("96");
    bitrate_values.push_back("128");
    bitrate_values.push_back("192");
    bitrate_values.push_back("256");

    vector<string> english_bitrate_values;

    english_bitrate_values.push_back("0");
    english_bitrate_values.push_back("24");
    english_bitrate_values.push_back("48");
    english_bitrate_values.push_back("64");
    english_bitrate_values.push_back("96");
    english_bitrate_values.push_back("128");
    english_bitrate_values.push_back("192");
    english_bitrate_values.push_back("256");

    bitrate_p = new Option(false, dgettext("mms-audio", "minimum bitrate"), "bitrate", 0, bitrate_values, english_bitrate_values);
    val.push_back(bitrate_p);
  } else {
    vector<string> bitrate_values;
    bitrate_values.push_back(dgettext("mms-audio", "n/a"));

    vector<string> english_bitrate_values;
    english_bitrate_values.push_back("n/a");

    bitrate_p = new Option(false, dgettext("mms-audio", "minimum bitrate"), "bitrate", 0, bitrate_values, english_bitrate_values);
    val.push_back(bitrate_p);
  }
  
  if (audio_conf->p_graphical_audio_mode() && conf->p_idle_time() != 0) {
    vector<string> extract_meta_values;

    extract_meta_values.push_back(dgettext("mms-audio", "no"));
    extract_meta_values.push_back(dgettext("mms-audio", "yes"));

    vector<string> english_extract_meta_values;

    english_extract_meta_values.push_back("no");
    english_extract_meta_values.push_back("yes");

    extract_meta_p = new Option(false, dgettext("mms-audio", "bg metadata extraction"), "bg metadata extraction", 0, extract_meta_values, english_extract_meta_values);
    val.push_back(extract_meta_p);
  }
}

Audio::AudioOpts::~AudioOpts()
{
  AudioConfig *audio_conf = S_AudioConfig::get_instance();

  save();

  if (!audio_conf->p_graphical_audio_mode())
    delete reload_p;

  delete repeat_p;
  delete shutdown_p;
  delete shuffle_p;
  delete lyrics_p;
  delete time_mode_p;

  if (audio_conf->p_graphical_audio_mode() && conf->p_idle_time() != 0)
    delete extract_meta_p;
}
