// FLTK GUI related functions
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
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sys/stat.h>

#ifndef _WIN32
 #include <sys/wait.h>
#endif
#include "cfg.h"
#include "butt.h"
#include "util.h"
#include "port_audio.h"
#include "timer.h"
#include "flgui.h"
#include "fl_funcs.h"
#include "shoutcast.h"
#include "icecast.h"


void fill_cfg_widgets()
{
    int i;
    static bool sr_updated = 0;

    int bitrate[] = { 32, 40, 48, 56, 64, 80, 96, 112,
                      128, 160, 192, 224, 256, 320, 0 };


    fl_g->radio_cfg_codec_ogg->deactivate();
    fl_g->radio_rec_codec_ogg->deactivate();

    fl_g->radio_cfg_codec_mp3->deactivate();
    fl_g->radio_rec_codec_mp3->deactivate();

    //fill the main section
    for(i = 0; i < cfg.audio.dev_count; i++)
        fl_g->choice_cfg_dev->add(cfg.audio.pcm_list[i]->name);

    fl_g->choice_cfg_dev->value(cfg.audio.dev_num);

    for(i = 0; i < cfg.main.num_of_srv; i++)
        fl_g->choice_cfg_act_srv->add(cfg.srv[i]->name);

    if(cfg.main.num_of_srv > 0)
    {
        fl_g->button_cfg_edit_srv->activate();
        fl_g->button_cfg_del_srv->activate();
    }
    else
    {
        fl_g->button_cfg_edit_srv->deactivate();
        fl_g->button_cfg_del_srv->deactivate();
    }

    for(i = 0; i < cfg.main.num_of_icy; i++)
        fl_g->choice_cfg_act_icy->add(cfg.icy[i]->name);

    if(cfg.main.num_of_icy > 0)
    {
        fl_g->button_cfg_edit_icy->activate();
        fl_g->button_cfg_del_icy->activate();
    }
    else
    {
        fl_g->button_cfg_edit_icy->deactivate();
        fl_g->button_cfg_del_icy->deactivate();
    }

    fl_g->choice_cfg_act_srv->value(cfg.selected_srv);
    fl_g->choice_cfg_act_icy->value(cfg.selected_icy);


	fl_g->check_cfg_connect->value(cfg.main.connect_at_startup);


    //fill the audio (stream) section
    if(!strcmp(cfg.audio.codec, "mp3"))
        fl_g->radio_cfg_codec_mp3->setonly();
    else if(!strcmp(cfg.audio.codec, "ogg"))
        fl_g->radio_cfg_codec_ogg->setonly();

    if(cfg.audio.channel == 1)
        fl_g->radio_cfg_channel_mono->setonly();
    else
        fl_g->radio_cfg_channel_stereo->setonly();

    for(i = 0; bitrate[i] != 0; i++)
        if(cfg.audio.bitrate == bitrate[i])
            fl_g->choice_cfg_bitrate->value(i);

    if(cfg.main.song_update)
        fl_g->check_song_update_active->value(1);

    fl_g->input_cfg_song_file->value(cfg.main.song_path);

    //fill the record section
    fl_g->input_rec_filename->value(cfg.rec.filename);
    fl_g->input_rec_folder->value(cfg.rec.folder);

    if(!strcmp(cfg.rec.codec, "mp3"))
        fl_g->radio_rec_codec_mp3->setonly();
    else if(!strcmp(cfg.rec.codec, "ogg"))
           fl_g->radio_rec_codec_ogg->setonly();
    else //wav
    {
        fl_g->radio_rec_codec_wav->setonly();
        fl_g->choice_rec_bitrate->deactivate();
    }

    if(cfg.rec.channel == 1)
        fl_g->radio_rec_channel_mono->setonly();
    else
        fl_g->radio_rec_channel_stereo->setonly();

    for(i = 0; bitrate[i] != 0; i++)
        if(cfg.rec.bitrate == bitrate[i])
            fl_g->choice_rec_bitrate->value(i);

    if(cfg.rec.start_rec)
        fl_g->check_cfg_rec->value(1);
    else
        fl_g->check_cfg_rec->value(0);

    if(!sr_updated)
    {
        update_samplerates();
        sr_updated = 1;
    }

    //fill the GUI section
    fl_g->button_gui_bg_color->color(cfg.main.bg_color,
            fl_lighter((Fl_Color)cfg.main.bg_color));
    fl_g->button_gui_text_color->color(cfg.main.txt_color,
            fl_lighter((Fl_Color)cfg.main.txt_color));
    fl_g->check_gui_attach->value(cfg.gui.attach);
    fl_g->check_gui_ontop->value(cfg.gui.ontop);
    if(cfg.gui.ontop)
    {
        fl_g->window_main->stay_on_top(1);
        fl_g->window_cfg->stay_on_top(1);
    }


}

