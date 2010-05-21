//#define DLOG
#include <cerrno>
#include <sstream>
#include "xine.hpp"
#include "global_options.hpp"
#include "playlist_parser.hpp"

Xine::Xine() :
  AudioPlayer("", "", "", 0, 0, 0), 
  running(true),
  initted(false),
  xine_request(_XINE_REQUEST_NONE),
  stream(NULL),
  vo_port(NULL),
  ao_port(NULL),
  event_queue(NULL),
  quitting(false),
  seamless(false),
  start_paused(false)
  {
#ifdef use_nls
  // gettext
  setlocale(LC_ALL, ""); /* set from the environment variables */
  bindtextdomain("mms-audio-xine", mms_prefix"/share/locale");
  bind_textdomain_codeset("mms-audio-xine", nl_langinfo(CODESET));
#endif
  retry_track = false;

  pthread_mutex_init(&stream_mutex, NULL);
  pthread_cond_init(&stream_switch, NULL);
  }
void Xine::init(){

  /* make sure options are loaded and updated */
  xine = xine_new();
  configfile = xine_get_homedir();
  configfile += "/.xine/config";
  S_Global::get_instance()->register_reconfigure(boost::bind(&AudioPlayer::reconfigure, this));
  xine_config_load(xine, configfile.c_str());
  xine_init(xine);
  cd_manager = S_Cd::get_instance();
  cd_device = cd_manager->get_device();
  make_new_stream();
  /* start run thread */
  pthread_create(&thread_loop, NULL, (void *(*)(void *))(pre_run), this);
  }

Xine::~Xine(){
  pthread_mutex_lock(&stream_mutex);
  quitting = true;
  pthread_cond_broadcast(&stream_switch);
  pthread_mutex_unlock(&stream_mutex);
  if (!initted)
    return;
  pthread_join(thread_loop, NULL);
  pthread_mutex_lock(&stream_mutex);
  if(stream){
    xine_close(stream);
    xine_event_dispose_queue(event_queue);
    event_queue = NULL;
    xine_dispose(stream);
    stream = NULL;
  }
  if (ao_port)
    xine_close_audio_driver(xine, ao_port);
  ao_port = NULL;
  xine_exit(xine);
  /* booo bye */
}

bool Xine::make_new_stream(){

  if (stream)
    xine_dispose(stream);

  std::string ao_driver = "auto";
  GlobalOptions * go =  S_GlobalOptions::get_instance();
  global_audio_device gad = go->get_audio_device_audio();
  if (gad.dev_output != ""){
    ao_driver = gad.dev_output;
  }

  if (ao_port)
    xine_close_audio_driver(xine, ao_port);
  /* fprintf(stderr, "Audio driver: %s (%d)\n", ao_driver.c_str(), gad.pos); */

  ao_port = xine_open_audio_driver(xine, ao_driver.c_str(), NULL);

  if (ao_port == NULL){

    DialogWaitPrint pdialog(4000);
    pdialog.add_line(dgettext("mms-audio-xine", "Audio device unavailable: ") + ao_driver);
    pdialog.print();
    return false;
  }

  /* Xine uses its own config file for a lot of stuff */
   xine_cfg_entry_t config;

  /* for some reason this doesn't work for someone

  if (!xine_config_lookup_entry(xine, "input.cdda_device", &config)) {
    fprintf(stderr, "Xine: Can't set cdda device\n");
  }
  else{
    std::string str = S_Cd::get_instance()->get_device();
    config.str_value =  const_cast<char*>(str.c_str());
    xine_config_update_entry(xine, &config);
  } */

  /* Disable CDDB, we have our own CDDB library  */
  if (xine_config_lookup_entry(xine, "media.audio_cd.use_cddb", &config)){
    config.num_value = 0;
    xine_config_update_entry(xine, &config);
  }

  stream = xine_stream_new(xine, ao_port, NULL);

  if (stream == NULL){
    xine_close_audio_driver(xine, ao_port);
    ao_port = NULL;
    return false;
  }

  /* xine_set_param(stream, XINE_PARAM_VERBOSITY, XINE_VERBOSITY_DEBUG); */

  if (event_queue)
    xine_event_dispose_queue(event_queue);

  xine_set_param( stream, XINE_PARAM_METRONOM_PREBUFFER, 6000 );
  xine_set_param( stream, XINE_PARAM_IGNORE_VIDEO, 1 );
#ifdef XINE_PARAM_EARLY_FINISHED_EVENT
#ifdef XINE_PARAM_GAPLESS_SWITCH
  if (xine_check_version(1,1,1))
    xine_set_param(stream, XINE_PARAM_EARLY_FINISHED_EVENT, 1);

#ifdef XINE_PARAM_DELAY_FINISHED_EVENT
  xine_set_param(stream, XINE_PARAM_DELAY_FINISHED_EVENT, 0);
#endif
 
#endif
#endif

  event_queue = xine_event_new_queue(stream);
  mixer_cap = false;
  is_muted = xine_get_param(stream, XINE_PARAM_AUDIO_MUTE);
  if (is_muted == -1){
    is_muted = xine_get_param(stream, XINE_PARAM_AUDIO_AMP_MUTE);
    mixer_cap = true;
  }

  xine_event_create_listener_thread(event_queue, &Xine::event_listener, reinterpret_cast<void*>(this));
  return true;
}

