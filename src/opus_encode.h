// opus encoding functions for butt
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

#ifndef OPUS_ENCODE_H
#define OPUS_ENCODE_H

#include <opus.h>
#include <opus_multistream.h>
#include <ogg/ogg.h>


struct opus_enc {
    ogg_stream_state os; /* take physical pages, weld into a logical stream of packets */
    ogg_page         og; /* one Ogg bitstream page.  opus packets are inside */
    ogg_packet       op; /* one raw packet of data for decode */
    OpusMSEncoder	*encoder;

    int bitrate;
    int samplerate;
    int channel;
    int state;
};

enum {
    OPUS_READY = 0,
    OPUS_BUSY = 1
};

extern opus_info  opus_vi;
extern char* opus_buf;

int opus_enc_init(opus_enc *opus);
int opus_enc_encode(opus_enc *opus, short *pcm_buf, char *enc_buf, int size);

int opus_enc_reinit(opus_enc *opus);

void opus_enc_write_header(opus_enc *opus);
void opus_enc_close(opus_enc *opus);

#endif

