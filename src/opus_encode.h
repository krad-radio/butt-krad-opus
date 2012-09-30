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

#include <opus/opus.h>
#include <opus/opus_multistream.h>
#include <ogg/ogg.h>

#define APPVERSION "OpusTransmiter Special Edition"
#define DEFAULT_OPUS_BITRATE 128000

typedef struct {
   int version;
   int channels; /* Number of channels: 1..255 */
   int preskip;
   ogg_uint32_t input_sample_rate;
   int gain; /* in dB S7.8 should be zero whenever possible */
   int channel_mapping;
   /* The rest is only used if channel_mapping != 0 */
   int nb_streams;
   int nb_coupled;
   unsigned char stream_map[255];
} OpusHeader;

struct opus_enc {
    ogg_stream_state os;
    ogg_page         og;
    ogg_packet       op;
    OpusEncoder    *encoder;
	OpusHeader *header;
	unsigned char *header_data;
	unsigned char *tags;
	int tags_size;
	int header_size;
	
	int packetno;
	int granulepos;
	
	int last_bitrate;
    int bitrate;
    int samplerate;
    int channel;
    int state;
    
	unsigned char *buffer;
    
};

enum {
    OPUS_READY = 0,
    OPUS_BUSY = 1
};

//extern opus_info  opus_vi;
extern OpusEncoder opus_vi;
extern char* opus_buf;
int opus_header_to_packet(const OpusHeader *h, unsigned char *packet, int len);
int opus_enc_init(opus_enc *opus);
int opus_enc_encode(opus_enc *opus, short *pcm_buf, char *enc_buf, int size);

int opus_enc_reinit(opus_enc *opus);

void opus_enc_write_header(opus_enc *opus);
void opus_enc_close(opus_enc *opus);

#endif

