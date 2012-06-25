// wav functions for butt
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

#include <stdio.h>
#include <string.h>

#include "wav_header.h"

int wav_write_header(FILE *fd, short ch, int srate, short bps)
{
    long int cur_size;
    wav_hdr hdr;

    cur_size = ftell(fd);

    hdr.wav.riff_size = cur_size >= 44 ? cur_size-8 : 0;
    memcpy(&hdr.wav.riff_id, "RIFF", 4);
    memcpy(&hdr.wav.riff_format, "WAVE", 4);

    memcpy(hdr.wav.fmt_id, "fmt ", 4);
    hdr.wav.fmt_size = 16;
    hdr.wav.fmt_format = 1;
    hdr.wav.fmt_channel = ch;
    hdr.wav.fmt_samplerate = srate;
    hdr.wav.fmt_bps = 16;
    hdr.wav.fmt_block_align = ch * hdr.wav.fmt_bps / 8;
    hdr.wav.fmt_byte_rate = srate * hdr.wav.fmt_block_align;

    memcpy(&hdr.wav.data_id, "data", 4);
    hdr.wav.data_size = cur_size >= 44 ? cur_size-44 : 0;

    //write the header to the beginning of the file
    rewind(fd);
    fwrite(&hdr.data, 1, sizeof(hdr.data), fd);

    //set the fd back to the fileend
    fseek(fd, cur_size, SEEK_SET);

    return 0;
}



