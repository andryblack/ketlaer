/* -- scan.c -- */

#include "x11vnc.h"
#include "xinerama.h"
#include "xwrappers.h"
#include "xdamage.h"
#include "xrandr.h"
#include "win_utils.h"
#include "8to24.h"
#include "screen.h"
#include "pointer.h"
#include "cleanup.h"

/*
 * routines for scanning and reading the X11 display for changes, and
 * for doing all the tile work (shm, etc).
 */
void initialize_tiles(void);
void free_tiles(void);
void shm_delete(XShmSegmentInfo *shm);
void shm_clean(XShmSegmentInfo *shm, XImage *xim);
void initialize_polling_images(void);
void scale_rect(double factor, int blend, int interpolate, int Bpp,
    char *src_fb, int src_bytes_per_line, char *dst_fb, int dst_bytes_per_line,
    int Nx, int Ny, int nx, int ny, int X1, int Y1, int X2, int Y2, int mark);
void scale_and_mark_rect(int X1, int Y1, int X2, int Y2);
void mark_rect_as_modified(int x1, int y1, int x2, int y2, int force);
int copy_screen(void);
int copy_snap(void);
void nap_sleep(int ms, int split);
void set_offset(void);
int scan_for_updates(int count_only);


static void set_fs_factor(int max);
static char *flip_ximage_byte_order(XImage *xim);
static int shm_create(XShmSegmentInfo *shm, XImage **ximg_ptr, int w, int h,
    char *name);
static void create_tile_hint(int x, int y, int tw, int th, hint_t *hint);
static void extend_tile_hint(int x, int y, int tw, int th, hint_t *hint);
static void save_hint(hint_t hint, int loc);
static void hint_updates(void);
static void mark_hint(hint_t hint);
static int copy_tiles(int tx, int ty, int nt);
static int copy_all_tiles(void);
static int copy_all_tile_runs(void);
static int copy_tiles_backward_pass(void);
static int copy_tiles_additional_pass(void);
static int gap_try(int x, int y, int *run, int *saw, int along_x);
static int fill_tile_gaps(void);
static int island_try(int x, int y, int u, int v, int *run);
static int grow_islands(void);
static void blackout_regions(void);
static void nap_set(int tile_cnt);
static void nap_check(int tile_cnt);
static void ping_clients(int tile_cnt);
static int blackout_line_skip(int n, int x, int y, int rescan,
    int *tile_count);
static int blackout_line_cmpskip(int n, int x, int y, char *dst, char *src,
    int w, int pixelsize);
static int scan_display(int ystart, int rescan);


/* array to hold the hints: */
static hint_t *hint_list;

/* nap state */
int nap_ok = 0;
static int nap_diff_count = 0;

static int scan_count = 0;	/* indicates which scan pattern we are on  */
static int scan_in_progress = 0;	


typedef struct tile_change_region {
	/* start and end lines, along y, of the changed area inside a tile. */
	unsigned short first_line, last_line;
	short first_x, last_x;
	/* info about differences along edges. */
	unsigned short left_diff, right_diff;
	unsigned short top_diff,  bot_diff;
} region_t;

/* array to hold the tiles region_t-s. */
static region_t *tile_region;




/*
 * setup tile numbers and allocate the tile and hint arrays:
 */
void initialize_tiles(void) {

	ntiles_x = (dpy_x - 1)/tile_x + 1;
	ntiles_y = (dpy_y - 1)/tile_y + 1;
	ntiles = ntiles_x * ntiles_y;

	tile_has_diff = (unsigned char *)
		malloc((size_t) (ntiles * sizeof(unsigned char)));
	tile_has_xdamage_diff = (unsigned char *)
		malloc((size_t) (ntiles * sizeof(unsigned char)));
	tile_row_has_xdamage_diff = (unsigned char *)
		malloc((size_t) (ntiles_y * sizeof(unsigned char)));
	tile_tried    = (unsigned char *)
		malloc((size_t) (ntiles * sizeof(unsigned char)));
	tile_copied   = (unsigned char *)
		malloc((size_t) (ntiles * sizeof(unsigned char)));
	tile_blackout    = (tile_blackout_t *)
		malloc((size_t) (ntiles * sizeof(tile_blackout_t)));
	tile_region = (region_t *) malloc((size_t) (ntiles * sizeof(region_t)));

	tile_row = (XImage **)
		malloc((size_t) ((ntiles_x + 1) * sizeof(XImage *)));
	tile_row_shm = (XShmSegmentInfo *)
		malloc((size_t) ((ntiles_x + 1) * sizeof(XShmSegmentInfo)));

	/* there will never be more hints than tiles: */
	hint_list = (hint_t *) malloc((size_t) (ntiles * sizeof(hint_t)));
}

void free_tiles(void) {
	if (tile_has_diff) {
		free(tile_has_diff);
		tile_has_diff = NULL;
	}
	if (tile_has_xdamage_diff) {
		free(tile_has_xdamage_diff);
		tile_has_xdamage_diff = NULL;
	}
	if (tile_row_has_xdamage_diff) {
		free(tile_row_has_xdamage_diff);
		tile_row_has_xdamage_diff = NULL;
	}
	if (tile_tried) {
		free(tile_tried);
		tile_tried = NULL;
	}
	if (tile_copied) {
		free(tile_copied);
		tile_copied = NULL;
	}
	if (tile_blackout) {
		free(tile_blackout);
		tile_blackout = NULL;
	}
	if (tile_region) {
		free(tile_region);
		tile_region = NULL;
	}
	if (tile_row) {
		free(tile_row);
		tile_row = NULL;
	}
	if (tile_row_shm) {
		free(tile_row_shm);
		tile_row_shm = NULL;
	}
	if (hint_list) {
		free(hint_list);
		hint_list = NULL;
	}
}

/*
 * silly function to factor dpy_y until fullscreen shm is not bigger than max.
 * should always work unless dpy_y is a large prime or something... under
 * failure fs_factor remains 0 and no fullscreen updates will be tried.
 */
static int fs_factor = 0;

static void set_fs_factor(int max) {
	int f, fac = 1, n = dpy_y;

	fs_factor = 0;
	if ((bpp/8) * dpy_x * dpy_y <= max)  {
		fs_factor = 1;
		return;
	}
	for (f=2; f <= 101; f++) {
		while (n % f == 0) {
			n = n / f;
			fac = fac * f;
			if ( (bpp/8) * dpy_x * (dpy_y/fac) <= max )  {
				fs_factor = fac;
				return;
			}
		}
	}
}

static char *flip_ximage_byte_order(XImage *xim) {
	char *order;
	if (xim->byte_order == LSBFirst) {
		order = "MSBFirst";
		xim->byte_order = MSBFirst;
		xim->bitmap_bit_order = MSBFirst;
	} else {
		order = "LSBFirst";
		xim->byte_order = LSBFirst;
		xim->bitmap_bit_order = LSBFirst;
	}
	return order;
}

/*
 * set up an XShm image, or if not using shm just create the XImage.
 */
static int shm_create(XShmSegmentInfo *shm, XImage **ximg_ptr, int w, int h,
    char *name) {

	XImage *xim;
	static int reported_flip = 0;

	shm->shmid = -1;
	shm->shmaddr = (char *) -1;
	*ximg_ptr = NULL;

	if (nofb) {
		return 1;
	}

	X_LOCK;

	if (! using_shm) {
		/* we only need the XImage created */
		xim = XCreateImage_wr(dpy, default_visual, depth, ZPixmap,
		    0, NULL, w, h, raw_fb ? 32 : BitmapPad(dpy), 0);

		X_UNLOCK;

		if (xim == NULL) {
			rfbErr("XCreateImage(%s) failed.\n", name);
			if (quiet) {
				fprintf(stderr, "XCreateImage(%s) failed.\n",
				    name);
			}
			return 0;
		}
		xim->data = (char *) malloc(xim->bytes_per_line * xim->height);
		if (xim->data == NULL) {
			rfbErr("XCreateImage(%s) data malloc failed.\n", name);
			if (quiet) {
				fprintf(stderr, "XCreateImage(%s) data malloc"
				    " failed.\n", name);
			}
			return 0;
		}
		if (flip_byte_order) {
			char *order = flip_ximage_byte_order(xim);
			if (! reported_flip && ! quiet) {
				rfbLog("Changing XImage byte order"
				    " to %s\n", order);
				reported_flip = 1;
			}
		}

		*ximg_ptr = xim;
		return 1;
	}

	xim = XShmCreateImage_wr(dpy, default_visual, depth, ZPixmap, NULL,
	    shm, w, h);

	if (xim == NULL) {
		rfbErr("XShmCreateImage(%s) failed.\n", name);
		if (quiet) {
			fprintf(stderr, "XShmCreateImage(%s) failed.\n", name);
		}
		X_UNLOCK;
		return 0;
	}

	*ximg_ptr = xim;

#if LIBVNCSERVER_HAVE_XSHM
	shm->shmid = shmget(IPC_PRIVATE,
	    xim->bytes_per_line * xim->height, IPC_CREAT | 0777);

	if (shm->shmid == -1) {
		rfbErr("shmget(%s) failed.\n", name);
		rfbLogPerror("shmget");

		XDestroyImage(xim);
		*ximg_ptr = NULL;

		X_UNLOCK;
		return 0;
	}

	shm->shmaddr = xim->data = (char *) shmat(shm->shmid, 0, 0);

	if (shm->shmaddr == (char *)-1) {
		rfbErr("shmat(%s) failed.\n", name);
		rfbLogPerror("shmat");

		XDestroyImage(xim);
		*ximg_ptr = NULL;

		shmctl(shm->shmid, IPC_RMID, 0);
		shm->shmid = -1;

		X_UNLOCK;
		return 0;
	}

	shm->readOnly = False;

	if (! XShmAttach_wr(dpy, shm)) {
		rfbErr("XShmAttach(%s) failed.\n", name);
		XDestroyImage(xim);
		*ximg_ptr = NULL;

		shmdt(shm->shmaddr);
		shm->shmaddr = (char *) -1;

		shmctl(shm->shmid, IPC_RMID, 0);
		shm->shmid = -1;

		X_UNLOCK;
		return 0;
	}
#endif

	X_UNLOCK;
	return 1;
}

