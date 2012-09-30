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

#include <stdio.h>
#include <string.h>

#include "cfg.h"
#include "butt.h"

#include "config.h"

#ifdef _WIN32
 const char CONFIG_FILE[] = "opustransmitter";
#else
 const char CONFIG_FILE[] = ".opustransmitter";
#endif

config_t cfg;
char *cfg_path;
bool unsaved_changes;

int cfg_write_file()
{
    FILE *cfg_fd;
    int i;
    char info_buf[256];

    cfg_fd = fopen(cfg_path, "wb+");
    if(cfg_fd == NULL)
    {
        snprintf(info_buf, sizeof(info_buf), "Could not write to file: %s", cfg_path);
        print_info(cfg_path, 1);
        return 1;
    }

    fprintf(cfg_fd, "This is a configuration file for butt (broadcast using this tool)\n\n");
    fprintf(cfg_fd, "[main]\n");

#if HAVE_FLTK
    fprintf(cfg_fd, "bg_color = %d\n", cfg.main.bg_color);
    fprintf(cfg_fd, "txt_color = %d\n", cfg.main.txt_color);
#endif

    if(cfg.main.num_of_srv > 0)
        fprintf(cfg_fd, 
                "server = %s\n"
                "srv_ent = %s\n",
                cfg.main.srv,
                cfg.main.srv_ent
               );
    else
        fprintf(cfg_fd, 
                "server = \n"
                "srv_ent = \n"
               );

    if(cfg.main.num_of_icy > 0)
        fprintf(cfg_fd, 
                "icy = %s\n"
                "icy_ent = %s\n",
                cfg.main.icy,
                cfg.main.icy_ent
               );
    else
        fprintf(cfg_fd, 
                "icy = \n"
                "icy_ent = \n"
               );

    fprintf(cfg_fd,
            "num_of_srv = %d\n"
            "num_of_icy = %d\n",
            cfg.main.num_of_srv,
            cfg.main.num_of_icy
           );

    if(cfg.main.song_path != NULL)
        fprintf(cfg_fd, "song_path = %s\n", cfg.main.song_path);
    else
        fprintf(cfg_fd, "song_path = \n");

    fprintf(cfg_fd, "song_update = %d\n", cfg.main.song_update);

	fprintf(cfg_fd, "connect_at_startup = %d\n\n", cfg.main.connect_at_startup);


    fprintf(cfg_fd, 
            "[audio]\n"
            "device = %d\n"
            "samplerate = %d\n"
            "bitrate = %d\n"
            "channel = %d\n"
            "codec = %s\n\n",
            cfg.audio.dev_num,
            cfg.audio.samplerate,
            cfg.audio.bitrate,
            cfg.audio.channel,
            cfg.audio.codec
           );

    fprintf(cfg_fd, 
            "[record]\n"
            "samplerate = %d\n"
            "bitrate = %d\n"
            "channel = %d\n"
            "codec = %s\n"
            "start_rec = %d\n"
            "filename = %s\n"
            "folder = %s\n\n",
            cfg.rec.samplerate,
            cfg.rec.bitrate,
            cfg.rec.channel,
            cfg.rec.codec,
            cfg.rec.start_rec,
            cfg.rec.filename,
            cfg.rec.folder
           );

    fprintf(cfg_fd,
            "[gui]\n"
            "attach = %d\n"
            "ontop = %d\n\n",
            cfg.gui.attach,
            cfg.gui.ontop
           );

    for(i = 0; i < cfg.main.num_of_srv; i++)
    {
        fprintf(cfg_fd, 
                "[%s]\n"
                "address = %s\n"
                "port = %u\n"
                "password = %s\n"
                "type = %d\n",
                cfg.srv[i]->name,
                cfg.srv[i]->addr,
                cfg.srv[i]->port,
                cfg.srv[i]->pwd,
                cfg.srv[i]->type
               );

        if(cfg.srv[i]->type == ICECAST)
            fprintf(cfg_fd, "mount = %s\n\n", cfg.srv[i]->mount);
        else //Shoutcast has no mountpoint
            fprintf(cfg_fd, "mount =\n\n");
    }

    for(i = 0; i < cfg.main.num_of_icy; i++)
    {
        fprintf(cfg_fd,
                "[%s]\n"
                "pub = %s\n",
                cfg.icy[i]->name,
                cfg.icy[i]->pub
               );

        if(cfg.icy[i]->desc != NULL)
            fprintf(cfg_fd, "description = %s\n", cfg.icy[i]->desc);
        else
            fprintf(cfg_fd, "description = \n");

        if(cfg.icy[i]->genre != NULL)
            fprintf(cfg_fd, "genre = %s\n", cfg.icy[i]->genre);
        else
            fprintf(cfg_fd, "genre = \n");

        if(cfg.icy[i]->url != NULL)
            fprintf(cfg_fd, "url = %s\n", cfg.icy[i]->url);
        else
            fprintf(cfg_fd, "url = \n");

        if(cfg.icy[i]->irc != NULL)
            fprintf(cfg_fd, "irc = %s\n", cfg.icy[i]->irc);
        else
            fprintf(cfg_fd, "irc = \n");

        if(cfg.icy[i]->icq != NULL)
            fprintf(cfg_fd, "icq = %s\n", cfg.icy[i]->icq);
        else
            fprintf(cfg_fd, "icq = \n");

        if(cfg.icy[i]->aim != NULL)
            fprintf(cfg_fd, "aim = %s\n\n", cfg.icy[i]->aim);
        else
            fprintf(cfg_fd, "aim = \n\n");

    }

    fclose(cfg_fd);

    snprintf(info_buf, sizeof(info_buf), "config written to %s", cfg_path);
    print_info(info_buf, 0);

    unsaved_changes = 0;

    return 0;
}

