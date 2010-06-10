/* -- gui.c -- */

#include "x11vnc.h"
#include "xevents.h"
#include "win_utils.h"
#include "remote.h"
#include "cleanup.h"

#include "tkx11vnc.h"

#define SYSTEM_TRAY_REQUEST_DOCK    0
#define SYSTEM_TRAY_BEGIN_MESSAGE   1
#define SYSTEM_TRAY_CANCEL_MESSAGE  2
#define XEMBED_VERSION 0
#define XEMBED_MAPPED  (1 << 0)

int icon_mode = 0;		/* hack for -gui tray */
char *icon_mode_file = NULL;
FILE *icon_mode_fh = NULL;
int icon_mode_socks[ICON_MODE_SOCKS];
int tray_manager_ok = 0;
Window tray_request = None;
Window tray_window = None;
int tray_unembed = 0;


char *get_gui_code(void);
int tray_embed(Window iconwin, int remove);
void do_gui(char *opts, int sleep);


static Window tweak_tk_window_id(Window win);
static int tray_manager_running(Display *d, Window *manager);
static void run_gui(char *gui_xdisplay, int connect_to_x11vnc, int start_x11vnc,
    int simple_gui, pid_t parent, char *gui_opts);


char *get_gui_code(void) {
	return gui_code;
}

static Window tweak_tk_window_id(Window win) {
	char *name = NULL;
	Window parent, new;

	/* hack for tk, does not report outermost window */
	new = win;
	parent = parent_window(win, &name);
	if (parent && name != NULL) {
		lowercase(name);
		if (strstr(name, "wish") || strstr(name, "x11vnc")) {
			new = parent;
			rfbLog("tray_embed: using parent: %s\n", name);
		}
	}
	if (name != NULL) {
		XFree(name);
	}
	return new;
}

int tray_embed(Window iconwin, int remove) {
	XEvent ev;
	XErrorHandler old_handler;
	Window manager;
	Atom xembed_info;
	Atom tatom;
	XWindowAttributes attr;
	long info[2] = {XEMBED_VERSION, XEMBED_MAPPED};
	long data = 0;

	if (remove) {
		if (!valid_window(iconwin, &attr, 1)) {
			return 0;
		}
		iconwin = tweak_tk_window_id(iconwin);
		trapped_xerror = 0;
		old_handler = XSetErrorHandler(trap_xerror);

		/*
		 * unfortunately no desktops seem to obey this
		 * part of the XEMBED spec yet...
		 */
		XReparentWindow(dpy, iconwin, rootwin, 0, 0);

		XSetErrorHandler(old_handler);
		if (trapped_xerror) {
			trapped_xerror = 0;
			return 0;
		}
		trapped_xerror = 0;
		return 1;
	}

	xembed_info = XInternAtom(dpy, "_XEMBED_INFO", False);
	if (xembed_info == None) {
		return 0;
	}

	if (!tray_manager_running(dpy, &manager)) {
		return 0;
	}

	memset(&ev, 0, sizeof(ev));
	ev.xclient.type = ClientMessage;
	ev.xclient.window = manager;
	ev.xclient.message_type = XInternAtom(dpy, "_NET_SYSTEM_TRAY_OPCODE",
	    False);
	ev.xclient.format = 32;
	ev.xclient.data.l[0] = CurrentTime;
	ev.xclient.data.l[1] = SYSTEM_TRAY_REQUEST_DOCK;
	ev.xclient.data.l[2] = iconwin;
	ev.xclient.data.l[3] = 0;
	ev.xclient.data.l[4] = 0;

	if (!valid_window(iconwin, &attr, 1)) {
		return 0;
	}

	iconwin = tweak_tk_window_id(iconwin);
	ev.xclient.data.l[2] = iconwin;

	XUnmapWindow(dpy, iconwin);

	trapped_xerror = 0;
	old_handler = XSetErrorHandler(trap_xerror);

	XSendEvent(dpy, manager, False, NoEventMask, &ev);
	XSync(dpy, False);

	if (trapped_xerror) {
		XSetErrorHandler(old_handler);
		trapped_xerror = 0;
		return 0;
	}

	XChangeProperty(dpy, iconwin, xembed_info, xembed_info, 32,
	    PropModeReplace, (unsigned char *)&info, 2);

	/* kludge for KDE evidently needed... */
	tatom = XInternAtom(dpy, "KWM_DOCKWINDOW", False);
	XChangeProperty(dpy, iconwin, tatom, tatom, 32, PropModeReplace,
	    (unsigned char *)&data, 1);
	tatom = XInternAtom(dpy, "_KDE_NET_WM_SYSTEM_TRAY_WINDOW_FOR", False);
	XChangeProperty(dpy, iconwin, tatom, XA_WINDOW, 32, PropModeReplace,
	    (unsigned char *)&data, 1);

	XSetErrorHandler(old_handler);
	trapped_xerror = 0;
	return 1;
}