bool Xine::validate_stream(){
  if(!stream)
    return make_new_stream();
  return true;
}

void * Xine::pre_run(void * ptr){
  Xine * thisobj = reinterpret_cast<Xine*>(ptr);
  thisobj->initted = true;
  thisobj->run();
  return 0;
}

void Xine::run(){
  /* this is the main thread */
  playing = false;
  seamless = false;
  pthread_mutex_lock(&stream_mutex);
  struct timespec ts = time_helper::compute_interval(5000);
  Audio_s *audio_state = S_Audio_s::get_instance();
  volume = 0;
  if(validate_stream())
    volume = xine_get_param(stream, XINE_PARAM_AUDIO_VOLUME);
  old_volume = volume;

  while(!quitting){
    xine_request = _XINE_REQUEST_NONE;
    int ret = pthread_cond_timedwait(&stream_switch, &stream_mutex, &ts);
    if (!running || quitting){
      ts = time_helper::compute_interval(5000);
      continue;
    }
    if (ret != ETIMEDOUT && xine_request == _XINE_REQUEST_NONE)
      continue; /* we're not done waiting */
/*
    if (xine_request != _XINE_REQUEST_NONE)
      fprintf (stderr, "Loop received event request (%d)\n", xine_request);
*/
    switch(xine_request){

    case _XINE_REQUEST_PLAY_AT:
    case _XINE_REQUEST_PLAY:{
      retry_track = false;			      
      if(!validate_stream())
        break;
#ifdef XINE_PARAM_EARLY_FINISHED_EVENT
      if (seamless && xine_check_version(1,1,1) && oldfile.type != "web" && file.type != "web"){
	xine_set_param(stream, XINE_PARAM_GAPLESS_SWITCH, 1);
      }
//      else{
//	xine_close(stream);
//      }
	seamless = false;
#endif
      oldfile = file;

      /* fprintf(stderr, "file.path %s\n", file.path.c_str()); */
      std::string _str = file.path;
      if (file.type == "media-track"){
        std::string _path = "cdda:/" + cd_device + "/";
        _str = regex_tools::regex_replace(_str, _path.c_str(), "^cdda:/", false, false);
        /* fprintf(stderr, "%s\n", _str.c_str()); */
      }
      else {
        vector<string> urls = PlaylistParser::resolve_playlist(_str);
        if (!urls.empty())
          _str = urls[0]; /* TODO: Handle multiple playlist entries here */
      }

      if(!xine_open(stream, _str.c_str())){
#ifdef XINE_PARAM_GAPLESS_SWITCH
	if (xine_check_version(1,1,1))
	  xine_set_param(stream, XINE_PARAM_GAPLESS_SWITCH, 0);
#endif
       break;
      }
      int len = 0;
      off_t play_at = 0;

      const bool has_audio     = xine_get_stream_info( stream, XINE_STREAM_INFO_HAS_AUDIO );
      const bool audio_handled = xine_get_stream_info( stream, XINE_STREAM_INFO_AUDIO_HANDLED );
      if (xine_request == _XINE_REQUEST_PLAY_AT){
        xine_get_pos_length(stream, 0, 0, &len);
        play_at = len > 0 ? static_cast<double>(cur_time*65535)/(len/1000) : 0;
      }
      else
        cur_time = 0;
      _cur_time = cur_time;

      total_time = 0;
      _total_time = 0;

      if ((has_audio || file.type == "web") && audio_handled && xine_play(stream, play_at, cur_time * 1000)){
        playing = true;
        //gather_info_internal();
        audio_state->set_playing(true);
        buffering_state = 0;
      }
      else{
        playing = false;
        audio_state->set_playing(false);
        xine_close(stream);
      }
      audio_state->get_audio()->update_playlist_view();
#ifdef XINE_PARAM_EARLY_FINISHED_EVENT
#ifdef XINE_PARAM_GAPLESS_SWITCH
      if (xine_check_version(1,1,1)){
	if (!audio_state->get_audio()->next_helper(false).path.empty()){
	  xine_set_param(stream, XINE_PARAM_EARLY_FINISHED_EVENT, 1);
/*	  fprintf(stderr, "Not last track\n"); */
#ifdef XINE_PARAM_DELAY_FINISHED_EVENT
	  xine_set_param(stream, XINE_PARAM_DELAY_FINISHED_EVENT, 0);
#endif
	}
	else {
	  /* If we don't disable XINE_PARAM_DELAY_FINISHED_EVENT for the last track in the playlist,
	   * xine won't play the last couple of seconds of it */
	  xine_set_param(stream, XINE_PARAM_EARLY_FINISHED_EVENT, 0);
/*	  fprintf(stderr, "Last track\n"); */ 
	}
      }
#endif
#endif
      if (start_paused){
	start_paused = false;
	audio_state->set_pause(true);
      }
      else
	audio_state->set_pause(false);

      break;
    }
    case _XINE_REQUEST_STOP:
    case _XINE_REQUEST_RELEASE_DEVICE:
      retry_track = false;
      seamless = false;
      if(stream){
        xine_close(stream);
        xine_event_dispose_queue(event_queue);
	event_queue = NULL;
        xine_dispose(stream);
        stream = NULL;
      }
      playing = false;
      audio_state->set_playing(false);
      if (xine_request == _XINE_REQUEST_RELEASE_DEVICE){
	xine_close_audio_driver(xine, ao_port);
	ao_port = NULL;
      }
      break;

    case _XINE_REQUEST_PAUSE:
      if(!validate_stream()||!playing)
        break;

      if(xine_get_param(stream, XINE_PARAM_SPEED) != XINE_SPEED_PAUSE) {
        xine_set_param(stream, XINE_PARAM_SPEED, XINE_SPEED_PAUSE);
        audio_state->set_pause(true);
      }
      else {
        xine_set_param(stream, XINE_PARAM_SPEED, XINE_SPEED_NORMAL);
        audio_state->set_pause(false);
      }
      break;

    case _XINE_REQUEST_FF:{
      if(!playing || !validate_stream())
	break;
      gather_info_internal();

      if (_cur_time + 5 > _total_time)
	break;
      _cur_time +=5;
      off_t pos = _total_time > 0 ? static_cast<double>(_cur_time*65535)/_total_time : 0;

      xine_play(stream, pos, _cur_time*1000);
      audio_state->set_pause(false);
      break;
    }

    case _XINE_REQUEST_FB:{
      if(!playing || !validate_stream())
	break;
      gather_info_internal();

      if (static_cast<int>(_cur_time) - 5 < 0)
	break;
      _cur_time -=5;
      off_t pos = _total_time > 0 ? static_cast<double>(_cur_time*65535)/_total_time : 0;
      xine_play(stream, pos, _cur_time*1000);
      audio_state->set_pause(false);
      break;
    }

    default:
      break;
    }

    if (ret == ETIMEDOUT && playing)
      gather_info_internal();

    if (playing)
      ts = time_helper::compute_interval(500);
    else
      ts = time_helper::compute_interval(5000);
  }

  if(stream){
    xine_close(stream);
    xine_event_dispose_queue(event_queue);
    event_queue = NULL;
    xine_dispose(stream);
    stream = NULL;
  }
  playing = false;
  if (ao_port)
    xine_close_audio_driver(xine, ao_port);
  ao_port = NULL;
  pthread_mutex_unlock(&stream_mutex);
  /* bye */
}

