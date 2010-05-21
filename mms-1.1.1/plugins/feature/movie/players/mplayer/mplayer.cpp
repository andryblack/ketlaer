#include "mplayer.hpp"

#include "graphical_movie.hpp"
#include "simple_movie.hpp"
#include "updater.hpp"

#include "global.hpp"

#include "renderer.hpp"

#include <fstream>

#ifdef use_nls
#include <langinfo.h>
#include <locale.h>
#endif

using std::string;

#define bsize 1024

Mplayer::Mplayer()
  : MoviePlayer(true, true, true, true), paused(false), running(false),
    in_fullscreen(false), pos(0), stop_playback_ok(true), broken_pipe(false)
{
#ifdef use_nls
  // gettext
  setlocale(LC_ALL, ""); /* set from the environment variables */
  bindtextdomain("mms-movie-mplayer", mms_prefix"/share/locale");
  bind_textdomain_codeset("mms-movie-mplayer", nl_langinfo(CODESET));
#endif

  ScreenUpdater *screen_updater = S_ScreenUpdater::get_instance();
  screen_updater->timer.add(TimeElement("mplayer status", boost::bind(&Mplayer::check_playback_timeout, this),
					boost::bind(&Mplayer::check_playback, this)));
  screen_updater->timer.deactivate("mplayer status");
  mplayer_conf = S_MplayerConfig::get_instance();

  opts.set_header("MPlayer");
  opts.set_save_name("MPlayerOptions");
  std::vector<std::string> val1;
  std::vector<std::string> val2;
  gen_head_opt_p = new Option (true, dgettext("mms-movie-mplayer", "General options"), "General options",
			       0, val1, val2);

  dvd_head_opt_p = new Option (true, dgettext("mms-movie-mplayer", "DVD options"), "DVD options",
			       0, val1, val2);

  vcd_head_opt_p = new Option (true, dgettext("mms-movie-mplayer", "VCD options"), "VCD options",
			       0, val1, val2);

  val1.push_back(dgettext("mms-movie-mplayer", "none"));
  val1.push_back("en");
  val1.push_back("da");
  val1.push_back("de");
  val1.push_back("es");
  val1.push_back("fi");
  val1.push_back("fr");
  val1.push_back("hu");
  val1.push_back("it");
  val1.push_back("ko");
  val1.push_back("nl");
  val1.push_back("no");
  val1.push_back("pl");
  val1.push_back("pt");
  val1.push_back("sv");
  val1.push_back("th");
  val1.push_back("zh");
  val1.push_back("und");

  val2 = val1;
  val2[0]="";

  sub_lang_p = new Option(false, dgettext("mms-movie-mplayer", "subtitle language"), "subtitle language", 0,
                          val1, val2);

  val1[0]=dgettext("mms-movie-mplayer", "default");
  val2[0]="";
  dvd_audio_lang_p = new Option(false, dgettext("mms-movie-mplayer", "dvd audio language"),
				"dvd audio language", 0, val1, val2);

  val1.clear();
  val2.clear();

  val1.push_back(dgettext("mms-movie-mplayer", "default"));
  val1.push_back(mplayer_conf->p_mplayer_ttf_font());
  val2 = val1;
  val2[0]="";

  sub_font_p = new Option(false, dgettext("mms-movie-mplayer", "font"), "font", 0,
                          val1, val2);
  val1.clear();
  val2.clear();

  val1.push_back(dgettext("mms-movie-mplayer", "default"));
  val1.push_back("1");
  val1.push_back("1.5");
  val1.push_back("2");
  val1.push_back("2.5");
  val1.push_back("3");
  val1.push_back("3.5");
  val1.push_back("4");
  val1.push_back("4.5");
  val1.push_back("5");
  val1.push_back("5.5");
  val2 = val1;
  val2[0]="";

  sub_size_p = new Option(false, dgettext("mms-movie-mplayer", "font size"), "font size", 0,
                          val1, val2);

  val1.clear();
  val2.clear();

  val1.push_back(dgettext("mms-movie-mplayer", "default"));
  val2.push_back("");
  for (int i = 1; i < 100; ++i) {
    val1.push_back(conv::itos(i));
    val2.push_back(conv::itos(i));
  }

  dvdtitle_p = new Option(false, dgettext("mms-movie-mplayer", "dvd title"), "dvd title", 0, val1, val2);
  vcdtitle_p = new Option(false, dgettext("mms-movie-mplayer", "vcd title"), "vcd title", 0, val1, val2);

  val1.clear();
  val2.clear();

  val1.push_back(dgettext("mms-movie-mplayer", "on"));
  val1.push_back(dgettext("mms-movie-mplayer", "off"));
  val2.push_back(" -fs ");
  val2.push_back("");
  full_screen_p = new Option(false, dgettext("mms-movie-mplayer", "Fullscreen"), "Fullscreen", 0,
		 val1, val2);
  val1.clear();
  val2.clear();

  val1.push_back(dgettext("mms-movie-mplayer", "default"));
  val1.push_back(dgettext("mms-movie-mplayer", "16:9"));
  val1.push_back(dgettext("mms-movie-mplayer", "4:3"));
  val2.push_back("");
  val2.push_back(" -monitoraspect 16:9 ");
  val2.push_back(" -monitoraspect 4:3 ");
  monitor_aspect_p = new Option(false, dgettext("mms-movie-mplayer", "Monitor aspect"), "Monitor aspect", 0,
				val1, val2);

  val1[0]=dgettext("mms-movie-mplayer", "off");
  val2[1]=(" -aspect 16:9 ");
  val2[2]=(" -aspect 4:3 ");
  val1.push_back(dgettext("mms-movie-mplayer", "wide (2.35:1)"));
  val2.push_back(" -aspect 2.35 ");

  val1.push_back(dgettext("mms-movie-mplayer", "square"));
  val2.push_back(" -aspect 1.25 ");

  force_geometry_p = new Option(false, dgettext("mms-movie-mplayer", "Force geometry"), "Force geometry", 0,
				val1, val2);

  opts.add_opt(gen_head_opt_p);
  opts.add_opt(monitor_aspect_p);
  opts.add_opt(force_geometry_p);
  opts.add_opt(full_screen_p);
  opts.add_opt(sub_lang_p);

  opts.add_opt(sub_font_p);
  opts.add_opt(sub_size_p);
  opts.add_opt(dvd_head_opt_p);
  opts.add_opt(dvd_audio_lang_p);
  opts.add_opt(dvdtitle_p);
  opts.add_opt(vcd_head_opt_p);
  opts.add_opt(vcdtitle_p);

  opts.load();
}