static int tray_manager_running(Display *d, Window *manager) {
	char tray_string[100];
	Atom tray_manager;
	Window tray_win;

	if (manager) {
		*manager = None;
	}
	sprintf(tray_string, "_NET_SYSTEM_TRAY_S%d", scr);

	tray_manager = XInternAtom(d, tray_string, True);
	if (tray_manager == None) {
		return 0;
	}

	tray_win = XGetSelectionOwner(d, tray_manager);
	if (manager) {
		*manager = tray_win;
	}

	if (tray_win == None) {
		return 0;
	} else {
		return 1;
	}
}

static char *gui_geometry = NULL;
static int icon_in_tray = 0;
static char *icon_mode_embed_id = NULL;
static char *icon_mode_font = NULL;
static char *icon_mode_params = NULL;

static void run_gui(char *gui_xdisplay, int connect_to_x11vnc, int start_x11vnc,
    int simple_gui, pid_t parent, char *gui_opts) {
	char *x11vnc_xdisplay = NULL;
	char extra_path[] = ":/usr/local/bin:/usr/bin/X11:/usr/sfw/bin"
	    ":/usr/X11R6/bin:/usr/openwin/bin:/usr/dt/bin";
	char cmd[100];
	char *wish = NULL, *orig_path, *full_path, *tpath, *p;
	char *old_xauth = NULL;
	int try_max = 4, sleep = 300;
	pid_t mypid = getpid();
	FILE *pipe, *tmpf;

	if (*gui_code == '\0') {
		rfbLog("gui: gui not compiled into this program.\n");
		exit(0);
	}
	if (getenv("DISPLAY") != NULL) {
		/* worst case */
		x11vnc_xdisplay = strdup(getenv("DISPLAY"));
	}
	if (use_dpy) {
		/* better */
		x11vnc_xdisplay = strdup(use_dpy);
	}
	if (connect_to_x11vnc) {
		int rc, i;
		rfbLogEnable(1);
		if (! client_connect_file) {
			if (getenv("XAUTHORITY") != NULL) {
				old_xauth = strdup(getenv("XAUTHORITY"));
			} else {
				old_xauth = strdup("");
			}
			dpy = XOpenDisplay(x11vnc_xdisplay); 
			if (! dpy && auth_file) {
				set_env("XAUTHORITY", auth_file);
				dpy = XOpenDisplay(x11vnc_xdisplay);
			}
			if (! dpy && ! x11vnc_xdisplay) {
				/* worstest case */
				x11vnc_xdisplay = strdup(":0");
				dpy = XOpenDisplay(x11vnc_xdisplay); 
			}
			if (! dpy) {
				rfbLog("gui: could not open x11vnc "
				    "display: %s\n", NONUL(x11vnc_xdisplay));
				exit(1);
			}
			scr = DefaultScreen(dpy);
			rootwin = RootWindow(dpy, scr);
			initialize_vnc_connect_prop();
		}
		usleep(2200*1000);
		fprintf(stderr, "\n");
		if (!quiet) {
			rfbLog("gui: trying to contact a x11vnc server at X"
			    " display %s ...\n", NONUL(x11vnc_xdisplay));
		}
		for (i=0; i<try_max; i++) {
			usleep(sleep*1000);
			if (!quiet) {
				rfbLog("gui: pinging %s try=%d ...\n",
				    NONUL(x11vnc_xdisplay), i+1);
			}
			rc = send_remote_cmd("qry=ping", 1, 1);
			if (rc == 0) {
				break;
			}
			if (parent && mypid != parent && kill(parent, 0) != 0) {
				rfbLog("gui: parent process %d has gone"
				    " away: bailing out.\n", parent);
				rc = 1;
				break;
			}
		}
		set_env("X11VNC_XDISPLAY", x11vnc_xdisplay);
		if (getenv("XAUTHORITY") != NULL) {
			set_env("X11VNC_AUTH_FILE", getenv("XAUTHORITY"));
		}
		if (rc == 0) {
			rfbLog("gui: ping succeeded.\n");
			set_env("X11VNC_CONNECT", "1");
		} else {
			rfbLog("gui: could not connect to: '%s', try"
			    " again manually.\n", x11vnc_xdisplay);
		}
		if (client_connect_file) {
			set_env("X11VNC_CONNECT_FILE", client_connect_file);
		}
		if (dpy)  {
			XCloseDisplay(dpy);
			dpy = NULL;
		}
		if (old_xauth) {
			if (*old_xauth == '\0') {
				/* wasn't set, hack it out if it is now */
				char *xauth = getenv("XAUTHORITY");
				if (xauth) {
					*(xauth-2) = '_';	/* yow */
				}
			} else {
				set_env("XAUTHORITY", old_xauth);
			}
			free(old_xauth);
		}
		rfbLogEnable(0);
	}

	orig_path = getenv("PATH");
	if (! orig_path) {
		orig_path = strdup("/bin:/usr/bin:/usr/bin/X11");
	}
	full_path = (char *) malloc(strlen(orig_path)+strlen(extra_path)+1);
	strcpy(full_path, orig_path);
	strcat(full_path, extra_path);

	tpath = strdup(full_path);
	p = strtok(tpath, ":");

	while (p) {
		char *try;
		struct stat sbuf;
		char *wishes[] = {"wish", "wish8.3", "wish8.4", "wish8.5",
		    "wish8.0"};
		int nwishes = 3, i;

		try = (char *) malloc(strlen(p) + 1 + strlen("wish8.4") + 1);
		for (i=0; i<nwishes; i++) {
			sprintf(try, "%s/%s", p, wishes[i]);
			if (stat(try, &sbuf) == 0) {
				/* assume executable, should check mode */
				wish = wishes[i];
			}
		}
		free(try);
		if (wish) {
			break;
		}
		p = strtok(NULL, ":");
	}
	free(tpath);
	if (!wish) {
		wish = strdup("wish");
	}
	set_env("PATH", full_path);
	set_env("DISPLAY", gui_xdisplay);
	set_env("X11VNC_PROG", program_name);
	set_env("X11VNC_CMDLINE", program_cmdline);
	set_env("X11VNC_WISHCMD", wish);
	if (simple_gui) {
		set_env("X11VNC_SIMPLE_GUI", "1");
	}
	if (gui_opts) {
		set_env("X11VNC_GUI_PARAMS", gui_opts);
	}
	if (gui_geometry) {
		set_env("X11VNC_GUI_GEOM", gui_geometry);
	}
	if (start_x11vnc) {
		set_env("X11VNC_STARTED", "1");
	}
	if (icon_mode) {
		set_env("X11VNC_ICON_MODE", "1");
		if (icon_mode_file) {
			set_env("X11VNC_CLIENT_FILE", icon_mode_file);
		}
		if (icon_in_tray) {
			if (tray_manager_ok) {
				set_env("X11VNC_ICON_MODE", "TRAY:RUNNING");
			} else {
				set_env("X11VNC_ICON_MODE", "TRAY");
			}
		} else {
			set_env("X11VNC_ICON_MODE", "ICON");
		}
		if (icon_mode_params) {
			char *p, *str = strdup(icon_mode_params);
			p = strtok(str, ":-/,.+");
			while (p) {
				if(strstr(p, "setp") == p) {
					set_env("X11VNC_ICON_SETPASS", "1");
				} else if(strstr(p, "noadvanced") == p) {
					set_env("X11VNC_ICON_NOADVANCED", "1");
				} else if(strstr(p, "minimal") == p) {
					set_env("X11VNC_ICON_MINIMAL", "1");
				} else if (strstr(p, "0x") == p) {
					set_env("X11VNC_ICON_EMBED_ID", p);
					icon_mode_embed_id = strdup(p);
				}
				p = strtok(NULL, ":-/,.+");
			}
			free(str);
		}
	}
	if (icon_mode_font) {
		set_env("X11VNC_ICON_FONT", icon_mode_font);
	}

	if (no_external_cmds) {
		fprintf(stderr, "cannot run external commands in -nocmds "
		    "mode:\n");
		fprintf(stderr, "   \"%s\"\n", "gui + wish");
		fprintf(stderr, "   exiting.\n");
		fflush(stderr);
		exit(1);
	}

	tmpf = tmpfile();
	if (tmpf == NULL) {
		/* if no tmpfile, use a pipe */
		if (icon_mode_embed_id) {
			if (strlen(icon_mode_embed_id) < 20) {
				strcat(cmd, " -use ");
				strcat(cmd, icon_mode_embed_id);
			}
		}
		pipe = popen(cmd, "w");
		if (! pipe) {
			fprintf(stderr, "could not run: %s\n", cmd);
			perror("popen");
		}
		fprintf(pipe, "%s", gui_code);
		pclose(pipe);
	} else {
		/*
		 * we prefer a tmpfile since then this x11vnc process
		 * will then be gone, otherwise the x11vnc program text
		 * will still be in use.
		 */
		int n = fileno(tmpf);
		fprintf(tmpf, "%s", gui_code);
		fflush(tmpf);
		rewind(tmpf);
		dup2(n, 0);
		close(n);
		if (icon_mode_embed_id) {
			execlp(wish, wish, "-", "-use", icon_mode_embed_id,
			    (char *) NULL); 
		} else {
			execlp(wish, wish, "-", (char *) NULL); 
		}
		fprintf(stderr, "could not exec wish: %s -\n", wish);
		perror("execlp");
	}
	exit(0);
}

