/*
** $Id: libvbox.c,v 1.12 2001/03/01 14:59:16 paul Exp $
**
** Copyright (C) 1996, 1997 Michael 'Ghandi' Herold
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <fcntl.h>
#include <dirent.h>
#include <ctype.h>
#include <signal.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
              
#include "libvbox.hpp"                     

/** Defines ***************************************************************/

#define returnerror()		    return(FALSE)
#define returnok()			    return(TRUE)
#define returntrue()           return(TRUE)
#define printstring			    sprintf
#define close_and_mone(F)      { close(F); F = -1; }
#define close_and_null(S)      { fclose(S); S = NULL; }


#define VBOX_ONE_FD_MASK(M, F) { FD_ZERO(M); FD_SET(F, M); }

#define VBOXD_GET_MSG_TIMEOUT     (30) /* Timeout get message from server */


#define VBOXD_VAL_HELP           "180"                   /* Help messages */


#define VBOXD_VAL_HEADER         "183"            /* Get a message header */

#define VBOXD_VAL_STATUSCTRL     "185"                  /* Control status */
#define VBOXD_VAL_CREATECTRL     "186"                  /* Create control */
#define VBOXD_VAL_REMOVECTRL     "187"                  /* Remove control */


#define VBOXD_VAL_SERVEROK       "280"                    /* Server ready */
#define VBOXD_VAL_SERVERQUIT     "281"                     /* Server quit */
#define VBOXD_VAL_ACCESSOK       "282"                       /* Access ok */
#define VBOXD_VAL_LOGINOK        "283"                   /* User login ok */
#define VBOXD_VAL_STATUSCTRLOK   "284"               /* Control status ok */
#define VBOXD_VAL_CREATECTRLOK   "285"               /* Create control ok */
#define VBOXD_VAL_REMOVECTRLOK   "286"               /* Remove control ok */


#define VBOXD_VAL_ACCESSDENIED   "580"                       /* No access */
#define VBOXD_VAL_BADARGS        "583"     /* Not enough or too many args */
#define VBOXD_VAL_BADPASSWD      "584"                    /* Bad password */
#define VBOXD_VAL_BADMESSAGE     "585"                     /* Bad message */
#define VBOXD_VAL_BADCOMMAND     "586"                     /* Bad command */
#define VBOXD_VAL_TEMPERROR      "589"                      /* Misc error */

#if HAVE_VSNPRINTF
#   define vnprintstring(B, S, F, A) vsnprintf(B, S, F, A)
#else
#   define vnprintstring(B, S, F, A) vsprintf(B, F, A)
#endif

/** Variables ************************************************************/

const char *cLibVBOX::compressions[] =
{
   "?", "?", "ADPCM-2", "ADPCM-3", "ADPCM-4", "ALAW", "ULAW"
};
/**************************************************************************/
/** VOICE AUDIO SUPPORT                                                  **/
/**************************************************************************/

/** Defines ***************************************************************/

#define KERNEL_SAMPLE_FREQ		(8000)

#define VAH_MAGIC					"VBOX"							     /* Magic id */
#define VAH_MAX_MAGIC			(4)

#define VHA_MAX_RESERVED_A    (32)
#define VHA_MAX_RESERVED_B    (64)

/**************************************************************************/
/** CONTROL FILES                                                        **/
/**************************************************************************/

/** Defines ***************************************************************/




/** Structures ***********************************************************/

class vaheader_t
{
public:
	char					magic[4];
	unsigned long int	time;
	unsigned long int	compression;
	char					callerid[VAH_MAX_CALLERID + 1];
	char					name[VAH_MAX_NAME + 1];
	char					phone[VAH_MAX_PHONE + 1];
	char					location[VAH_MAX_LOCATION + 1];
	char              reserved1[VHA_MAX_RESERVED_A + 1];
	char              reserved2[VHA_MAX_RESERVED_A + 1];
	char              reserved3[VHA_MAX_RESERVED_B + 1];
	char              reserved4[VHA_MAX_RESERVED_B + 1];
};