void shm_delete(XShmSegmentInfo *shm) {
#if LIBVNCSERVER_HAVE_XSHM
	if (shm != NULL && shm->shmaddr != (char *) -1) {
		shmdt(shm->shmaddr);
	}
	if (shm != NULL && shm->shmid != -1) {
		shmctl(shm->shmid, IPC_RMID, 0);
	}
#endif
}

void shm_clean(XShmSegmentInfo *shm, XImage *xim) {

	X_LOCK;
#if LIBVNCSERVER_HAVE_XSHM
	if (shm != NULL && shm->shmid != -1 && dpy) {	/* raw_fb hack */
		XShmDetach_wr(dpy, shm);
	}
#endif
	if (xim != NULL) {
		XDestroyImage(xim);
		xim = NULL;
	}
	X_UNLOCK;

	shm_delete(shm);
}

void initialize_polling_images(void) {
	int i, MB = 1024 * 1024;

	/* set all shm areas to "none" before trying to create any */
	scanline_shm.shmid	= -1;
	scanline_shm.shmaddr	= (char *) -1;
	scanline		= NULL;
	fullscreen_shm.shmid	= -1;
	fullscreen_shm.shmaddr	= (char *) -1;
	fullscreen		= NULL;
	snaprect_shm.shmid	= -1;
	snaprect_shm.shmaddr	= (char *) -1;
	snaprect		= NULL;
	for (i=1; i<=ntiles_x; i++) {
		tile_row_shm[i].shmid	= -1;
		tile_row_shm[i].shmaddr	= (char *) -1;
		tile_row[i]		= NULL;
	}

	/* the scanline (e.g. 1280x1) shared memory area image: */

	if (! shm_create(&scanline_shm, &scanline, dpy_x, 1, "scanline")) {
		clean_up_exit(1);
	}

	/*
	 * the fullscreen (e.g. 1280x1024/fs_factor) shared memory area image:
	 * (we cut down the size of the shm area to try avoid and shm segment
	 * limits, e.g. the default 1MB on Solaris)
	 */
	if (UT.sysname && strstr(UT.sysname, "Linux")) {
		set_fs_factor(10 * MB);
	} else {
		set_fs_factor(1 * MB);
	}
	if (fs_frac >= 1.0) {
		fs_frac = 1.1;
		fs_factor = 0;
	}
	if (! fs_factor) {
		rfbLog("warning: fullscreen updates are disabled.\n");
	} else {
		if (! shm_create(&fullscreen_shm, &fullscreen, dpy_x,
		    dpy_y/fs_factor, "fullscreen")) {
			clean_up_exit(1);
		} 
	}
	if (use_snapfb) {
		if (! fs_factor) {
			rfbLog("warning: disabling -snapfb mode.\n");
			use_snapfb = 0;
		} else if (! shm_create(&snaprect_shm, &snaprect, dpy_x,
		    dpy_y/fs_factor, "snaprect")) {
			clean_up_exit(1);
		} 
	}

	/*
	 * for copy_tiles we need a lot of shared memory areas, one for
	 * each possible run length of changed tiles.  32 for 1024x768
	 * and 40 for 1280x1024, etc. 
	 */

	tile_shm_count = 0;
	for (i=1; i<=ntiles_x; i++) {
		if (! shm_create(&tile_row_shm[i], &tile_row[i], tile_x * i,
		    tile_y, "tile_row")) {
			if (i == 1) {
				clean_up_exit(1);
			}
			rfbLog("shm: Error creating shared memory tile-row for"
			    " len=%d,\n", i);
			rfbLog("shm: reverting to -onetile mode. If this"
			    " problem persists\n");
			rfbLog("shm: try using the -onetile or -noshm options"
			    " to limit\n");
			rfbLog("shm: shared memory usage, or run ipcrm(1)"
			    " to manually\n");
			rfbLog("shm: delete unattached shm segments.\n");
			single_copytile_count = i;
			single_copytile = 1;
		}
		tile_shm_count++;
		if (single_copytile && i >= 1) {
			/* only need 1x1 tiles */
			break;
		}
	}
	if (!quiet) {
		if (using_shm) {
			rfbLog("created %d tile_row shm polling images.\n",
			    tile_shm_count);
		} else {
			rfbLog("created %d tile_row polling images.\n",
			    tile_shm_count);
		}
	}
}

/*
 * A hint is a rectangular region built from 1 or more adjacent tiles
 * glued together.  Ultimately, this information in a single hint is sent
 * to libvncserver rather than sending each tile separately.
 */
static void create_tile_hint(int x, int y, int tw, int th, hint_t *hint) {
	int w = dpy_x - x;
	int h = dpy_y - y;

	if (w > tw) {
		w = tw;
	}
	if (h > th) {
		h = th;
	}

	hint->x = x;
	hint->y = y;
	hint->w = w;
	hint->h = h;
}

static void extend_tile_hint(int x, int y, int tw, int th, hint_t *hint) {
	int w = dpy_x - x;
	int h = dpy_y - y;

	if (w > tw) {
		w = tw;
	}
	if (h > th) {
		h = th;
	}

	if (hint->x > x) {			/* extend to the left */
		hint->w += hint->x - x;
		hint->x = x;
	}
	if (hint->y > y) {			/* extend upward */
		hint->h += hint->y - y;
		hint->y = y;
	}

	if (hint->x + hint->w < x + w) {	/* extend to the right */
		hint->w = x + w - hint->x;
	}
	if (hint->y + hint->h < y + h) {	/* extend downward */
		hint->h = y + h - hint->y;
	}
}

static void save_hint(hint_t hint, int loc) {
	/* simply copy it to the global array for later use. */
	hint_list[loc].x = hint.x;
	hint_list[loc].y = hint.y;
	hint_list[loc].w = hint.w;
	hint_list[loc].h = hint.h;
}

/*
 * Glue together horizontal "runs" of adjacent changed tiles into one big
 * rectangle change "hint" to be passed to the vnc machinery.
 */
static void hint_updates(void) {
	hint_t hint;
	int x, y, i, n, ty, th, tx, tw;
	int hint_count = 0, in_run = 0;

	for (y=0; y < ntiles_y; y++) {
		for (x=0; x < ntiles_x; x++) {
			n = x + y * ntiles_x;

			if (tile_has_diff[n]) {
				ty = tile_region[n].first_line;
				th = tile_region[n].last_line - ty + 1;

				tx = tile_region[n].first_x;
				tw = tile_region[n].last_x - tx + 1;
				if (tx < 0) {
					tx = 0;
					tw = tile_x;
				}

				if (! in_run) {
					create_tile_hint( x * tile_x + tx,
					    y * tile_y + ty, tw, th, &hint);
					in_run = 1;
				} else {
					extend_tile_hint( x * tile_x + tx,
					    y * tile_y + ty, tw, th, &hint);
				}
			} else {
				if (in_run) {
					/* end of a row run of altered tiles: */
					save_hint(hint, hint_count++);
					in_run = 0;
				}
			}
		}
		if (in_run) {	/* save the last row run */
			save_hint(hint, hint_count++);
			in_run = 0;
		}
	}

	for (i=0; i < hint_count; i++) {
		/* pass update info to vnc: */
		mark_hint(hint_list[i]);
	}
}

/*
 * kludge, simple ceil+floor for non-negative doubles:
 */
#define CEIL(x)  ( (double) ((int) (x)) == (x) ? \
	(double) ((int) (x)) : (double) ((int) (x) + 1) )
#define FLOOR(x) ( (double) ((int) (x)) )

