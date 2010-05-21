/*
** Copyright (C) 2003 Gunnar Roth
** based on libvbox.h
** $Id: libvbox.h,v 1.8 1998/03/26 13:10:33 keil Exp $
**
** Copyright (C) 1996, 1997 Michael 'Ghandi' Herold
*/

#ifndef _VBOX_LIBVBOX_H
#define _VBOX_LIBVBOX_H 1
/*
 * Define to include both, <time.h> *and* <sys/time.h>:
 */

#define TIME_WITH_SYS_TIME 1

/*
 * Define if you have the <sys/time.h> header file:
 */

#define HAVE_SYS_TIME_H 1

/*
 * Define if you have the vsnprintf() function:
 */

#define HAVE_VSNPRINTF 1
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>

/**************************************************************************/
/** VBOXD                                                                **/
/**************************************************************************/

/** Defines ***************************************************************/



#define VBOXC_ERR_OK             ( 0)
#define VBOXC_ERR_UNKNOWNHOST    (-1)
#define VBOXC_ERR_NOSOCKET       (-2)
#define VBOXC_ERR_NOCONNECT      (-3)
#define VBOXC_ERR_NOFILEIO       (-4)
#define VBOXC_ERR_GETMESSAGE     (-5)
#define VBOXC_ERR_LOGIN          (-6)
#define VBOXC_ERR_ERROR          (-7)



/**************************************************************************/
/** MISC UTILS                                                           **/
/**************************************************************************/

/** Defines ***************************************************************/

#ifdef TRUE
#undef TRUE
#endif

#ifdef FALSE
#undef FALSE
#endif

#define TRUE					   (1)
#define FALSE					   (0)

#define VBOXD_MAX_LINE           (256)     /* Max line length from server */

#ifndef NAME_MAX
#define NAME_MAX 			255
#endif

#define VAH_MAX_NAME				(32)
#define VAH_MAX_CALLERID		(32)
#define VAH_MAX_PHONE			(32)
#define VAH_MAX_LOCATION		(64)

#define VBOXD_VAL_COUNT          "181"              /* Count new messages */
#define VBOXD_VAL_MESSAGE        "182"                   /* Get a message */
#define VBOXD_VAL_TOGGLE         "188"         /* Toggle message new flag */
#define VBOXD_VAL_LIST           "184"                   /* List messages */
#define VBOXD_VAL_DELETEOK       "287"               /* Delete message ok */


#define CTRL_NAME_MAGIC       "vboxctrl-"

#define CTRL_NAME_STOP		CTRL_NAME_MAGIC "stop"
#define CTRL_NAME_ANSWERNOW	CTRL_NAME_MAGIC "answernow"
#define CTRL_NAME_ANSWERALL	CTRL_NAME_MAGIC "answerall"
#define CTRL_NAME_REJECT	CTRL_NAME_MAGIC "reject"
#define CTRL_NAME_AUDIO		CTRL_NAME_MAGIC "audio"
#define CTRL_NAME_SUSPEND	CTRL_NAME_MAGIC "suspend"


/** Prototypes ***********************************************************/

extern void          xstrncpy(char *, char *, int);
extern void          xstrncat(char *, char *, int);
extern void          xpause(unsigned long);
extern long          xstrtol(char *, long);
extern unsigned long xstrtoul(char *, unsigned long);

struct tVboxMsg
{
	char   messagename[NAME_MAX + 1];
	time_t ctime;
	time_t mtime;
	int    compression;
    int    size;
	char   name[VAH_MAX_NAME + 1];
	char   callerid[VAH_MAX_CALLERID + 1];
	char   phone[VAH_MAX_PHONE + 1];
	char   location[VAH_MAX_LOCATION + 1];
	bool   bNew;
	bool   bDelete;
};

class cLibVBOX
{

/** Variables *************************************************************/

	int   vboxd_r_fd;                          /* Read file descriptor */
	int   vboxd_w_fd;                         /* Write file descriptor */
	char  vboxd_message[VBOXD_MAX_LINE + 1];

public:
	cLibVBOX() {vboxd_r_fd = vboxd_w_fd = -1;}
	int   vboxd_connect(char *, int);
	int   vboxd_login(char *, char *);
	bool  vboxd_isconnected() { return vboxd_r_fd != -1 && vboxd_w_fd != -1 ;}
	void  vboxd_disconnect(void);
	void  vboxd_put_message(const char *, ...);
	char *vboxd_get_message(void);
        int vboxd_test_response(const char *);
	void vboxd_keepalive() { vboxd_put_message("noop");}
	int vboxd_CreateAndLoadMessageFromServer(int fd, int size);
	int get_message_ptime(int, int);
	int vboxd_statusctrl(const char * ctrlname,int *pStatus);
	int vboxd_createctrl(const char * ctrlname);
	int vboxd_removectrl(const char * ctrlname);
protected:

private:

public:
	static const char *compressions[];

};
#endif /* _VBOX_LIBVBOX_H */
