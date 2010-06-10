/* -- pointer.c -- */

#include "x11vnc.h"
#include "xwrappers.h"
#include "keyboard.h"
#include "xinerama.h"
#include "xrecord.h"
#include "win_utils.h"
#include "cursor.h"
#include "userinput.h"
#include "connections.h"
#include "cleanup.h"

int pointer_queued_sent = 0;

void initialize_pointer_map(char *pointer_remap);
void do_button_mask_change(int mask, int button);
void pointer(int mask, int x, int y, rfbClientPtr client);
void initialize_pipeinput(void);
int check_pipeinput(void);


static void buttonparse(int from, char **s);
static void update_x11_pointer_position(int x, int y);
static void update_x11_pointer_mask(int mask);
static void pipe_pointer(int mask, int x, int y, rfbClientPtr client);


/*
 * pointer event (motion and button click) handling routines.
 */
typedef struct ptrremap {
	KeySym keysym;
	KeyCode keycode;
	int end;
	int button;
	int down;
	int up;
} prtremap_t;

#ifdef LIBVNCSERVER_HAVE_LIBPTHREAD
MUTEX(pointerMutex);
#endif
#define MAX_BUTTONS 5
#define MAX_BUTTON_EVENTS 50
static prtremap_t pointer_map[MAX_BUTTONS+1][MAX_BUTTON_EVENTS];

/*
 * For parsing the -buttonmap sections, e.g. "4" or ":Up+Up+Up:"
 */
static void buttonparse(int from, char **s) {
	char *q;
	int to, i;
	int modisdown[256];

	q = *s;

	for (i=0; i<256; i++) {
		modisdown[i] = 0;
	}

	if (*q == ':') {
		/* :sym1+sym2+...+symN: format */
		int l = 0, n = 0;
		char list[1000];
		char *t, *kp = q + 1;
		KeyCode kcode;

		while (*(kp+l) != ':' && *(kp+l) != '\0') {
			/* loop to the matching ':' */
			l++;
			if (l >= 1000) {
				rfbLog("buttonparse: keysym list too long: "
				    "%s\n", q);
				break;
			}
		}
		*(kp+l) = '\0';
		strncpy(list, kp, l);
		list[l] = '\0';
		rfbLog("remap button %d using \"%s\"\n", from, list);

		/* loop over tokens separated by '+' */
		t = strtok(list, "+");
		while (t) {
			KeySym ksym;
			unsigned int ui;
			int i;
			if (n >= MAX_BUTTON_EVENTS - 20) {
				rfbLog("buttonparse: too many button map "
					"events: %s\n", list);
				break;
			}
			if (sscanf(t, "0x%x", &ui) == 1) {
				ksym = (KeySym) ui;	/* hex value */
			} else {
				X_LOCK;
				ksym = XStringToKeysym(t); /* string value */
				X_UNLOCK;
			}
			if (ksym == NoSymbol) {
				/* see if Button<N> "keysym" was used: */
				if (sscanf(t, "Button%d", &i) == 1) {
					rfbLog("   event %d: button %d\n",
					    from, n+1, i);
					if (i == 0) i = -1; /* bah */
					pointer_map[from][n].keysym  = NoSymbol;
					pointer_map[from][n].keycode = NoSymbol;
					pointer_map[from][n].button = i;
					pointer_map[from][n].end    = 0;
					pointer_map[from][n].down   = 0;
					pointer_map[from][n].up     = 0;
				} else {
					rfbLog("buttonparse: ignoring unknown "
					    "keysym: %s\n", t);
					n--;
				}
			} else {
				/*
				 * XXX may not work with -modtweak or -xkb
				 */
				X_LOCK;
				kcode = XKeysymToKeycode(dpy, ksym);

				pointer_map[from][n].keysym  = ksym;
				pointer_map[from][n].keycode = kcode;
				pointer_map[from][n].button = 0;
				pointer_map[from][n].end    = 0;
				if (! ismodkey(ksym) ) {
					/* do both down then up */
					pointer_map[from][n].down = 1;
					pointer_map[from][n].up   = 1;
				} else {
					if (modisdown[kcode]) {
						pointer_map[from][n].down = 0;
						pointer_map[from][n].up   = 1;
						modisdown[kcode] = 0;
					} else {
						pointer_map[from][n].down = 1;
						pointer_map[from][n].up   = 0;
						modisdown[kcode] = 1;
					}
				}
				rfbLog("   event %d: keysym %s (0x%x) -> "
				    "keycode 0x%x down=%d up=%d\n", n+1,
				    XKeysymToString(ksym), ksym, kcode,
				    pointer_map[from][n].down,
				    pointer_map[from][n].up);
				X_UNLOCK;
			}
			t = strtok(NULL, "+");
			n++;
		}

		/* we must release any modifiers that are still down: */
		for (i=0; i<256; i++) {
			kcode = (KeyCode) i;
			if (n >= MAX_BUTTON_EVENTS) {
				rfbLog("buttonparse: too many button map "
					"events: %s\n", list);
				break;
			}
			if (modisdown[kcode]) {
				pointer_map[from][n].keysym  = NoSymbol;
				pointer_map[from][n].keycode = kcode;
				pointer_map[from][n].button = 0;
				pointer_map[from][n].end    = 0;
				pointer_map[from][n].down = 0;
				pointer_map[from][n].up   = 1;
				modisdown[kcode] = 0;
				n++;
			}
		}

		/* advance the source pointer position */
		(*s) += l+2;
	} else {
		/* single digit format */
		char str[2];
		str[0] = *q;
		str[1] = '\0';

		to = atoi(str);
		if (to < 1) {
			rfbLog("skipping invalid remap button \"%d\" for button"
			    " %d from string \"%s\"\n",
			    to, from, str);
		} else {
			rfbLog("remap button %d using \"%s\"\n", from, str);
			rfbLog("   button: %d -> %d\n", from, to);
			pointer_map[from][0].keysym  = NoSymbol;
			pointer_map[from][0].keycode = NoSymbol;
			pointer_map[from][0].button = to;
			pointer_map[from][0].end    = 0;
			pointer_map[from][0].down   = 0;
			pointer_map[from][0].up     = 0;
		}
		/* advance the source pointer position */
		(*s)++;
	}
}

