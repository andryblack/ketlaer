#ifndef MPEG_HPP
#define MPEG_HPP

typedef unsigned char UINT8;

struct mpeg_frame {
 UINT8 *data; /* generated MPEG- frame */
 int size;    /* size of data in *data, data is malloced to a constant size, but only part is filled */
};

typedef struct mpeg_frame mpeg_frame;

/* set up a few globals and initialize codec */
void mpeg_init(bool ntsc);

/* send a BGRA buffer to this with *buf, and its size and you will get an mpeg_frame in return */
mpeg_frame mpeg_draw(UINT8 *buf);

/* cleanup and free */
void mpeg_deinit();

#endif