void Xine::event_listener(void *user_data, const xine_event_t *event)
{
  Xine* xine = reinterpret_cast<Xine*>(user_data);
  Audio_s *audio_state = S_Audio_s::get_instance();
  /* fprintf(stderr, "got event type #%d\n", event->type); */

  switch(event->type) {

  case XINE_EVENT_QUIT:
    xine->playing = false;
    break;

  case XINE_EVENT_UI_PLAYBACK_FINISHED:
    pthread_mutex_lock(&xine->stream_mutex);
    if (xine->retry_track){
      xine->retry_track = false;
      xine->xine_request = _XINE_REQUEST_PLAY;
      pthread_cond_broadcast(&xine->stream_switch);
      pthread_mutex_unlock(&xine->stream_mutex);
    }
    else{
      xine->playing = false;
      xine->play_next_track = true;
      xine->seamless = true;
      pthread_mutex_unlock(&xine->stream_mutex);
      audio_state->get_audio()->next_audio_track();
    }
    //audio_state->get_audio()->update_playlist_view();
    break;
  case XINE_EVENT_PROGRESS:
  {
    xine_progress_data_t *pevent = reinterpret_cast<xine_progress_data_t *>(event->data);
      /* fprintf(stderr, "%s [%d%%]\n", pevent->description, pevent->percent); */
      audio_state->p->buffering_state = pevent->percent;
  }
  break;

#ifdef XINE_EVENT_MRL_REFERENCE_EXT
  case XINE_EVENT_MRL_REFERENCE_EXT:
  {
     std::string real_mrl = static_cast<xine_mrl_reference_data_ext_t*>(event->data)->mrl;
     pthread_mutex_lock(&xine->stream_mutex);
     dprintf("Xine: XINE_EVENT_MRL_REFERENCE_EXT -> '%s'\n", real_mrl);
     xine->file.path = real_mrl;
     xine->retry_track = true;
     pthread_mutex_unlock(&xine->stream_mutex);
  }
  break;
#endif

  case XINE_EVENT_UI_MESSAGE:
  {
    xine_ui_message_data_t *msg = reinterpret_cast<xine_ui_message_data_t*>(event->data);
    std::string err_m;
    std::string str = "Unknown event";
    switch(msg->type){

    case XINE_MSG_GENERAL_WARNING:
      err_m = dgettext("mms-audio-xine", "Xine engine warning");
      break;
    case XINE_MSG_UNKNOWN_HOST:
    case XINE_MSG_UNKNOWN_DEVICE:
    case XINE_MSG_NETWORK_UNREACHABLE:
    case XINE_MSG_CONNECTION_REFUSED:
    case XINE_MSG_FILE_NOT_FOUND:
    case XINE_MSG_READ_ERROR:
    case XINE_MSG_LIBRARY_LOAD_ERROR:
    case XINE_MSG_ENCRYPTED_SOURCE:
    case XINE_MSG_AUDIO_OUT_UNAVAILABLE:
    case XINE_MSG_PERMISSION_ERROR:
    case XINE_MSG_FILE_EMPTY:
      err_m = dgettext("mms-audio-xine", "Xine engine error");
      break;

    default:
      err_m = dgettext("mms-audio-xine","Xine engine message");
      break;
    }
    if(msg->explanation)
      str = reinterpret_cast<char*>(msg) + msg->explanation;

    if(msg->parameters){
      str += " - ";
      str += reinterpret_cast<char*>(msg) + msg->parameters;
    }

    DialogWaitPrint pdialog(4000);
    pdialog.add_line(err_m);
    pdialog.add_line(str);
    pdialog.print();
    fprintf (stderr, "%s\n%s\n", err_m.c_str(), str.c_str());
  }
  default:
    {
      break; /* nothing */
    }
  }
}