//Updates the samplerate drop down menu for the audio
//device the user has selected
void update_samplerates()
{
    int i;
    int *sr_list;
    char sr_asc[10];

    fl_g->choice_cfg_samplerate->clear();
    fl_g->choice_rec_samplerate->clear();

    sr_list = cfg.audio.pcm_list[cfg.audio.dev_num]->sr_list;

    for(i = 0; sr_list[i] != 0; i++)
    {
        snprintf(sr_asc, 10, "%dHz", sr_list[i]);
        fl_g->choice_cfg_samplerate->add(sr_asc);
        fl_g->choice_rec_samplerate->add(sr_asc);

        if(cfg.audio.samplerate == sr_list[i])
            fl_g->choice_cfg_samplerate->value(i);

        if(cfg.rec.samplerate == sr_list[i])
            fl_g->choice_rec_samplerate->value(i);
    }
    if(i == 0)
    {
        fl_g->choice_cfg_samplerate->add("dev. not supported");
        fl_g->choice_rec_samplerate->add("dev. not supported");
        fl_g->choice_cfg_samplerate->value(0);
        fl_g->choice_rec_samplerate->value(0);
    }
}

void print_info(const char* info, int info_type)
{
    char timebuf[10];
    time_t test;

    struct tm  *mytime;
    static struct tm time_bak;

    test = time(NULL);
    mytime = localtime(&test);

    if( (time_bak.tm_min != mytime->tm_min) || (time_bak.tm_hour != mytime->tm_hour) )
    {
        time_bak.tm_min = mytime->tm_min;
        time_bak.tm_hour = mytime->tm_hour;
        strftime(timebuf, sizeof(timebuf), "\n%H:%M:", mytime);
        fl_g->info_buffer->append(timebuf);
    }

    fl_g->info_buffer->append((const char*)"\n");
    fl_g->info_buffer->append((const char*)info);

    //always scroll to the last line
    fl_g->info_output->scroll(fl_g->info_buffer->count_lines(0,     //count the lines from char 0 to the last character
                            fl_g->info_buffer->length()),           //returns the number of characters in the buffer
                            0);
}

void print_lcd(const char *text, int len, int home, int clear)
{
    if(clear)
        fl_g->lcd->clear();

    fl_g->lcd->print((const uchar*)text, len);

    if(home)
        fl_g->lcd->cursor_pos(0);
}

void check_frames(void*)
{
    if(pa_new_frames)
        snd_update_vu();

    Fl::repeat_timeout(0.01, &check_frames);
}

void check_time(void*)
{
    char lcd_text_buf[33];

    if(display_info == SENT_DATA)
    {
        sprintf(lcd_text_buf, "info: on air\nsent: %dkb",
                bytes_sent / 1024);
        print_lcd(lcd_text_buf, strlen(lcd_text_buf), 0, 1);
    }

    if(display_info == STREAM_TIME && timer_is_elapsed(&stream_timer))
    {
        sprintf(lcd_text_buf, "info: on air\ntime: %s",
                timer_get_time_str(&stream_timer));
        print_lcd(lcd_text_buf, strlen(lcd_text_buf), 0, 1);
    }

    if(display_info == REC_TIME && timer_is_elapsed(&rec_timer))
    {
        sprintf(lcd_text_buf, "info: record\ntime: %s",
                timer_get_time_str(&rec_timer));
        print_lcd(lcd_text_buf, strlen(lcd_text_buf), 0, 1);
    }

    if(display_info == REC_DATA)
    {
        sprintf(lcd_text_buf, "info: record\nsize: %dkb",
                bytes_written / 1024);
        print_lcd(lcd_text_buf, strlen(lcd_text_buf), 0, 1);
    }

    Fl::repeat_timeout(0.1, &check_time);
}