Mplayer::~Mplayer()
{
  opts.save();

  ScreenUpdater *screen_updater = S_ScreenUpdater::get_instance();
  screen_updater->timer.del("mplayer status");
  delete gen_head_opt_p;
  delete dvd_head_opt_p;
  delete vcd_head_opt_p; //mov_head_opt_p,
  delete dvd_audio_lang_p;
  delete sub_font_p;
  delete sub_size_p;
  delete sub_lang_p;
  delete vcdtitle_p;
  delete dvdtitle_p;
  delete monitor_aspect_p;
  delete force_geometry_p;
}

bool Mplayer::wants_exclusivity()
{
  return !(S_Render::get_instance()->device->supports_wid() && movie_conf->p_playback_in_mms());
}

std::pair<bool, Pipe> Mplayer::play_movie_in_slave_mode(char *buffer, const std::string& sub_font, const std::string& sub_size, const string& sub_lang,
							const string& audio_lang,
							const string& audio_mode, const string& paths, bool paths_find)
{
  string maspect=monitor_aspect_p->english_values[monitor_aspect_p->pos];
  string fgeometry=force_geometry_p->english_values[force_geometry_p->pos];
  string fulls=full_screen_p->english_values[full_screen_p->pos];

  string command;

  if (paths_find)
    command = paths + " " + mplayer_conf->p_mplayer_path() + " " +
      mplayer_conf->p_mplayer_slave_opts() +
      " -zoom" + " -vf expand=:::::" + conv::itos(conf->p_h_res()) + "/" +
      conv::itos(conf->p_v_res()) +
      " -noconsolecontrols -nojoystick -nolirc -nomouseinput " +
      " -nocache -slave -quiet -osdlevel 0 -wid " + buffer +
    (sub_font.empty() ? "" : " -font " + sub_font) +
    (sub_size.empty() ? "" : " -subfont-text-scale " + sub_size) +
      (sub_lang.empty() ? "" : " -slang " + sub_lang) +
      (audio_lang.empty() ? "" : " -alang " + audio_lang) +
      fulls + fgeometry + maspect +
      ' ' + audio_mode;
  else {
    command = mplayer_conf->p_mplayer_path() + " " +
      mplayer_conf->p_mplayer_slave_opts() +
      " -zoom" + " -vf expand=:::::" + conv::itos(conf->p_h_res()) + "/" +
      conv::itos(conf->p_v_res()) +
    (sub_font.empty() ? "" : " -font " + sub_font) +
    (sub_size.empty() ? "" : " -subfont-text-scale " + sub_size) +
      " -noconsolecontrols -nojoystick -nolirc -nomouseinput " +
      " -nocache -slave -quiet -osdlevel 0 -wid " + buffer +
      (sub_lang.empty() ? "" : " -slang " + sub_lang) +
      (audio_lang.empty() ? "" : " -alang " + audio_lang) +
      fulls + fgeometry + maspect +
      ' ' + audio_mode + ' ';
    if ( (string_format::lowercase(paths)).rfind("video_ts") != string::npos )
	command += paths.substr(0, (string_format::lowercase(paths)).rfind("video_ts")) + "'";
    else
	command += paths;
  }

  return run::exclusive_external_program_pipe(command);
}

