#define _XOPEN_SOURCE /* glibc2 needs this */
#include "weather_config.hpp"
#include "config.h"
#include "gettext.hpp"
#include "graphics.hpp"
#include "busy_indicator.hpp"

#include "startmenu.hpp"
#include "weather.hpp"
#include "updater.hpp"
#include "common-feature.hpp"
#include "tinyxml.h"
#include <string>
#include <sstream>
#include <vector>
#include <list>
#include <theme.hpp>

#ifdef use_nls
#include <langinfo.h>
#include <locale.h>
#endif


using namespace std;
//using std::list;
//using std::string;

string CData::get_name ( int dindex ) {
   return string (day[ dindex ].name);
}

string CData::get_icon ( int dindex ) {
   return string(day[ dindex ].icon);
}

int CData::get_high_temp ( int dindex ) {
   return day[ dindex ].hi;
}

int CData::get_low_temp ( int dindex ) {
   return day[ dindex ].low;
}

string CData::get_city ( void ) {
   return city;
}

string CData::get_text ( void ) {
   return text;
}
string CData::get_date ( void ) {
   return date;
}
string CData::get_wind ( void ) {
   return wind;
}
string CData::get_humid ( void ) {
   return humid;
}
string CData::get_unit ( void ) {
   return unit;
}

CData::CData()
  : weather_file_last_mod_time(0)
{}