/*
 * process the -buttonmap string
 */
void initialize_pointer_map(char *pointer_remap) {
	unsigned char map[MAX_BUTTONS];
	int i, k;
	/*
	 * This routine counts the number of pointer buttons on the X
	 * server (to avoid problems, even crashes, if a client has more
	 * buttons).  And also initializes any pointer button remapping
	 * from -buttonmap option.
	 */
	
	X_LOCK;
	num_buttons = XGetPointerMapping(dpy, map, MAX_BUTTONS);
	X_UNLOCK;

	if (num_buttons < 0) {
		num_buttons = 0;
	}

	/* FIXME: should use info in map[] */
	for (i=1; i<= MAX_BUTTONS; i++) {
		for (k=0; k < MAX_BUTTON_EVENTS; k++) {
			pointer_map[i][k].end = 1;
		}
		pointer_map[i][0].keysym  = NoSymbol;
		pointer_map[i][0].keycode = NoSymbol;
		pointer_map[i][0].button = i;
		pointer_map[i][0].end    = 0;
		pointer_map[i][0].down   = 0;
		pointer_map[i][0].up     = 0;
	}

	if (pointer_remap && *pointer_remap != '\0') {
		/* -buttonmap, format is like: 12-21=2 */
		char *p, *q, *remap = strdup(pointer_remap);	
		int n;

		if ((p = strchr(remap, '=')) != NULL) {
			/* undocumented max button number */
			n = atoi(p+1);	
			*p = '\0';
			if (n < num_buttons || num_buttons == 0) {
				num_buttons = n;
			} else {
				rfbLog("warning: increasing number of mouse "
				    "buttons from %d to %d\n", num_buttons, n);
				num_buttons = n;
			}
		}
		if ((q = strchr(remap, '-')) != NULL) {
			/*
			 * The '-' separates the 'from' and 'to' lists,
			 * then it is kind of like tr(1).  
			 */
			char str[2];
			int from;

			rfbLog("remapping pointer buttons using string:\n");
			rfbLog("   \"%s\"\n", remap);

			p = remap;
			q++;
			i = 0;
			str[1] = '\0';
			while (*p != '-') {
				str[0] = *p;
				from = atoi(str);
				buttonparse(from, &q);
				p++;
			}
		}
		free(remap);
	}
}

