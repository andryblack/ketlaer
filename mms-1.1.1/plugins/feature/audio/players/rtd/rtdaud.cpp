#include "rtdaud.hpp"

#include "audio_s.hpp"
#include "config.hpp"
#include "global_options.hpp"
#include "updater.hpp"

#include <string>

#include <iostream>
#include <sstream>

static int stop_status = 0;

using std::string;

string RtdAud::cd_track_path(int track_nr)
{
  std::ostringstream tmp;

  tmp << "cdda://" << track_nr;

  return tmp.str();
}

int RtdAud::is_playing()
{
  Audio_s *audio_state = S_Audio_s::get_instance();

  if (audio_state->p_pause())
    return 1;

#ifndef KETLAER
  RtdAudState state;
  gst_element_get_state(player, &state, 0, 0);

  if ( (state == GST_STATE_PLAYING || state == GST_STATE_PAUSED))
    return 1;
  else
#endif
    return 0;
}

int RtdAud::is_buffering()
{
  return 0;
}

int RtdAud::is_mute()
{
  return (old_volume != 0);
}

void RtdAud::gather_info()
{
#ifndef KETLAER
  RtdAudFormat fmt = GST_FORMAT_TIME;
  gint64 pos = 0, len = 0;

 if (gst_element_query_position(player, &fmt, &pos) &&
     gst_element_query_duration(player, &fmt, &len)) {
   total_time = len / GST_SECOND;
   cur_time = pos / GST_SECOND;
 }
#endif
}

void RtdAud::addfile(const Simplefile &file)
{
  if (!check_media(file))
    return;

  stop_player();


#ifndef KETLAER
  setfile(file);
  gst_element_set_state(player, GST_STATE_PLAYING);
#endif
  Audio_s *audio_state = S_Audio_s::get_instance();

  audio_state->set_playing(true);
  audio_state->set_pause(false);
}

void RtdAud::play()
{
  Audio_s *audio_state = S_Audio_s::get_instance();

#ifndef KETLAER
  if (audio_state->p_pause()) {
    gst_element_set_state(player, GST_STATE_PLAYING);
    audio_state->set_pause(false);
  } else {
    setfile(cur_nr);
    setpos(0);
    gst_element_set_state(player, GST_STATE_PLAYING);

    S_Audio_s::get_instance()->set_playing(true);
  }
#endif
}

void RtdAud::stop_player()
{
#ifndef KETLAER
  Audio_s *audio_state = S_Audio_s::get_instance();
  audio_state->set_playing(false);

  gst_element_set_state(player, GST_STATE_READY);
  gst_element_set_state (GST_ELEMENT (audiosink), GST_STATE_NULL);//unlock
								  //audiodevice

  audio_state->p->set_title("");
  audio_state->p->set_artist("");
  audio_state->p->set_album("");
#endif
}

void RtdAud::pause()
{
#ifndef KETLAER
  Audio_s *audio_state = S_Audio_s::get_instance();

  RtdAudState state;

  if (gst_element_get_state(player, &state, 0, 0) &&
      state != GST_STATE_PAUSED) {
    gst_element_set_state(player, GST_STATE_PAUSED);
    audio_state->set_pause(true);
  } else {
    gst_element_set_state(player, GST_STATE_PLAYING);
    audio_state->set_pause(false);
  }
#endif
}

void RtdAud::ff()
{
  gather_info();

  if (!((cur_time + 10) > total_time))
    setpos(cur_time + 10);
}

void RtdAud::fb()
{
  gather_info();

  if (cur_time == 0)
    return;

  if ((cur_time - 10) < 0)
    setpos(0);
  else
    setpos(cur_time - 10);
}

int RtdAud::getpos()
{
  return cur_time;
}

void RtdAud::setpos(int p)
{
#ifndef KETLAER
  gst_element_seek(player, 1.0, GST_FORMAT_TIME,
		   GST_SEEK_FLAG_FLUSH, GST_SEEK_TYPE_SET,
		   p * GST_SECOND, GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE);
#endif
}

void RtdAud::mute()
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

void RtdAud::volup()
{
  int vol = getvol() + 4;
  if (vol > 100)
    vol = 100;
  setvol(vol);
}

void RtdAud::voldown()
{
  int vol = getvol() - 4;
  if (vol < 0)
    vol = 0;
  setvol(vol);
}

void RtdAud::setvol(int volume)
{
#ifndef KETLAER
  g_object_set (G_OBJECT(player), "volume",
		(gdouble) (1. * volume / 100), NULL);
#endif
}

int RtdAud::getvol()
{
#ifndef KETLAER
  gdouble vol;

  g_object_get (G_OBJECT(player), "volume", &vol, NULL);

  return (gint) (vol * 100 + 0.5);
#else
  return 100;
#endif
}

bool RtdAud::supports_rtp() const
{
  // FIXME: does it?
  return false;
}

void RtdAud::reconfigure()
{
}

void RtdAud::collect_info(const string& filename)
{
  Simplefile s;
  s.path = filename;
  s.type = "file";

#ifndef KETLAER
  setfile(s);
#endif
  gather_info();
}

bool RtdAud::loaded()
{
  return is_loaded;
}

void RtdAud::release_device()
{
  stop();

  //gst_object_unref (GST_OBJECT (player));
  is_loaded = false;
}

void RtdAud::restore_device()
{
  is_loaded = true;
}

RtdAud::RtdAud()
  : AudioPlayer("", "", "", 0, 0, 0), is_loaded(true)
{
}

void RtdAud::init()
{
}

RtdAud::~RtdAud()
{
}

