#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <errno.h>
#include <assert.h>

#include <libketlaer.h>
#include <qtkeys.h>

#define CONTROL_OK            0
#define CONTROL_UNKNOWN      -1 
#define CONTROL_PARAM_ERROR  -2 
#define CONTROL_DISCONNECTED -3

#define OSD_DEF_WIDTH      720
#define OSD_DEF_HEIGHT     576
#define HUD_MAX_WIDTH      1920
#define HUD_MAX_HEIGHT     1080

#define PACKED  __attribute__((packed))

typedef enum  {
  OSD_Nop         = 0,    /* Do nothing ; used to initialize delay_ms counter */
  OSD_Size        = 1,    /* Set size of VDR OSD area (usually 720x576) */
  OSD_Set_RLE     = 2,    /* Create/update OSD window. Data is rle-compressed. */
  OSD_SetPalette  = 3,    /* Modify palette of already created OSD window */ 
  OSD_Move        = 4,    /* Change x/y position of already created OSD window */ 
  OSD_Close       = 5,    /* Close OSD window */
  OSD_Set_YUV     = 6,    /* Create/update OSD window. Data is in YUV420 format. */
  OSD_Commit      = 7     /* All OSD areas have been updated, commit changes to display */
} osd_command_id_t;

#define OSDFLAG_YUV_CLUT        0x01 /* palette is in YUV format */
#define OSDFLAG_REFRESH         0x02 /* OSD data refresh for new config, clients, etc. - no changes in bitmap */
#define OSDFLAG_UNSCALED        0x04 /* xine-lib unscaled (hardware) blending                   */
#define OSDFLAG_UNSCALED_LOWRES 0x08 /* unscaled blending when video resolution < .95 * 720x576 */

typedef struct xine_clut_s {
  union {
    uint8_t cb  /*: 8*/;
    uint8_t g;
  };
  union {
    uint8_t cr  /*: 8*/;
    uint8_t b;
  };
  union {
    uint8_t y   /*: 8*/;
    uint8_t r;
  };
  uint8_t alpha /*: 8*/;
} PACKED xine_clut_t; /* from xine, alphablend.h */

typedef struct xine_rle_elem_s {
  uint16_t len;
  uint16_t color;
} PACKED xine_rle_elem_t; /* from xine */

typedef struct osd_rect_s {
  uint16_t x1;
  uint16_t y1;
  uint16_t x2;
  uint16_t y2;
} osd_rect_t;

typedef struct osd_command_s {
  uint32_t cmd;      /* osd_command_id_t */

  uint32_t wnd;      /* OSD window handle */

  int64_t  pts;      /* execute at given pts */
  uint32_t delay_ms; /* execute 'delay_ms' ms after previous command (for same window). */

  uint16_t x;         /* window position, x */
  uint16_t y;         /* window position, y */
  uint16_t w;         /* window width */
  uint16_t h;         /* window height */

  uint32_t datalen;   /* size of image data, in bytes */
  uint32_t num_rle;
  union {
    xine_rle_elem_t *data; /* RLE compressed image */
    uint8_t         *raw_data;
    uint64_t         dummy01;
  };
  uint32_t colors;         /* palette size */
  union {
    xine_clut_t     *palette;  /* palette (YCrCb) */
    uint64_t         dummy02;
  };

  osd_rect_t dirty_area;
  uint8_t    flags;
  uint8_t    scaling;

} PACKED osd_command_t;

#define ntohull(val) \
  ((uint64_t) ntohl((uint32_t)((val) >> 32)) |  \
   (uint64_t) ntohl((uint32_t)(val)) << 32)
#define htonull(val) \
  ((uint64_t) htonl((uint32_t)((val) >> 32)) |  \
   (uint64_t) htonl((uint32_t)(val)) << 32)
#define ntohll(val) ((int64_t)ntohull((uint64_t)val))
#define htonll(val) ((int64_t)htonull((uint64_t)val))