/*
 * Send a pointer position event to the X server.
 */
static void update_x11_pointer_position(int x, int y) {
	int rc;

	if (raw_fb && ! dpy) return;	/* raw_fb hack */

	X_LOCK;
	if (use_xwarppointer) {
		/*
		 * off_x and off_y not needed with XWarpPointer since
		 * window is used:
		 */
		XWarpPointer(dpy, None, window, 0, 0, 0, 0, x + coff_x,
		    y + coff_y);
	} else {
		XTestFakeMotionEvent_wr(dpy, scr, x + off_x + coff_x,
		    y + off_y + coff_y, CurrentTime);
	}
	X_UNLOCK;

	if (cursor_x != x || cursor_y != y) {
		last_pointer_motion_time = dnow();
	}

	cursor_x = x;
	cursor_y = y;

	/* record the x, y position for the rfb screen as well. */
	cursor_position(x, y);

	/* change the cursor shape if necessary */
	rc = set_cursor(x, y, get_which_cursor());
	cursor_changes += rc;

	last_event = last_input = last_pointer_input = time(0);
}

void do_button_mask_change(int mask, int button) {
	int mb, k, i = button-1;

	/*
	 * this expands to any pointer_map button -> keystrokes
	 * remappings.  Usually just k=0 and we send one button event.
	 */
	for (k=0; k < MAX_BUTTON_EVENTS; k++) {
		int bmask = (mask & (1<<i));

		if (pointer_map[i+1][k].end) {
			break;
		}

		if (pointer_map[i+1][k].button) {
			/* send button up or down */

			mb = pointer_map[i+1][k].button;
			if ((num_buttons && mb > num_buttons) || mb < 1) {
				rfbLog("ignoring mouse button out of "
				    "bounds: %d>%d mask: 0x%x -> 0x%x\n",
				    mb, num_buttons, button_mask, mask);
				continue;
			}
			if (debug_pointer) {
				rfbLog("pointer(): sending button %d"
				    " %s (event %d)\n", mb, bmask
				    ? "down" : "up", k+1);
			}
			XTestFakeButtonEvent_wr(dpy, mb, (mask & (1<<i))
			    ? True : False, CurrentTime);
		} else {
			/* send keysym up or down */
			KeyCode key = pointer_map[i+1][k].keycode;
			int up   = pointer_map[i+1][k].up;
			int down = pointer_map[i+1][k].down;

			if (! bmask) {
				/* do not send keysym on button up */
				continue; 
			}
			if (debug_pointer) {
				rfbLog("pointer(): sending button %d "
				    "down as keycode 0x%x (event %d)\n",
				    i+1, key, k+1);
				rfbLog("           down=%d up=%d "
				    "keysym: %s\n", down, up,
				    XKeysymToString(XKeycodeToKeysym(
				    dpy, key, 0)));
			}
			if (down) {
				XTestFakeKeyEvent_wr(dpy, key, True,
				    CurrentTime);
			}
			if (up) {
				XTestFakeKeyEvent_wr(dpy, key, False,
				    CurrentTime);
			}
		}
	}
}

/*
 * Send a pointer button event to the X server.
 */