/*
 * Scaling:
 *
 * For shrinking, a destination (scaled) pixel will correspond to more
 * than one source (i.e. main fb) pixel.  Think of an x-y plane made with
 * graph paper.  Each unit square in the graph paper (i.e. collection of
 * points (x,y) such that N < x < N+1 and M < y < M+1, N and M integers)
 * corresponds to one pixel in the unscaled fb.  There is a solid
 * color filling the inside of such a square.  A scaled pixel has width
 * 1/scale_fac, e.g. for "-scale 3/4" the width of the scaled pixel
 * is 1.333.  The area of this scaled pixel is 1.333 * 1.333 (so it
 * obviously overlaps more than one source pixel, each which have area 1).
 *
 * We take the weight an unscaled pixel (source) contributes to a
 * scaled pixel (destination) as simply proportional to the overlap area
 * between the two pixels.  One can then think of the value of the scaled
 * pixel as an integral over the portion of the graph paper it covers.
 * The thing being integrated is the color value of the unscaled source.
 * That color value is constant over a graph paper square (source pixel),
 * and changes discontinuously from one unit square to the next.
 *

Here is an example for -scale 3/4, the solid lines are the source pixels
(graph paper unit squares), while the dotted lines denote the scaled
pixels (destination pixels):

            0         1 4/3     2     8/3 3         4=12/3
            |---------|--.------|------.--|---------|.                
            |         |  .      |      .  |         |.                
            |    A    |  . B    |      .  |         |.                
            |         |  .      |      .  |         |.                
            |         |  .      |      .  |         |.                
          1 |---------|--.------|------.--|---------|.                
         4/3|.........|.........|.........|.........|.                
            |         |  .      |      .  |         |.                
            |    C    |  . D    |      .  |         |.                
            |         |  .      |      .  |         |.                
          2 |---------|--.------|------.--|---------|.                
            |         |  .      |      .  |         |.                
            |         |  .      |      .  |         |.                
         8/3|.........|.........|.........|.........|.                
            |         |  .      |      .  |         |.                
          3 |---------|--.------|------.--|---------|.                

So we see the first scaled pixel (0 < x < 4/3 and 0 < y < 4/3) mostly
overlaps with unscaled source pixel "A".  The integration (averaging)
weights for this scaled pixel are:

			A	 1
			B	1/3
			C	1/3
			D	1/9

 *
 * The Red, Green, and Blue color values must be averaged over separately
 * otherwise you can get a complete mess (except in solid regions),
 * because high order bits are averaged differently from the low order bits.
 *
 * So the algorithm is roughly:
 *
 *   - Given as input a rectangle in the unscaled source fb with changes,
 *     find the rectangle of pixels this affects in the scaled destination fb.
 *
 *   - For each of the affected scaled (dest) pixels, determine all of the
 *     unscaled (source) pixels it overlaps with.
 *  
 *   - Average those unscaled source values together, weighted by the area
 *     overlap with the destination pixel.  Average R, G, B separately.
 *
 *   - Take this average value and convert to a valid pixel value if
 *     necessary (e.g. rounding, shifting), and then insert it into the
 *     destination framebuffer as the pixel value.
 *
 *   - On to the next destination pixel...
 *
 * ========================================================================
 *
 * For expanding, e.g. -scale 1.1 (which we don't think people will do
 * very often... or at least so we hope, the framebuffer can become huge)
 * the situation is reversed and the destination pixel is smaller than a
 * "graph paper" unit square (source pixel).  Some destination pixels
 * will be completely within a single unscaled source pixel.
 *
 * What we do here is a simple 4 point interpolation scheme:
 * 
 * Let P00 be the source pixel closest to the destination pixel but with
 * x and y values less than or equal to those of the destination pixel.
 * (for simplicity, think of the upper left corner of a pixel defining the
 * x,y location of the pixel, the center would work just as well).  So it
 * is the source pixel immediately to the upper left of the destination
 * pixel.  Let P10 be the source pixel one to the right of P00.  Let P01
 * be one down from P00.  And let P11 be one down and one to the right
 * of P00.  They form a 2x2 square we will interpolate inside of.
 * 
 * Let V00, V10, V01, and V11 be the color values of those 4 source
 * pixels.  Let dx be the displacement along x the destination pixel is
 * from P00.  Note: 0 <= dx < 1 by definition of P00.  Similarly let
 * dy be the displacement along y.  The weighted average for the
 * interpolation is:
 * 
 * 	V_ave = V00 * (1 - dx) * (1 - dy)
 * 	      + V10 *      dx  * (1 - dy)
 * 	      + V01 * (1 - dx) *      dy
 * 	      + V11 *      dx  *      dy
 * 
 * Note that the weights (1-dx)*(1-dy) + dx*(1-dy) + (1-dx)*dy + dx*dy
 * automatically add up to 1.  It is also nice that all the weights are
 * positive (unsigned char stays unsigned char).  The above formula can
 * be motivated by doing two 1D interpolations along x:
 * 
 * 	VA = V00 * (1 - dx) + V10 * dx
 * 	VB = V01 * (1 - dx) + V11 * dx
 * 
 * and then interpolating VA and VB along y:
 * 
 * 	V_ave = VA * (1 - dy) + VB * dy
 * 
 *                      VA 
 *           v   |<-dx->|
 *           -- V00 ------ V10
 *           dy  |          |  
 *           --  |      o...|...    "o" denotes the position of the desired
 *           ^   |      .   |  .    destination pixel relative to the P00
 *               |      .   |  .    source pixel.
 *              V10 ----.- V11 .
 *                      ........
 *                      |  
 *                      VB 
 *
 * 
 * Of course R, G, B averages are done separately as in the shrinking
 * case.  This gives reasonable results, and the implementation for
 * shrinking can simply be used with different choices for weights for
 * the loop over the 4 pixels.
 */