void CData::Update()
{
  int iter=1;
  bool abort = false;
  WeatherConfig *weather_conf = S_WeatherConfig::get_instance();
   /*
  printf("weather: Using <%s> for updating weather information.\n", xml_filename.c_str() );
  printf("weather: Using <%s> as the code for your city.\n", city_code.c_str() );
  printf("weather: Using <%s> for file where information is stored.\n", data_file.c_str() );
  printf("weather: Using <%c> as unit (c=Celsius; f=Fahrenheit).\n", data_unit.c_str() );
 */ 

  time_t mod_time = modification_time(weather_file);

  if (mod_time == weather_file_last_mod_time)
    return;
  else
    weather_file_last_mod_time = mod_time;

  TiXmlDocument doc( weather_file.c_str() );
  TiXmlElement* root_element;
  TiXmlElement* element = NULL;
  bool loadOkay = doc.LoadFile( TIXML_ENCODING_UTF8 );
  if (loadOkay)
    {
      //      printf("\n%s:\n", data_file.c_str());
      //dump_to_stdout( &doc ); // defined later in the tutorial
    }
  else
    {
      cout << "weather: DEBUG: Failed to load file" << weather_file.c_str()  << endl;
      day[0].name  = "missing";
      return;
    }

  cout << "Weather: DEBUG -> " << weather_file.c_str() << " opened..." << endl;
  root_element = doc.RootElement();
  day[0].name  = "N/A";
  if (root_element != 0)
  {
     TiXmlHandle myhandle(&doc);
     element=myhandle.FirstChild("xml_api_reply").FirstChild("weather").FirstChild("forecast_information").FirstChild("city").Element();
     if ( element==NULL) abort=true;
 // ##################################### getting the data #################################     
     if ( abort == false ) {
        // unit (SI=Celsius/US=Fahrenheit)
        element=myhandle.FirstChild("xml_api_reply").FirstChild("weather").FirstChild("forecast_information").FirstChild("unit_system").Element();
        unit=element->Attribute("data"); 
        cout << "weather DEBUG: unit_system=" << weather_conf->p_unit_system() << endl;
        //city 
        element=myhandle.FirstChild("xml_api_reply").FirstChild("weather").FirstChild("forecast_information").FirstChild("city").Element();
        city=element->Attribute("data");
        //date
        element=myhandle.FirstChild("xml_api_reply").FirstChild("weather").FirstChild("forecast_information").FirstChild("current_date_time").Element();
        date=element->Attribute("data");
        date.erase(date.length()-5,5);
        //text
        element=myhandle.FirstChild("xml_api_reply").FirstChild("weather").FirstChild("current_conditions").FirstChild("condition").Element();
        text=element->Attribute("data");
        //humid
        element=myhandle.FirstChild("xml_api_reply").FirstChild("weather").FirstChild("current_conditions").FirstChild("humidity").Element();
        humid=element->Attribute("data");
        //wind
        element=myhandle.FirstChild("xml_api_reply").FirstChild("weather").FirstChild("current_conditions").FirstChild("wind_condition").Element();
        wind=element->Attribute("data");
        //day of week (first element always now)
        day[0].name="Now";
        // if unit is Fahrenheit and it's not forced to Celsius, leave Fahrenheit, else do Celsius
        if ( unit == "US" &&  weather_conf->p_unit_system()=="Fahrenheit") {
           element=myhandle.FirstChild("xml_api_reply").FirstChild("weather").FirstChild("current_conditions").FirstChild("temp_f").Element();  
        } else {
           element=myhandle.FirstChild("xml_api_reply").FirstChild("weather").FirstChild("current_conditions").FirstChild("temp_c").Element(); } 
        element->QueryIntAttribute("data",&day[0].hi);
        element->QueryIntAttribute("data",&day[0].low);
        element=myhandle.FirstChild("xml_api_reply").FirstChild("weather").FirstChild("current_conditions").FirstChild("icon").Element();
        day[0].icon=element->Attribute("data");
        if ( (day[0].icon).length() != 0 ) {
          (day[0].icon).erase(0,16);
          (day[0].icon).erase((day[0].icon).length() - 4,4);
        } else {
           day[0].icon="no_icon";
        }
        element = myhandle.FirstChild("xml_api_reply").FirstChild("weather").FirstChild("forecast_conditions").Element();
        for ( ; element ; element=element->NextSiblingElement("forecast_conditions") )// FIXME: a while() would be better
        {
            day[iter].name=element->FirstChildElement("day_of_week")->Attribute("data");
            element->FirstChildElement("low")->QueryIntAttribute("data",  &day[iter].low);
            // If Fahrenheit, but Celsius forced, do recalculation
            if ( unit == "US" &&  weather_conf->p_unit_system() == "Celsius" ) 
               day[iter].low = (( day[iter].low - 32 ) * 5/9);
            element->FirstChildElement("high")->QueryIntAttribute("data", &day[iter].hi);
            if ( unit == "US" &&  weather_conf->p_unit_system() == "Celsius" ) 
               day[iter].hi = (( day[iter].hi  - 32 ) * 5/9);
            // get icon name and strip it
            day[iter].icon=element->FirstChildElement("icon")->Attribute("data");
            if ( (day[iter].icon).length() != 0 ) {
               (day[iter].icon).erase(0,16);
               (day[iter].icon).erase((day[iter].icon).length() - 4,4);
            } else {
              day[iter].icon="no_icon";
            }
            iter++;
            /*
            //Gutes Bsp, um an den Text zwischen den Tags zu kommen
            TiXmlHandle myhandle(child);
            TiXmlText* text = myhandle.FirstChildElement("hi").FirstChild().Text();
            const char* message = text->Value();
            printf("weather: DEBUG: Temperature: hi:<%s>\n",  message );
         */
          
       }
     }
   }
}

Weather::Weather() : Unit("ºC"), enabled(false)
{
#ifdef use_nls
   setlocale(LC_ALL, ""); /* set from the environment variables */
   bindtextdomain("mms-weather", mms_prefix"/share/locale");
   bind_textdomain_codeset("mms-weather", nl_langinfo(CODESET));
#endif
   curlayer = 0;

#ifdef use_notify_area
   nArea->add(NotifyElement("Weather", boost::bind(&Weather::ShowNotifyArea, this))); 
#endif
  weather_file = S_Config::get_instance()->p_var_data_dir() + "/Weather.xml";
  wdata.weather_file = weather_file;  
  update_wdata();

  ScreenUpdater *screen_updater = S_ScreenUpdater::get_instance();
  screen_updater->timer.add(TimeElement("weather update", 
	boost::bind(&Weather::update_idle, this),
        boost::bind(&Weather::update_wdata, this)));

  header_font = "Vera/" + conv::itos(resolution_dependant_font_size(28, conf->p_v_res()));
}

int Weather::update_idle()
{
  return 1000 * 15;
}

void Weather::update_wdata()
{
  wdata.Update();

  if (wdata.get_name(0) == "N/A" || wdata.get_name(0) == "missing")
	enabled = false;
  else
	enabled = true; 

#ifdef use_notify_area
  if (enabled)
    nArea->activate("Weather");
  else
    nArea->deactivate("Weather");
#endif
}