void do_gui(char *opts, int sleep) {
	char *s, *p;
	char *old_xauth = NULL;
	char *gui_xdisplay = NULL;
	int got_gui_xdisplay = 0;
	int start_x11vnc = 1;
	int connect_to_x11vnc = 0;
	int simple_gui = 0, none_gui = 0;
	Display *test_dpy;

	if (opts) {
		s = strdup(opts);
	} else {
		s = strdup("");
	}

	if (use_dpy) {
		/* worst case */
		gui_xdisplay = strdup(use_dpy);
		
	}
	if (getenv("DISPLAY") != NULL) {
		/* better */
		gui_xdisplay = strdup(getenv("DISPLAY"));
	}

	p = strtok(s, ",");

	while(p) {
		if (*p == '\0') {
			;
		} else if (strchr(p, ':') != NULL) {
			/* best */
			if (gui_xdisplay) {
				free(gui_xdisplay);
			}
			gui_xdisplay = strdup(p);
			got_gui_xdisplay = 1;
		} else if (!strcmp(p, "wait")) {
			start_x11vnc = 0;
			connect_to_x11vnc = 0;
		} else if (!strcmp(p, "none")) {
			none_gui = 1;
		} else if (!strcmp(p, "conn") || !strcmp(p, "connect")) {
			start_x11vnc = 0;
			connect_to_x11vnc = 1;
		} else if (!strcmp(p, "ez") || !strcmp(p, "simple")) {
			simple_gui = 1;
		} else if (strstr(p, "iconfont") == p) {
			char *q;
			if ((q = strchr(p, '=')) != NULL) {
				icon_mode_font = strdup(q+1);
			}
		} else if (!strcmp(p, "full")) {
			;
		} else if (strstr(p, "tray") == p || strstr(p, "icon") == p) {
			char *q;
			icon_mode = 1;
			if ((q = strchr(p, '=')) != NULL) {
				icon_mode_params = strdup(q+1);
				if (strstr(icon_mode_params, "setp")) {
					deny_all = 1;
				}
			}
			if (strstr(p, "tray") == p) {
				icon_in_tray = 1;
			}
		} else if (strstr(p, "geom") == p) {
			char *q;
			if ((q = strchr(p, '=')) != NULL) {
				gui_geometry = strdup(q+1);
			}
		} else {
			fprintf(stderr, "unrecognized gui opt: %s\n", p);
		}
		
		p = strtok(NULL, ",");
	}
	free(s);

	if (none_gui) {
		if (!start_x11vnc) {
			exit(0);
		}
		return;
	}
	if (start_x11vnc) {
		connect_to_x11vnc = 1;
	}

	if (icon_mode && !got_gui_xdisplay) {
		/* for tray mode, prefer the polled DISPLAY */
		if (use_dpy) {
			if (gui_xdisplay) {
				free(gui_xdisplay);
			}
			gui_xdisplay = strdup(use_dpy);
		}
	}

	if (! gui_xdisplay) {
		fprintf(stderr, "error: cannot determine X DISPLAY for gui"
		    " to display on.\n");
		exit(1);
	}
	if (!quiet) {
		fprintf(stderr, "starting gui, trying display: %s\n",
		    gui_xdisplay);
	}
	test_dpy = XOpenDisplay(gui_xdisplay);
	if (! test_dpy && auth_file) {
		if (getenv("XAUTHORITY") != NULL) {
			old_xauth = strdup(getenv("XAUTHORITY"));
		}
		set_env("XAUTHORITY", auth_file);
		test_dpy = XOpenDisplay(gui_xdisplay);
	}
	if (! test_dpy) {
		if (! old_xauth && getenv("XAUTHORITY") != NULL) {
			old_xauth = strdup(getenv("XAUTHORITY"));
		}
		set_env("XAUTHORITY", "");
		test_dpy = XOpenDisplay(gui_xdisplay);
	}
	if (! test_dpy) {
		fprintf(stderr, "error: cannot connect to gui X DISPLAY: %s\n",
		    gui_xdisplay);
		exit(1);
	}
	if (icon_mode && icon_in_tray) {
		if (tray_manager_running(test_dpy, NULL)) {
			tray_manager_ok = 1;
		} else {
			tray_manager_ok = 0;
		}
	}
	XCloseDisplay(test_dpy);

	if (start_x11vnc) {

#if LIBVNCSERVER_HAVE_FORK
		/* fork into the background now */
		int p;
		pid_t parent = getpid();

		if (icon_mode) {
			char tf[100]; 
			double dn = dnow();
			struct stat sbuf;
			/* FIXME */
			dn = dn - ((int) dn);
			sprintf(tf, "/tmp/x11vnc.tray%d%d", (int) (1000000*dn),
			    (int) getpid());
			unlink(tf);
			/* race begins.. */
			if (stat(tf, &sbuf) == 0) {
				icon_mode = 0;
			} else {
				icon_mode_fh = fopen(tf, "w");
				if (! icon_mode_fh) {
					icon_mode = 0;
				} else {
					chmod(tf, 0400);
					icon_mode_file = strdup(tf);
					fprintf(icon_mode_fh, "none\n");
					fprintf(icon_mode_fh, "none\n");
					fflush(icon_mode_fh);
					if (! got_connect_once) {
						/* want -forever for tray */
						connect_once = 0;
					}
				}
			}
		}

		if ((p = fork()) > 0)  {
			;	/* parent */
		} else if (p == -1) {
			fprintf(stderr, "could not fork\n");
			perror("fork");
			clean_up_exit(1);
		} else {
			if (sleep > 0) {
				usleep(sleep * 1000 * 1000);
			}
			run_gui(gui_xdisplay, connect_to_x11vnc, start_x11vnc,
			    simple_gui, parent, opts);
			exit(1);
		}
#else
		fprintf(stderr, "system does not support fork: start "
		    "x11vnc in the gui.\n");
		start_x11vnc = 0;
#endif
	}
	if (!start_x11vnc) {
		run_gui(gui_xdisplay, connect_to_x11vnc, start_x11vnc,
		    simple_gui, 0, opts);
		exit(1);
	}
	if (old_xauth) {
		set_env("XAUTHORITY", old_xauth);
	}
}


