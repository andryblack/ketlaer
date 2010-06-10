/* -- xrandr.c -- */

#include "x11vnc.h"
#include "cleanup.h"
#include "connections.h"
#include "remote.h"
#include "screen.h"
#include "win_utils.h"

time_t last_subwin_trap = 0;
int subwin_trap_count = 0;
XErrorHandler old_getimage_handler;

int xrandr_present = 0;
int xrandr_width  = -1;
int xrandr_height = -1;
int xrandr_rotation = -1;
Time xrandr_timestamp = 0;
Time xrandr_cfg_time = 0;

void initialize_xrandr(void);
int check_xrandr_event(char *msg);
int known_xrandr_mode(char *s);


static int handle_subwin_resize(char *msg);
static void handle_xrandr_change(int new_x, int new_y);


void initialize_xrandr(void) {
	if (xrandr_present) {
#if LIBVNCSERVER_HAVE_LIBXRANDR
		Rotation rot;

		X_LOCK;
		xrandr_width  = XDisplayWidth(dpy, scr);
		xrandr_height = XDisplayHeight(dpy, scr);
		XRRRotations(dpy, scr, &rot);
		xrandr_rotation = (int) rot;
		if (xrandr) {
			XRRSelectInput(dpy, rootwin, RRScreenChangeNotifyMask);
		} else {
			XRRSelectInput(dpy, rootwin, 0);
		}
		X_UNLOCK;
#endif
	} else if (xrandr) {
		rfbLog("-xrandr mode specified, but no RANDR support on\n");
		rfbLog(" display or in client library. Disabling -xrandr "
		    "mode.\n");
		xrandr = 0;
	}
}

static int handle_subwin_resize(char *msg) {
	int new_x, new_y;
	int i, check = 10, ms = 250;	/* 2.5 secs total... */

	if (msg) {}	/* unused vars warning: */
	if (! subwin) {
		return 0;	/* hmmm... */
	}
	if (! valid_window(subwin, NULL, 0)) {
		rfbLogEnable(1);
		rfbLog("subwin 0x%lx went away!\n", subwin);
		X_UNLOCK;
		clean_up_exit(1);
	}
	if (! get_window_size(subwin, &new_x, &new_y)) {
		rfbLogEnable(1);
		rfbLog("could not get size of subwin 0x%lx\n", subwin);
		X_UNLOCK;
		clean_up_exit(1);
	}
	if (wdpy_x == new_x && wdpy_y == new_y) {
		/* no change */
		return 0;
	}

	/* window may still be changing (e.g. drag resize) */
	for (i=0; i < check; i++) {
		int newer_x, newer_y;
		usleep(ms * 1000);

		if (! get_window_size(subwin, &newer_x, &newer_y)) {
			rfbLogEnable(1);
			rfbLog("could not get size of subwin 0x%lx\n", subwin);
			clean_up_exit(1);
		}
		if (new_x == newer_x && new_y == newer_y) {
			/* go for it... */
			break;
		} else {
			rfbLog("subwin 0x%lx still changing size...\n", subwin);
			new_x = newer_x;
			new_y = newer_y;
		}
	}

	rfbLog("subwin 0x%lx new size: x: %d -> %d, y: %d -> %d\n",
	    subwin, wdpy_x, new_x, wdpy_y, new_y);
	rfbLog("calling handle_xrandr_change() for resizing\n");

	X_UNLOCK;
	handle_xrandr_change(new_x, new_y);
	return 1;
}

