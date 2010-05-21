/****************************************************************************
 *
 *   Copyright (C) 2003, 2005, 2006 Wolfgang G. Reissnegger, All Rights Reserved
 *
 *   gnagflow@users.sourceforge.net                                    
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

#ifndef LCD_HPP
#define LCD_HPP

#include "printer.hpp"
#include "lcd_config.hpp"

#include "print.hpp"
#include "singleton.hpp"

#include <iostream>
#include <string>

class LcdPrint : public Print
{
public:
  LcdPrint();

  void print();
};

class Lcd : public Printer
{
 public:

  static pthread_mutex_t singleton_mutex;

  void lcdprint(const std::string& mes);

  LcdPrint *printer;

  Lcd();
  ~Lcd();

 private:

  int  display_init();
  int  display_fd();
  int  display_lines();
  int  display_columns();
  void display_close();
  void display_recv_lcd_message();

  LcdConfig *lcd_conf;

  struct display_d
  {
    int	sock;

    int	lines;
    int	columns;
  };

  struct display_d display;
};

typedef Singleton<Lcd> S_Lcd;

#endif
