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

/* Copyright (C)2012 Xiph.Org Foundation
   File: opus_header.c

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

   - Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.

   - Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR
   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif


#include <string.h>
#include <stdio.h>

typedef struct {
   unsigned char *data;
   int maxlen;
   int pos;
} Packet;

typedef struct {
   const unsigned char *data;
   int maxlen;
   int pos;
} ROPacket;

static int write_uint32(Packet *p, ogg_uint32_t val)
{
   if (p->pos>p->maxlen-4)
      return 0;
   p->data[p->pos  ] = (val    ) & 0xFF;
   p->data[p->pos+1] = (val>> 8) & 0xFF;
   p->data[p->pos+2] = (val>>16) & 0xFF;
   p->data[p->pos+3] = (val>>24) & 0xFF;
   p->pos += 4;
   return 1;
}

static int write_uint16(Packet *p, ogg_uint16_t val)
{
   if (p->pos>p->maxlen-2)
      return 0;
   p->data[p->pos  ] = (val    ) & 0xFF;
   p->data[p->pos+1] = (val>> 8) & 0xFF;
   p->pos += 2;
   return 1;
}

static int write_chars(Packet *p, const unsigned char *str, int nb_chars)
{
   int i;
   if (p->pos>p->maxlen-nb_chars)
      return 0;
   for (i=0;i<nb_chars;i++)
      p->data[p->pos++] = str[i];
   return 1;
}

int opus_header_to_packet(const OpusHeader *h, unsigned char *packet, int len)
{
   int i;
   Packet p;
   unsigned char ch;

   p.data = packet;
   p.maxlen = len;
   p.pos = 0;
   if (len<19)return 0;
   if (!write_chars(&p, (const unsigned char*)"OpusHead", 8))
      return 0;
   /* Version is 1 */
   ch = 1;
   if (!write_chars(&p, &ch, 1))
      return 0;

   ch = h->channels;
   if (!write_chars(&p, &ch, 1))
      return 0;

   if (!write_uint16(&p, h->preskip))
      return 0;

   if (!write_uint32(&p, h->input_sample_rate))
      return 0;

   if (!write_uint16(&p, h->gain))
      return 0;

   ch = h->channel_mapping;
   if (!write_chars(&p, &ch, 1))
      return 0;

   if (h->channel_mapping != 0)
   {
      ch = h->nb_streams;
      if (!write_chars(&p, &ch, 1))
         return 0;

      ch = h->nb_coupled;
      if (!write_chars(&p, &ch, 1))
         return 0;

      /* Multi-stream support */
      for (i=0;i<h->channels;i++)
      {
         if (!write_chars(&p, &h->stream_map[i], 1))
            return 0;
      }
   }

   return p.pos;
}

int opus_enc_init(opus_enc *opus)
{
    int err;

	err = 0;
	opus->header = (OpusHeader *)calloc(1, sizeof(OpusHeader));
	opus->header_data = (unsigned char *)calloc (1, 1024);	
	opus->tags = (unsigned char *)calloc (1, 1024);
	opus->buffer = (unsigned char *)calloc (1, 4 * 4096);
    srand(time(NULL));
    ogg_stream_init(&opus->os, rand());
	opus->header->gain = 0;
	opus->header->channels = 2;
	
	if ((opus->bitrate < 9600) || (opus->bitrate > 320000)) {
		opus->bitrate = DEFAULT_OPUS_BITRATE;
	}
	
	opus->header->input_sample_rate = 48000;
	opus->encoder = opus_encoder_create (48000, 2, OPUS_APPLICATION_AUDIO, &err);
	opus_encoder_ctl (opus->encoder, OPUS_SET_BITRATE(opus->bitrate));
	if (opus->encoder == NULL) {
		printf("Opus Encoder creation error: %s\n", opus_strerror (err));
		return 1;
	}
	opus->last_bitrate = opus->bitrate;
	opus_encoder_ctl (opus->encoder, OPUS_GET_LOOKAHEAD (&opus->header->preskip));
	opus->header_size = opus_header_to_packet (opus->header, opus->header_data, 100);

	opus->tags_size = 
	8 + 4 + strlen (opus_get_version_string ()) + 4 + 4 + strlen ("ENCODER=") + strlen (APPVERSION);
	
	memcpy (opus->tags, "OpusTags", 8);
	
	opus->tags[8] = strlen (opus_get_version_string ());
	
	memcpy (opus->tags + 12, opus_get_version_string (), strlen (opus_get_version_string ()));

	opus->tags[12 + strlen (opus_get_version_string ())] = 1;

	opus->tags[12 + strlen (opus_get_version_string ()) + 4] = strlen ("ENCODER=") + strlen (APPVERSION);
	
	memcpy (opus->tags + 12 + strlen (opus_get_version_string ()) + 4 + 4, "ENCODER=", strlen ("ENCODER="));
	
	memcpy (opus->tags + 12 + strlen (opus_get_version_string ()) + 4 + 4 + strlen ("ENCODER="),
			APPVERSION,
			strlen (APPVERSION));	

	//printf("Opus Encoder Created\n");

    return 0;
}