int Xine::is_playing(){
  Audio_s *audio_state = S_Audio_s::get_instance(); 
  if (audio_state->p_pause()) 
    return 1;
  return playing;
}

int Xine::is_buffering(){
  return 0; /* for some reason mms works better if xine doesn't notify mms its buffering state */

  //return (buffering_state > 0 && buffering_state != 100);
}

int Xine::is_mute(){
  return (is_muted > 0);
}

void Xine::gather_info(){
  if (!playing || !stream)
    return;

  pthread_mutex_lock(&stream_mutex);
  title = _title;
  album = _album;
  artist = _artist;
  cur_time = _cur_time;
  total_time = _total_time;
  //fprintf(stderr, "Gather info (position %d)\n", cur_time);
  pthread_mutex_unlock(&stream_mutex);
}

void Xine::gather_info_internal(){
  if (quitting)
    return;
  if (!validate_stream())
    return;
  if (file.type == "media-track"){
    _title = "";
    _artist = "";
    _album = "";
  }
  else{
    const char *meta_info = xine_get_meta_info(stream, XINE_META_INFO_TITLE);
    _title = (meta_info) ? meta_info : "";
    meta_info = xine_get_meta_info(stream, XINE_META_INFO_ARTIST);
    _artist = (meta_info) ? meta_info : "";
    meta_info = xine_get_meta_info(stream, XINE_META_INFO_ALBUM);
    _album = (meta_info) ? meta_info : "";
    meta_info = xine_get_meta_info(stream, XINE_META_INFO_INPUT_PLUGIN);
    if (meta_info) /* If it is not a local file, _artist = _album, so that mms doesn't just show the filename */
      if (strcasecmp(meta_info, "pluginfile") != 0){
	_artist = _album;
	_album = "";
      }
  }


  int position_, length_ = 0;
  for (int t = 0; t < 5; t++){ /* sometimes xine doesn't return a valid position, especially after a seek
                                while playing an audio CD (it sucks) */
    xine_get_pos_length(stream, 0, &position_, &length_);
    if (position_ > 0)
      break;
    mmsUsleep(50000);
  }

  if (position_ > 0){
    _cur_time = position_/1000;
    _total_time = length_/1000;
  }
/*
  else
  {
    _title = "";
    _artist ="";
    _album = "Buffering...";
  }
*/ 
  //fprintf(stderr, "Gather info internal (position %d)\n", _cur_time);
}

