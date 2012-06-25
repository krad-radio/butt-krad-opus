// lame encoding functions for butt
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

#ifndef LAME_ENCODE_H
#define LAME_ENCODE_H

#include <stdlib.h>
#include <lame/lame.h>


struct lame_enc {
    lame_global_flags *gfp;
    int bitrate;
    int samplerate_in;
    int samplerate_out;
    int channel;
    volatile int state;
};

enum {
    LAME_READY = 0,
    LAME_BUSY = 1
};

int lame_enc_init(lame_enc *lame);
int lame_enc_encode(lame_enc *lame, short *pcm_buf, char *enc_buf, int samples, int size);
int lame_enc_reinit(lame_enc *lame);
void lame_enc_close(lame_enc *lame);

#endif

