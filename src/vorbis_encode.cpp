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

#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "vorbis_encode.h"

int vorbis_enc_init(vorbis_enc *vorbis)
{
    int ret;

    //TODO: add error handling
    vorbis_info_init(&(vorbis->vi));

    ret = vorbis_encode_init(&(vorbis->vi),
                  vorbis->channel,
                  vorbis->samplerate,
                  vorbis->bitrate*1000,
                  vorbis->bitrate*1000,
                  vorbis->bitrate*1000);
    if(ret)
        return ret;

    vorbis_comment_init(&(vorbis->vc));
    vorbis_comment_add_tag(&(vorbis->vc), "ENCODER", VERSION);

    vorbis_analysis_init(&(vorbis->vd), &(vorbis->vi));
    vorbis_block_init(&(vorbis->vd), &(vorbis->vb));

    return 0;
}

//This function needs to be called before
//every connection
void vorbis_enc_write_header(vorbis_enc *vorbis)
{
    ogg_packet header;
    ogg_packet header_comm;
    ogg_packet header_code;

    srand(time(NULL));
    ogg_stream_init(&(vorbis->os), rand());

    vorbis_analysis_headerout(&(vorbis->vd), &(vorbis->vc),
            &header, &header_comm, &header_code);
    ogg_stream_packetin(&(vorbis->os), &header);
    ogg_stream_packetin(&(vorbis->os), &header_comm);
    ogg_stream_packetin(&(vorbis->os), &header_code);
}

int vorbis_enc_reinit(vorbis_enc *vorbis)
{
     vorbis_enc_close(vorbis);
     return vorbis_enc_init(vorbis);
}

int vorbis_enc_encode(vorbis_enc *vorbis, short *pcm_buf, char *enc_buf, int size)
{
    int i, result;
    int eos = 0;
    int w = 0;
    float **vorbis_buf;

    if(size == 0)
        return 0;


    /* This ensures the actual
     * audio data will start on a new page, as per spec
     */
    while(!eos)
    {
        result = ogg_stream_flush(&(vorbis->os), &(vorbis->og));
        if(result == 0)
            break;
        memcpy(enc_buf+w, vorbis->og.header, vorbis->og.header_len);
        w += vorbis->og.header_len;
        memcpy(enc_buf+w, vorbis->og.body, vorbis->og.body_len);
        w += vorbis->og.body_len;
    }

    vorbis_buf = vorbis_analysis_buffer(&(vorbis->vd), size);

    //deinterlace audio data and convert it from short to float
    if(vorbis->channel == 1)
         for(i = 0 ; i < size ; i++)
             vorbis_buf[0][i] = pcm_buf[i]/32768.f;
    else
        for(i = 0 ; i < size; i++)
        {
            vorbis_buf[0][i] = pcm_buf[i*2]/32768.f;
            vorbis_buf[1][i] = pcm_buf[i*2+1]/32768.f;
        }

    vorbis_analysis_wrote(&(vorbis->vd), size);

    while(vorbis_analysis_blockout(&(vorbis->vd), &(vorbis->vb)) == 1)
    {
        vorbis_analysis(&(vorbis->vb),&(vorbis->op));
        vorbis_bitrate_addblock(&(vorbis->vb));

        while(vorbis_bitrate_flushpacket(&(vorbis->vd),&(vorbis->op)))
        {
            /* weld the packet into the bitstream */
            ogg_stream_packetin(&(vorbis->os),&(vorbis->op));

            /* write out pages (if any) */
            while(!eos)
            {
                result = ogg_stream_pageout(&(vorbis->os), &(vorbis->og));
                if(result == 0) 
                    break;
                memcpy(enc_buf+w, vorbis->og.header, vorbis->og.header_len);
                w += vorbis->og.header_len;
                memcpy(enc_buf+w, vorbis->og.body, vorbis->og.body_len);
                w += vorbis->og.body_len;
                if(ogg_page_eos(&(vorbis->og)))
                    eos=1;
            }
        }
    }

    return w;
}

void vorbis_enc_close(vorbis_enc *vorbis)
{
    ogg_stream_clear(&(vorbis->os));
    vorbis_block_clear(&(vorbis->vb));
    vorbis_dsp_clear(&(vorbis->vd));
    vorbis_comment_clear(&(vorbis->vc));
    vorbis_info_clear(&(vorbis->vi));
}