int cfg_set_values()
{
    int i;
    char *srv_ent;
    char *icy_ent;
    char *strtok_buf;
    char info_buf[256];

    unsaved_changes = 0;

    if(cfg_parse_file(cfg_path) == -1)
    {
        snprintf(info_buf, sizeof(info_buf), "Missing config file %s", cfg_path);
        print_info(info_buf, 1);
        return 1;
    }

    cfg.audio.dev_num    = cfg_get_int("audio", "device");
    cfg.audio.samplerate = cfg_get_int("audio", "samplerate");
    cfg.audio.resolution = 16;
    cfg.audio.bitrate    = cfg_get_int("audio", "bitrate");
    cfg.audio.channel    = cfg_get_int("audio", "channel");
    cfg.audio.codec      = cfg_get_str("audio", "codec");
    cfg.audio.pcm_list   = snd_get_devices(&cfg.audio.dev_count);

    if(cfg.audio.samplerate == -1)
        cfg.audio.samplerate = 48000;

    if(cfg.audio.bitrate == -1)
        cfg.audio.bitrate = 128;

    if(cfg.audio.channel == -1)
        cfg.audio.channel = 2;

    if(cfg.audio.codec == NULL)
    {
        cfg.audio.codec = (char*)malloc(8*sizeof(char));


		strcpy(cfg.audio.codec, "opus");

    }
    
    //for config backward compability
    if(cfg.audio.dev_num >= cfg.audio.dev_count)
        cfg.audio.dev_num = 0;

    cfg.rec.bitrate    = cfg_get_int("record", "bitrate");
    cfg.rec.channel    = cfg_get_int("record", "channel");
    cfg.rec.samplerate = cfg_get_int("record", "samplerate");
    cfg.rec.start_rec  = cfg_get_int("record", "start_rec");
    cfg.rec.codec      = cfg_get_str("record", "codec");
    cfg.rec.filename   = cfg_get_str("record", "filename");
    cfg.rec.folder     = cfg_get_str("record", "folder");

    if(cfg.rec.bitrate == -1)
        cfg.rec.bitrate = 192;

    if(cfg.rec.channel == -1)
        cfg.rec.channel = 2;

    if(cfg.rec.samplerate == -1)
        cfg.rec.samplerate = 48000;

    if(cfg.rec.start_rec == -1)
        cfg.rec.start_rec = 1;

    if(cfg.rec.codec == NULL)
    {
        cfg.rec.codec = (char*)malloc(8*sizeof(char));

        strcpy(cfg.rec.codec, "opus");        

    }

    if(cfg.rec.filename == NULL)
    {
        cfg.rec.filename = (char*)malloc(strlen("rec_(%m_%d_%y)_%i.exxt")+1);

        strcpy(cfg.rec.filename, "rec_(%m_%d_%y)_%i.opus");

    }

    if(cfg.rec.folder == NULL)
    {
        cfg.rec.folder = (char*)malloc(6*sizeof(char));
        strcpy(cfg.rec.folder, "./");
    }


    cfg.main.num_of_srv = cfg_get_int("main", "num_of_srv");
    if(cfg.main.num_of_srv > 0)
    {
        cfg.main.srv     = cfg_get_str("main", "server");
        cfg.main.srv_ent = cfg_get_str("main", "srv_ent");

        cfg.srv = (server_t**)malloc(sizeof(server_t*) * cfg.main.num_of_srv);

        for(i = 0; i < cfg.main.num_of_srv; i++)
            cfg.srv[i] = (server_t*)malloc(sizeof(server_t));

        srv_ent = (char*)malloc((MAX_SECTION_LENGTH+1) * sizeof(char));
        strtok_buf = strdup(cfg.main.srv_ent);
        srv_ent = strtok(strtok_buf, ";");

        for(i = 0; srv_ent != NULL; i++)
        {
            cfg.srv[i]->name = (char*)malloc((MAX_SECTION_LENGTH+1) * sizeof(char));
            snprintf(cfg.srv[i]->name, MAX_SECTION_LENGTH, srv_ent, "");

            cfg.srv[i]->addr  = cfg_get_str(srv_ent, "address");
            cfg.srv[i]->port  = cfg_get_int(srv_ent, "port");
            cfg.srv[i]->pwd   = cfg_get_str(srv_ent, "password");
            cfg.srv[i]->type  = cfg_get_int(srv_ent, "type");
            cfg.srv[i]->mount = cfg_get_str(srv_ent, "mount");

            if(cfg.srv[i]->type == -1)
                cfg.srv[i]->type = SHOUTCAST;

            if(!strcmp(cfg.srv[i]->name, cfg.main.srv))
                cfg.selected_srv = i;

            srv_ent = strtok(NULL, ";");
        }
    }// if(cfg.main.num_of_srv > 0)

    cfg.main.num_of_icy = cfg_get_int("main", "num_of_icy");

    if(cfg.main.num_of_icy > 0)
    {
        cfg.main.icy     = cfg_get_str("main", "icy");
        cfg.main.icy_ent = cfg_get_str("main", "icy_ent");          //icy entries

        cfg.icy = (icy_t**)malloc(sizeof(icy_t*) * cfg.main.num_of_icy);

        for(i = 0; i < cfg.main.num_of_icy; i++)
            cfg.icy[i] = (icy_t*)malloc(sizeof(icy_t));

        icy_ent = (char*)malloc(MAX_SECTION_LENGTH * sizeof(char)+1);
        strtok_buf = strdup(cfg.main.icy_ent);
        icy_ent = strtok(strtok_buf, ";");

        for(i = 0; icy_ent != NULL; i++)
        {
            cfg.icy[i]->name = (char*)malloc(MAX_SECTION_LENGTH * sizeof(char)+1);
            snprintf(cfg.icy[i]->name, MAX_SECTION_LENGTH, icy_ent, "");

            cfg.icy[i]->desc  = cfg_get_str(icy_ent, "description");
            cfg.icy[i]->genre = cfg_get_str(icy_ent, "genre");
            cfg.icy[i]->url   = cfg_get_str(icy_ent, "url");
            cfg.icy[i]->irc   = cfg_get_str(icy_ent, "irc");
            cfg.icy[i]->icq   = cfg_get_str(icy_ent, "icq");
            cfg.icy[i]->aim   = cfg_get_str(icy_ent, "aim");
            cfg.icy[i]->pub   = cfg_get_str(icy_ent, "pub");

            if(!strcmp(cfg.icy[i]->name, icy_ent))
                cfg.selected_icy = i;

            icy_ent = strtok(NULL, ";");
        }
    }//if(cfg.main.num_of_icy > 0)

    cfg.main.song_path = cfg_get_str("main", "song_path");

    cfg.main.song_update = cfg_get_int("main", "song_update");
    if(cfg.main.song_update == -1)
        cfg.main.song_update = 0; //song update from file is default off

	cfg.main.connect_at_startup = cfg_get_int("main", "connect_at_startup");
	if(cfg.main.connect_at_startup == -1)
		cfg.main.connect_at_startup = 0;

    //read GUI stuff 
    cfg.gui.attach = cfg_get_int("gui", "attach");
    cfg.gui.ontop = cfg_get_int("gui", "ontop");

    if(cfg.gui.attach == -1)
        cfg.gui.attach = 0;

    if(cfg.gui.ontop == -1)
        cfg.gui.ontop = 0;

    //read FLTK related stuff
#if HAVE_FLTK
    cfg.main.bg_color = cfg_get_int("main", "bg_color");
    if(cfg.main.bg_color == -1)
        cfg.main.bg_color = 426056448;

    cfg.main.txt_color = cfg_get_int("main", "txt_color");
    if(cfg.main.txt_color == -1)
        cfg.main.txt_color = 67043328; //white
#endif

    return 0;
}