void check_if_disconnected(void*)
{
    if(!connected)
    {
        print_info("ERROR: connection lost\nreconnecting...", 1);
        if(cfg.srv[cfg.selected_srv]->type == SHOUTCAST)
            sc_disconnect();
        else
            ic_disconnect();

        Fl::remove_timeout(&check_time);
        Fl::remove_timeout(&check_if_disconnected);

        //reconnect
        button_connect_cb();

        return;
    }

    Fl::repeat_timeout(0.5, &check_if_disconnected);
}

void check_cfg_win_pos(void*)
{

#ifdef _WIN32
    fl_g->window_cfg->position(fl_g->window_main->x() +
                                fl_g->window_main->w()+7,
                                fl_g->window_main->y());
#else //UNIX
    fl_g->window_cfg->position(fl_g->window_main->x() +
                                fl_g->window_main->w(),
                                fl_g->window_main->y());
#endif

    Fl::repeat_timeout(0.1, &check_cfg_win_pos);
}
void check_song_update(void*)
{
    int len;
    char song[501];
    struct stat s;
    static time_t old_t;


    if(cfg.main.song_path == NULL)
        goto exit;

    if(stat(cfg.main.song_path, &s) != 0)
    {
       fl_alert("could not stat:\n%s\nplease check permissions", cfg.main.song_path);
       fl_g->check_song_update_active->value(0);
       fl_g->check_song_update_active->redraw();
       song_timeout_running = 0;
       return;
    }

    if(old_t == s.st_mtime) //file hasn't changed
        goto exit;

    old_t = s.st_mtime;

   if((cfg.main.song_fd = fopen(cfg.main.song_path, "rb")) == NULL)
   {
       fl_alert("could not open:\n%s\nplease check permissions", cfg.main.song_path);
       fl_g->check_song_update_active->value(0);
       fl_g->check_song_update_active->redraw();
       song_timeout_running = 0;
       return;
   }

   if(fgets(song, 500, cfg.main.song_fd) != NULL)
   {
       len = strlen(song);
       //remove newline character
       if(song[len-1] == '\n' || song[len-1] == '\r')
           song[len-1] = '\0';

       cfg.main.song = (char*) realloc(cfg.main.song, strlen(song) +1);
       strcpy(cfg.main.song, song);
       button_cfg_song_go_cb();
   }

   fclose(cfg.main.song_fd);

exit:
    Fl::repeat_timeout(1.0, &check_song_update);
}

void test_file_extension()
{
    char *ext;

    ext = util_get_file_extension(cfg.rec.filename);
    if(ext == NULL)
	{
        fl_alert("Warning:\nrecord filename hasn't got an extension");
		return;
	}


    if(strcmp(ext, cfg.rec.codec))
        fl_alert("Warning:\nthe extension (%s) of your record file\n"
                "doesn't match your record codec (%s)", ext, cfg.rec.codec);
}

