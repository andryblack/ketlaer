/****************************************************************************
 *
 *   Copyright (C) 2003, 2005 Wolfgang G. Reissnegger, All Rights Reserved
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

#ifndef SOCKET_H_
#define SOCKET_H_

#include <sys/types.h> /* size_t */

int socket_send(int fd, const char *string);
int socket_receive(int fd, void *dest, size_t maxlen);
int socket_connect(const char *host, unsigned short int port);
int socket_close(int fd);

#endif
