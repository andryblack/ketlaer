/* GyS-TermIO v2.0 (for GySmail v3)          (C) 1999 A'rpi/ESP-team */
/* a very small replacement of ncurses library */

/* Screen size. Initialized by load_termcap() and get_screen_size() */
#ifdef __cplusplus
extern "C" {
#endif
extern int screen_width;
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
extern int screen_height;
#ifdef __cplusplus
}
#endif

/* Get screen-size using IOCTL call. */
#ifdef __cplusplus
extern "C" {
#endif
extern void get_screen_size();
#ifdef __cplusplus
}
#endif

/* Load key definitions from the TERMCAP database. 'termtype' can be NULL */
#ifdef __cplusplus
extern "C" {
#endif
extern int load_termcap(char *termtype);
#ifdef __cplusplus
}
#endif

/* Enable and disable STDIN line-buffering */
#ifdef __cplusplus
extern "C" {
#endif
extern void getch2_enable();
#ifdef __cplusplus
}
#endif
#ifdef __cplusplus
extern "C" {
#endif
extern void getch2_disable();
#ifdef __cplusplus
}
#endif

/* Read a character or a special key code (see keycodes.h) */
#ifdef __cplusplus
extern "C" {
#endif
extern int getch2(int halfdelay_time);
#ifdef __cplusplus
}
#endif