std::string Weather::Print( int current_pos, int max_entries )
{
  if (!enabled) {

    DialogWaitPrint pdialog(3000);

    pdialog.add_line(dgettext("mms-weather", "Error downloading weather data"));
    pdialog.add_line("");
    pdialog.add_line(dgettext("mms-weather", "Please check your connection"));
    pdialog.add_line(dgettext("mms-weather", "and/or configuration file"));
    pdialog.print();

    return "failed";
  }
   int X=conf->p_h_res(); int Y=conf->p_v_res();
   int BORD=Y/15;
   int XDL= (Y/2)-BORD*2;
   int YDL= XDL - BORD;
   int XR = XDL + BORD*2;
   int XDR= X-XDL-BORD*3;
   int YD = YDL + BORD*3;
   int XDD= X - BORD*2;
   int YDD= XDL ;
   int DI=(X-BORD*2)/4;
   int TEXT5=(Y/2-BORD*3)/5;
   // font scales from 30 to 18, where for Y-size of 1200 font is 30, and for 600 it's 18.
   string font_size = conv::itos(35-((1200-Y) * .03));
   string font_size2 = conv::itos(45-((1200-Y) * .02));


  render->prepare_new_image();

  render->current.add(new PObj(themes->weather_background, 0, 0, 0, SCALE_FULL));
  std::pair<int, int> header_size = string_format::calculate_string_size("abcltuwHPMjJg", header_font);
  int header_box_size = static_cast<int>(header_size.second * 0.75);

  PObj *back = new PFObj(themes->startmenu_weather, 25, 10, header_box_size, header_box_size, 2, true);

  S_Touch::get_instance()->register_area(TouchArea(rect(back->x, back->y, back->w, back->h), back->layer,
						   boost::bind(&Weather::exit, this)));

  render->current.add(back);

  TObj *t = new TObj(dgettext("mms-weather","Weather"), header_font,
			       25 + header_box_size + 10, 10 + (header_box_size - header_size.second)/2,
			       themes->weather_header_font1, themes->weather_header_font2,
		     themes->weather_header_font3, 2);

  render->current.add(t);

  S_Touch::get_instance()->register_area(TouchArea(rect(t->x, t->y, t->w, t->h), t->layer,
						   global->toggle_playlist));

  // Go and get weather data
  //int dx_pos=conf->p_h_res() / 6;
  // counter
  int max_width =  string_format::calculate_string_width(conv::itos(current_pos+1)  + 
                                                   "/" + conv::itos(max_entries),  "Vera/"+font_size );
  render->current.add(new TObj( conv::itos(current_pos+1)  + "/" + conv::itos(max_entries),
            "Vera/"+font_size, 
            X-BORD-max_width , 
            BORD/2,
            themes->weather_header_font1,
            themes->weather_header_font2,
            themes->weather_header_font3, 2) );

  // city
  render->current.add(new TObj( dgettext("mms-weather", "City: ") + wdata.get_city() ,
            "Vera/"+font_size, XR + BORD/4 , BORD*2,
            themes->weather_header_font1,
            themes->weather_header_font2,
            themes->weather_header_font3, 2) );
  // date
  render->current.add(new TObj( dgettext ("mms-weather", "Date: ") + wdata.get_date(),
            "Vera/"+font_size, XR+ BORD/4  ,  BORD*2+TEXT5,
            themes->weather_header_font1,
            themes->weather_header_font2,
            themes->weather_header_font3, 2) );
  // weather condition (text)
  render->current.add(new TObj( dgettext ( "mms-weather", "Condition: ") + wdata.get_text(),
            "Vera/"+font_size, XR+ BORD/4  ,  BORD*2+TEXT5*2,
            themes->weather_header_font1,
            themes->weather_header_font2,
            themes->weather_header_font3, 2) );

  // humidity + temp
  string UNIT="°C";
  if ( S_WeatherConfig::get_instance()->p_unit_system()=="Fahrenheit" && wdata.get_unit()=="US" ) UNIT="°F";
  render->current.add(new TObj( wdata.get_humid() + "  /  " + dgettext("mms-weather","Temp: ") +  conv::itos(wdata.get_high_temp(0)) + UNIT,
            "Vera/"+font_size, XR+ BORD/4  ,  BORD*2+TEXT5*3,
            themes->weather_header_font1,
            themes->weather_header_font2,
            themes->weather_header_font3, 2) );
  // wind
  render->current.add(new TObj( wdata.get_wind(),
            "Vera/"+font_size, XR+BORD/4 ,  BORD*2+TEXT5*4,
            themes->weather_header_font1,
            themes->weather_header_font2,
            themes->weather_header_font3, 2) );
  // current icon
  render->current.add(new PObj( render->default_path + "/weather/icons-500x500/" + 
                      wdata.get_icon(0) + ".png", 
                      BORD*2-BORD/3+(((Y/2-BORD*3)*.1)/2), 
                      BORD*2+       (((Y/2-BORD*3)*.1)/2), 
                      (Y/2-BORD*3)*.9, 
                      (Y/2-BORD*3)*.9, 
                      false, 2));
   // go for all 4 icons, week names and temperatures
   for ( int iicon=0; iicon <4; iicon ++ ) {
       //draw four icons
       //Position fraction of the lower box
       int FRAC=((DI*(iicon+1)) - (DI/2));

       render->current.add(new PObj( render->default_path + "/weather/icons-500x500/" + 
                      wdata.get_icon(iicon+1) + ".png", 
                      BORD + FRAC - (Y/2-BORD*4)/2,
                      YD + BORD, 
                      Y/2-BORD*4, 
                      Y/2-BORD*4, 
                      false, 2));

       // day of week
       int max_width1 =  string_format::calculate_string_width(wdata.get_name(iicon+1),  "Vera/"+font_size );
       render->current.add(new TObj(wdata.get_name(iicon+1),
                 "Vera/"+font_size,  
                 BORD + FRAC - max_width1/2,
                 Y/2 + BORD/5,
                 themes->weather_header_font1,
                 themes->weather_header_font2,
                 themes->weather_header_font3, 2) );
       // low+high temp
       int max_width2=string_format::calculate_string_width(conv::itos(wdata.get_low_temp(iicon+1)) +
                                                            conv::itos(wdata.get_high_temp(iicon+1)),  "Vera/"+font_size );
       render->current.add(new TObj(conv::itos(wdata.get_low_temp(iicon+1)) + "/" + 
                 conv::itos(wdata.get_high_temp(iicon+1)),
                 "Vera/"+font_size,  
                 BORD + FRAC - max_width2/2,
                 Y-BORD*2 - BORD,
                 themes->weather_header_font1,
                 themes->weather_header_font2,
                 themes->weather_header_font3, 2) );

   }
   // Upper Left Box
   render->current.add(new RObj( BORD, BORD*2 , XDL, YDL, 150, 150, 150, 50, 2));
   // Upper Right Box
   render->current.add(new RObj( XR  , BORD*2 , XDR, YDL, 150, 150, 150, 50, 2));
   // Lower Square
   render->current.add(new RObj( BORD, YD     , XDD, YDD, 150, 150, 150, 50, 2));

  render->device->animation_section_begin(true);
  render->device->switch_to_layer(curlayer+1);
  render->device->reset_layout_attribs_nowait();
  render->device->set_layout_alpha(0.0, curlayer+1);
  render->device->animation_zoom(0.90,0.90,1,1,40,curlayer+1);
  render->device->animation_fade(0,1,50,curlayer+1);
  render->device->animation_fade(1,0,60,curlayer);
  render->device->animation_section_end();

  render->draw_and_release("mms-weather");

  return "";
}

