// config functions for butt
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

#ifndef CFG_H
#define CFG_H

#include "port_audio.h"
#include "parseconfig.h"

enum {

    SHOUTCAST = 0,
    ICECAST = 1
};

extern const char CONFIG_FILE[];
typedef struct
{
    char *name;
    char *addr;
    char *pwd;
    char *mount;        //mountpoint for icecast server
    unsigned int port;
    int type;           //SHOUTCAST or ICECAST

}server_t;


typedef struct
{
        char *name;
        char *desc; //description
        char *genre;
        char *url;
        char *irc;
        char *icq;
        char *aim;
        char *pub;

}icy_t;


typedef struct
{
    int selected_srv;
    int selected_icy;

    struct
    {
        char *srv;
        char *icy;
        char *srv_ent;
        char *icy_ent;
        char *song;
        char *song_path;
        FILE *song_fd;
        int song_update;   //1 = song info will be read from file
        int num_of_srv;
        int num_of_icy;
        int bg_color, txt_color;
		int connect_at_startup;

    }main;

    struct
    {
        int dev_count;
        int dev_num;
        snd_dev_t **pcm_list;
        int samplerate;
        int resolution;
        int channel;
        int bitrate;
        char *codec;

    }audio;

    struct
    {
        int channel;
        int bitrate;
        int quality;
        int samplerate;
        char *codec;
        char *filename;
        char *folder;
        char *path;
        FILE *fd;
        int start_rec;

    }rec;

    struct
    {
        int attach;
        int ontop;
    }gui;

    server_t **srv;
    icy_t **icy;

}config_t;


extern char *cfg_path;      //Path to config file
extern config_t cfg;        //Holds config parameters
extern bool unsaved_changes;//is checked when closing butt and informs the user for unsaved changes

int cfg_write_file();       //Writes current config_t struct to cfg_path
int cfg_set_values();       //Reads config file and fills the config_t struct
int cfg_create_default();   //Creates a default config file, if there isn't one yet

#endif