static void handle_xrandr_change(int new_x, int new_y) {
	rfbClientIteratorPtr iter;
	rfbClientPtr cl;

	/* sanity check xrandr_mode */
	if (! xrandr_mode) {
		xrandr_mode = strdup("default");
	} else if (! known_xrandr_mode(xrandr_mode)) {
		free(xrandr_mode);
		xrandr_mode = strdup("default");
	}
	rfbLog("xrandr_mode: %s\n", xrandr_mode);
	if (!strcmp(xrandr_mode, "exit")) {
		close_all_clients();
		rfbLog("  shutting down due to XRANDR event.\n");
		clean_up_exit(0);
	}
	if (!strcmp(xrandr_mode, "newfbsize") && screen) {
		iter = rfbGetClientIterator(screen);
		while( (cl = rfbClientIteratorNext(iter)) ) {
			if (cl->useNewFBSize) {
				continue;
			}
			rfbLog("  closing client %s (no useNewFBSize"
			    " support).\n", cl->host);
			rfbCloseClient(cl);
			rfbClientConnectionGone(cl);
		}
		rfbReleaseClientIterator(iter);
	}
	
	/* default, resize, and newfbsize create a new fb: */
	rfbLog("check_xrandr_event: trying to create new framebuffer...\n");
	if (new_x < wdpy_x || new_y < wdpy_y) {
		check_black_fb();
	}
	do_new_fb(1);
	rfbLog("check_xrandr_event: fb       WxH: %dx%d\n", wdpy_x, wdpy_y);
}

int check_xrandr_event(char *msg) {
	XEvent xev;
	if (subwin) {
		return handle_subwin_resize(msg);
	}
#if LIBVNCSERVER_HAVE_LIBXRANDR
	if (! xrandr || ! xrandr_present) {
		return 0;
	}
	if (xrandr_base_event_type && XCheckTypedEvent(dpy,
	    xrandr_base_event_type + RRScreenChangeNotify, &xev)) {
		int do_change;
		XRRScreenChangeNotifyEvent *rev;

		rev = (XRRScreenChangeNotifyEvent *) &xev;
		rfbLog("check_xrandr_event():\n");
		rfbLog("Detected XRANDR event at location '%s':\n", msg);
		rfbLog("  serial:          %d\n", (int) rev->serial);
		rfbLog("  timestamp:       %d\n", (int) rev->timestamp);
		rfbLog("  cfg_timestamp:   %d\n", (int) rev->config_timestamp);
		rfbLog("  size_id:         %d\n", (int) rev->size_index);
		rfbLog("  sub_pixel:       %d\n", (int) rev->subpixel_order);
		rfbLog("  rotation:        %d\n", (int) rev->rotation);
		rfbLog("  width:           %d\n", (int) rev->width);
		rfbLog("  height:          %d\n", (int) rev->height);
		rfbLog("  mwidth:          %d mm\n", (int) rev->mwidth);
		rfbLog("  mheight:         %d mm\n", (int) rev->mheight);
		rfbLog("\n");
		rfbLog("check_xrandr_event: previous WxH: %dx%d\n",
		    wdpy_x, wdpy_y);
		if (wdpy_x == rev->width && wdpy_y == rev->height &&
		    xrandr_rotation == (int) rev->rotation) {
		    rfbLog("check_xrandr_event: no change detected.\n");
			do_change = 0;
		} else {
			do_change = 1;
		}

		xrandr_width  = rev->width;
		xrandr_height = rev->height;
		xrandr_timestamp = rev->timestamp;
		xrandr_cfg_time  = rev->config_timestamp;
		xrandr_rotation = (int) rev->rotation;

		rfbLog("check_xrandr_event: updating config...\n");
		XRRUpdateConfiguration(&xev);

		if (do_change) {
			X_UNLOCK;
			handle_xrandr_change(rev->width, rev->height);
		}
		rfbLog("check_xrandr_event: current  WxH: %dx%d\n",
		    XDisplayWidth(dpy, scr), XDisplayHeight(dpy, scr));
		rfbLog("check_xrandr_event(): returning control to"
		    " caller...\n");
		return do_change;
	}
#endif
	return 0;
}

int known_xrandr_mode(char *s) {
/*
 * default:	
 * resize:	the default
 * exit:	shutdown clients and exit.
 * newfbsize:	shutdown clients that do not support NewFBSize encoding.
 */
	if (strcmp(s, "default") && strcmp(s, "resize") && 
	    strcmp(s, "exit") && strcmp(s, "newfbsize")) {
		return 0;
	} else {
		return 1;
	}
}