int Weather::Wget( string location ) {
    std::string cmd;
    std::string url;
    
    url = string_format::str_printf("'http://www.google.com/ig/api?weather=%s'", location.c_str());
    
    cmd = string_format::str_printf("wget -q -t 4 -T 20 --user-agent=\"%s\" %s -O %s~",
                user_agent.c_str(),
                url.c_str(),
                weather_file.c_str());
    
    print_debug(cmd);
    
    run::external_program(cmd.c_str());
  
    cmd = string_format::str_printf("iconv -f %s -t UTF-8 < %s~ > %s",
            codepage.c_str(),
            weather_file.c_str(),
            weather_file.c_str());

    print_debug(cmd);
    
    run::external_program(cmd.c_str());

    return 1;
}

std::string Weather::mainloop()
{
  exit_loop=false;
  bool update_needed=false;
  bool failed=false;
  int  cursor = 0;
  vector<string> locations;

  BusyIndicator *busy_indicator = S_BusyIndicator::get_instance();
  WeatherConfig *weather_conf = S_WeatherConfig::get_instance();
  list<string> weather_locations;
  weather_locations = weather_conf->p_weather_location();
  
  user_agent = weather_conf->p_user_agent();
  codepage = weather_conf->p_codepage();

  foreach ( string& locat, weather_locations ) {
    locations.push_back( locat );
  }

  if (locations.size() == 0) {
    DialogWaitPrint pdialog(4000);
    pdialog.add_line(dgettext("mms-weather", "No cities defined in WeatherConfig"));
    pdialog.add_line(dgettext("mms-weather", "Please define at least one city"));
    pdialog.print();
    return "";
  }

  Wget( locations.at(0) );
  render->device->animation_section_begin();
  curlayer = render->device->get_current_layer();
  render->device->animation_section_end();
  update_wdata();

  if (Print(0,locations.size()) == "failed")
    return "";

  Input input;

  // main input loop
  while (!exit_loop) {
    if (update_needed){
      update_needed=false;

      render->device->animation_section_begin();
      render->device->swap_layers(curlayer, curlayer+1);
      render->device->animation_section_end();
      failed =  ( Print(cursor,locations.size()) == "failed" );
    }
    input = input_master->get_input_busy_wrapped(busy_indicator);

    if (input.key == "touch_input") {
      S_Touch::get_instance()->run_callback();
      update_needed = true;
      continue;
    }

    // step forward through the given citys 
    if (input.command == "right" )
    {
      if (locations.size() > 1) {
	if ( cursor >= locations.size()-1 )
	  cursor=-1;
        cursor++;
        Wget( locations[cursor] );
        update_needed=true;
        Print ( cursor, locations.size() );
        update_wdata();
      }
    }
    // step backward through the given citys 
    if (input.command == "left" )
    {
      if (locations.size() > 1) {
	if (cursor <= 0)
	  cursor=locations.size();
        cursor--;
        Wget( locations[cursor] );
        update_needed=true;
        Print (cursor, locations.size() );
        update_wdata();
      }
    }
    if (input.command == "back" || failed) {
        exit();
    } else if (input.command == "startmenu") {
      exit();
    }
    else
      update_needed=!global->check_commands(input);
  }
  render->device->animation_section_begin();
  render->device->switch_to_layer(curlayer);
  render->device->reset_layout_attribs_nowait();
  render->device->animation_fade(0,1,80,curlayer);
  render->device->animation_zoom(1,1,4,4,40,curlayer+1);
  render->device->animation_fade(1,0,40,curlayer+1);
  render->device->animation_section_end();
  return "";
}

