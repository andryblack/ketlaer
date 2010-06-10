#ifndef _X11VNC_PARAMS_H
#define _X11VNC_PARAMS_H

/* -- params.h -- */

#define ICON_MODE_SOCKS 16

#ifndef WIREFRAME_PARMS
#define WIREFRAME_PARMS "0xff,3,0,32+8+8+8,all,0.15+0.30+5.0+0.125"
#endif

#ifndef SCROLL_COPYRECT_PARMS
#define SCROLL_COPYRECT_PARMS "0+64+32+32,0.02+0.10+0.9,0.03+0.06+0.5+0.1+5.0"
#endif

#define LATENCY0 20     /* 20ms */
#define NETRATE0 20     /* 20KB/sec */

#define POINTER_MODE_NOFB 2

/* scan pattern jitter from x0rfbserver */
#define NSCAN 32

#define FB_COPY 0x1
#define FB_MOD  0x2
#define FB_REQ  0x4

#define VNC_CONNECT_MAX 16384
#define PROP_MAX (131072L)

#define MAXN 256

#endif /* _X11VNC_PARAMS_H */