void scale_rect(double factor, int blend, int interpolate, int Bpp,
    char *src_fb, int src_bytes_per_line, char *dst_fb, int dst_bytes_per_line,
    int Nx, int Ny, int nx, int ny, int X1, int Y1, int X2, int Y2, int mark) {
/*
 * Notation:
 * "i" an x pixel index in the destination (scaled) framebuffer
 * "j" a  y pixel index in the destination (scaled) framebuffer
 * "I" an x pixel index in the source (un-scaled, i.e. main) framebuffer
 * "J" a  y pixel index in the source (un-scaled, i.e. main) framebuffer
 *
 *  Similarly for nx, ny, Nx, Ny, etc.  Lowercase: dest, Uppercase: source.
 */
	int i, j, i1, i2, j1, j2;	/* indices for scaled fb (dest) */
	int I, J, I1, I2, J1, J2;	/* indices for main fb   (source) */

	double w, wx, wy, wtot;	/* pixel weights */

	double x1, y1, x2, y2;	/* x-y coords for destination pixels edges */
	double dx, dy;		/* size of destination pixel */
	double ddx=0, ddy=0;	/* for interpolation expansion */

	char *src, *dest;	/* pointers to the two framebuffers */


	unsigned short us = 0;
	unsigned char  uc = 0;
	unsigned int   ui = 0;

	int use_noblend_shortcut = 1;
	int shrink;		/* whether shrinking or expanding */
	static int constant_weights = -1, mag_int = -1;
	static int last_Nx = -1, last_Ny = -1, cnt = 0;
	static double last_factor = -1.0;
	int b, k;
	double pixave[4];	/* for averaging pixel values */

	if (factor <= 1.0) {
		shrink = 1;
	} else {
		shrink = 0;
	}

	/*
	 * N.B. width and height (real numbers) of a scaled pixel.
	 * both are > 1   (e.g. 1.333 for -scale 3/4)
	 * they should also be equal but we don't assume it.
	 *
	 * This new way is probably the best we can do, take the inverse
	 * of the scaling factor to double precision.
	 */
	dx = 1.0/factor;
	dy = 1.0/factor;

	/*
	 * There is some speedup if the pixel weights are constant, so
	 * let's special case these.
	 *
	 * If scale = 1/n and n divides Nx and Ny, the pixel weights
	 * are constant (e.g. 1/2 => equal on 2x2 square).
	 */
	if (factor != last_factor || Nx != last_Nx || Ny != last_Ny) {
		constant_weights = -1;
		mag_int = -1;
		last_Nx = Nx;
		last_Ny = Ny;
		last_factor = factor;
	}

	if (constant_weights < 0) {
		int n = 0;

		constant_weights = 0;
		mag_int = 0;

		for (i = 2; i<=128; i++) {
			double test = ((double) 1)/ i;
			double diff, eps = 1.0e-7;
			diff = factor - test;
			if (-eps < diff && diff < eps) {
				n = i;
				break;
			}
		}
		if (! blend || ! shrink || interpolate) {
			;
		} else if (n != 0) {
			if (Nx % n == 0 && Ny % n == 0) {
				static int didmsg = 0;
				if (mark && ! didmsg) {
					didmsg = 1;
					rfbLog("scale_and_mark_rect: using "
					    "constant pixel weight speedup "
					    "for 1/%d\n", n);
				}
				constant_weights = 1;
			}
		}

		n = 0;
		for (i = 2; i<=32; i++) {
			double test = (double) i;
			double diff, eps = 1.0e-7;
			diff = factor - test;
			if (-eps < diff && diff < eps) {
				n = i;
				break;
			}
		}
		if (! blend && factor > 1.0 && n) {
			mag_int = n;
		}
	}

	if (mark && factor > 1.0 && blend) {
		/*
		 * kludge: correct for interpolating blurring leaking
		 * up or left 1 destination pixel.
		 */
		if (X1 > 0) X1--;
		if (Y1 > 0) Y1--;
	}

	/*
	 * find the extent of the change the input rectangle induces in
	 * the scaled framebuffer.
	 */

	/* Left edges: find largest i such that i * dx <= X1  */
	i1 = FLOOR(X1/dx);

	/* Right edges: find smallest i such that (i+1) * dx >= X2+1  */
	i2 = CEIL( (X2+1)/dx ) - 1;

	/* To be safe, correct any overflows: */
	i1 = nfix(i1, nx);
	i2 = nfix(i2, nx) + 1;	/* add 1 to make a rectangle upper boundary */

	/* Repeat above for y direction: */
	j1 = FLOOR(Y1/dy);
	j2 = CEIL( (Y2+1)/dy ) - 1;

	j1 = nfix(j1, ny);
	j2 = nfix(j2, ny) + 1;

	/* special case integer magnification with no blending */
	if (mark && ! blend && mag_int && Bpp != 3) {
		int jmin, jmax, imin, imax;

		/* outer loop over *source* pixels */
		for (J=Y1; J < Y2; J++) {
		    jmin = J * mag_int;
		    jmax = jmin + mag_int;
		    for (I=X1; I < X2; I++) {
			/* extract value */
			src = src_fb + J*src_bytes_per_line + I*Bpp;
			if (Bpp == 4) {
				ui = *((unsigned int *)src);
			} else if (Bpp == 2) {
				us = *((unsigned short *)src);
			} else if (Bpp == 1) {
				uc = *((unsigned char *)src);
			}
			imin = I * mag_int;
			imax = imin + mag_int;
			/* inner loop over *dest* pixels */
			for (j=jmin; j<jmax; j++) {
			    dest = dst_fb + j*dst_bytes_per_line + imin*Bpp;
			    for (i=imin; i<imax; i++) {
				if (Bpp == 4) {
					*((unsigned int *)dest) = ui;
				} else if (Bpp == 2) {
					*((unsigned short *)dest) = us;
				} else if (Bpp == 1) {
					*((unsigned char *)dest) = uc;
				}
				dest += Bpp;
			    }
			}
		    }
		}
		goto markit;
	}

	/* set these all to 1.0 to begin with */
	wx = 1.0;
	wy = 1.0;
	w  = 1.0;

	/*
	 * Loop over destination pixels in scaled fb:
	 */
	for (j=j1; j<j2; j++) {
		y1 =  j * dy;	/* top edge */
		if (y1 > Ny - 1) {
			/* can go over with dy = 1/scale_fac */
			y1 = Ny - 1;
		}
		y2 = y1 + dy;	/* bottom edge */

		/* Find main fb indices covered by this dest pixel: */
		J1 = (int) FLOOR(y1);
		J1 = nfix(J1, Ny);

		if (shrink && ! interpolate) {
			J2 = (int) CEIL(y2) - 1;
			J2 = nfix(J2, Ny);
		} else {
			J2 = J1 + 1;	/* simple interpolation */
			ddy = y1 - J1;
		}

		/* destination char* pointer: */
		dest = dst_fb + j*dst_bytes_per_line + i1*Bpp;
		
		for (i=i1; i<i2; i++) {

			x1 =  i * dx;	/* left edge */
			if (x1 > Nx - 1) {
				/* can go over with dx = 1/scale_fac */
				x1 = Nx - 1;
			}
			x2 = x1 + dx;	/* right edge */

			cnt++;

			/* Find main fb indices covered by this dest pixel: */
			I1 = (int) FLOOR(x1);
			if (I1 >= Nx) I1 = Nx - 1;

			if (! blend && use_noblend_shortcut) {
				/*
				 * The noblend case involves no weights,
				 * and 1 pixel, so just copy the value
				 * directly.
				 */
				src = src_fb + J1*src_bytes_per_line + I1*Bpp;
				if (Bpp == 4) {
					*((unsigned int *)dest)
					    = *((unsigned int *)src);
				} else if (Bpp == 2) {
					*((unsigned short *)dest)
					    = *((unsigned short *)src);
				} else if (Bpp == 1) {
					*(dest) = *(src);
				} else if (Bpp == 3) {
					/* rare case */
					for (k=0; k<=2; k++) {
						*(dest+k) = *(src+k);
					}
				}
				dest += Bpp;
				continue;
			}
			
			if (shrink && ! interpolate) {
				I2 = (int) CEIL(x2) - 1;
				if (I2 >= Nx) I2 = Nx - 1;
			} else {
				I2 = I1 + 1;	/* simple interpolation */
				ddx = x1 - I1;
			}

			/* Zero out accumulators for next pixel average: */
			for (b=0; b<4; b++) {
				pixave[b] = 0.0; /* for RGB weighted sums */
			}

			/*
			 * wtot is for accumulating the total weight.
			 * It should always sum to 1/(scale_fac * scale_fac).
			 */
			wtot = 0.0;

			/*
			 * Loop over source pixels covered by this dest pixel.
			 * 
			 * These "extra" loops over "J" and "I" make
			 * the cache/cacheline performance unclear.
			 * For example, will the data brought in from
			 * src for j, i, and J=0 still be in the cache
			 * after the J > 0 data have been accessed and
			 * we are at j, i+1, J=0?  The stride in J is
			 * main_bytes_per_line, and so ~4 KB.
			 *
			 * Typical case when shrinking are 2x2 loop, so
			 * just two lines to worry about.
			 */
			for (J=J1; J<=J2; J++) {
			    /* see comments for I, x1, x2, etc. below */
			    if (constant_weights) {
				;
			    } else if (! blend) {
				if (J != J1) {
					continue;
				}
				wy = 1.0;

				/* interpolation scheme: */
			    } else if (! shrink || interpolate) {
				if (J >= Ny) {
					continue;
				} else if (J == J1) {
					wy = 1.0 - ddy;
				} else if (J != J1) {
					wy = ddy;
				}

				/* integration scheme: */
			    } else if (J < y1) {
				wy = J+1 - y1;
			    } else if (J+1 > y2) {
				wy = y2 - J;
			    } else {
				wy = 1.0;
			    }

			    src = src_fb + J*src_bytes_per_line + I1*Bpp;

			    for (I=I1; I<=I2; I++) {

				/* Work out the weight: */

				if (constant_weights) {
					;
				} else if (! blend) {
					/*
					 * Ugh, PseudoColor colormap is
					 * bad news, to avoid random
					 * colors just take the first
					 * pixel.  Or user may have
					 * specified :nb to fraction.
					 * The :fb will force blending
					 * for this case.
					 */
					if (I != I1) {
						continue;
					}
					wx = 1.0;

					/* interpolation scheme: */
				} else if (! shrink || interpolate) {
					if (I >= Nx) {
						continue;	/* off edge */
					} else if (I == I1) {
						wx = 1.0 - ddx;
					} else if (I != I1) {
						wx = ddx;
					}

					/* integration scheme: */
				} else if (I < x1) {
					/* 
					 * source left edge (I) to the
					 * left of dest left edge (x1):
					 * fractional weight
					 */
					wx = I+1 - x1;
				} else if (I+1 > x2) {
					/* 
					 * source right edge (I+1) to the
					 * right of dest right edge (x2):
					 * fractional weight
					 */
					wx = x2 - I;
				} else {
					/* 
					 * source edges (I and I+1) completely
					 * inside dest edges (x1 and x2):
					 * full weight
					 */
					wx = 1.0;
				}

				w = wx * wy;
				wtot += w;

				/* 
				 * We average the unsigned char value
				 * instead of char value: otherwise
				 * the minimum (char 0) is right next
				 * to the maximum (char -1)!  This way
				 * they are spread between 0 and 255.
				 */
				if (Bpp == 4) {
					/* unroll the loops, can give 20% */
					pixave[0] += w *
					    ((unsigned char) *(src  ));
					pixave[1] += w *
					    ((unsigned char) *(src+1));
					pixave[2] += w *
					    ((unsigned char) *(src+2));
					pixave[3] += w *
					    ((unsigned char) *(src+3));
				} else if (Bpp == 2) {
					/*
					 * 16bpp: trickier with green
					 * split over two bytes, so we
					 * use the masks:
					 */
					us = *((unsigned short *) src);
					pixave[0] += w*(us & main_red_mask);
					pixave[1] += w*(us & main_green_mask);
					pixave[2] += w*(us & main_blue_mask);
				} else if (Bpp == 1) {
					pixave[0] += w *
					    ((unsigned char) *(src));
				} else {
					for (b=0; b<Bpp; b++) {
						pixave[b] += w *
						    ((unsigned char) *(src+b));
					}
				}
				src += Bpp;
			    }
			}

			if (wtot <= 0.0) {
				wtot = 1.0;
			}
			wtot = 1.0/wtot;	/* normalization factor */

			/* place weighted average pixel in the scaled fb: */
			if (Bpp == 4) {
				*(dest  ) = (char) (wtot * pixave[0]);
				*(dest+1) = (char) (wtot * pixave[1]);
				*(dest+2) = (char) (wtot * pixave[2]);
				*(dest+3) = (char) (wtot * pixave[3]);
			} else if (Bpp == 2) {
				/* 16bpp / 565 case: */
				pixave[0] *= wtot;
				pixave[1] *= wtot;
				pixave[2] *= wtot;
				us =  (main_red_mask   & (int) pixave[0])
				    | (main_green_mask & (int) pixave[1])
				    | (main_blue_mask  & (int) pixave[2]);
				*( (unsigned short *) dest ) = us;
			} else if (Bpp == 1) {
				*(dest) = (char) (wtot * pixave[0]);
			} else {
				for (b=0; b<Bpp; b++) {
					*(dest+b) = (char) (wtot * pixave[b]);
				}
			}
			dest += Bpp;
		}
	}
	markit:
	if (mark) {
		mark_rect_as_modified(i1, j1, i2, j2, 1);
	}
}

void scale_and_mark_rect(int X1, int Y1, int X2, int Y2) {
	char *src_fb = main_fb;
	int Bpp = bpp/8, fac = 1;

	if (!screen || !rfb_fb || !main_fb) {
		return;
	}
	if (! screen->serverFormat.trueColour) {
		/*
		 * PseudoColor colormap... blending leads to random colors.
		 * User can override with ":fb"
		 */
		if (scaling_blend == 1) {
			/* :fb option sets it to 2 */
			if (default_visual->class == StaticGray) {
				/*
				 * StaticGray can be blended OK, otherwise
				 * user can disable with :nb
				 */
				;
			} else {
				scaling_blend = 0;
			}
		}
	}

	if (cmap8to24 && cmap8to24_fb) {
		src_fb = cmap8to24_fb;
		if (scaling && depth == 8) {
			fac = 4;
		}
	}

	scale_rect(scale_fac, scaling_blend, scaling_interpolate, fac * Bpp,
	    src_fb, fac * main_bytes_per_line, rfb_fb, rfb_bytes_per_line,
	    dpy_x, dpy_y, scaled_x, scaled_y, X1, Y1, X2, Y2, 1);
}

void mark_rect_as_modified(int x1, int y1, int x2, int y2, int force) {

	if (damage_time != 0) {
		/*
		 * This is not XDAMAGE, rather a hack for testing
		 * where we allow the framebuffer to be corrupted for
		 * damage_delay seconds.
		 */
		int debug = 0;
		if (time(0) > damage_time + damage_delay) {
			if (! quiet) {
				rfbLog("damaging turned off.\n");
			}
			damage_time = 0;
			damage_delay = 0;
		} else {
			if (debug) {
				rfbLog("damaging viewer fb by not marking "
				    "rect: %d,%d,%d,%d\n", x1, y1, x2, y2);
			}
			return;
		}
	}

	if (rfb_fb == main_fb || force) {
		rfbMarkRectAsModified(screen, x1, y1, x2, y2);
		return;
	}

	if (cmap8to24) {
		bpp8to24(x1, y1, x2, y2);
	}

	if (scaling) {
		scale_and_mark_rect(x1, y1, x2, y2);
	} else {
		rfbMarkRectAsModified(screen, x1, y1, x2, y2);
	}
}

/*
 * Notifies libvncserver of a changed hint rectangle.
 */
static void mark_hint(hint_t hint) {
	int x = hint.x;	
	int y = hint.y;	
	int w = hint.w;	
	int h = hint.h;	

	mark_rect_as_modified(x, y, x + w, y + h, 0);
}

