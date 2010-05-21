/****************************************************************************
 *
 *   Copyright (C) 2003 Wolfgang G. Reissnegger, All Rights Reserved
 *
 *   gnagflow@users.sourceforge.net
 *
 *   Modified for My Media System by Anders Rune Jensen
 *
 *   This program is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU General Public License as
 *   published by the Free Software Foundation; either version 2 of
 *   the license, or (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public
 *   License along with this program;  if not, write to the
 *       Free Software Foundation, Inc.,
 *       59 Temple Place - Suite 330,
 *       Boston, MA 02111-1307
 *
 ****************************************************************************/

#include "lcd.hpp"

#include "global.hpp"

#include "socket.hpp"
#include "common.hpp"
#include "gettext.hpp"
#include "config.hpp"

#include "boost.hpp"

#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include <sstream>
#include <string>

#ifdef use_nls
#include <langinfo.h>
#include <locale.h>
#endif

pthread_mutex_t Lcd::singleton_mutex = PTHREAD_MUTEX_INITIALIZER;

LcdPrint::LcdPrint()
  : Print(INFO)
{}

void LcdPrint::print()
{
  Lcd *lcd = S_Lcd::get_instance();
  Global *global = S_Global::get_instance();

  int pos = 1;

  foreach (std::string& message, messages) {
  lcd->lcdprint("widget_set MMS line" + conv::itos(pos-1) +
    " 1 " + conv::itos(pos) + " " + conv::itos(global->lcd_pixels()) + " 1 h 4" +
	" {" + message + "}\n");

    ++pos;
  }
  messages.clear();
}

Lcd::Lcd()
{
#ifdef use_nls
  // gettext
  setlocale(LC_ALL, ""); /* set from the environment variables */
  bindtextdomain("mms-lcd", mms_prefix"/share/locale");
  bind_textdomain_codeset("mms-lcd", nl_langinfo(CODESET));
#endif

  lcd_conf = S_LcdConfig::get_instance();
  lcd_conf->parse_configuration_file(S_Config::get_instance()->p_homedir());

  display_init();
  printer = new LcdPrint;

  S_Global::get_instance()->register_lcd_output_func(boost::bind(&LcdPrint::add_line, this->printer, _1));
  S_Global::get_instance()->register_lcd_print_output_func(boost::bind(&LcdPrint::print, this->printer));
  S_Global::get_instance()->set_lcd_resolution(display.columns, display.lines);
}

Lcd::~Lcd()
{
  Global *global = S_Global::get_instance();
  printer->add_line(string_format::pretty_pad_string("My Media System",global->lcd_pixels(),' '));
  printer->add_line(string_format::pretty_pad_string("GOOD BYE",global->lcd_pixels(),' '));
  printer->print();

  display_close();

  delete printer;
}

void Lcd::lcdprint(const std::string& mes)
{
  socket_send(display_fd(), mes.c_str());
}

int Lcd::display_init()
{
  const char	*server = "localhost";

  int	port = 13 * 1000 + 666; /* the number of the beast */
  int	i;

  // set LCD size
  display.columns = lcd_conf->p_lcd_pixels();
  display.lines	= lcd_conf->p_lcd_rows();

  // wait for the server to start up
  mmsUsleep(500000);

  display.sock = socket_connect(server, port);
  if (display.sock <= 0)
    {
      DebugPrint perror(Print::DEBUGGING, DebugPrint::CRITICAL, "LCD");
      perror.add_line(dgettext("mms-lcd", "Error connecting to server ") +
		      std::string(server) +
		      dgettext("mms-lcd", " on port ") + conv::itos(port));
      perror.add_line(dgettext("mms-lcd", "Check to see that the server is running and operating normally"));
      perror.print();
      return -1;
    }
  socket_send(display.sock, "hello\n");
  mmsUsleep(500000);		// wait for the server to say hi.

  // Init the screen
  lcdprint("screen_add MMS\n");
  lcdprint("screen_set MMS -priority 16 -name MMS -heartbeat off\n");

  for (i = 0; i < display.lines; ++i)
    {
      std::ostringstream ss;
      ss << "widget_add MMS line" << i << " scroller\n";
      lcdprint(ss.str());
    }

  return 0;
}

int Lcd::display_fd()
{
  return display.sock;
}

int  Lcd::display_lines()
{
  return display.lines;
}

int  Lcd::display_columns()
{
  return display.columns;
}

void Lcd::display_close()
{
  socket_close(display.sock);
}

// The token handling code below has been snipped out of the LCDproc client.
void Lcd::display_recv_lcd_message()
{
  int	i;

  char	buf[8192];
  char	*argv[256];
  int	argc;
  int	newtoken;
  int	len;

  // Check for server input...
  len = socket_receive(display.sock, buf, sizeof(buf));

  // Handle server input...
  while (len > 0)
    {
      // Now split the string into tokens...
      //
      argc = 0;
      newtoken = 1;
      for (i = 0; i < len; ++i)
	{
	  switch (buf[i])
	    {
	    case ' ':
	      newtoken = 1;
	      buf[i] = 0;
	      break;
	    default:	// regular chars, keep tokenizing
	      if (newtoken)
		{
		  argv[argc] = buf + i;
		  ++argc;
		}
	      newtoken = 0;
	      break;
	    case '\0':
	    case '\n':
	      buf[i] = 0;
	      if (argc > 0)
		{
		  if (0 == strcmp(argv[0], "listen"))
		    {
		      printf("Listen %s\n", argv[1]);
		    }
		  else if (0 == strcmp(argv[0], "ignore"))
		    {
		      printf("Ignore %s\n", argv[1]);
		    }
		  else if (0 == strcmp(argv[0], "key"))
		    {
		      printf("Key %s\n", argv[1]);
		    }
		  else if (0 == strcmp(argv[0], "menu"))
		    {
		      printf("Menu %s\n", argv[1]);
		    }
		  else if (0 == strcmp(argv[0], "connect"))
		    {
		      int a;

		      for (a = 1; a < argc; ++a)
			{
			  if (0 == strcmp(argv[a], "wid"))
			    {
			      ++a;
			      display.columns = atoi(argv[a]);
			    }
			  else if (0 == strcmp(argv[a], "hgt"))
			    {
			      ++a;
			      display.lines = atoi(argv[a]);
			    }
			  else if (0 == strcmp(argv[a], "cellwid"))
			    {
			      ++a;
			    }
			  else if (0 == strcmp(argv[a], "cellhgt"))
			    {
			      ++a;
			    }
			}
		      lcdprint("client_set -name MMS\n");
		    }
		  else if (0 == strcmp(argv[0], "bye"))
		    {
		      // ignore
		    }
		  else if (0 == strcmp(argv[0], "success"))
		    {
		      // ignore
		    }
		  else
		    {
		      // print any remaining
		      // unhandled messages
		      //
		      int j;

		      for (j = 0; j < argc; ++j)
			printf("%s ", argv[j]);
		      printf("\n");
		    }
		}

	      // Restart tokenizing
	      argc = 0;
	      newtoken = 1;
	      break;
	    }	// switch(buf[i])
	}

      len = socket_receive(display.sock, buf, sizeof(buf));
    }
}


