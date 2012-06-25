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

#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "opus_encode.h"

int opus_enc_init(opus_enc *opus)
{
    int ret;


    return 0;
}

//This function needs to be called before
//every connection
void opus_enc_write_header(opus_enc *opus)
{
    ogg_packet header;
    ogg_packet header_comm;
    ogg_packet header_code;

    srand(time(NULL));
    ogg_stream_init(&(opus->os), rand());

}

int opus_enc_reinit(opus_enc *opus)
{
     opus_enc_close(opus);
     return opus_enc_init(opus);
}

int opus_enc_encode(opus_enc *opus, short *pcm_buf, char *enc_buf, int size)
{
    int i, result;
    int eos = 0;
    int w = 0;
    float **opus_buf;

    if(size == 0)
        return 0;

	/*
    if(opus->channel == 1)
         for(i = 0 ; i < size ; i++)
             opus_buf[0][i] = pcm_buf[i]/32768.f;
    else
        for(i = 0 ; i < size; i++)
        {
            opus_buf[0][i] = pcm_buf[i*2]/32768.f;
            opus_buf[1][i] = pcm_buf[i*2+1]/32768.f;
        }

	*/
    return w;
}

void opus_enc_close(opus_enc *opus)
{
    ogg_stream_clear(&(opus->os));

}