string Mplayer::get_audio_mode()
{
  string audio_mode = " -ao ";
  GlobalOptions * go =  S_GlobalOptions::get_instance();
  global_audio_device gad = go->get_audio_device_video();

  audio_mode += gad.dev_output;

  if (gad.dev_output == "alsa")
    audio_mode += ":device=" + gad.dev_path;
  else if (gad.dev_output == "oss")
    audio_mode +=  ":" + gad.dev_path;

  // set Channels option
  if (gad.dev_channels == 2 || gad.dev_channels == 4 || gad.dev_channels == 6)
      audio_mode += " -channels " + conv::itos(gad.dev_channels);

  // set SPDIF options
  if (gad.dev_digital)
      audio_mode += " -afm hwac3";
  return audio_mode;
}

int Mplayer::check_playback_timeout()
{
  return 500; // ms
}

void Mplayer::check_playback()
{
  send_command("get_time_pos");
}

void Mplayer::exclusive_top()
{
  if (running) {
    pos = 0;
    stop_and_switch();
  } else {
    ScreenUpdater *screen_updater = S_ScreenUpdater::get_instance();
    screen_updater->timer.activate("mplayer status");
  }
}

void Mplayer::killall_thumbnailers()
{
  // make sure that we don't automatically thumbnail again
  add_thumbnail("cp", S_Render::get_instance()->default_path + S_Themes::get_instance()->movie_mcover);

  run::external_program("kill -9 " + conv::itos(external_pid));

  S_InputMaster::get_instance()->clear_cancel_callback();
}