/*
 * copy_tiles() gives a slight improvement over copy_tile() since
 * adjacent runs of tiles are done all at once there is some savings
 * due to contiguous memory access.  Not a great speedup, but in some
 * cases it can be up to 2X.  Even more on a SunRay or ShadowFB where
 * no graphics hardware is involved in the read.  Generally, graphics
 * devices are optimized for write, not read, so we are limited by the
 * read bandwidth, sometimes only 5 MB/sec on otherwise fast hardware.
 */
static int *first_line = NULL, *last_line;
static unsigned short *left_diff, *right_diff;

static int copy_tiles(int tx, int ty, int nt) {
	int x, y, line;
	int size_x, size_y, width1, width2;
	int off, len, n, dw, dx, t;
	int w1, w2, dx1, dx2;	/* tmps for normal and short tiles */
	int pixelsize = bpp/8;
	int first_min, last_max;
	int first_x = -1, last_x = -1;

	char *src, *dst, *s_src, *s_dst, *m_src, *m_dst;
	char *h_src, *h_dst;
	if (! first_line) {
		/* allocate arrays first time in. */
		int n = ntiles_x + 1;
		first_line = (int *) malloc((size_t) (n * sizeof(int)));
		last_line  = (int *) malloc((size_t) (n * sizeof(int)));
		left_diff  = (unsigned short *)
			malloc((size_t) (n * sizeof(unsigned short)));
		right_diff = (unsigned short *)
			malloc((size_t) (n * sizeof(unsigned short)));
	}

	x = tx * tile_x;
	y = ty * tile_y;

	size_x = dpy_x - x;
	if ( size_x > tile_x * nt ) {
		size_x = tile_x * nt;
		width1 = tile_x;
		width2 = tile_x;
	} else {
		/* short tile */
		width1 = tile_x;	/* internal tile */
		width2 = size_x - (nt - 1) * tile_x;	/* right hand tile */
	}

	size_y = dpy_y - y;
	if ( size_y > tile_y ) {
		size_y = tile_y;
	}

	n = tx + ty * ntiles_x;		/* number of the first tile */

	if (blackouts && tile_blackout[n].cover == 2) {
		/*
		 * If there are blackouts and this tile is completely covered
		 * no need to poll screen or do anything else..
		 * n.b. we are in single copy_tile mode: nt=1
		 */
		tile_has_diff[n] = 0;
		return(0);
	}

	X_LOCK;
	XRANDR_SET_TRAP_RET(-1, "copy_tile-set");
	/* read in the whole tile run at once: */
	copy_image(tile_row[nt], x, y, size_x, size_y);
	XRANDR_CHK_TRAP_RET(-1, "copy_tile-chk");

	X_UNLOCK;

	if (blackouts && tile_blackout[n].cover == 1) {
		/*
		 * If there are blackouts and this tile is partially covered
		 * we should re-black-out the portion.
		 * n.b. we are in single copy_tile mode: nt=1
		 */
		int x1, x2, y1, y2, b;
		int w, s, fill = 0;

		for (b=0; b < tile_blackout[n].count; b++) {
			char *b_dst = tile_row[nt]->data;
			
			x1 = tile_blackout[n].bo[b].x1 - x;
			y1 = tile_blackout[n].bo[b].y1 - y;
			x2 = tile_blackout[n].bo[b].x2 - x;
			y2 = tile_blackout[n].bo[b].y2 - y;

			w = (x2 - x1) * pixelsize;
			s = x1 * pixelsize;

			for (line = 0; line < size_y; line++) {
				if (y1 <= line && line < y2) {
					memset(b_dst + s, fill, (size_t) w);
				}
				b_dst += tile_row[nt]->bytes_per_line;
			}
		}
	}

	src = tile_row[nt]->data;
	dst = main_fb + y * main_bytes_per_line + x * pixelsize;

	s_src = src;
	s_dst = dst;

	for (t=1; t <= nt; t++) {
		first_line[t] = -1;
	}

	/* find the first line with difference: */
	w1 = width1 * pixelsize;
	w2 = width2 * pixelsize;

	/* foreach line: */
	for (line = 0; line < size_y; line++) {
		/* foreach horizontal tile: */
		for (t=1; t <= nt; t++) {
			if (first_line[t] != -1) {
				continue;
			}

			off = (t-1) * w1;
			if (t == nt) {
				len = w2;	/* possible short tile */
			} else {
				len = w1;
			}
			
			if (memcmp(s_dst + off, s_src + off, len)) {
				first_line[t] = line;
			}
		}
		s_src += tile_row[nt]->bytes_per_line;
		s_dst += main_bytes_per_line;
	}

	/* see if there were any differences for any tile: */
	first_min = -1;
	for (t=1; t <= nt; t++) {
		tile_tried[n+(t-1)] = 1;
		if (first_line[t] != -1) {
			if (first_min == -1 || first_line[t] < first_min) {
				first_min = first_line[t];
			}
		}
	}
	if (first_min == -1) {
		/* no tile has a difference, note this and get out: */
		for (t=1; t <= nt; t++) {
			tile_has_diff[n+(t-1)] = 0;
		}
		return(0);
	} else {
		/*
		 * at least one tile has a difference.  make sure info
		 * is recorded (e.g. sometimes we guess tiles and they
		 * came in with tile_has_diff 0)
		 */
		for (t=1; t <= nt; t++) {
			if (first_line[t] == -1) {
				tile_has_diff[n+(t-1)] = 0;
			} else {
				tile_has_diff[n+(t-1)] = 1;
			}
		}
	}

	m_src = src + (tile_row[nt]->bytes_per_line * size_y);
	m_dst = dst + (main_bytes_per_line * size_y);

	for (t=1; t <= nt; t++) {
		last_line[t] = first_line[t];
	}

	/* find the last line with difference: */
	w1 = width1 * pixelsize;
	w2 = width2 * pixelsize;

	/* foreach line: */
	for (line = size_y - 1; line > first_min; line--) {

		m_src -= tile_row[nt]->bytes_per_line;
		m_dst -= main_bytes_per_line;

		/* foreach tile: */
		for (t=1; t <= nt; t++) {
			if (first_line[t] == -1
			    || last_line[t] != first_line[t]) {
				/* tile has no changes or already done */
				continue;
			}

			off = (t-1) * w1;
			if (t == nt) {
				len = w2;	/* possible short tile */
			} else {
				len = w1;
			}
			if (memcmp(m_dst + off, m_src + off, len)) {
				last_line[t] = line;
			}
		}
	}
	
	/*
	 * determine the farthest down last changed line
	 * will be used below to limit our memcpy() to the framebuffer.
	 */
	last_max = -1;
	for (t=1; t <= nt; t++) {
		if (first_line[t] == -1) {
			continue;
		}
		if (last_max == -1 || last_line[t] > last_max) {
			last_max = last_line[t];
		}
	}

	/* look for differences on left and right hand edges: */
	for (t=1; t <= nt; t++) {
		left_diff[t] = 0;
		right_diff[t] = 0;
	}

	h_src = src;
	h_dst = dst;

	w1 = width1 * pixelsize;
	w2 = width2 * pixelsize;

	dx1 = (width1 - tile_fuzz) * pixelsize;
	dx2 = (width2 - tile_fuzz) * pixelsize;
	dw = tile_fuzz * pixelsize; 

	/* foreach line: */
	for (line = 0; line < size_y; line++) {
		/* foreach tile: */
		for (t=1; t <= nt; t++) {
			if (first_line[t] == -1) {
				/* tile has no changes at all */
				continue;
			}

			off = (t-1) * w1;
			if (t == nt) {
				dx = dx2;	/* possible short tile */
				if (dx <= 0) {
					break;
				}
			} else {
				dx = dx1;
			}

			if (! left_diff[t] && memcmp(h_dst + off,
			    h_src + off, dw)) {
				left_diff[t] = 1;
			}
			if (! right_diff[t] && memcmp(h_dst + off + dx,
			    h_src + off + dx, dw) ) {
				right_diff[t] = 1;
			}
		}
		h_src += tile_row[nt]->bytes_per_line;
		h_dst += main_bytes_per_line;
	}

	/* now finally copy the difference to the rfb framebuffer: */
	s_src = src + tile_row[nt]->bytes_per_line * first_min;
	s_dst = dst + main_bytes_per_line * first_min;

	for (line = first_min; line <= last_max; line++) {
		/* for I/O speed we do not do this tile by tile */
		memcpy(s_dst, s_src, size_x * pixelsize);
		if (nt == 1) {
			/*
			 * optimization for tall skinny lines, e.g. wm
			 * frame. try to find first_x and last_x to limit
			 * the size of the hint.  could help for a slow
			 * link.  Unfortunately we spent a lot of time
			 * reading in the many tiles.
			 *
			 * BTW, we like to think the above memcpy leaves
			 * the data we use below in the cache... (but
			 * it could be two 128 byte segments at 32bpp)
			 * so this inner loop is not as bad as it seems.
			 */
			int k, kx;
			kx = pixelsize;
			for (k=0; k<size_x; k++) {
				if (memcmp(s_dst + k*kx, s_src + k*kx, kx))  {
					if (first_x == -1 || k < first_x) {
						first_x = k;
					}
					if (last_x == -1 || k > last_x) {
						last_x = k;
					}
				}
			}
		}
		s_src += tile_row[nt]->bytes_per_line;
		s_dst += main_bytes_per_line;
	}

	/* record all the info in the region array for this tile: */
	for (t=1; t <= nt; t++) {
		int s = t - 1;

		if (first_line[t] == -1) {
			/* tile unchanged */
			continue;
		}
		tile_region[n+s].first_line = first_line[t];
		tile_region[n+s].last_line  = last_line[t];

		tile_region[n+s].first_x = first_x;
		tile_region[n+s].last_x  = last_x;

		tile_region[n+s].top_diff = 0;
		tile_region[n+s].bot_diff = 0;
		if ( first_line[t] < tile_fuzz ) {
			tile_region[n+s].top_diff = 1;
		}
		if ( last_line[t] > (size_y - 1) - tile_fuzz ) {
			tile_region[n+s].bot_diff = 1;
		}

		tile_region[n+s].left_diff  = left_diff[t];
		tile_region[n+s].right_diff = right_diff[t];

		tile_copied[n+s] = 1;
	}

	return(1);
}

