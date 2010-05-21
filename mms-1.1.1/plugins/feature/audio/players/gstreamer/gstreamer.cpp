//#define GST_DIRTY_HACK

#include "gstreamer.hpp"

#include "audio_s.hpp"
#include "config.hpp"
#include "global_options.hpp"
#include "updater.hpp"

#include <string>

#include <iostream>
#include <sstream>


#ifdef GST_DIRTY_HACK
struct _GstBusPrivate
{
  guint num_sync_message_emitters;
  GCond *queue_cond;
};
#endif

static int stop_status = 0;

using std::string;

string Gst::cd_track_path(int track_nr)
{
  std::ostringstream tmp;

  tmp << "cdda://" << track_nr;

  return tmp.str();
}

int Gst::is_playing()
{
  Audio_s *audio_state = S_Audio_s::get_instance();

  if (audio_state->p_pause())
    return 1;

  GstState state;
  gst_element_get_state(player, &state, 0, 0);

  if ( (state == GST_STATE_PLAYING || state == GST_STATE_PAUSED))
    return 1;
  else
    return 0;
}

int Gst::is_buffering()
{
  return 0;
}

int Gst::is_mute()
{
  return (old_volume != 0);
}

void Gst::gather_info()
{
  GstFormat fmt = GST_FORMAT_TIME;
  gint64 pos = 0, len = 0;

 if (gst_element_query_position(player, &fmt, &pos) &&
     gst_element_query_duration(player, &fmt, &len)) {
   total_time = len / GST_SECOND;
   cur_time = pos / GST_SECOND;
 }
}

void Gst::setfile(const Simplefile& file)
{
  string path;

  if ((file.path.find("cdda://") != string::npos) || file.type == "web")
    path = file.path;
  else
    path = "file://" + file.path;

  g_object_set(G_OBJECT (player), "uri", path.c_str(), NULL);
}

void Gst::addfile(const Simplefile &file)
{
  if (!check_media(file))
    return;

  stop_player();

  setfile(file);

  gst_element_set_state(player, GST_STATE_PLAYING);
  Audio_s *audio_state = S_Audio_s::get_instance();

  audio_state->set_playing(true);
  audio_state->set_pause(false);
}

void Gst::play()
{
  Audio_s *audio_state = S_Audio_s::get_instance();

  if (audio_state->p_pause()) {
    gst_element_set_state(player, GST_STATE_PLAYING);
    audio_state->set_pause(false);
  } else {
    setfile(cur_nr);
    setpos(0);
    gst_element_set_state(player, GST_STATE_PLAYING);

    S_Audio_s::get_instance()->set_playing(true);
  }
}

void Gst::stop_player()
{
  Audio_s *audio_state = S_Audio_s::get_instance();
  audio_state->set_playing(false);

  gst_element_set_state(player, GST_STATE_READY);
  gst_element_set_state (GST_ELEMENT (audiosink), GST_STATE_NULL);//unlock
								  //audiodevice

  audio_state->p->set_title("");
  audio_state->p->set_artist("");
  audio_state->p->set_album("");
}

void Gst::pause()
{
  Audio_s *audio_state = S_Audio_s::get_instance();

  GstState state;

  if (gst_element_get_state(player, &state, 0, 0) &&
      state != GST_STATE_PAUSED) {
    gst_element_set_state(player, GST_STATE_PAUSED);
    audio_state->set_pause(true);
  } else {
    gst_element_set_state(player, GST_STATE_PLAYING);
    audio_state->set_pause(false);
  }
}

void Gst::ff()
{
  gather_info();

  if (!((cur_time + 10) > total_time))
    setpos(cur_time + 10);
}

void Gst::fb()
{
  gather_info();

  if (cur_time == 0)
    return;

  if ((cur_time - 10) < 0)
    setpos(0);
  else
    setpos(cur_time - 10);
}

int Gst::getpos()
{
  return cur_time;
}

void Gst::setpos(int p)
{
  gst_element_seek(player, 1.0, GST_FORMAT_TIME,
		   GST_SEEK_FLAG_FLUSH, GST_SEEK_TYPE_SET,
		   p * GST_SECOND, GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE);
}