bool Mplayer::create_thumbnail(const std::string& file, const std::string& thumbnail, int width, int height)
{
  thumbnail_path = thumbnail;

  S_InputMaster::get_instance()->register_cancel_callback(boost::bind(&Mplayer::killall_thumbnailers, this));

  external_pid = run::external_program("midentify " + string_format::escape_string(file) + " > " + conf->p_temp_image_loc() + "/movie_length");

  std::ifstream In((conf->p_temp_image_loc() + "/movie_length").c_str());
  string str;
  string::size_type pos = 0;
  string length = "", video_width, video_height;
  while (getline(In, str)) {
    if ((pos = str.rfind("ID_LENGTH=")) != string::npos) {
      length = str.substr(10);
    }
    if ((pos = str.rfind("ID_VIDEO_WIDTH=")) != string::npos) {
      video_width = str.substr(15);
    }
    if ((pos = str.rfind("ID_VIDEO_HEIGHT=")) != string::npos) {
      video_height = str.substr(16);
    }
  }

  if (length.empty()) {
    Print print(dgettext("mms-movie-mplayer", "midentify failed to parse the length of file: ") + file,
		Print::INFO, "MPlayer");
    S_InputMaster::get_instance()->clear_cancel_callback();
    return false;
  }

  string scale = "-vf scale=" + conv::itos(width) + ":-2";

  if (conv::atoi(video_height) > conv::atoi(video_width))
    scale = "-vf scale=-2:" + conv::itos(height);

  int seeklength = conv::atoi(length)/2;

  run::external_program(string("mplayer -noconsolecontrols -loop 1 -nojoystick -nolirc -nomouseinput -ao null ") + scale + " -frames 4 -vo jpeg:outdir=" + conf->p_temp_image_loc() + " -ss " + conv::itos(seeklength) + " " + string_format::escape_string(file) + " > /dev/null");


  add_thumbnail("mv", conf->p_temp_image_loc() + "/00000004.jpg");

  S_InputMaster::get_instance()->clear_cancel_callback();

  return true;
}

void Mplayer::add_thumbnail(const string& command, const string& thumb_path)
{
  string thumbnail_dir = thumbnail_path.substr(0, thumbnail_path.rfind('/')+1);
  abs_recurse_mkdir(thumbnail_dir, NULL);

  run::external_program(command + " " + thumb_path + " " +
			string_format::escape_string(thumbnail_path));
}

void Mplayer::setup_movie_in_slave_mode(const string& audio_mode, const std::string& sub_font, const std::string& sub_size, const string& sub_lang,
					const string& audio_lang, const string& paths,
					bool paths_find, bool window)
{
  Render *render = S_Render::get_instance();

  char buffer[1024];

  std::pair<bool, Pipe> result;

  if (window) {

    render->device->hide_fs_video();
    mmsUsleep(100*1000);
    snprintf(buffer, 1024, "0x%x", (int) render->device->wid());
    result = play_movie_in_slave_mode(buffer, sub_font, sub_size, sub_lang, audio_lang, audio_mode, paths, paths_find);
    render->device->show_window_video();

  } else {

    render->device->hide_window_video();
    mmsUsleep(100*1000);
    snprintf(buffer, 1024, "0x%x", (int) render->device->fs_wid());
    result = play_movie_in_slave_mode(buffer, sub_font, sub_size, sub_lang, audio_lang, audio_mode, paths, paths_find);
    render->device->show_fs_video();

  }

  checker = new PlaybackChecker(this);
  checker->start();

  setup_playback(result, window);

  path = paths;
}