#define hton_osdcmd(cmdP) \
  do { \
  cmdP.cmd      = htonl (cmdP.cmd);                \
  cmdP.wnd      = htonl (cmdP.wnd);                \
  cmdP.pts      = htonll(cmdP.pts);                \
  cmdP.delay_ms = htonl (cmdP.delay_ms);           \
  cmdP.x        = htons (cmdP.x);                  \
  cmdP.y        = htons (cmdP.y);                  \
  cmdP.w        = htons (cmdP.w);                  \
  cmdP.h        = htons (cmdP.h);                  \
  cmdP.datalen  = htonl (cmdP.datalen);            \
  cmdP.num_rle  = htonl (cmdP.num_rle);            \
  cmdP.colors   = htonl (cmdP.colors);             \
  cmdP.dirty_area.x1 = htons(cmdP.dirty_area.x1);  \
  cmdP.dirty_area.y1 = htons(cmdP.dirty_area.y1);  \
  cmdP.dirty_area.x2 = htons(cmdP.dirty_area.x2);  \
  cmdP.dirty_area.y2 = htons(cmdP.dirty_area.y2);  \
} while(0)

#define ntoh_osdcmd(cmdP) \
  do { \
  cmdP.cmd      = ntohl (cmdP.cmd);                \
  cmdP.wnd      = ntohl (cmdP.wnd);                \
  cmdP.pts      = ntohll(cmdP.pts);                \
  cmdP.delay_ms = ntohl (cmdP.delay_ms);           \
  cmdP.x        = ntohs (cmdP.x);                  \
  cmdP.y        = ntohs (cmdP.y);                  \
  cmdP.w        = ntohs (cmdP.w);                  \
  cmdP.h        = ntohs (cmdP.h);                  \
  cmdP.datalen  = ntohl (cmdP.datalen);            \
  cmdP.num_rle  = ntohl (cmdP.num_rle);            \
  cmdP.colors   = ntohl (cmdP.colors);             \
  cmdP.dirty_area.x1 = ntohs(cmdP.dirty_area.x1);  \
  cmdP.dirty_area.y1 = ntohs(cmdP.dirty_area.y1);  \
  cmdP.dirty_area.x2 = ntohs(cmdP.dirty_area.x2);  \
  cmdP.dirty_area.y2 = ntohs(cmdP.dirty_area.y2);  \
} while(0)

static char conn_url[256];
static int  conn_fd = -1;

static int            osd_width  = OSD_DEF_WIDTH;;
static int            osd_height = OSD_DEF_HEIGHT;
static HANDLE         g_hOSD = NULL;
static VideoPlayback *g_pb = NULL;