//This function needs to be called before
//every connection
void opus_enc_write_header(opus_enc *opus)
{

	ogg_packet op;

	ogg_stream_clear (&opus->os);
	ogg_stream_init(&opus->os, rand());	
	opus_encoder_ctl (opus->encoder, OPUS_RESET_STATE);

	opus->packetno = 0;
	opus->granulepos = 0;
	
	op.b_o_s = 1;
	op.e_o_s = 0;
	op.granulepos = 0;
	op.packetno = opus->packetno++;
	op.packet = opus->header_data;
	op.bytes = opus->header_size;

	ogg_stream_packetin (&opus->os, &op);
	
	op.b_o_s = 0;
	op.e_o_s = 0;
	op.granulepos = 0;
	op.packetno = opus->packetno++;
	op.packet = opus->tags;
	op.bytes = opus->tags_size;

	ogg_stream_packetin (&opus->os, &op);
	
	//printf("Opus Encoder write headers\n");

}

int opus_enc_reinit(opus_enc *opus)
{
     opus_enc_close(opus);
     return opus_enc_init(opus);
}

int opus_enc_encode(opus_enc *opus, short *pcm_buf, char *enc_buf, int size)
{

    int w = 0;
	ogg_packet op;
	int ret;

    if(size == 0)
        return 0;

	//printf("Opus Encoder encoding %d samples\n", size);

	if (opus->encoder == NULL) {
		printf("Opus Encoder NULL wtf?\n");
		return 0;
	}
	
	while (ogg_stream_flush(&opus->os, &opus->og) != 0) {
        memcpy(enc_buf+w, opus->og.header, opus->og.header_len);
        w += opus->og.header_len;
        memcpy(enc_buf+w, opus->og.body, opus->og.body_len);
        w += opus->og.body_len;
	}
	
	if (opus->last_bitrate != opus->bitrate) {
		if ((opus->bitrate < 9600) || (opus->bitrate > 320000)) {
			opus->bitrate = DEFAULT_OPUS_BITRATE;
		}	
		opus_encoder_ctl (opus->encoder, OPUS_SET_BITRATE(opus->bitrate));
		opus->last_bitrate = opus->bitrate;
	}
	

	ret = opus_encode (opus->encoder, pcm_buf, 960, opus->buffer, 2048 * 4);
	
	//printf("Opus Encoder encoding %d samples, got back %d bytes\n", size, ret);
	
	op.b_o_s = 0;
	op.e_o_s = 0;
	op.granulepos = opus->granulepos;
	op.packetno = opus->packetno++;
	op.packet = opus->buffer;
	op.bytes = ret;

	opus->granulepos += 960;

	ogg_stream_packetin (&opus->os, &op);
	
	while (ogg_stream_flush(&opus->os, &opus->og) != 0) {
        memcpy(enc_buf+w, opus->og.header, opus->og.header_len);
        w += opus->og.header_len;
        memcpy(enc_buf+w, opus->og.body, opus->og.body_len);
        w += opus->og.body_len;
	}
	
    return w;
}

void opus_enc_close(opus_enc *opus)
{

	if (opus->buffer != NULL) {
		ogg_stream_clear (&opus->os);
		opus_encoder_destroy (opus->encoder);

		free (opus->header_data);
		free (opus->header);
		free (opus->tags);
		free (opus->buffer);
		opus->buffer = NULL;
	}
	//printf("Opus Encoder Destroyed\n");

}
