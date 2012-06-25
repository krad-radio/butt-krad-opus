// vorbis encoding functions for butt
//
// Copyright 2007-2008 by Daniel Noethen.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2, or (at your option)
// any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//

#ifndef VORBIS_ENCODE_H
#define VORBIS_ENCODE_H

#include <vorbis/vorbisenc.h>


struct vorbis_enc {
    ogg_stream_state os; /* take physical pages, weld into a logical stream of packets */
    ogg_page         og; /* one Ogg bitstream page.  Vorbis packets are inside */
    ogg_packet       op; /* one raw packet of data for decode */
    vorbis_info      vi;
    vorbis_comment   vc; /* struct that stores all the user comments */
    vorbis_block     vb;
    vorbis_dsp_state vd; /* central working state for the packet->PCM decoder */

    int bitrate;
    int samplerate;
    int channel;
    int state;
};

enum {
    VORBIS_READY = 0,
    VORBIS_BUSY = 1
};

extern vorbis_info  vorbis_vi;
extern char* vorbis_buf;

int vorbis_enc_init(vorbis_enc *vorbis);
int vorbis_enc_encode(vorbis_enc *vorbis, short *pcm_buf, char *enc_buf, int size);

int vorbis_enc_reinit(vorbis_enc *vorbis);

void vorbis_enc_write_header(vorbis_enc *vorbis);
void vorbis_enc_close(vorbis_enc *vorbis);

#endif