void Xine::addfile(const Simplefile &f){
  if (quitting)
    return;
  dprintf("xine addfile() '%s'\n", f.path.c_str());
  pthread_mutex_lock(&stream_mutex);
  cd_device = cd_manager->get_device(); /* Update CD device, which migh have changed */
  /* fprintf(stderr, "Will play '%s'\n", f.path.c_str()); */
  file = f;
  xine_request = _XINE_REQUEST_PLAY;
  pthread_cond_broadcast(&stream_switch);
  pthread_mutex_unlock(&stream_mutex);
}

void Xine::play(){
  if (quitting)
    return;
  dprintf("xine play()\n");
  pthread_mutex_lock(&stream_mutex);
  cd_device = cd_manager->get_device(); /* Update CD device, which might have changed */
  if (!cur_nr.path.empty())
    file = cur_nr;
  start_paused = false;
  xine_request = _XINE_REQUEST_PLAY;
  pthread_cond_broadcast(&stream_switch);
  pthread_mutex_unlock(&stream_mutex);
}

void Xine::setpos(int p){
  if (quitting)
    return;
  dprintf("xine setpos() %d\n", p);
  pthread_mutex_lock(&stream_mutex);
  xine_request = _XINE_REQUEST_PLAY_AT;
  cur_time = p;
  start_paused = false;
  pthread_cond_broadcast(&stream_switch);
  pthread_mutex_unlock(&stream_mutex);
}

void Xine::resume_playback(const Simplefile &f, int p, bool paused){
  if (quitting)
    return;
  dprintf("xine resume_playback() '%s', %d, %d\n", f.path.c_str(), p, paused);
  pthread_mutex_lock(&stream_mutex);
  xine_request = _XINE_REQUEST_PLAY_AT;
  file = f;
  cur_time = p;
  start_paused = paused;
  pthread_cond_broadcast(&stream_switch);
  pthread_mutex_unlock(&stream_mutex);
}

void Xine::stop_player(){
  if (quitting)
    return;

  dprintf("xine stop_player()\n");
  pthread_mutex_lock(&stream_mutex);
  xine_request = _XINE_REQUEST_STOP;
  pthread_cond_broadcast(&stream_switch);
  pthread_mutex_unlock(&stream_mutex);
}

void Xine::pause(){
  if (quitting)
    return;
  dprintf("xine pause()\n");
  pthread_mutex_lock(&stream_mutex);
  xine_request = _XINE_REQUEST_PAUSE;
  pthread_cond_broadcast(&stream_switch);
  pthread_mutex_unlock(&stream_mutex);
}

void Xine::ff(){
  if (quitting)
    return;
  pthread_mutex_lock(&stream_mutex);
  xine_request = _XINE_REQUEST_FF;
  pthread_cond_broadcast(&stream_switch);
  pthread_mutex_unlock(&stream_mutex);
}

void Xine::fb(){
  if (quitting)
    return;
  pthread_mutex_lock(&stream_mutex);
  xine_request = _XINE_REQUEST_FB;
  pthread_cond_broadcast(&stream_switch);
  pthread_mutex_unlock(&stream_mutex);
}