void vu_meter(short left, short right)
{
    int i;
    static int left_delay[9] = { DELAY };
    static int right_delay[9] = { DELAY };
    static int left_state[9] = {OFF};
    static int right_state[9] = {OFF};

    if(left < 0)
        left =-left;
    if(right < 0)
        right =-right;


    if(left > TRESHOLD_1 && left_state[0] == OFF)
    {
        fl_g->left_1_light->show();
        left_state[0] = ON;
    }

    if(left < TRESHOLD_9 && left_state[8] == ON)
    {
        if(!left_delay[8])
        {
            fl_g->left_9_light->hide();
            left_state[8] = OFF;

            for(i = 0; i < 8; i++)
                left_delay[i] = DELAY;
        }
        else
            left_delay[8]--;
    }

    if(left > TRESHOLD_2 && left_state[1] == OFF)
    {
        fl_g->left_2_light->show();
        left_state[1] = ON;
    }

    if(left < TRESHOLD_8 && left_state[7] == ON)
    {
        if(!left_delay[7])
        {
            fl_g->left_8_light->hide();
            left_state[7] = OFF;

            for(i = 0; i < 7; i++)
                left_delay[i] = DELAY;
        }
        else
            left_delay[7]--;
    }

    if(left > TRESHOLD_3 && left_state[2] == OFF)
    {
        fl_g->left_3_light->show();
        left_state[2] = ON;
    }

    if(left < TRESHOLD_7 && left_state[6] == ON)
    {
        if(!left_delay[6])
        {
            fl_g->left_7_light->hide();
            left_state[6] = OFF;

            for(i = 0; i < 6; i++)
                left_delay[i] = DELAY;
        }
        else
            left_delay[6]--;

    }

    if(left > TRESHOLD_4 && left_state[3] == OFF)
    {
        fl_g->left_4_light->show();
        left_state[3] = ON;
    }

    if(left < TRESHOLD_6 && left_state[5] == ON)
    {
        if(!left_delay[5])
        {
            fl_g->left_6_light->hide();
            left_state[5] = OFF;

            for(i = 0; i < 5; i++)
                left_delay[i] = DELAY;
        }
        else
            left_delay[5]--;

    }

    if(left > TRESHOLD_5 && left_state[4] == OFF)
    {
        fl_g->left_5_light->show();
        left_state[4] = ON;
    }

    if(left < TRESHOLD_5 && left_state[4] == ON)
    {
        if(!left_delay[4])
        {
            fl_g->left_5_light->hide();
            left_state[4] = OFF;

            for(i = 0; i < 4; i++)
                left_delay[i] = DELAY;
        }
        else
            left_delay[4]--;

    }

    if(left > TRESHOLD_6 && left_state[5] == OFF)
    {
        fl_g->left_6_light->show();
        left_state[5] = ON;
    }

    if(left < TRESHOLD_4 && left_state[3] == ON)
    {
        if(!left_delay[3])
        {
            fl_g->left_4_light->hide();
            left_state[3] = OFF;

            for(i = 0; i < 3; i++)
                left_delay[i] = DELAY;
        }
        else
            left_delay[3]--;

    }

    if(left > TRESHOLD_7 && left_state[6] == OFF)
    {
        fl_g->left_7_light->show();
        left_state[6] = ON;
    }

    if(left < TRESHOLD_3 && left_state[2] == ON)
    {
        if(!left_delay[2])
        {
            fl_g->left_3_light->hide();
            left_state[2] = OFF;

            for(i = 0; i < 2; i++)
                left_delay[i] = DELAY;
        }
        else
            left_delay[2]--;

    }

    if(left > TRESHOLD_8 && left_state[7] == OFF)
    {
        fl_g->left_8_light->show();
        left_state[7] = ON;
    }

    if(left < TRESHOLD_2 && left_state[1] == ON)
    {
        if(!left_delay[1])
        {
            fl_g->left_2_light->hide();
            left_state[1] = OFF;

            for(i = 0; i < 1; i++)
                left_delay[i] = DELAY;
        }
        else
            left_delay[1]--;

    }

    if(left > TRESHOLD_9 && left_state[8] == OFF)
    {
        fl_g->left_9_light->show();
        left_state[8] = ON;
    }

    if(left < TRESHOLD_1 && left_state[0] == ON)
    {
        if(!left_delay[0])
        {
            fl_g->left_1_light->hide();
            left_state[0] = OFF;

            left_delay[0] = DELAY;
        }
        else
            left_delay[0]--;

    }


    if(right > TRESHOLD_1 && right_state[0] == OFF)
    {
        fl_g->right_1_light->show();
        right_state[0] = ON;
    }

    if(right < TRESHOLD_9 && right_state[8] == ON)
    {
        if(!right_delay[8])
        {
            fl_g->right_9_light->hide();
            right_state[8] = OFF;

            for(i = 0; i < 8; i++)
                right_delay[i] = DELAY;
        }
        else
            right_delay[8]--;
    }

    if(right > TRESHOLD_2 && right_state[1] == OFF)
    {
        fl_g->right_2_light->show();
        right_state[1] = ON;
    }

    if(right < TRESHOLD_8 && right_state[7] == ON)
    {
        if(!right_delay[7])
        {
            fl_g->right_8_light->hide();
            right_state[7] = OFF;

            for(i = 0; i < 7; i++)
                right_delay[i] = DELAY;
        }
        else
            right_delay[7]--;
    }

    if(right > TRESHOLD_3 && right_state[2] == OFF)
    {
        fl_g->right_3_light->show();
        right_state[2] = ON;
    }

    if(right < TRESHOLD_7 && right_state[6] == ON)
    {
        if(!right_delay[6])
        {
            fl_g->right_7_light->hide();
            right_state[6] = OFF;

            for(i = 0; i < 6; i++)
                right_delay[i] = DELAY;
        }
        else
            right_delay[6]--;

    }

    if(right > TRESHOLD_4 && right_state[3] == OFF)
    {
        fl_g->right_4_light->show();
        right_state[3] = ON;
    }

    if(right < TRESHOLD_6 && right_state[5] == ON)
    {
        if(!right_delay[5])
        {
            fl_g->right_6_light->hide();
            right_state[5] = OFF;

            for(i = 0; i < 5; i++)
                right_delay[i] = DELAY;
        }
        else
            right_delay[5]--;

    }

    if(right > TRESHOLD_5 && right_state[4] == OFF)
    {
        fl_g->right_5_light->show();
        right_state[4] = ON;
    }

    if(right < TRESHOLD_5 && right_state[4] == ON)
    {
        if(!right_delay[4])
        {
            fl_g->right_5_light->hide();
            right_state[4] = OFF;

            for(i = 0; i < 4; i++)
                right_delay[i] = DELAY;
        }
        else
            right_delay[4]--;

    }

    if(right > TRESHOLD_6 && right_state[5] == OFF)
    {
        fl_g->right_6_light->show();
        right_state[5] = ON;
    }

    if(right < TRESHOLD_4 && right_state[3] == ON)
    {
        if(!right_delay[3])
        {
            fl_g->right_4_light->hide();
            right_state[3] = OFF;

            for(i = 0; i < 3; i++)
                right_delay[i] = DELAY;
        }
        else
            right_delay[3]--;

    }

    if(right > TRESHOLD_7 && right_state[6] == OFF)
    {
        fl_g->right_7_light->show();
        right_state[6] = ON;
    }

    if(right < TRESHOLD_3 && right_state[2] == ON)
    {
        if(!right_delay[2])
        {
            fl_g->right_3_light->hide();
            right_state[2] = OFF;

            for(i = 0; i < 2; i++)
                right_delay[i] = DELAY;
        }
        else
            right_delay[2]--;

    }

    if(right > TRESHOLD_8 && right_state[7] == OFF)
    {
        fl_g->right_8_light->show();
        right_state[7] = ON;
    }

    if(right < TRESHOLD_2 && right_state[1] == ON)
    {
        if(!right_delay[1])
        {
            fl_g->right_2_light->hide();
            right_state[1] = OFF;

            for(i = 0; i < 1; i++)
                right_delay[i] = DELAY;
        }
        else
            right_delay[1]--;

    }

    if(right > TRESHOLD_9 && right_state[8] == OFF)
    {
        fl_g->right_9_light->show();
        right_state[8] = ON;
    }

    if(right < TRESHOLD_1 && right_state[0] == ON)
    {
        if(!right_delay[0])
        {
            fl_g->right_1_light->hide();
            right_state[0] = OFF;

            right_delay[0] = DELAY;
        }
        else
            right_delay[0]--;

    }


    //Fl::check();
}