void Gst::mute()
{
  int vol = getvol();

  if (vol == 0) {
    setvol(old_volume);
    old_volume = 0;
  } else {
    old_volume = vol;
    setvol(0);
  }
}

void Gst::volup()
{
  int vol = getvol() + 4;
  if (vol > 100)
    vol = 100;
  setvol(vol);
}

void Gst::voldown()
{
  int vol = getvol() - 4;
  if (vol < 0)
    vol = 0;
  setvol(vol);
}

void Gst::setvol(int volume)
{
  g_object_set (G_OBJECT(player), "volume",
		(gdouble) (1. * volume / 100), NULL);
}

int Gst::getvol()
{
  gdouble vol;

  g_object_get (G_OBJECT(player), "volume", &vol, NULL);

  return (gint) (vol * 100 + 0.5);
}

bool Gst::supports_rtp() const
{
  // FIXME: does it?
  return false;
}

void Gst::reconfigure()
{
  deconfigure_device();
  configure_device();
}

void Gst::collect_info(const string& filename)
{
  Simplefile s;
  s.path = filename;
  s.type = "file";

  setfile(s);
  gather_info();
}

bool Gst::loaded()
{
  return is_loaded;
}

void Gst::release_device()
{
  stop();

  deconfigure_device();
  //gst_object_unref (GST_OBJECT (player));
  is_loaded = false;
}

void Gst::restore_device()
{
  configure_device();
  //player = gst_element_factory_make("playbin", "play");
  is_loaded = true;
}

static void found_tag_cb(GstElement*, GstElement*, GstTagList* taglist, gpointer)
{
  Audio_s *audio_state = S_Audio_s::get_instance();

  char* data = NULL;

  if (gst_tag_list_get_string(taglist, GST_TAG_TITLE, &data) && data)
    audio_state->p->set_title(data);

  if (gst_tag_list_get_string(taglist, GST_TAG_ARTIST, &data) && data)
    audio_state->p->set_artist(data);

  if (gst_tag_list_get_string(taglist, GST_TAG_ALBUM, &data) && data)
    audio_state->p->set_album(data);
}

void process_tag(const GstTagList *taglist, const gchar *tag)
{
  Audio_s *audio_state = S_Audio_s::get_instance();

  char* data = NULL;

  if (gst_tag_list_get_string(taglist, GST_TAG_TITLE, &data) && data)
    audio_state->p->set_title(data);

  if (gst_tag_list_get_string(taglist, GST_TAG_ARTIST, &data) && data)
    audio_state->p->set_artist(data);

  if (gst_tag_list_get_string(taglist, GST_TAG_ALBUM, &data) && data)
    audio_state->p->set_album(data);
}

bool bus_cb(GstBus *bus, GstMessage *message, void*)
{
  switch (GST_MESSAGE_TYPE(message)) {
  case GST_MESSAGE_TAG: {
    GstTagList *tags;
    gst_message_parse_tag(message, &tags);

    gst_tag_list_foreach(tags, (GstTagForeachFunc)process_tag, 0);
    gst_tag_list_free(tags);
    break;
  }
  case GST_MESSAGE_EOS: {
    stop_status = 1;
    break;
  }
  case GST_MESSAGE_ERROR: {
    stop_status = 1;
    gchar *debug;
    GError *err;
    gst_message_parse_error (message, &err, &debug);
    g_free (debug);
    g_print ("Error: %s\n", err->message);
    g_error_free (err);
    break;
    }
  default:
    break;
  }

  gst_message_unref(message);
  return true;
}

void Gst::check_bus()
{
  if (player == NULL )
    return;

  if (stop_status ) {
    gst_element_set_state(player, GST_STATE_READY);

    if (gst_element_get_state (player, NULL, NULL, 0) != GST_STATE_CHANGE_SUCCESS){
      fprintf(stderr, "Gstreamer: failed to change state\n");
      return;
    }

    fprintf(stderr, "Gstreamer: succeded in changing state\n");
    play_next_track = true;
    stop_status = 0;
  }

  GstBus *bus = gst_element_get_bus(player);
  assert(bus != NULL);
#ifdef GST_DIRTY_HACK
  GstMessage *msg = dirty_gst_bus_timed_pop(bus, GST_SECOND/20);
#else
  GstMessage *msg = gst_bus_timed_pop(bus, GST_SECOND/20);
#endif
  if (msg != NULL)
    bus_cb(bus, msg, NULL);
}