int cfg_create_default()
{
    char ext[8];
    char codec[8];
    FILE *cfg_fd;
    cfg_fd = fopen(cfg_path, "wb+");
    if(cfg_fd == NULL)
        return 1;

    strcpy(ext, "opus");
    strcpy(codec, "opus");


    fprintf(cfg_fd, "This is a configuration file for butt (broadcast using this tool)\n\n");
    fprintf(cfg_fd, 
            "[main]\n"
            "server =\n"
            "icy =\n"
            "num_of_srv = 0\n"
            "num_of_icy = 0\n"
            "srv_ent =\n"
            "icy_ent =\n"
            "song_path =\n"
            "song_update = 0\n\n"
           );

    fprintf(cfg_fd,
            "[audio]\n"
            "device = default\n"
            "samplerate = 48000\n"
            "bitrate = 128\n"
            "channel = 2\n"
            "codec = %s\n\n",
            codec
           );

    fprintf(cfg_fd,
            "[record]\n"
            "samplerate = 48000\n"
            "bitrate = 192\n"
            "channel = 2\n"
            "codec = %s\n"
            "start_rec = 0\n"
            "filename = rec_(%%m_%%d_%%y)_%%i.%s\n"
            "folder = ./\n\n",
            ext, ext
           );

    fprintf(cfg_fd, 
            "[gui]\n"
            "attach = 0\n"
            "ontop = 0\n\n"
            );

    fclose(cfg_fd);

    return 0;
}