static void update_x11_pointer_mask(int mask) {
	int snapped = 0, xr_mouse = 1, i;

	last_event = last_input = last_pointer_input = time(0);

	if (raw_fb && ! dpy) return;	/* raw_fb hack */

	if (mask != button_mask) {
		last_pointer_click_time = dnow();
	}

	if (nofb) {
		xr_mouse = 0;
	} else if (!strcmp(scroll_copyrect, "never")) {
		xr_mouse = 0;
	} else if (!strcmp(scroll_copyrect, "keys")) {
		xr_mouse = 0;
	} else if (skip_cr_when_scaling("scroll")) {
		xr_mouse = 0;
	} else if (xrecord_skip_button(mask, button_mask)) {
		xr_mouse = 0;
	}

	if (mask && use_xrecord && ! xrecording && xr_mouse) {
		static int px, py, x, y, w, h, got_wm_frame;
		static XWindowAttributes attr;
		Window frame = None, mwin = None;
		int skip = 0;

		if (!button_mask) {
			X_LOCK;
			if (get_wm_frame_pos(&px, &py, &x, &y, &w, &h,
			    &frame, &mwin)) {
				got_wm_frame = 1;
if (debug_scroll > 1) fprintf(stderr, "wm_win: 0x%lx\n", mwin);
				if (mwin != None) {
					if (!valid_window(mwin, &attr, 1)) {
						mwin = None;
					}
				}
			} else {
				got_wm_frame = 0;
			}
			X_UNLOCK;
		}
		if (got_wm_frame) {
			if (wireframe && near_wm_edge(x, y, w, h, px, py)) {
				/* step out of wireframe's way */
				skip = 1;
			} else {
				int ok = 0;
				int btn4 = (1<<3);
				int btn5 = (1<<4);

				if (near_scrollbar_edge(x, y, w, h, px, py)) {
					ok = 1;
				}
				if (mask & (btn4|btn5)) {
					/* scroll wheel mouse */
					ok = 1;
				}
				if (mwin != None) {
					/* skinny internal window */
					int w = attr.width;
					int h = attr.height;
					if (h > 10 * w || w > 10 * h) {
if (debug_scroll > 1) fprintf(stderr, "internal scrollbar: %dx%d\n", w, h);
						ok = 1;
					}
				}
				if (! ok) {
					skip = 1;
				}
			}
		}

		if (! skip) {
			xrecord_watch(1, SCR_MOUSE);
			snapshot_stack_list(0, 0.50);
			snapped = 1;
			if (button_mask) {
				xrecord_set_by_mouse = 1;
			} else {
				update_stack_list();
				xrecord_set_by_mouse = 2;
			}
		}
	}

	if (mask && !button_mask) {
		/* button down, snapshot the stacking list before flushing */
		if (wireframe && !wireframe_in_progress &&
		    strcmp(wireframe_copyrect, "never")) {
			if (! snapped) {
				snapshot_stack_list(0, 0.0);
			}
		}
	}

	X_LOCK;

	/* look for buttons that have be clicked or released: */
	for (i=0; i < MAX_BUTTONS; i++) {
	    if ( (button_mask & (1<<i)) != (mask & (1<<i)) ) {
		if (debug_pointer) {
			rfbLog("pointer(): mask change: mask: 0x%x -> "
			    "0x%x button: %d\n", button_mask, mask,i+1);
		}
		do_button_mask_change(mask, i+1);	/* button # is i+1 */
	    }
	}

	X_UNLOCK;

	/*
	 * Remember the button state for next time and also for the
	 * -nodragging case:
	 */
	button_mask_prev = button_mask;
	button_mask = mask;
}

/* for -pipeinput */