/*
 * The copy_tile() call in the loop below copies the changed tile into
 * the rfb framebuffer.  Note that copy_tile() sets the tile_region
 * struct to have info about the y-range of the changed region and also
 * whether the tile edges contain diffs (within distance tile_fuzz).
 *
 * We use this tile_region info to try to guess if the downward and right
 * tiles will have diffs.  These tiles will be checked later in the loop
 * (since y+1 > y and x+1 > x).
 *
 * See copy_tiles_backward_pass() for analogous checking upward and
 * left tiles.
 */
static int copy_all_tiles(void) {
	int x, y, n, m;
	int diffs = 0, ct;

	for (y=0; y < ntiles_y; y++) {
		for (x=0; x < ntiles_x; x++) {
			n = x + y * ntiles_x;

			if (tile_has_diff[n]) {
				ct = copy_tiles(x, y, 1);
				if (ct < 0) return ct;	/* fatal */
			}
			if (! tile_has_diff[n]) {
				/*
				 * n.b. copy_tiles() may have detected
				 * no change and reset tile_has_diff to 0.
				 */
				continue;
			}
			diffs++;

			/* neighboring tile downward: */
			if ( (y+1) < ntiles_y && tile_region[n].bot_diff) {
				m = x + (y+1) * ntiles_x;
				if (! tile_has_diff[m]) {
					tile_has_diff[m] = 2;
				}
			}
			/* neighboring tile to right: */
			if ( (x+1) < ntiles_x && tile_region[n].right_diff) {
				m = (x+1) + y * ntiles_x;
				if (! tile_has_diff[m]) {
					tile_has_diff[m] = 2;
				}
			}
		}
	}
	return diffs;
}

/*
 * Routine analogous to copy_all_tiles() above, but for horizontal runs
 * of adjacent changed tiles.
 */
static int copy_all_tile_runs(void) {
	int x, y, n, m, i;
	int diffs = 0, ct;
	int in_run = 0, run = 0;
	int ntave = 0, ntcnt = 0;

	for (y=0; y < ntiles_y; y++) {
		for (x=0; x < ntiles_x + 1; x++) {
			n = x + y * ntiles_x;

			if (x != ntiles_x && tile_has_diff[n]) {
				in_run = 1;
				run++;
			} else {
				if (! in_run) {
					in_run = 0;
					run = 0;
					continue;
				}
				ct = copy_tiles(x - run, y, run);
				if (ct < 0) return ct;	/* fatal */

				ntcnt++;
				ntave += run;
				diffs += run;

				/* neighboring tile downward: */
				for (i=1; i <= run; i++) {
					if ((y+1) < ntiles_y
					    && tile_region[n-i].bot_diff) {
						m = (x-i) + (y+1) * ntiles_x;
						if (! tile_has_diff[m]) {
							tile_has_diff[m] = 2;
						}
					}
				}

				/* neighboring tile to right: */
				if (((x-1)+1) < ntiles_x
				    && tile_region[n-1].right_diff) {
					m = ((x-1)+1) + y * ntiles_x;
					if (! tile_has_diff[m]) {
						tile_has_diff[m] = 2;
					}
					
					/* note that this starts a new run */
					in_run = 1;
					run = 1;
				} else {
					in_run = 0;
					run = 0;
				}
			}
		}
		/*
		 * Could some activity go here, to emulate threaded
		 * behavior by servicing some libvncserver tasks?
		 */
	}
	return diffs;
}

/*
 * Here starts a bunch of heuristics to guess/detect changed tiles.
 * They are:
 *   copy_tiles_backward_pass, fill_tile_gaps/gap_try, grow_islands/island_try
 */

/*
 * Try to predict whether the upward and/or leftward tile has been modified.
 * copy_all_tiles() has already done downward and rightward tiles.
 */
static int copy_tiles_backward_pass(void) {
	int x, y, n, m;
	int diffs = 0, ct;

	for (y = ntiles_y - 1; y >= 0; y--) {
	    for (x = ntiles_x - 1; x >= 0; x--) {
		n = x + y * ntiles_x;		/* number of this tile */

		if (! tile_has_diff[n]) {
			continue;
		}

		m = x + (y-1) * ntiles_x;	/* neighboring tile upward */

		if (y >= 1 && ! tile_has_diff[m] && tile_region[n].top_diff) {
			if (! tile_tried[m]) {
				tile_has_diff[m] = 2;
				ct = copy_tiles(x, y-1, 1);
				if (ct < 0) return ct;	/* fatal */
			}
		}

		m = (x-1) + y * ntiles_x;	/* neighboring tile to left */

		if (x >= 1 && ! tile_has_diff[m] && tile_region[n].left_diff) {
			if (! tile_tried[m]) {
				tile_has_diff[m] = 2;
				ct = copy_tiles(x-1, y, 1);
				if (ct < 0) return ct;	/* fatal */
			}
		}
	    }
	}
	for (n=0; n < ntiles; n++) {
		if (tile_has_diff[n]) {
			diffs++;
		}
	}
	return diffs;
}

static int copy_tiles_additional_pass(void) {
	int x, y, n;
	int diffs = 0, ct;

	for (y=0; y < ntiles_y; y++) {
		for (x=0; x < ntiles_x; x++) {
			n = x + y * ntiles_x;		/* number of this tile */

			if (! tile_has_diff[n]) {
				continue;
			}
			if (tile_copied[n]) {
				continue;
			}

			ct = copy_tiles(x, y, 1);
			if (ct < 0) return ct;	/* fatal */
		}
	}
	for (n=0; n < ntiles; n++) {
		if (tile_has_diff[n]) {
			diffs++;
		}
	}
	return diffs;
}

static int gap_try(int x, int y, int *run, int *saw, int along_x) {
	int n, m, i, xt, yt, ct;

	n = x + y * ntiles_x;

	if (! tile_has_diff[n]) {
		if (*saw) {
			(*run)++;	/* extend the gap run. */
		}
		return 0;
	}
	if (! *saw || *run == 0 || *run > gaps_fill) {
		*run = 0;		/* unacceptable run. */
		*saw = 1;
		return 0;
	}

	for (i=1; i <= *run; i++) {	/* iterate thru the run. */
		if (along_x) {
			xt = x - i;
			yt = y;
		} else {
			xt = x;
			yt = y - i;
		}

		m = xt + yt * ntiles_x;
		if (tile_tried[m]) {	/* do not repeat tiles */
			continue;
		}

		ct = copy_tiles(xt, yt, 1);
		if (ct < 0) return ct;	/* fatal */
	}
	*run = 0;
	*saw = 1;
	return 1;
}

/*
 * Look for small gaps of unchanged tiles that may actually contain changes.
 * E.g. when paging up and down in a web broswer or terminal there can
 * be a distracting delayed filling in of such gaps.  gaps_fill is the
 * tweak parameter that sets the width of the gaps that are checked.
 *
 * BTW, grow_islands() is actually pretty successful at doing this too...
 */
static int fill_tile_gaps(void) {
	int x, y, run, saw;
	int n, diffs = 0, ct;

	/* horizontal: */
	for (y=0; y < ntiles_y; y++) {
		run = 0;
		saw = 0;
		for (x=0; x < ntiles_x; x++) {
			ct = gap_try(x, y, &run, &saw, 1);
			if (ct < 0) return ct;	/* fatal */
		}
	}

	/* vertical: */
	for (x=0; x < ntiles_x; x++) {
		run = 0;
		saw = 0;
		for (y=0; y < ntiles_y; y++) {
			ct = gap_try(x, y, &run, &saw, 0);
			if (ct < 0) return ct;	/* fatal */
		}
	}

	for (n=0; n < ntiles; n++) {
		if (tile_has_diff[n]) {
			diffs++;
		}
	}
	return diffs;
}

static int island_try(int x, int y, int u, int v, int *run) {
	int n, m, ct;

	n = x + y * ntiles_x;
	m = u + v * ntiles_x;

	if (tile_has_diff[n]) {
		(*run)++;
	} else {
		*run = 0;
	}

	if (tile_has_diff[n] && ! tile_has_diff[m]) {
		/* found a discontinuity */

		if (tile_tried[m]) {
			return 0;
		} else if (*run < grow_fill) {
			return 0;
		}

		ct = copy_tiles(u, v, 1);
		if (ct < 0) return ct;	/* fatal */
	}
	return 1;
}

/*
 * Scan looking for discontinuities in tile_has_diff[].  Try to extend
 * the boundary of the discontinuity (i.e. make the island larger).
 * Vertical scans are skipped since they do not seem to yield much...
 */
static int grow_islands(void) {
	int x, y, n, run;
	int diffs = 0, ct;

	/*
	 * n.b. the way we scan here should keep an extension going,
	 * and so also fill in gaps effectively...
	 */

	/* left to right: */
	for (y=0; y < ntiles_y; y++) {
		run = 0;
		for (x=0; x <= ntiles_x - 2; x++) {
			ct = island_try(x, y, x+1, y, &run);
			if (ct < 0) return ct;	/* fatal */
		}
	}
	/* right to left: */
	for (y=0; y < ntiles_y; y++) {
		run = 0;
		for (x = ntiles_x - 1; x >= 1; x--) {
			ct = island_try(x, y, x-1, y, &run);
			if (ct < 0) return ct;	/* fatal */
		}
	}
	for (n=0; n < ntiles; n++) {
		if (tile_has_diff[n]) {
			diffs++;
		}
	}
	return diffs;
}

/*
 * Fill the framebuffer with zeros for each blackout region
 */
static void blackout_regions(void) {
	int i;
	for (i=0; i < blackouts; i++) {
		zero_fb(blackr[i].x1, blackr[i].y1, blackr[i].x2, blackr[i].y2);
	}
}