void Xine::collect_info(const std::string &f){
  if (quitting)
    return;
  pthread_mutex_lock(&stream_mutex);
  if (!playing || file.path != f){
    /* Exceptionally we don't let the loop handle this */
    playing = false;
    if(!validate_stream()){
      pthread_mutex_unlock(&stream_mutex);
      return;
    }

    xine_close(stream); /* you never know */
    xine_event_dispose_queue(event_queue);
    event_queue = NULL;
    if(!xine_open(stream, f.c_str())){
      pthread_mutex_unlock(&stream_mutex);
      return;
    }

    const bool has_audio     = xine_get_stream_info( stream, XINE_STREAM_INFO_HAS_AUDIO );
    const bool audio_handled = xine_get_stream_info( stream, XINE_STREAM_INFO_AUDIO_HANDLED );

    if (has_audio && audio_handled && xine_play(stream, 0, 0)){
      gather_info_internal();
      xine_close(stream);
    }
    else{
      pthread_mutex_unlock(&stream_mutex);
      return;
    }
  }
  else{
    if(!validate_stream()){
      pthread_mutex_unlock(&stream_mutex);
      return;
    }

    gather_info_internal();
  }
  title = _title;
  album = _album;
  artist = _artist;
  cur_time = _cur_time;
  total_time = _total_time;
  pthread_mutex_unlock(&stream_mutex);
}

void Xine::release_device(){
  if (quitting)
    return;
  pthread_mutex_lock(&stream_mutex);
  xine_request = _XINE_REQUEST_RELEASE_DEVICE;
  pthread_cond_broadcast(&stream_switch);
  pthread_mutex_unlock(&stream_mutex);
}

void Xine::mute(){
  if (quitting)
    return;
  pthread_mutex_lock(&stream_mutex);
  if(!validate_stream()){
    pthread_mutex_unlock(&stream_mutex);
    return;
  }
  is_muted = !is_muted;
  if (mixer_cap)
    xine_set_param(stream, XINE_PARAM_AUDIO_AMP_MUTE, is_muted);
  else
    xine_set_param(stream, XINE_PARAM_AUDIO_MUTE, is_muted);

  xine_set_param(stream, XINE_PARAM_AUDIO_VOLUME,
     (is_muted) ? 0 : volume);
  pthread_mutex_unlock(&stream_mutex);
}

void Xine::volup(){
  if (quitting)
    return;
  pthread_mutex_lock(&stream_mutex);
  if(!validate_stream()){
    pthread_mutex_unlock(&stream_mutex);
    return;
  }
  volume +=2;

  if (volume > 100)
    volume = 100;
  xine_set_param(stream, XINE_PARAM_AUDIO_VOLUME, volume);
  if (mixer_cap)
    xine_set_param(stream, XINE_PARAM_AUDIO_AMP_MUTE, 0);
  else
    xine_set_param(stream, XINE_PARAM_AUDIO_MUTE, 0);
  is_muted = 0;
  pthread_mutex_unlock(&stream_mutex);
}

void Xine::voldown(){
  if (quitting)
    return;
  pthread_mutex_lock(&stream_mutex);
  if(!validate_stream()){
    pthread_mutex_unlock(&stream_mutex);
    return;
  }
  volume -=2;

  if (volume < 0)
    volume = 0;
  xine_set_param(stream, XINE_PARAM_AUDIO_VOLUME, volume);
  if (!volume){
    if (mixer_cap)
      xine_set_param(stream, XINE_PARAM_AUDIO_AMP_MUTE, 1);
    else
      xine_set_param(stream, XINE_PARAM_AUDIO_MUTE, 1);
    is_muted = 1;
  }
  pthread_mutex_unlock(&stream_mutex);
}

int Xine::getvol(){
  return volume;
}

void Xine::setvol(int vol){
  if (quitting)
    return;
  pthread_mutex_lock(&stream_mutex);
  volume = vol;
  if(!validate_stream()){
    pthread_mutex_unlock(&stream_mutex);
    return;
  }
  xine_set_param(stream, XINE_PARAM_AUDIO_VOLUME, vol);
  pthread_mutex_unlock(&stream_mutex);
}

bool Xine::supports_rtp() const {return true;};

void Xine::reconfigure(){
  release_device();
  if(!playing)
    return;
  while (playing)
    mmsUsleep(100000);
  setpos(cur_time);
};

bool Xine::loaded(){return true;};

void Xine::restore_device(){};

int Xine::getpos(){return cur_time;};

std::string Xine::cd_track_path(int track_nr){
  return string_format::str_printf("cdda:/%d", track_nr);
}