static void pipe_pointer(int mask, int x, int y, rfbClientPtr client) {
	int can_input = 0, uid;
	allowed_input_t input;
	ClientData *cd = (ClientData *) client->clientData;
	char hint[MAX_BUTTONS * 20];

	if (pipeinput_fh == NULL) {
		return;
	}

	if (! view_only) {
		get_allowed_input(client, &input);
		if (input.motion || input.button) {
			can_input = 1;	/* XXX distinguish later */
		}
	}
	uid = cd->uid;
	if (! can_input) {
		uid = -uid;
	}

	hint[0] = '\0';
	if (mask == button_mask) {
		strcat(hint, "None");
	} else {
		int i, old, new, m = 1, cnt = 0;
		for (i=0; i<MAX_BUTTONS; i++) {
			char s[20];

			old = button_mask & m;
			new = mask & m;
			m = m << 1;

			if (old == new) {
				continue;
			}
			if (hint[0] != '\0') {
				strcat(hint, ",");
			}
			if (new && ! old) {
				sprintf(s, "ButtonPress-%d", i+1);
				cnt++;
			} else if (! new && old)  {
				sprintf(s, "ButtonRelease-%d", i+1);
				cnt++;
			}
			strcat(hint, s);
		}
		if (! cnt) {
			strcpy(hint, "None");
		}
	}

	fprintf(pipeinput_fh, "Pointer %d %d %d %d %s\n", uid, x, y,
	    mask, hint);
	fflush(pipeinput_fh);
	check_pipeinput();
}

/*
 * Actual callback from libvncserver when it gets a pointer event.
 * This may queue pointer events rather than sending them immediately
 * to the X server. (see update_x11_pointer*())
 */