/*
 * copy the whole X screen to the rfb framebuffer.  For a large enough
 * number of changed tiles, this is faster than tiles scheme at retrieving
 * the info from the X server.  Bandwidth to client and compression time
 * are other issues...  use -fs 1.0 to disable.
 */
int copy_screen(void) {
	int pixelsize = bpp/8;
	char *fbp;
	int i, y, block_size;

	if (! fs_factor) {
		return 0;
	}

	block_size = (dpy_x * (dpy_y/fs_factor) * pixelsize);

	if (! main_fb) {
		return 0;
	}
	fbp = main_fb;
	y = 0;

	X_LOCK;

	/* screen may be too big for 1 shm area, so broken into fs_factor */
	for (i=0; i < fs_factor; i++) {
		XRANDR_SET_TRAP_RET(-1, "copy_screen-set");
		copy_image(fullscreen, 0, y, 0, 0);
		XRANDR_CHK_TRAP_RET(-1, "copy_screen-chk");

		memcpy(fbp, fullscreen->data, (size_t) block_size);

		y += dpy_y / fs_factor;
		fbp += block_size;
	}

	X_UNLOCK;

	if (blackouts) {
		blackout_regions();
	}

	mark_rect_as_modified(0, 0, dpy_x, dpy_y, 0);
	return 0;
}

int copy_snap(void) {
	int pixelsize = bpp/8;
	char *fbp;
	int i, y, block_size;
	double dt;
	static int first = 1;

	if (! fs_factor) {
		return 0;
	}

	block_size = (dpy_x * (dpy_y/fs_factor) * pixelsize);

	if (! snap_fb || ! snap || ! snaprect) {
		return 0;
	}
	fbp = snap_fb;
	y = 0;

	dtime0(&dt);
	X_LOCK;

	/* screen may be too big for 1 shm area, so broken into fs_factor */
	for (i=0; i < fs_factor; i++) {
		XRANDR_SET_TRAP_RET(-1, "copy_snap-set");
		copy_image(snaprect, 0, y, 0, 0);
		XRANDR_CHK_TRAP_RET(-1, "copy_snap-chk");

		memcpy(fbp, snaprect->data, (size_t) block_size);

		y += dpy_y / fs_factor;
		fbp += block_size;
	}

	X_UNLOCK;
	dt = dtime(&dt);
	if (first) {
		rfbLog("copy_snap: time for -snapfb snapshot: %.3f sec\n", dt);
		first = 0;
	}

	return 0;
}


/*
 * Utilities for managing the "naps" to cut down on amount of polling.
 */
static void nap_set(int tile_cnt) {
	int nap_in = nap_ok;
	time_t now = time(0);

	if (scan_count == 0) {
		/* roll up check for all NSCAN scans */
		nap_ok = 0;
		if (naptile && nap_diff_count < 2 * NSCAN * naptile) {
			/* "2" is a fudge to permit a bit of bg drawing */
			nap_ok = 1;
		}
		nap_diff_count = 0;
	}
	if (nap_ok && ! nap_in && use_xdamage) {
		if (XD_skip > 0.8 * XD_tot) 	{
			/* X DAMAGE is keeping load low, so skip nap */
			nap_ok = 0;
		}
	}
	if (! nap_ok && client_count) {
		if(now > last_fb_bytes_sent + no_fbu_blank) {
			if (debug_tiles > 1) {
				printf("nap_set: nap_ok=1: now: %d last: %d\n",
				    (int) now, (int) last_fb_bytes_sent);
			}
			nap_ok = 1;
		}
	}

	if (show_cursor) {
		/* kludge for the up to 4 tiles the mouse patch could occupy */
		if ( tile_cnt > 4) {
			last_event = now;
		}
	} else if (tile_cnt != 0) {
		last_event = now;
	}
}

/*
 * split up a long nap to improve the wakeup time
 */
void nap_sleep(int ms, int split) {
	int i, input = got_user_input;

	for (i=0; i<split; i++) {
		usleep(ms * 1000 / split);
		if (! use_threads && i != split - 1) {
			rfbPE(-1);
		}
		if (input != got_user_input) {
			break;
		}
	}
}

/*
 * see if we should take a nap of some sort between polls
 */
static void nap_check(int tile_cnt) {
	time_t now;

	nap_diff_count += tile_cnt;

	if (! take_naps) {
		return;
	}

	now = time(0);

	if (screen_blank > 0) {
		int dt_ev, dt_fbu, ms = 2000;

		/* if no activity, pause here for a second or so. */
		dt_ev  = (int) (now - last_event);
		dt_fbu = (int) (now - last_fb_bytes_sent);
		if (dt_fbu > screen_blank) {
			/* sleep longer for no fb requests */
			nap_sleep(2 * ms, 16);
			return;
		}
		if (dt_ev > screen_blank) {
			nap_sleep(ms, 8);
			return;
		}
	}
	if (naptile && nap_ok && tile_cnt < naptile) {
		int ms = napfac * waitms;
		ms = ms > napmax ? napmax : ms;
		if (now - last_input <= 2) {
			nap_ok = 0;
		} else {
			nap_sleep(ms, 1);
		}
	}
}

/*
 * This is called to avoid a ~20 second timeout in libvncserver.
 * May no longer be needed.
 */
static void ping_clients(int tile_cnt) {
	static time_t last_send = 0;
	time_t now = time(0);

	if (rfbMaxClientWait < 20000) {
		rfbMaxClientWait = 20000;
		rfbLog("reset rfbMaxClientWait to %d msec.\n",
		    rfbMaxClientWait);
	}
	if (tile_cnt) {
		last_send = now;
	} else if (now - last_send > 1) {
		/* Send small heartbeat to client */
		mark_rect_as_modified(0, 0, 1, 1, 1);
		last_send = now;
	}
}

/*
 * scan_display() wants to know if this tile can be skipped due to
 * blackout regions: (no data compare is done, just a quick geometric test)
 */
static int blackout_line_skip(int n, int x, int y, int rescan,
    int *tile_count) {
	
	if (tile_blackout[n].cover == 2) {
		tile_has_diff[n] = 0;
		return 1;	/* skip it */

	} else if (tile_blackout[n].cover == 1) {
		int w, x1, y1, x2, y2, b, hit = 0;
		if (x + NSCAN > dpy_x) {
			w = dpy_x - x;
		} else {
			w = NSCAN;
		}

		for (b=0; b < tile_blackout[n].count; b++) {
			
			/* n.b. these coords are in full display space: */
			x1 = tile_blackout[n].bo[b].x1;
			x2 = tile_blackout[n].bo[b].x2;
			y1 = tile_blackout[n].bo[b].y1;
			y2 = tile_blackout[n].bo[b].y2;

			if (x2 - x1 < w) {
				/* need to cover full width */
				continue;
			}
			if (y1 <= y && y < y2) {
				hit = 1;
				break;
			}
		}
		if (hit) {
			if (! rescan) {
				tile_has_diff[n] = 0;
			} else {
				*tile_count += tile_has_diff[n];
			}
			return 1;	/* skip */
		}
	}
	return 0;	/* do not skip */
}

static int blackout_line_cmpskip(int n, int x, int y, char *dst, char *src,
    int w, int pixelsize) {

	int i, x1, y1, x2, y2, b, hit = 0;
	int beg = -1, end = -1; 

	if (tile_blackout[n].cover == 0) {
		return 0;	/* 0 means do not skip it. */
	} else if (tile_blackout[n].cover == 2) {
		return 1;	/* 1 means skip it. */
	}

	/* tile has partial coverage: */

	for (i=0; i < w * pixelsize; i++)  {
		if (*(dst+i) != *(src+i)) {
			beg = i/pixelsize;	/* beginning difference */
			break;
		}
	}
	for (i = w * pixelsize - 1; i >= 0; i--)  {
		if (*(dst+i) != *(src+i)) {
			end = i/pixelsize;	/* ending difference */
			break;
		}
	}
	if (beg < 0 || end < 0) {
		/* problem finding range... */
		return 0;
	}

	/* loop over blackout rectangles: */
	for (b=0; b < tile_blackout[n].count; b++) {
		
		/* y in full display space: */
		y1 = tile_blackout[n].bo[b].y1;
		y2 = tile_blackout[n].bo[b].y2;

		/* x relative to tile origin: */
		x1 = tile_blackout[n].bo[b].x1 - x;
		x2 = tile_blackout[n].bo[b].x2 - x;

		if (y1 > y || y >= y2) {
			continue;
		}
		if (x1 <= beg && end <= x2) {
			hit = 1;
			break;
		}
	}
	if (hit) {
		return 1;
	} else {
		return 0;
	}
}

/*
 * For the subwin case follows the window if it is moved.
 */
void set_offset(void) {
	Window w;
	if (! subwin) {
		return;
	}
	X_LOCK;
	xtranslate(window, rootwin, 0, 0, &off_x, &off_y, &w, 0);
	X_UNLOCK;
}

/*
 * Loop over 1-pixel tall horizontal scanlines looking for changes.  
 * Record the changes in tile_has_diff[].  Scanlines in the loop are
 * equally spaced along y by NSCAN pixels, but have a slightly random
 * starting offset ystart ( < NSCAN ) from scanlines[].
 */