/**************************************************************************/
/** vboxd_connect(): Connect to the vbox message server. After connect   **/
/**                  the function reads the server startup message!      **/
/**************************************************************************/
/** machine          String with the hostname or the ip address.         **/
/** port             Port number to connect.                             **/
/** <return>         0 on success; < 0 on error.                         **/
/**************************************************************************/

int cLibVBOX::vboxd_connect(char *machine, int port)
{
	struct sockaddr_in   sockp;
   struct hostent      *hostp;
	struct hostent       defaulthost;
	struct in_addr       defaultaddr;
	char                 defaultname[256];
	char                *defaultlist[2];
	char               **p;
	int                  c;
	int                  s;

	if (isdigit(*machine))
	{
		defaultlist[0] = NULL;
		defaultlist[1] = NULL;

		if (inet_aton(machine, &defaultaddr) != 0)
		{
			xstrncpy(defaultname, machine, 255);
			defaultlist[0] = (char*)&defaultaddr;
			defaulthost.h_name      = (char *)defaultname;
			defaulthost.h_addr_list = defaultlist;
			defaulthost.h_length    = sizeof(struct in_addr);
			defaulthost.h_addrtype  = AF_INET;
			defaulthost.h_aliases   = 0;

			hostp = &defaulthost;
		}
		else hostp = gethostbyname(machine);
	}
	else hostp = gethostbyname(machine);

	if (!hostp) return(VBOXC_ERR_UNKNOWNHOST);

	memset((char *)&sockp, '\0', sizeof(struct sockaddr_in));

   sockp.sin_family = hostp->h_addrtype;
   sockp.sin_port   = htons(port);

	c = -1;
	s = -1;

	errno = 0;

   for (p = hostp->h_addr_list; ((p) && (*p)); p++)
	{
      s = socket(hostp->h_addrtype, SOCK_STREAM, 0);

      if (s < 0) return(VBOXC_ERR_NOSOCKET);

      memcpy((char *)&sockp.sin_addr, *p, hostp->h_length);

		c = connect(s, (struct sockaddr *)&sockp, sizeof(struct sockaddr_in));

		if (c == 0) break;

		close(s);
	}

	if (c < 0) return(VBOXC_ERR_NOCONNECT);

	vboxd_r_fd = s;
	vboxd_w_fd = dup(vboxd_r_fd);

	if ((vboxd_w_fd < 0) || (vboxd_r_fd < 0))
	{
		vboxd_disconnect();

		return(VBOXC_ERR_NOFILEIO);
	}

	if (!vboxd_get_message())
	{
		vboxd_disconnect();

		return(VBOXC_ERR_GETMESSAGE);
	}

	if (!vboxd_test_response(VBOXD_VAL_SERVEROK))
	{
		vboxd_disconnect();

		return(VBOXC_ERR_GETMESSAGE);
	}

	return(VBOXC_ERR_OK);
}

/**************************************************************************/
/** vboxd_disconnect(): Sends "quit" and disconnect from the server.     **/
/**************************************************************************/

void cLibVBOX::vboxd_disconnect(void)
{
	if (vboxd_w_fd != -1) vboxd_put_message("quit");

	if (vboxd_r_fd != -1) close_and_mone(vboxd_r_fd);
	if (vboxd_w_fd != -1) close_and_mone(vboxd_w_fd);
}

/**************************************************************************/
/** vboxd_login(): Login to the vboxd server.                            **/
/**************************************************************************/
/** username       Username to login.                                    **/
/** password       Password to login.                                    **/
/** <return>       0 on success; < 0 on error.                           **/
/**************************************************************************/

int cLibVBOX::vboxd_login(char *username, char *password)
{
	vboxd_put_message("login %s %s", username, password);

	if (vboxd_get_message())
	{
		if (vboxd_test_response(VBOXD_VAL_LOGINOK)) return(VBOXC_ERR_OK);
	}

	return(VBOXC_ERR_LOGIN);
}