void pointer(int mask, int x, int y, rfbClientPtr client) {
	allowed_input_t input;
	int sent = 0, buffer_it = 0;
	double now;

	if (mask >= 0) {
		got_pointer_calls++;
	}

	if (debug_pointer && mask >= 0) {
		static int show_motion = -1;
		static double last_pointer = 0.0;
		double tnow, dt;
		static int last_x, last_y;
		if (show_motion == -1) {
			if (getenv("X11VNC_DB_NOMOTION")) {
				show_motion = 0;
			} else {
				show_motion = 1;
			}
		}
		dtime0(&tnow);
		tnow -= x11vnc_start;
		dt = tnow - last_pointer;
		last_pointer = tnow;
		if (show_motion) {
			rfbLog("# pointer(mask: 0x%x, x:%4d, y:%4d) "
			    "dx: %3d dy: %3d dt: %.4f t: %.4f\n", mask, x, y,
			    x - last_x, y - last_y, dt, tnow);
		}
		last_x = x;
		last_y = y;
	}

	if (scaling) {
		/* map from rfb size to X11 size: */
		x = ((double) x / scaled_x) * dpy_x;
		x = nfix(x, dpy_x);
		y = ((double) y / scaled_y) * dpy_y;
		y = nfix(y, dpy_y);
	}

	if (pipeinput_fh != NULL && mask >= 0) {
		pipe_pointer(mask, x, y, client);
		if (! pipeinput_tee) {
			if (! view_only || raw_fb) {	/* raw_fb hack */
				got_user_input++;
				got_keyboard_input++;
				last_pointer_client = client;
			}
			if (view_only && raw_fb) {
				/* raw_fb hack track button state */
				button_mask_prev = button_mask;
				button_mask = mask;
			}
			return;
		}
	}

	if (view_only) {
		return;
	}

	now = dnow();

	if (mask >= 0) {
		/*
		 * mask = -1 is a special case call from scan_for_updates()
		 * to flush the event queue; there is no real pointer event.
		 */
		get_allowed_input(client, &input);
		if (! input.motion && ! input.button) {
			return;
		}

		got_user_input++;
		got_pointer_input++;
		last_pointer_client = client;

		last_pointer_time = now;

		if (blackout_ptr && blackouts) {
			int b, ok = 1;
			/* see if it goes into the blacked out region */
			for (b=0; b < blackouts; b++) {
				if (x < blackr[b].x1 || x > blackr[b].x2) {
					continue;
				}
				if (y < blackr[b].y1 || y > blackr[b].y2) {
					continue;
				}
				/* x1 <= x <= x2 and y1 <= y <= y2 */
				ok = 0;
				break;
			}
			if (! ok) {
				if (debug_pointer) {
				    rfbLog("pointer(): blackout_ptr skipping "
					"x=%d y=%d in rectangle %d,%d %d,%d\n", x, y,
					blackr[b].x1, blackr[b].y1,
					blackr[b].x2, blackr[b].y2);
				}
				return;
			}
		}
	}

	/*
	 * The following is hopefully an improvement wrt response during
	 * pointer user input (window drags) for the threaded case.
	 * See check_user_input() for the more complicated things we do
	 * in the non-threaded case.
	 */
	if ((use_threads && pointer_mode != 1) || pointer_flush_delay > 0.0) {
#		define NEV 32
		/* storage for the event queue */
		static int mutex_init = 0;
		static int nevents = 0;
		static int ev[NEV][3];
		int i;
		/* timer things */
		static double dt = 0.0, tmr = 0.0, maxwait = 0.4;

		if (! mutex_init) {
			INIT_MUTEX(pointerMutex);
			mutex_init = 1;
		}

		if (pointer_flush_delay > 0.0) {
			maxwait = pointer_flush_delay;
		}
		if (mask >= 0) {
			if (fb_copy_in_progress || pointer_flush_delay > 0.0) {
				buffer_it = 1;
			}
		}

		LOCK(pointerMutex);

		/* 
		 * If the framebuffer is being copied in another thread
		 * (scan_for_updates()), we will queue up to 32 pointer
		 * events for later.  The idea is by delaying these input
		 * events, the screen is less likely to change during the
		 * copying period, and so will give rise to less window
		 * "tearing".
		 *
		 * Tearing is not completely eliminated because we do
		 * not suspend work in the other libvncserver threads.
		 * Maybe that is a possibility with a mutex...
		 */
		if (buffer_it) {
			/* 
			 * mask = -1 is an all-clear signal from
			 * scan_for_updates().
			 *
			 * dt is a timer in seconds; we only queue for so long.
			 */
			dt += dtime(&tmr);

			if (nevents < NEV && dt < maxwait) {
				i = nevents++;
				ev[i][0] = mask;
				ev[i][1] = x;
				ev[i][2] = y;
				if (! input.button) {
					ev[i][0] = -1;
				}
				if (! input.motion) {
					ev[i][1] = -1;
					ev[i][2] = -1;
				}
				UNLOCK(pointerMutex);
				if (debug_pointer) {
					rfbLog("pointer(): deferring event %d"
					    " %.4f\n", i, tmr - x11vnc_start);
				}
				return;
			}
		}

		/* time to send the queue */
		for (i=0; i<nevents; i++) {
			int sent = 0;
			if (mask < 0 && client != NULL) {
				/* hack to only push the latest event */
				if (i < nevents - 1) {
					if (debug_pointer) {
						rfbLog("- skip deferred event:"
						    " %d\n", i);
					}
					continue;
				}
			}
			if (debug_pointer) {
				rfbLog("pointer(): sending event %d %.4f\n",
				    i+1, dnow() - x11vnc_start);
			}
			if (ev[i][1] >= 0) {
				update_x11_pointer_position(ev[i][1], ev[i][2]);
				sent = 1;
			}
			if (ev[i][0] >= 0) {
				update_x11_pointer_mask(ev[i][0]);
				sent = 1;
			}

			if (sent) {
				pointer_queued_sent++;
			}
		}
		if (nevents && dt > maxwait) {
		    if (dpy) {	/* raw_fb hack */
			if (mask < 0) {
				if (debug_pointer) {
					rfbLog("pointer(): calling XFlush "
					    "%.4f\n", dnow() - x11vnc_start);
				}
				X_LOCK;
				XFlush(dpy);	
				X_UNLOCK;
			}
		    }
		}
		nevents = 0;	/* reset everything */
		dt = 0.0;
		dtime0(&tmr);

		UNLOCK(pointerMutex);
	}
	if (mask < 0) {		/* -1 just means flush the event queue */
		if (debug_pointer) {
			rfbLog("pointer(): flush only.  %.4f\n",
			    dnow() - x11vnc_start);
		}
		return;
	}

	/* update the X display with the event: */
	if (input.motion) {
		update_x11_pointer_position(x, y);
		sent = 1;
	}
	if (input.button) {
		if (mask != button_mask) {
			button_change_x = cursor_x;
			button_change_y = cursor_y;
		}
		update_x11_pointer_mask(mask);
		sent = 1;
	}

	if (nofb && sent) {
		/* 
		 * nofb is for, e.g. Win2VNC, where fastest pointer
		 * updates are desired.
		 */
		X_LOCK;
		XFlush(dpy);
		X_UNLOCK;
	} else if (buffer_it) {
		if (debug_pointer) {
			rfbLog("pointer(): calling XFlush+"
			    "%.4f\n", dnow() - x11vnc_start);
		}
		X_LOCK;
		XFlush(dpy);	
		X_UNLOCK;
	}
}

