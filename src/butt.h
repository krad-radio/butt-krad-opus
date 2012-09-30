// butt - broadcast using this tool
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

#ifndef BUTT_H
#define BUTT_H

#include "timer.h"
#include "lame_encode.h"
#include "vorbis_encode.h"
#include "opus_encode.h"

extern bool record;
extern bool recording;      //TRUE if butt is recording
extern bool connected;      //TRUE if butt is connected to server
extern bool disconnect;     //TRUE if butt should disconnect
extern bool try_connect;    //but will try to connect to a server while TRUE
extern bool streaming;
extern bool song_timeout_running; //TRUE if automatic song updating is running

extern int stream_socket;
extern unsigned int bytes_sent;
extern unsigned int bytes_written;

extern sec_timer rec_timer;
extern sec_timer stream_timer;
extern sec_timer xrun_timer;

extern lame_enc lame_stream;
extern lame_enc lame_rec;
extern vorbis_enc vorbis_stream;
extern vorbis_enc vorbis_rec;
extern opus_enc opus_stream;
extern opus_enc opus_rec;

#endif