void Mplayer::playback(const string& paths, bool paths_find, const string& audio_lang,
		       const string& opts, bool window)
{
  Render *render = S_Render::get_instance();

  if (render->device->supports_wid() && movie_conf->p_playback_in_mms())
    exclusive_top();

  string audio_mode = get_audio_mode();
  string sub_lang =sub_lang_p->english_values[sub_lang_p->pos];
  string sub_font = sub_font_p->english_values[sub_font_p->pos];
  string sub_size = sub_size_p->english_values[sub_size_p->pos];
  string maspect=monitor_aspect_p->english_values[monitor_aspect_p->pos];
  string fgeometry=force_geometry_p->english_values[force_geometry_p->pos];
  string fulls=full_screen_p->english_values[full_screen_p->pos];

  if (render->device->supports_wid() && movie_conf->p_playback_in_mms()) {

    setup_movie_in_slave_mode(audio_mode, sub_font, sub_size, sub_lang, audio_lang, paths, paths_find, window);

  } else {

    string command;

    if (paths_find)
      command = paths + " " + (mplayer_conf->p_mplayer_path() + " " + opts +
			      (sub_font.empty() ? "" : " -font " + sub_font) +
			      (sub_size.empty() ? "" : " -subfont-text-scale " + sub_size) +
			      (sub_lang.empty() ? "" : " -slang " + sub_lang) +
			      (audio_lang.empty() ? "" : " -alang " + audio_lang) +
			      maspect + fgeometry + fulls + " " + audio_mode);
    else {
      command = (mplayer_conf->p_mplayer_path() + " " + opts +
		 (sub_font.empty() ? "" : " -font " + sub_font) +
		 (sub_size.empty() ? "" : " -subfont-text-scale " + sub_size) +
		 (sub_lang.empty() ? "" : " -slang " + sub_lang) +
		 (audio_lang.empty() ? "" : " -alang " + audio_lang) +
		 maspect + fgeometry + fulls + " " + audio_mode + " ");
      if ( (string_format::lowercase(paths)).rfind("video_ts") != string::npos ) 
	command += paths.substr(0, (string_format::lowercase(paths)).rfind("video_ts")) + "'"; 
      else
	command += paths;
    }

    std::cout << "running mplayer with following command:" << command << std::endl;

    run::exclusive_external_program(command);
  }
}

void Mplayer::play_movie(const string& paths, bool window)
{
  playback(paths, false, "", mplayer_conf->p_mplayer_opts(), window);
  type = MOVIE;
}

void Mplayer::setup_playback(const std::pair<bool, Pipe>& result, bool window)
{
  if (result.first) {
    Global *global = S_Global::get_instance();
    global->set_playback("Movie");
    p = result.second;
    broken_pipe = false;
    running = true;

    if (window)
      in_fullscreen = false;
    else
      in_fullscreen = true;

    get_class<Movie>(dgettext("mms-movie-mplayer", "Video"))->set_movie_playback(true);
  }
}

void Mplayer::play_disc(const string& play_cmd)
{
  playback(play_cmd, true, "",  mplayer_conf->p_mplayer_opts(), false);
}

void Mplayer::play_vcd(const string& device)
{
  string vcdtitle = vcdtitle_p->english_values[vcdtitle_p->pos];
  string paths = "vcd://" + vcdtitle + "/" + device;

  playback(paths, false, "", mplayer_conf->p_mplayer_opts(), false);
  type = VCD;
}

void Mplayer::play_dvd(const string& device)
{
  string dvdtitle = dvdtitle_p->english_values[dvdtitle_p->pos];
  string dvd_audio_lang = dvd_audio_lang_p->english_values[dvd_audio_lang_p->pos];

  string paths;
  if (!mplayer_conf->p_dvdnav())
    paths =  " dvd://" + dvdtitle + " -dvd-device '" + device + "'";
  else
    paths =  " dvdnav://" + dvdtitle + " -dvd-device '" + device + "'";

  playback(paths, false, dvd_audio_lang, mplayer_conf->p_mplayer_dvd_opts(), false);
  type = DVD;
}

void Mplayer::play()
{
  if (paused)
    pause();
}

void Mplayer::pause()
{
  send_command("pause");
  paused = !paused;
}

void Mplayer::stop()
{
  send_command("quit");
  stop_playback();
}

void Mplayer::stop_and_switch()
{
  send_command("quit");

  playback_ended();
}

void Mplayer::stop_playback()
{
  playback_ended();
  run::release_exclusive_plugin_access();
  pos = 0;
}

void Mplayer::ff()
{
  send_command("seek 10 0");
}

void Mplayer::fb()
{
  send_command("seek -10 0");
}

bool Mplayer::fullscreen()
{
  return in_fullscreen;
}

bool Mplayer::playing()
{
  return running;
}