void initialize_pipeinput(void) {
	char *p;

	if (pipeinput_fh != NULL) {
		rfbLog("closing pipeinput stream: %p\n", pipeinput_fh);
		pclose(pipeinput_fh);
		pipeinput_fh = NULL;
	}

	pipeinput_tee = 0;
	if (pipeinput_opts) {
		free(pipeinput_opts);
		pipeinput_opts = NULL;
	}

	if (! pipeinput_str) {
		return;
	}

	/* look for options:  tee, reopen, ... */
	p = strchr(pipeinput_str, ':');
	if (p != NULL) {
		char *str, *opt, *q;
		int got = 0;
		*p = '\0';
		str = strdup(pipeinput_str);
		opt = strdup(pipeinput_str);
		*p = ':';
		q = strtok(str, ",");
		while (q) {
			if (!strcmp(q, "key") || !strcmp(q, "keycodes")) {
				got = 1;
			}
			if (!strcmp(q, "reopen")) {
				got = 1;
			}
			if (!strcmp(q, "tee")) {
				pipeinput_tee = 1;
				got = 1;
			}
			q = strtok(NULL, ",");
		}
		if (got) {
			pipeinput_opts = opt;
		} else {
			free(opt);
		}
		free(str);
		p++;
	} else {
		p = pipeinput_str;
	}

	set_child_info();
	if (no_external_cmds) {
		rfbLogEnable(1);
		rfbLog("cannot run external commands in -nocmds mode:\n");
		rfbLog("   \"%s\"\n", p);
		rfbLog("   exiting.\n");
		clean_up_exit(1);
	}
	rfbLog("pipeinput: starting: \"%s\"...\n", p);
	pipeinput_fh = popen(p, "w");

	if (! pipeinput_fh) {
		rfbLog("popen(\"%s\", \"w\") failed.\n", p);
		rfbLogPerror("popen");
		rfbLog("Disabling -pipeinput mode.\n");
		return;
	}

	fprintf(pipeinput_fh, "%s",
"# \n"
"# Format of the -pipeinput stream:\n"
"# --------------------------------\n"
"#\n"
"# Lines like these beginning with '#' are to be ignored.\n"
"#\n"
"# Pointer events (mouse motion and button clicks) come in the form:\n"
"#\n"
"#\n"
"# Pointer <client#> <x> <y> <mask> <hint>\n"
"#\n"
"#\n"
"# The <client#> is a decimal integer uniquely identifying the client\n"
"# that generated the event.  If it is negative that means this event\n"
"# would have been discarded since the client was viewonly.\n"
"#\n"
"# <x> and <y> are decimal integers reflecting the position on the screen\n"
"# the event took place at.\n"
"#\n"
"# <mask> is the button mask indicating the button press state, as normal\n"
"# 0 means no buttons pressed, 1 means button 1 is down 3 (11) means buttons\n"
"# 1 and 2 are down, etc.\n"
"#\n"
"# <hint> is a string containing no spaces and may be ignored.\n"
"# It contains some interpretation about what has happened.\n"
"# It can be:\n"
"#\n"
"#	None		(nothing to report)\n"
"#	ButtonPress-N	(this event will cause button-1 to be pressed) \n"
"#	ButtonRelease-N	(this event will cause button-1 to be released) \n"
"#\n"
"# if two more more buttons change state in one event they are listed\n"
"# separated by commas.\n"
"#\n"
"# One might parse a Pointer line with:\n"
"#\n"
"# int client, x, y, mask; char *hint;\n"
"# sscanf(line, \"Pointer %d %d %d %s\", &client, &x, &y, &mask, &hint);\n"
"#\n"
"#\n"
"# Keysym events (keyboard presses and releases) come in the form:\n"
"#\n"
"#\n"
"# Keysym <client#> <down> <keysym#> <keysym-name> <hint>\n"
"#\n"
"#\n"
"# The <client#> is as with Pointer.\n"
"#\n"
"# <down> is a decimal either 1 or 0 indicating KeyPress or KeyRelease,\n"
"# respectively.\n"
"#\n"
"# <keysym#> is a decimal integer incidating the Keysym of the event.\n"
"#\n"
"# <keysym-name> is the corresponding Keysym name.\n"
"#\n"
"# See the file /usr/include/X11/keysymdef.h for the mappings.\n"
"# You basically remove the leading 'XK_' prefix from the macro name in\n"
"# that file to get the Keysym name.\n"
"#\n"
"# One might parse a Keysym line with:\n"
"#\n"
"# int client, down, keysym; char *name, *hint;\n"
"# sscanf(line, \"Keysym %d %d %s %s\", &client, &down, &keysym, &name, &hint);\n"
"#\n"
"# The <hint> value is currently just None, KeyPress, or KeyRelease.\n"
"#\n"
"# In the future <hint> will provide a hint for the sequence of KeyCodes\n"
"# (i.e. keyboard scancodes) that x11vnc would inject to an X display to\n"
"# simulate the Keysym.\n"
"#\n"
"# You see, some Keysyms will require more than one injected Keycode to\n"
"# generate the symbol.  E.g. the Keysym \"ampersand\" going down usually\n"
"# requires a Shift key going down, then the key with the \"&\" on it going\n"
"# down, and, perhaps, the Shift key going up (that is how x11vnc does it).\n"
"#\n"
"# The Keysym => Keycode(s) stuff gets pretty messy.  Hopefully the Keysym\n"
"# info will be enough for most purposes (having identical keyboards on\n"
"# both sides helps).\n"
"#\n"
"# Here comes your stream.  The following token will always indicate the\n"
"# end of this informational text:\n"
"# END_OF_TOP\n"
);
	fflush(pipeinput_fh);
	if (raw_fb_str) {
		/* the pipe program may actually create the fb */
		sleep(1);
	}
}

int check_pipeinput(void) {
	if (! pipeinput_fh) {
		return 1;
	}
	if (ferror(pipeinput_fh)) {
		rfbLog("pipeinput pipe has ferror. %p\n", pipeinput_fh);
		
		if (pipeinput_opts && strstr(pipeinput_opts, "reopen")) {
			rfbLog("restarting -pipeinput pipe...\n");
			initialize_pipeinput();
			if (pipeinput_fh) {
				return 1;
			} else {
				return 0;
			}
		} else {
			rfbLog("closing -pipeinput pipe...\n");
			pclose(pipeinput_fh);
			pipeinput_fh = NULL;
			return 0;
		}
	}
	return 1;
}


