#ifndef _X11VNC_OPTIONS_H
#define _X11VNC_OPTIONS_H

/* -- options.h -- */

/* 
 * variables for the command line options
 */
extern int debug;

extern char *use_dpy;
extern char *auth_file;
extern char *visual_str;
extern char *logfile;
extern int logfile_append;
extern char *flagfile;
extern char *passwdfile;
extern char *blackout_str;
extern int blackout_ptr;
extern char *clip_str;
extern int use_solid_bg;
extern char *solid_str;
extern char *solid_default;

extern char *wmdt_str;

extern char *speeds_str;
extern char *rc_rcfile;
extern int rc_rcfile_default;
extern int rc_norc;
extern int got_norc;
extern int opts_bg;

extern int shared;
extern int connect_once;
extern int got_connect_once;
extern int deny_all;
extern int accept_remote_cmds;
extern int query_default;
extern int safe_remote_only;
extern int priv_remote;
extern int more_safe;
extern int no_external_cmds;
extern int started_as_root;
extern int host_lookup;
extern char *users_list;
extern char *allow_list;
extern char *listen_str;
extern char *allow_once;
extern char *accept_cmd;
extern char *afteraccept_cmd;
extern char *gone_cmd;
extern int view_only;
extern char *allowed_input_view_only;
extern char *allowed_input_normal;
extern char *allowed_input_str;
extern char *viewonly_passwd;
extern char **passwd_list;
extern int begin_viewonly;
extern int inetd;
extern int filexfer; 
extern int first_conn_timeout;
extern int flash_cmap;
extern int shift_cmap;
extern int force_indexed_color;
extern int cmap8to24;
extern char *cmap8to24_str;
extern int launch_gui;

extern int use_modifier_tweak;
extern int use_iso_level3;
extern int clear_mods;
extern int nofb;
extern char *raw_fb_str;
extern char *pipeinput_str;
extern char *pipeinput_opts;
extern FILE *pipeinput_fh;
extern int pipeinput_tee;

extern unsigned long subwin;
extern int subwin_wait_mapped;

extern int debug_xevents;
extern int debug_xdamage;
extern int debug_wireframe;
extern int debug_tiles;
extern int debug_grabs;

extern int xtrap_input;
extern int xinerama;
extern int xrandr;
extern char *xrandr_mode;
extern char *pad_geometry;
extern time_t pad_geometry_time;
extern int use_snapfb;

extern int use_xrecord;
extern int noxrecord;

extern char *client_connect;
extern char *client_connect_file;
extern int vnc_connect;

extern int show_cursor;
extern int show_multiple_cursors;
extern char *multiple_cursors_mode;
extern int cursor_pos_updates;
extern int cursor_shape_updates;
extern int use_xwarppointer;
extern int show_dragging;
extern int wireframe;

extern char *wireframe_str;
extern char *wireframe_copyrect;
extern char *wireframe_copyrect_default;
extern int wireframe_in_progress;

extern char *scroll_copyrect_str;
extern char *scroll_copyrect;
extern char *scroll_copyrect_default;
extern char *scroll_key_list_str;
extern KeySym *scroll_key_list;

extern int scaling_copyrect0;
extern int scaling_copyrect;

extern int scrollcopyrect_min_area;
extern int debug_scroll;
extern double pointer_flush_delay;
extern double last_scroll_event;
extern int max_scroll_keyrate;
extern double max_keyrepeat_time;
extern char *max_keyrepeat_str;
extern char *max_keyrepeat_str0;
extern int max_keyrepeat_lo, max_keyrepeat_hi;

extern char **scroll_good_all;
extern char **scroll_good_key;
extern char **scroll_good_mouse;
extern char *scroll_good_str;
extern char *scroll_good_str0;

extern char **scroll_skip_all;
extern char **scroll_skip_key;
extern char **scroll_skip_mouse;
extern char *scroll_skip_str;
extern char *scroll_skip_str0;

extern char **scroll_term;
extern char *scroll_term_str;
extern char *scroll_term_str0;

extern char* screen_fixup_str;
extern double screen_fixup_V;
extern double screen_fixup_C;
extern double screen_fixup_X;

extern int no_autorepeat;
extern int no_repeat_countdown;
extern int watch_bell;
extern int sound_bell;
extern int xkbcompat;
extern int use_xkb_modtweak;
extern int skip_duplicate_key_events;
extern char *skip_keycodes;
extern int sloppy_keys;
extern int add_keysyms;

extern char *remap_file;
extern char *pointer_remap;
extern int pointer_mode;
extern int pointer_mode_max;	
extern int single_copytile;
extern int single_copytile_orig;
extern int single_copytile_count;
extern int tile_shm_count;

extern int using_shm;
extern int flip_byte_order;
extern int waitms;
extern double wait_ui;
extern double slow_fb;
extern int wait_bog;
extern int defer_update;
extern int got_defer;
extern int got_deferupdate;

extern int screen_blank;

extern int no_fbu_blank;
extern int take_naps;
extern int naptile;
extern int napfac;
extern int napmax;
extern int ui_skip;

extern int watch_selection;
extern int watch_primary;
extern char *sel_direction;

extern char *sigpipe;

extern VisualID visual_id;
extern int visual_depth;

extern int overlay;
extern int overlay_cursor;

extern double fs_frac;
extern int tile_fuzz;

extern int grow_fill;
extern int gaps_fill;

extern int debug_pointer;
extern int debug_keyboard;

extern int quiet;

extern int use_threads;

extern int got_rfbport;
extern int got_alwaysshared;
extern int got_nevershared;
extern int got_cursorpos;
extern int got_pointer_mode;
extern int got_noviewonly;
extern int got_wirecopyrect;
extern int got_scrollcopyrect;
extern int got_noxkb;
extern int got_nomodtweak;

#endif /* _X11VNC_OPTIONS_H */