void Mplayer::switch_screen_size()
{
  bool was_paused = paused;

  if (paused) // make sure that we can change resolution if paused
    pause();
  send_command("get_time_pos");
  stop_playback_ok = false;
  while (!stop_playback_ok)
    mmsUsleep(100*1000);
  stop_and_switch();

  if (type == MOVIE)
    play_movie(path, in_fullscreen); // in_fullscreen works because play_movie takes window
  else if (type == VCD)
    ; // FIXME
  else if (type == DVD)
    ; // FIXME

  mmsUsleep(100*1000);

  if (was_paused)
    pause();
}

void Mplayer::playback_ended()
{
  if (running) {
    get_class<Movie>(dgettext("mms-movie-mplayer", "Video"))->set_movie_playback(false);

    Global *global = S_Global::get_instance();
    global->set_playback("Audio");
    broken_pipe=true;
    running = false;
    Render *render = S_Render::get_instance();
    render->device->hide_video();
    run::close_pipe(p);

    ScreenUpdater *screen_updater = S_ScreenUpdater::get_instance();
    screen_updater->timer.deactivate("mplayer status");
  }
}

void Mplayer::send_command(const std::string& command)
{
  if (!running || (paused && command != "pause"))
    return;

  const char *buff = command.c_str();

  // check for writeable pipe i.e. prevent broken pipe signal
  if(!broken_pipe) {
    struct pollfd pfd;
    pfd.fd=p.inpipe1; pfd.events=POLLOUT; pfd.revents=0;
    int r=poll(&pfd,1,50);
    if(r>0) {
      if(pfd.revents & ~POLLOUT) {
	printf("mplayer: %s%s%s%sin MPlayerControl\n",pfd.revents&POLLOUT?"POLLOUT ":"",pfd.revents&POLLERR?"POLLERR ":"",pfd.revents&POLLHUP?"POLLHUP ":"",pfd.revents&POLLNVAL?"POLLNVAL ":"");
	stop_playback();
	printf("mplayer: after playback ended\n");
      }
      else if(pfd.revents & POLLOUT) {
	r=write(p.inpipe1,buff,strlen(buff));
	if(r<0) {
	  printf("mplayer: pipe write(1) failed\n");
	  stop_playback();
	}
	else {
	  r=write(p.inpipe1,"\n",1);
	  if(r<0) {
	    printf("mplayer: pipe write(2) failed\n");
	    stop_playback();
	  }
	}
      }
    }
    else if(r==0)
      printf("mplayer: poll timed out in MPlayerControl (hugh?)\n");
    else
      printf("mplayer: poll failed in MPlayerControl\n");
  } else {
    printf("mplayer: cmd pipe is broken\n");
    stop_playback();
    printf("mplayer: after playback ended\n");
  }

  //printf("mplayer: slave cmd: %s\n", buff);
}

PlaybackChecker::PlaybackChecker(Mplayer* mplayer)
  : player(mplayer)
{}

void PlaybackChecker::run()
{
  struct pollfd pfd;
  pfd.fd=player->p.outpipe0; pfd.events=POLLIN;

  char buff[bsize+2]; // additional space for fake newline
  int c=0;

  while (player->running) {
    poll(&pfd,1,300);
    int r=read(player->p.outpipe0,buff+c,bsize-c);
    if(r>0) c+=r;
    if(c>0) {
      buff[c]=0; // make sure buffer is NULL terminated
      char *p;
      do {
        p=strpbrk(buff,"\n\r");
        if(!p && c==bsize) { // Full buffer, but no newline found.
          p=&buff[c];        // Have to fake one.
          buff[c]='\n'; c++; buff[c]=0;
          }
        if(p) {
          *p++=0;
          float itime;
          if(strncmp(buff,"Starting playback",17)==0) {
	    if (player->pos > 0.0) {
	      char tmp[20];
	      sprintf(tmp, "seek %.1f 2", player->pos);
	      player->send_command(tmp);
	    }
	  }
          else if(sscanf(buff,"ANS_TIME_POSITION=%f",&itime)==1) {
	    player->stop_playback_ok = true;
            if(itime>0) {
	      player->pos=itime;
	    }
	  }
          c-=(p-buff);
          memmove(buff,p,c+1);
	}
      } while(c>0 && p);
    }
  }
}
