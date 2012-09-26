// FLTK callback functions for butt
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

#ifndef FL_CALLBACKS_H
#define FL_CALLBACKS_H

enum { STREAM_TIME = 0, REC_TIME, SENT_DATA, REC_DATA };
enum { STREAM = 0, RECORD };

class flgui;

extern int display_info;
extern flgui *fl_g;

void button_cfg_cb();
void button_info_cb();
void button_record_cb();
void button_connect_cb();
void choice_cfg_dev_cb();
void button_disconnect_cb();
void button_add_icy_add_cb();
void button_cfg_del_srv_cb();
void button_cfg_del_icy_cb();
void choice_cfg_act_srv_cb();
void choice_cfg_act_icy_cb();
void button_cfg_add_srv_cb();
void button_cfg_add_icy_cb();
void choice_cfg_bitrate_cb();
void choice_cfg_samplerate_cb();
void button_cfg_song_go_cb();
void radio_cfg_codec_mp3_cb();
void radio_cfg_codec_ogg_cb();
void radio_cfg_codec_opus_cb();
void button_add_icy_save_cb();
void button_add_srv_cancel_cb();
void button_add_icy_cancel_cb();
void radio_cfg_channel_mono_cb();
void radio_cfg_channel_stereo_cb();
void button_cfg_browse_songfile_cb();
void input_cfg_song_file_cb();
void input_cfg_song_cb();

void button_add_srv_add_cb();
void button_add_srv_save_cb();

void button_rec_browse_cb();
void choice_rec_bitrate_cb();
void choice_rec_samplerate_cb();
void radio_rec_channel_stereo_cb();
void radio_rec_channel_mono_cb();
void radio_rec_codec_mp3_cb();
void radio_rec_codec_ogg_cb();
void radio_rec_codec_wav_cb();
void button_cfg_edit_srv_cb();
void button_cfg_edit_icy_cb();
void check_song_update_active_cb();

void input_rec_filename_cb();
void input_rec_folder_cb();

void check_gui_attach_cb();
void check_gui_ontop_cb();
void button_gui_bg_color_cb();
void button_gui_text_color_cb();

void check_cfg_rec_cb();
void check_cfg_connect_cb();

void ILM216_cb();

#endif