void Weather::exit()
{
  exit_loop=true;
}

#ifdef use_notify_area
void Weather::ShowNotifyArea()
{
  string strMsg;
  std::pair<int, int> size;
  int x;
  int iicon = 1;

  strMsg = conv::itos(wdata.get_low_temp(iicon)) + " / " + conv::itos(wdata.get_high_temp(iicon)) ;

  render->image_mut.enterMutex();

  int total_height = global->getBottomHeight();

  string font = graphics::resolution_dependant_font_wrapper(12, conf);

  size = string_format::calculate_string_size(strMsg, font);
  nArea->status_overlay.add(new TObj(strMsg, font,
				     conf->p_h_res() - NotifyArea::width/2 - size.first/2,
				     conf->p_v_res() - size.second - 2,
				     themes->notify_area_font1,
				     themes->notify_area_font2,
				     themes->notify_area_font3, 5));
  PObj *icon = new PObj( render->default_path + "/weather/icons-500x500/" + wdata.get_icon(iicon) + ".png", 
			 x, conf->p_v_res() - size.second, total_height - size.second, 
			 total_height - size.second, false, 4);
  icon->x = icon->real_x = conf->p_h_res() - NotifyArea::width/2 - icon->w/2;
  icon->y = icon->real_y = conf->p_v_res() - size.second - icon->h;
  render->image_mut.leaveMutex();

  nArea->status_overlay.add(icon);
}
#endif
