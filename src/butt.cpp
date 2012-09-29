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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <lame/lame.h>
#include <signal.h>
#include <limits.h>
#include <pthread.h>

#ifdef _WIN32
 #include <time.h>
#endif

#include "cfg.h"
#include "butt.h"
#include "port_audio.h"
#include "lame_encode.h"
#include "shoutcast.h"
#include "parseconfig.h"

#include "config.h"

bool record;
bool recording;
bool connected;
bool streaming;
bool disconnect;
bool try_connect;
bool song_timeout_running;

int stream_socket;
unsigned int bytes_sent;
unsigned int bytes_written;

sec_timer rec_timer;
sec_timer stream_timer;
sec_timer xrun_timer;

lame_enc lame_stream;
lame_enc lame_rec;
vorbis_enc vorbis_stream;
vorbis_enc vorbis_rec;
opus_enc opus_stream;

int main()
{

    char *p;
    char lcd_buf[33];
    char info_buf[256];

#ifndef _WIN32
    signal(SIGPIPE, SIG_IGN); //ignore the SIGPIPE signal.
        //(in case the server closes the connection unexpected)
#endif

    SHOW_GUI();

    snprintf(info_buf, sizeof(info_buf),
            "starting %s\nwritten by Daniel Noethen\n", VERSION);
    print_info(info_buf, 0);

#ifdef _WIN32
    //If there is no "%APPDATA% we are probably in none-NT Windows
    //So we save the config file to the programm directory
    if((p = getenv("APPDATA")) == NULL)
    {
        cfg_path = (char*)malloc(strlen(CONFIG_FILE)+1);
        strcpy(cfg_path, CONFIG_FILE);
    }
    else
    {
        cfg_path = (char*)malloc(PATH_MAX+strlen(CONFIG_FILE)+1);
        snprintf(cfg_path, PATH_MAX+strlen(CONFIG_FILE), "%s\\%s", p, CONFIG_FILE);  
    }
#else //UNIX
    if((p = getenv("HOME")) == NULL)
    {
        ALERT("No home-directory found");
        return 1;
    }
    cfg_path = (char*)malloc(PATH_MAX+strlen(CONFIG_FILE)+1);
    snprintf(cfg_path, PATH_MAX+strlen(CONFIG_FILE), "%s/%s", p, CONFIG_FILE);  

#endif

    if(!snd_init())
        print_info("PortAudio init succeeded", 0);
    else
    {
        ALERT("PortAudio init failed");
        return 1;
    }

    if(cfg_set_values())        //read configfile and set the config struct
    {
        if(cfg_create_default())
        {
            ALERT("Could not create config file");
            return 1;
        }
        sprintf(info_buf, "butt created a default config file:\n(%s)\n",
                cfg_path );

        print_info(info_buf, 0);
        cfg_set_values();
    }

#ifdef HAVE_LIBLAME
    lame_stream.channel = cfg.audio.channel;
    lame_stream.bitrate = cfg.audio.bitrate;
    lame_stream.samplerate_in = cfg.audio.samplerate;
    lame_stream.samplerate_out = cfg.audio.samplerate;
    lame_enc_init(&lame_stream);

    lame_rec.channel = cfg.rec.channel;
    lame_rec.bitrate = cfg.rec.bitrate;
    lame_rec.samplerate_in = cfg.audio.samplerate;
    lame_rec.samplerate_out = cfg.rec.samplerate;
    lame_enc_init(&lame_rec);
#endif
#ifdef HAVE_LIBVORBIS
    vorbis_stream.channel = cfg.audio.channel;
    vorbis_stream.bitrate = cfg.audio.bitrate;
    vorbis_stream.samplerate = cfg.audio.samplerate;
    vorbis_enc_init(&vorbis_stream);

    vorbis_rec.channel = cfg.rec.channel;
    vorbis_rec.bitrate = cfg.rec.bitrate;
    vorbis_rec.samplerate = cfg.rec.samplerate;
    vorbis_enc_init(&vorbis_rec);
#endif
#ifdef HAVE_LIBOPUS
    opus_stream.channel = cfg.audio.channel;
    opus_stream.bitrate = cfg.audio.bitrate;
    opus_stream.samplerate = cfg.audio.samplerate;
    opus_enc_init(&opus_stream);

    // add recording later
#endif

    print_info("=========================\n", 0);

    snd_open_stream();

    strcpy(lcd_buf, "info: idle");
    PRINT_LCD(lcd_buf, strlen(lcd_buf), 0, 1);

    if(cfg.gui.ontop)
        fl_g->window_main->stay_on_top(1);

	if(cfg.main.connect_at_startup)
		button_connect_cb();

    GUI_LOOP();

    return 0;
}