static int scan_display(int ystart, int rescan) {
	char *src, *dst;
	int pixelsize = bpp/8;
	int x, y, w, n;
	int tile_count = 0;
	int nodiffs = 0, diff_hint;

	y = ystart;

	if (! main_fb) {
		rfbLog("scan_display: no main_fb!\n");
		return 0;
	}

	while (y < dpy_y) {

		if (use_xdamage) {
			XD_tot++;
			if (xdamage_hint_skip(y)) {
				XD_skip++;
				y += NSCAN;
				continue;
			}
		}

		/* grab the horizontal scanline from the display: */
		X_LOCK;
		XRANDR_SET_TRAP_RET(-1, "scan_display-set");
		copy_image(scanline, 0, y, 0, 0);
		XRANDR_CHK_TRAP_RET(-1, "scan_display-chk");
		X_UNLOCK;

		/* for better memory i/o try the whole line at once */
		src = scanline->data;
		dst = main_fb + y * main_bytes_per_line;

		if (! memcmp(dst, src, main_bytes_per_line)) {
			/* no changes anywhere in scan line */
			nodiffs = 1;
			if (! rescan) {
				y += NSCAN;
				continue;
			}
		}

		x = 0;
		while (x < dpy_x) {
			n = (x/tile_x) + (y/tile_y) * ntiles_x;
			diff_hint = 0;

			if (blackouts) {
				if (blackout_line_skip(n, x, y, rescan,
				    &tile_count)) {
					x += NSCAN;
					continue;
				}
			}

			if (rescan) {
				if (nodiffs || tile_has_diff[n]) {
					tile_count += tile_has_diff[n];
					x += NSCAN;
					continue;
				}
			} else if (xdamage_tile_count &&
			    tile_has_xdamage_diff[n]) {
				tile_has_xdamage_diff[n] = 2;
				diff_hint = 1;
			}

			/* set ptrs to correspond to the x offset: */
			src = scanline->data + x * pixelsize;
			dst = main_fb + y * main_bytes_per_line + x * pixelsize;

			/* compute the width of data to be compared: */
			if (x + NSCAN > dpy_x) {
				w = dpy_x - x;
			} else {
				w = NSCAN;
			}

			if (diff_hint || memcmp(dst, src, w * pixelsize)) {
				/* found a difference, record it: */
				if (! blackouts) {
					tile_has_diff[n] = 1;
					tile_count++;		
				} else {
					if (blackout_line_cmpskip(n, x, y,
					    dst, src, w, pixelsize)) {
						tile_has_diff[n] = 0;
					} else {
						tile_has_diff[n] = 1;
						tile_count++;		
					}
				}
			}
			x += NSCAN;
		}
		y += NSCAN;
	}
	return tile_count;
}


static int scanlines[NSCAN] = {
	 0, 16,  8, 24,  4, 20, 12, 28,
	10, 26, 18,  2, 22,  6, 30, 14,
	 1, 17,  9, 25,  7, 23, 15, 31,
	19,  3, 27, 11, 29, 13,  5, 21
};

/*
 * toplevel for the scanning, rescanning, and applying the heuristics.
 * returns number of changed tiles.
 */
int scan_for_updates(int count_only) {
	int i, tile_count, tile_diffs;
	int old_copy_tile;
	double frac1 = 0.1;   /* tweak parameter to try a 2nd scan_display() */
	double frac2 = 0.35;  /* or 3rd */
	double frac3 = 0.02;  /* do scan_display() again after copy_tiles() */
	static double last_poll = 0.0;

	if (slow_fb > 0.0) {
		double now = dnow();
		if (now < last_poll + slow_fb) {
			return 0;
		}
		last_poll = now;
	}

	for (i=0; i < ntiles; i++) {
		tile_has_diff[i] = 0;
		tile_has_xdamage_diff[i] = 0;
		tile_tried[i] = 0;
		tile_copied[i] = 0;
	}
	for (i=0; i < ntiles_y; i++) {
		/* could be useful, currently not used */
		tile_row_has_xdamage_diff[i] = 0;
	}
	xdamage_tile_count = 0;

	/*
	 * n.b. this program has only been tested so far with
	 * tile_x = tile_y = NSCAN = 32!
	 */

	if (!count_only) {
		scan_count++;
		scan_count %= NSCAN;

		/* some periodic maintenance */
		if (subwin) {
			set_offset();	/* follow the subwindow */
		}
		if (indexed_color && scan_count % 4 == 0) {
			/* check for changed colormap */
			set_colormap(0);
		}
		if (cmap8to24 && scan_count % 4 == 0) {
			check_for_multivis();
		}
		if (use_xdamage) {
			/* first pass collecting DAMAGE events: */
			collect_xdamage(scan_count, 0);
		}
	}

#define SCAN_FATAL(x) \
	if (x < 0) { \
		scan_in_progress = 0; \
		fb_copy_in_progress = 0; \
		return 0; \
	}

	/* scan with the initial y to the jitter value from scanlines: */
	scan_in_progress = 1;
	tile_count = scan_display(scanlines[scan_count], 0);
	SCAN_FATAL(tile_count);

	/*
	 * we do the XDAMAGE here too since after scan_display()
	 * there is a better chance we have received the events from
	 * the X server (otherwise the DAMAGE events will be processed
	 * in the *next* call, usually too late and wasteful since
	 * the unchanged tiles are read in again).
	 */
	if (use_xdamage) {
		collect_xdamage(scan_count, 1);
	}
	if (count_only) {
		scan_in_progress = 0;
		fb_copy_in_progress = 0;
		return tile_count;
	}

	if (xdamage_tile_count) {
		/* pick up "known" damaged tiles we missed in scan_display() */
		for (i=0; i < ntiles; i++) {
			if (tile_has_diff[i]) {
				continue;
			}
			if (tile_has_xdamage_diff[i]) {
				tile_has_diff[i] = 1;
				if (tile_has_xdamage_diff[i] == 1) {
					tile_has_xdamage_diff[i] = 2;
					tile_count++;
				}
			}
		}
	}

	nap_set(tile_count);

	if (fs_factor && frac1 >= fs_frac) {
		/* make frac1 < fs_frac if fullscreen updates are enabled */
		frac1 = fs_frac/2.0;
	}

	if (tile_count > frac1 * ntiles) {
		/*
		 * many tiles have changed, so try a rescan (since it should
		 * be short compared to the many upcoming copy_tiles() calls)
		 */

		/* this check is done to skip the extra scan_display() call */
		if (! fs_factor || tile_count <= fs_frac * ntiles) {
			int cp, tile_count_old = tile_count;
			
			/* choose a different y shift for the 2nd scan: */
			cp = (NSCAN - scan_count) % NSCAN;

			tile_count = scan_display(scanlines[cp], 1);
			SCAN_FATAL(tile_count);

			if (tile_count >= (1 + frac2) * tile_count_old) {
				/* on a roll... do a 3rd scan */
				cp = (NSCAN - scan_count + 7) % NSCAN;
				tile_count = scan_display(scanlines[cp], 1);
				SCAN_FATAL(tile_count);
			}
		}
		scan_in_progress = 0;

		/*
		 * At some number of changed tiles it is better to just
		 * copy the full screen at once.  I.e. time = c1 + m * r1
		 * where m is number of tiles, r1 is the copy_tiles()
		 * time, and c1 is the scan_display() time: for some m
		 * it crosses the full screen update time.
		 *
		 * We try to predict that crossover with the fs_frac
		 * fudge factor... seems to be about 1/2 the total number
		 * of tiles.  n.b. this ignores network bandwidth,
		 * compression time etc...
		 *
		 * Use -fs 1.0 to disable on slow links.
		 */
		if (fs_factor && tile_count > fs_frac * ntiles) {
			int cs;
			fb_copy_in_progress = 1;
			cs = copy_screen();
			fb_copy_in_progress = 0;
			SCAN_FATAL(cs);
			if (use_threads && pointer_mode != 1) {
				pointer(-1, 0, 0, NULL);
			}
			nap_check(tile_count);
			return tile_count;
		}
	}
	scan_in_progress = 0;

	/* copy all tiles with differences from display to rfb framebuffer: */
	fb_copy_in_progress = 1;

	if (single_copytile || tile_shm_count < ntiles_x) {
		/*
		 * Old way, copy I/O one tile at a time.
		 */
		old_copy_tile = 1;
	} else {
		/* 
		 * New way, does runs of horizontal tiles at once.
		 * Note that below, for simplicity, the extra tile finding
		 * (e.g. copy_tiles_backward_pass) is done the old way.
		 */
		old_copy_tile = 0;
	}
	if (old_copy_tile) {
		tile_diffs = copy_all_tiles();
	} else {
		tile_diffs = copy_all_tile_runs();
	}
	SCAN_FATAL(tile_diffs);

	/*
	 * This backward pass for upward and left tiles complements what
	 * was done in copy_all_tiles() for downward and right tiles.
	 */
	tile_diffs = copy_tiles_backward_pass();
	SCAN_FATAL(tile_diffs);

	if (tile_diffs > frac3 * ntiles) {
		/*
		 * we spent a lot of time in those copy_tiles, run
		 * another scan, maybe more of the screen changed.
		 */
		int cp = (NSCAN - scan_count + 13) % NSCAN;

		scan_in_progress = 1;
		tile_count = scan_display(scanlines[cp], 1);
		SCAN_FATAL(tile_count);
		scan_in_progress = 0;

		tile_diffs = copy_tiles_additional_pass();
		SCAN_FATAL(tile_diffs);
	}

	/* Given enough tile diffs, try the islands: */
	if (grow_fill && tile_diffs > 4) {
		tile_diffs = grow_islands();
	}
	SCAN_FATAL(tile_diffs);

	/* Given enough tile diffs, try the gaps: */
	if (gaps_fill && tile_diffs > 4) {
		tile_diffs = fill_tile_gaps();
	}
	SCAN_FATAL(tile_diffs);

	fb_copy_in_progress = 0;
	if (use_threads && pointer_mode != 1) {
		/*
		 * tell the pointer handler it can process any queued
		 * pointer events:
		 */
		pointer(-1, 0, 0, NULL);
	}

	if (blackouts) {
		/* ignore any diffs in completely covered tiles */
		int x, y, n;
		for (y=0; y < ntiles_y; y++) {
			for (x=0; x < ntiles_x; x++) {
				n = x + y * ntiles_x;
				if (tile_blackout[n].cover == 2) {
					tile_has_diff[n] = 0;
				}
			}
		}
	}

	hint_updates();	/* use x0rfbserver hints algorithm */

	/* Work around threaded rfbProcessClientMessage() calls timeouts */
	if (use_threads) {
		ping_clients(tile_diffs);
	}


	nap_check(tile_diffs);
	return tile_diffs;
}