/**************************************************************************/
/** vboxd_put_message(): Puts a message to the server.                   **/
/**************************************************************************/

void cLibVBOX::vboxd_put_message(const char *fmt, ...)
{
	if(vboxd_w_fd == -1)
		return;
   va_list arg;
	char    msgline[VBOXD_MAX_LINE + 1];

	va_start(arg, fmt);
	vnprintstring(msgline, VBOXD_MAX_LINE, fmt, arg);
	va_end(arg);

	write(vboxd_w_fd, msgline, strlen(msgline));
	write(vboxd_w_fd, "\r\n", 2);
}

/**************************************************************************/
/** vboxd_get_message(): Try to get a message from the server. The func- **/
/**                      tion us a timeout of VBOXD_GET_MSG_TIMEOUT sec- **/
/**                      ends to get the message.                        **/
/**************************************************************************/
/** <return>             Pointer to the message or NULL on error.        **/
/**************************************************************************/

char *cLibVBOX::vboxd_get_message(void)
{
	struct timeval timeval;
	fd_set         rmask;
	char          *stop;
	int            p;
	int            c;
	int            rc;

	*vboxd_message = '\0';

	p = 0;
	c = 0;

	while (TRUE)
	{
		VBOX_ONE_FD_MASK(&rmask, vboxd_r_fd);

		timeval.tv_sec  = VBOXD_GET_MSG_TIMEOUT;
		timeval.tv_usec = 0;

		rc = select((vboxd_r_fd + 1), &rmask, NULL, NULL, &timeval);

		if (rc <= 0)
		{
			if ((rc < 0) && (errno == EINTR)) continue;

			break;
		}

		if (!FD_ISSET(vboxd_r_fd, &rmask)) break;

		rc = read(vboxd_r_fd, &c, 1);

		if (rc <= 0)
		{
			if ((rc < 0) && (errno == EINTR)) continue;

			break;
		}

		if (c == '\n')
		{
			if ((stop = rindex(vboxd_message, '\r'))) *stop = '\0';

			return(vboxd_message);
		}

		vboxd_message[p + 0] = c;
		vboxd_message[p + 1] = '\0';

		if (p++ >= VBOXD_MAX_LINE) break;
	}

	return(NULL);
}

/**************************************************************************/
/** vboxd_test_response(): Test response of a vboxd message.             **/
/**************************************************************************/
/** response               Needed response code to check.                **/
/** <return>               TRUE (1) on success, FALSE (0) on error.      **/
/**************************************************************************/

int cLibVBOX::vboxd_test_response(const char *response)
{
	if (strlen(vboxd_message) > (strlen(response) + 1))
	{
		if (strncmp(response, vboxd_message, strlen(response)) == 0)
		{
			if (vboxd_message[strlen(response)] == ' ') returnok();
		}
	}

	returnerror();
}

/*************************************************************************/
/** get_message_ptime():	Returns the vbox message length in seconds.	**/
/**								The length is calculated from the size & the	**/
/**								compression mode.										**/
/*************************************************************************/
/** compression				Compression mode of the sample.					**/
/** size							Size of the sample.									**/
/** <return>					Sample length in seconds.							**/
/*************************************************************************/

int cLibVBOX::get_message_ptime(int compression, int size)
{
	if ((compression >= 2) && (compression <= 4))
	{
		size = ((size * 8) / compression);
	}
                           
	return((size / KERNEL_SAMPLE_FREQ));
}


/*************************************************************************/
/** xstrncpy():	Copys one string to another.									**/
/*************************************************************************/
/** dest				Pointer to the destination.									**/
/** source			Pointer to the source.											**/
/** max				Max length of destination.										**/
/*************************************************************************/

void xstrncpy(char *dest, char *source, int max)
{
	strncpy(dest, source, max);
	
	dest[max] = '\0';
}

/*************************************************************************/
/** xstrncat():	Cats one string to another.									**/
/*************************************************************************/
/** dest				Pointer to the destination.									**/
/** source			Pointer to the source.											**/
/** max				Max length of destination.										**/
/*************************************************************************/