static bool init_connection(const char *ahost, const char *aport)
{
  hostent *hent;
  int port = -1;
  sockaddr_in addr;
  int one = 1;

  if (!(hent = gethostbyname(ahost))) {
    herror(ahost);
    return false;
  }
  port = atoi(aport);
  if (port < 0) {
    fprintf(stderr, "bad port\n");
    return false;
  }
  snprintf(conn_url, sizeof(conn_url), "http://%s:%d", ahost, port);
  memset(&addr, 0, sizeof(addr));
  addr.sin_port = htons(port);
  addr.sin_family = AF_INET;
  memcpy(&addr.sin_addr.s_addr, hent->h_addr, sizeof(addr.sin_addr.s_addr));
  if ((conn_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("socket");
    return false;
  }
  if (connect(conn_fd, (sockaddr*)&addr, sizeof(addr)) < 0) {
    perror("connect");
    close(conn_fd);
    return false;
  }
  fcntl(conn_fd, F_SETFL, O_NONBLOCK);
  setsockopt(conn_fd, IPPROTO_TCP, TCP_NODELAY, &one, sizeof(int));
  return true;
}

static void write_control(const char *str, size_t len)
{
  size_t ret;
  while(len>0) {
    fd_set wset;
    timeval select_timeout;

    FD_ZERO(&wset);
    FD_SET(conn_fd, &wset);
    select_timeout.tv_sec  = 0;
    select_timeout.tv_usec = 500*1000; // 500 ms
    errno = 0;
    if(select(conn_fd+1, NULL, &wset, NULL, &select_timeout) <= 0) {
      perror("select()");
      return;
    }

    errno = 0;
    ret = write(conn_fd, str, len);

    if(ret <= 0) {
      if(errno == EAGAIN) continue;
      if(errno == EINTR)  continue;
      perror("write_control()");
      return;
    }
    len -= ret;
    str += ret;
  }
}

static void write_control(const char *str)
{
  write_control(str, strlen(str));
}

static int read_control(uint8_t *buf, int len)
{
  int num_bytes, total_bytes = 0, err;

  while(total_bytes < len) {
    fd_set rset;
    timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 500*1000;
    FD_ZERO(&rset);
    FD_SET(conn_fd, &rset);
    switch(select(conn_fd+1, &rset, NULL, NULL, &tv)) {
    case  0:
      continue;
    case -1:
      perror("select()");
      return -1;
    }

    errno = 0;
    num_bytes = read(conn_fd, buf + total_bytes, len - total_bytes);

    if (num_bytes <= 0) {
      if (errno == EAGAIN) continue;
      if (errno == EINTR)  continue;
      perror("read()");
      return -1;
    }
    total_bytes += num_bytes;
  }

  return total_bytes;
}

static int readline_control(char *buf, int maxlen, int timeout)
{
  int num_bytes = 0, total_bytes = 0, err;

  *buf = 0;
  while(total_bytes < maxlen-1) {
    errno = 0;
    if (timeout <= 0) {
      fd_set rset;
      timeval tv;
      tv.tv_sec = 0;
      tv.tv_usec = 500*1000;
      FD_ZERO(&rset);
      FD_SET(conn_fd, &rset);
      switch(select(conn_fd+1, &rset, NULL, NULL, &tv)) {
      case  0:
	continue;
      case -1:
	perror("select()");
	return -1;
      }
    }

    errno = 0;
    num_bytes = read(conn_fd, buf + total_bytes, 1);

    if (num_bytes <= 0) {
      if (errno == EAGAIN) continue;
      if (errno == EINTR)  continue;
      perror("read()");
      return -1;
    }      
    if(buf[total_bytes]) {
      if(buf[total_bytes] == '\r') {
	buf[total_bytes] = 0;
      } else if(buf[total_bytes] == '\n') {
	buf[total_bytes] = 0;
	break;
      } else {
	total_bytes++;
	buf[total_bytes] = 0;
      }
    }
  }
  return total_bytes;
}

static void hud_fill_img_memory(uint32_t *dst, const struct osd_command_s *cmd)
{
  int i, pixelcounter = 0;
  int idx = cmd->y * HUD_MAX_WIDTH + cmd->x;

  for(i = 0; i < cmd->num_rle; ++i) {
    uint32_t a = (cmd->palette + (cmd->data + i)->color)->alpha;
    uint32_t r = (cmd->palette + (cmd->data + i)->color)->r;
    uint32_t g = (cmd->palette + (cmd->data + i)->color)->g;
    uint32_t b = (cmd->palette + (cmd->data + i)->color)->b;
    int j;
    uint32_t finalcolor = 0;
    finalcolor |= ((b << 24) & 0xFF000000);
    finalcolor |= ((g << 16) & 0x00FF0000);
    finalcolor |= ((r <<  8) & 0x0000FF00);
    finalcolor |= ( a        & 0x000000FF);
    
    for(j = 0; j < (cmd->data + i)->len; ++j) {
      if(pixelcounter >= cmd->w) {
        idx += HUD_MAX_WIDTH - pixelcounter;
        pixelcounter = 0;
      }
      dst[idx++] = finalcolor;
      ++pixelcounter;
    }
  }
}

static int hud_osd_command(struct osd_command_s *cmd)
{
  switch(cmd->cmd) {
  case OSD_Nop: /* Do nothing ; used to initialize delay_ms counter */
    printf("HUD osd NOP\n");
    break;

  case OSD_Size: /* Set size of VDR OSD area */
    printf("HUD Set Size\n");
    osd_width = (cmd->w > 0) ? cmd->w : OSD_DEF_WIDTH;
    osd_height = (cmd->h > 0) ? cmd->h : OSD_DEF_HEIGHT;
    printf("h=%d,w=%d\n", osd_width, osd_height);
    break;

  case OSD_Set_RLE: /* Create/update OSD window. Data is rle-compressed. */
    printf("HUD Set RLE\n");

    int x = cmd->x + cmd->dirty_area.x1;
    int y = cmd->y + cmd->dirty_area.y1;
    int w = cmd->dirty_area.x2 - cmd->dirty_area.x1 + 1;
    int h = cmd->dirty_area.y2 - cmd->dirty_area.y1 + 1;

#define OSD_SCALE_X(x)	(int)((double)x * getScreenRect()->width  / 720)
#define OSD_SCALE_Y(y)	(int)((double)y * getScreenRect()->height / 576)

    int xs = OSD_SCALE_X(x);
    int ys = OSD_SCALE_Y(y);
    int ws = OSD_SCALE_X(w);
    int hs = OSD_SCALE_Y(h);
	
    hud_fill_img_memory((uint32_t*)DG_GetSurfaceRowDataPointer(g_hOSD), cmd);
    DG_Stretch(getScreenSurface(), 
	       xs,
	       ys,
	       ws,
	       hs,
	       g_hOSD,
	       x,
	       y,
	       w,
	       h,
	       ColorKey_Src,
	       RESERVED_COLOR_KEY,
	       true,
	       NULL,
	       SCALING_MODE_AUTO,
	       0,
	       0);
    break;

  case OSD_SetPalette: /* Modify palette of already created OSD window */
    printf("HUD osd SetPalette\n");
    break;

  case OSD_Move:       /* Change x/y position of already created OSD window */
    printf("HUD osd Move\n");
    break;

  case OSD_Set_YUV:    /* Create/update OSD window. Data is in YUV420 format. */
    printf("HUD osd set YUV\n");
    break;

  case OSD_Close: /* Close OSD window */
    printf("HUD osd Close\n");
    DG_DrawRectangle(getScreenSurface(), 
		     0, 
		     0, 
		     getScreenRect()->width, 
		     getScreenRect()->height, 
		     RESERVED_COLOR_KEY, 
		     NULL);
    break;

  default:
    printf("hud_osd_command: unknown osd command\n");
    break;
  }
  return 1;
}

static xine_rle_elem_t *uncompress_osd_net(uint8_t *raw, int elems, int datalen)
{
  xine_rle_elem_t *data = (xine_rle_elem_t*)malloc(elems*sizeof(xine_rle_elem_t));
  int i;

  /*
   * xine-lib rle format: 
   * - palette index and length are uint16_t
   *
   * network format: 
   * - palette entry is uint8_t
   * - length is uint8_t for lengths <=0x7f and uint16_t for lengths >0x7f
   * - high-order bit of first byte is used to signal size of length field:
   *   bit=0 -> 7-bit, bit=1 -> 15-bit
   */

  for(i=0; i<elems; i++) {
    if((*raw) & 0x80) {
      data[i].len = ((*(raw++)) & 0x7f) << 8;
      data[i].len |= *(raw++);
    } else
      data[i].len = *(raw++);
    data[i].color = *(raw++);
  }

  return data;
}

static int handle_control_osdcmd()
{
  osd_command_t osdcmd;
  int err = CONTROL_OK;

  if(read_control((unsigned char*)&osdcmd, sizeof(osd_command_t))
     != sizeof(osd_command_t)) {
    fprintf(stderr, "control: error reading OSDCMD data\n");
    return CONTROL_DISCONNECTED;
  }

  ntoh_osdcmd(osdcmd);

  if(osdcmd.palette && osdcmd.colors>0) {
    int bytes = sizeof(xine_clut_t)*(osdcmd.colors);
    osdcmd.palette = (xine_clut_t*)malloc(bytes);
    if(read_control((unsigned char *)osdcmd.palette, bytes)
       != bytes) {
      fprintf(stderr, "control: error reading OSDCMD palette\n");
      err = CONTROL_DISCONNECTED;
    }
  } else {
    osdcmd.palette = NULL;
  }

  if(err == CONTROL_OK && osdcmd.data && osdcmd.datalen>0) {
    osdcmd.data = (xine_rle_elem_t*)malloc(osdcmd.datalen);
    if(read_control((unsigned char *)osdcmd.data, osdcmd.datalen)
       != osdcmd.datalen) {
      fprintf(stderr, "control: error reading OSDCMD bitmap\n");
      err = CONTROL_DISCONNECTED;
    } else {
      uint8_t *raw = osdcmd.raw_data;
      osdcmd.data = uncompress_osd_net(raw, osdcmd.num_rle, osdcmd.datalen);
      osdcmd.datalen = osdcmd.num_rle*4;
      free(raw);
    }
  } else {
    osdcmd.data = NULL;
  }

  if(err == CONTROL_OK) 
    err = hud_osd_command(&osdcmd);

  free(osdcmd.data);
  free(osdcmd.palette);

  return err;
}

static void process_control()
{
  char line[8128];
  int err;

  /* read next command */
  line[0] = 0;
  while(true) {
    if((err=readline_control(line, sizeof(line)-1, -1)) <= 0) {
      if(err < 0) return;
      continue;
    } 
    break;
  }
  printf("control:%s\n", line);
  if (strncasecmp(line, "OSDCMD", 6) == 0) {
    handle_control_osdcmd();
  }
}

static void play_stream()
{
  g_pb->LoadMedia(conn_url);
  g_pb->m_pFManager->SetRate(256);
  g_pb->m_pAudioOut->SetFocus();
  g_pb->m_pAudioOut->SetVolume(0);
  g_pb->SetVideoOut(VO_VIDEO_PLANE_V1, 0, 0);
  g_pb->ScanFileStart(false);
  g_pb->m_pFManager->Run();
}

static bool init(char *args)
{
  char *host, *port;

  host = "192.168.0.50";
  port = "37890";

  init_libketlaer();
  if (!init_connection(host, port))
    return false;
  write_control("CONTROL\r\n");
  g_hOSD = getSurfaceHandle(HUD_MAX_WIDTH, HUD_MAX_HEIGHT, Format_32);
  g_pb = getVideoPlayback();
  play_stream();
  return true;
}

static void deinit()
{
  if (g_pb) {
    g_pb->m_pFManager->Stop();
    g_pb = NULL;
  }
  if (g_hOSD) {
    DG_CloseSurface(g_hOSD);
    g_hOSD = NULL;
  }
  if (conn_fd >= 0) {
    close(conn_fd);
    conn_fd = -1;
  }
  uninit_libketlaer();
}

static struct {
  int   key;
  char *str;
} vdr_keymap[] = {
  {Key_0,             "0"        },
  {Key_1,             "1"        },
  {Key_2,             "2"        },
  {Key_3,             "3"        },
  {Key_4,             "4"        },
  {Key_5,             "5"        },
  {Key_6,             "6"        },
  {Key_7,             "7"        },
  {Key_7,             "8"        },
  {Key_9,             "9"        },
  {Key_Up,            "Up"       },
  {Key_Down,          "Down"     },
  {Key_Left,          "Left"     },
  {Key_Right,         "Right"    },
  {Key_Enter,         "Ok"       },
  {Key_Home,          "Menu"     },
  {Key_Backspace,     "Back"     },
  {Key_MediaNext,     "Next"     },
  {Key_MediaPrevious, "Previous" },
  {Key_MediaPlay,     "Play"     },
  {Key_MediaStop,     "Stop"     },
  {Key_VolumeUp,      "Volume+"  },
  {Key_VolumeDown,    "Volume-"  },
  {Key_VolumeMute,    "Mute"     },
  {Key_PowerOff,      "Power"    },
  {Key_F9,            "Red"      },
  {Key_F10,           "Green"    },
  {Key_F11,           "Yellow"   },
  {Key_F12,           "Blue"     },
  {0, 0},
};

static void process_keyboard()
{
  int idx = 0,key = ir_getkey();
  char *str = NULL, cmd[32];

  printf("[VDR]Key = %lx\n", key);

  while(vdr_keymap[idx].str) {
    if (vdr_keymap[idx].key == key) {
      str = vdr_keymap[idx].str;
      break;
    }
    idx++;
  }

  printf("[VDR]Mapped = %s\n", str ? str : "NOTHING");
  if (str) {
    snprintf(cmd, sizeof(cmd), "KEY %s\r\n", str);
    write_control(cmd);
  }
}

static void mainloop()
{
  timeval tv;
  fd_set  set;
  int     ir_fd = ir_getfd();

  while(true) {
    tv.tv_sec  = 0;
    tv.tv_usec = 100000;
    FD_ZERO(&set);
    FD_SET(conn_fd, &set);
    FD_SET(ir_fd, &set);
    if (select(FD_SETSIZE, &set, NULL, NULL, &tv)) {
      if (FD_ISSET(conn_fd, &set))
	process_control();
      if (FD_ISSET(ir_fd, &set))
	process_keyboard();
    }
  }
}

int run_vdr_frontend(char *args)
{
  int ret = 0;

  if (init(args)) {
    mainloop();
  }
  deinit();

  return ret;
}