int Gst::check_bus_interval()
{
  return 200; // ms
}

void Gst::configure_device()
{
  Config *conf = S_Config::get_instance();
  GlobalOptions *go = S_GlobalOptions::get_instance();

  global_audio_device gad = go->get_audio_device_audio();

  string audio_sink = get_audiosink(gad.dev_output);
  string audio_dev = gad.dev_path;

  // Setting aplication name for Pulseaudio Mixer
  g_set_application_name("MMS");

  gst_init (NULL, NULL);

  player = gst_element_factory_make("playbin", "play");
  assert(player != NULL);
  audiosink = gst_element_factory_make (audio_sink.c_str(), "audiosink");
  assert(audiosink != NULL);
  if (audio_sink != "pulsesink")
    g_object_set (G_OBJECT (audiosink), "device", audio_dev.c_str(), NULL);
  g_object_set (G_OBJECT(player), "audio-sink", audiosink, NULL);
}

std::string Gst::get_audiosink(const std::string& output)
{
  if (output == "pulse")
    return "pulsesink";
  else if (output == "oss")
    return "osssink";
  else
    return "alsasink";
}

void Gst::deconfigure_device()
{
  stop_player();

  if (player != NULL)
    gst_object_unref (GST_OBJECT (player));
  if (audiosink != NULL)
    gst_object_unref (GST_OBJECT (audiosink));
}

Gst::Gst()
  : AudioPlayer("", "", "", 0, 0, 0), is_loaded(true), inited(false)
{
#ifdef GST_DIRTY_HACK
  fprintf(stderr, "\n\n\nGST_DIRTY_HACK is active, which is a dirty hack(tm)\n");
  fprintf(stderr, "Please, don't use it unless the installed gstreamer package is old and does not support\n");
  fprintf(stderr, "gst_bus_timed_pop(), and even if this is the case, please consider upgrading to a less ancient\n");
  fprintf(stderr, "version of gstreamer (0.10.12 or newer)\n");
  fprintf(stderr, "Do not report bugs about the gstreamer plugin if you see this warning, because\n");
  fprintf(stderr, "they won't be accepted.\nDid I mention you should upgrade the gstreamer package?\n\n\n\n");
#endif
}

void Gst::init()
{
  configure_device();

  // updater
  S_ScreenUpdater::get_instance()->timer.add(TimeElement("gst check bus",
							 boost::bind(&Gst::check_bus_interval, this),
							 boost::bind(&Gst::check_bus, this)));

  inited = true;
}

Gst::~Gst()
{
  if (inited)
    deconfigure_device();
}

#ifdef GST_DIRTY_HACK
GstMessage * Gst::dirty_gst_bus_timed_pop (GstBus * bus, GstClockTime timeout){
  GstMessage *message;
  GTimeVal *timeval, abstimeout;
  if (!bus)
    return NULL;
  bool gotsomething = false;
  g_mutex_lock (bus->queue_lock);
  g_get_current_time (&abstimeout);
  if (timeout == GST_CLOCK_TIME_NONE) {//wait forever and ever again
    timeval = NULL;
  }
  else{
    g_time_val_add (&abstimeout, timeout/1000);
    timeval = &abstimeout;
  }

  while(true){
    message = reinterpret_cast<GstMessage *>(g_queue_pop_head (bus->queue));
    if (message){//we got a message...
      g_mutex_unlock (bus->queue_lock);
      return message;
    }
    else if(timeout == 0){// we exit immediately, no wait
      g_mutex_unlock (bus->queue_lock);
      return NULL;
    }

    //we gotta wait....
    if (!g_cond_timed_wait (bus->priv->queue_cond, bus->queue_lock, timeval)){ //we timed out....
      g_mutex_unlock (bus->queue_lock);
      return NULL;
    }
    //recheck message...
  }

}
#endif
