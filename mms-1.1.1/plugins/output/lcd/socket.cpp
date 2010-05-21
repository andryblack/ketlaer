/****************************************************************************
 *
 *   Copyright (C) 2003 Wolfgang G. Reissnegger, All Rights Reserved
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

#include <assert.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>


#include "socket.hpp"

int socket_send(int fd, const char *string)
{
	int	len;
	int	offset = 0;

	assert(NULL != string);

	len = strlen(string);
	while (offset != len)
	{
		int bytes = write(fd, string + offset, len - offset);

		if (bytes == -1)
		{
			if (errno != EAGAIN)
			{
				return bytes;
			}
			continue;
		}
		else if (bytes == 0)
		{
			return bytes + offset;
		}

		offset += bytes;
	}

	return offset;
}


/******************************************************************************/

int socket_receive(int fd, void *dest, size_t maxlen)
{
	int err;

	assert(NULL != dest);

	err = read(fd, dest, maxlen);
	if (err < 0)
	{
		return err;
	}
	return err;
}

/******************************************************************************/

int socket_connect(const char *host, unsigned short int port)
{
	struct sockaddr_in	servername;
	struct hostent 		*hostinfo;
	int	sock;
	int	err;

	sock = socket(PF_INET, SOCK_STREAM, 0);
	if (sock < 0)
	{
		return sock;
	}

	memset(&servername, 0, sizeof(servername));
	servername.sin_family	= AF_INET;
	servername.sin_port	= htons(port);

	hostinfo = gethostbyname(host);
	if (!hostinfo)
	{
		return -1;
	}
	servername.sin_addr = *reinterpret_cast<struct in_addr *>(hostinfo->h_addr);

	err = connect(sock, reinterpret_cast<struct sockaddr *>(&servername), sizeof(servername));
	if (err < 0)
	{
		shutdown(sock, 2);
		return 0;	// Normal exit if server doesn't exist...
	}

	fcntl(sock, F_SETFL, O_NONBLOCK);
	return sock;
}


/******************************************************************************/

int socket_close(int fd)
{
	int err;

	err = shutdown(fd, 2);
	if (!err)
	{
		close(fd);
	}

	return err;
}