void xstrncat(char *dest, char *source, int max)
{
	if ((max - strlen(dest)) > 0) strncat(dest, source, max - strlen(dest));

	dest[max] = '\0';
}

/*************************************************************************/
/** xpause():	Waits some miliseconds.												**/
/*************************************************************************/
/** ms			Miliseconds to wait.													**/
/*************************************************************************/

void xpause(unsigned long ms)
{
	usleep(ms * 1000);
}

/*************************************************************************/
/** xstrtol():	Converts a string to a long number, using a default on	**/
/**				error.																	**/
/*************************************************************************/
/** str			String to convert to long.											**/
/** use			Default value if string can't converted.						**/
/** <return>	Converted string value on success; default on error.		**/
/*************************************************************************/

long xstrtol(char *str, long use)
{
	char *stop;
	long	line;

	line = strtol(str, &stop, 10);
	
	if ((line < 0) || (*stop != 0)) line = use;

	return(line);
}

/*************************************************************************/
/** xstrtoul(): Converts a string to a unsigned long number, using a    **/
/**             default on error.                                       **/
/*************************************************************************/
/** str			 String to convert to unsigned long.							**/
/** use			 Default value if string can't converted.						**/
/** <return>	 Converted string value on success; default on error.		**/
/*************************************************************************/

unsigned long xstrtoul(char *str, unsigned long use)
{
	char          *stop;
	unsigned long  line;

	line = strtoul(str, &stop, 10);
	
	if ((line < 0) || (*stop != 0)) line = use;

	return(line);
}


int cLibVBOX::vboxd_CreateAndLoadMessageFromServer(int fd, int size)
{
   struct timeval timeval;
   fd_set         rmask;

	unsigned char temp[256];
	int  have;
	int  take;
   int  rc;

	have = 0;

	while (have < size)
	{
      VBOX_ONE_FD_MASK(&rmask, vboxd_r_fd);

      timeval.tv_sec  = VBOXD_GET_MSG_TIMEOUT;
      timeval.tv_usec = 0;

      rc = select((vboxd_r_fd + 1), &rmask, NULL, NULL, &timeval);

		if (rc <= 0)
		{
			if ((rc < 0) && (errno == EINTR)) continue;

			break;
		}

		if (!FD_ISSET(vboxd_r_fd, &rmask)) break;

		if ((take = (size - have)) > 255) take = 255;

		rc = read(vboxd_r_fd, temp, take);

		if (rc <= 0)
		{
			if ((rc < 0) && (errno == EINTR)) continue;

			break;
		}

		have += rc;

		write(fd, temp, rc);
	}

	return(have);
}


int cLibVBOX::vboxd_statusctrl(const char * ctrlname,int *pStatus)
{
	char *answer = NULL;
	vboxd_put_message("statusctrl %s", ctrlname);

	if ((answer = vboxd_get_message()))
	{
		if (vboxd_test_response(VBOXD_VAL_STATUSCTRLOK))
		{			
			*pStatus = atoi(&answer[4]);
			return VBOXC_ERR_OK;
		}
	}
	return VBOXC_ERR_ERROR;
}

int cLibVBOX::vboxd_createctrl(const char * ctrlname)
{
	char *answer = NULL;
	vboxd_put_message("createctrl %s", ctrlname);
	if ((answer = vboxd_get_message()))
	{
		if (vboxd_test_response(VBOXD_VAL_CREATECTRLOK))
		{
			if (answer[4] == '1') 
				return VBOXC_ERR_OK;
		}
	}
	return VBOXC_ERR_ERROR;
}

int cLibVBOX::vboxd_removectrl(const char * ctrlname)
{
	char *answer = NULL;
	vboxd_put_message("removectrl %s", ctrlname);
	if ((answer = vboxd_get_message()))
	{
		if (vboxd_test_response(VBOXD_VAL_CREATECTRLOK))
		{
			if (answer[4] == '1') 
				return VBOXC_ERR_OK;
		}
	}
	return VBOXC_ERR_ERROR;
}
