// portaudio functions for butt
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
#include <unistd.h>

#include <string.h>
#include <pthread.h>

#ifdef _WIN32
 #include <windows.h>
#endif

#include "butt.h"
#include "cfg.h"
#include "port_audio.h"
#include "parseconfig.h"
#include "lame_encode.h"
#include "shoutcast.h"
#include "icecast.h"
#include "strfuncs.h"
#include "wav_header.h"
#include "ringbuffer.h"

#include "config.h"

#define PA_FRAMES 2048

char* encode_buf;
short *pa_pcm_buf;
int buf_index;
int buf_size;
int buf_pos;
int framepacket_size;

bool try_to_connect;
bool pa_new_frames;
bool reconnect;

struct ringbuf rec_rb;
struct ringbuf stream_rb;

pthread_t rec_thread;
pthread_t stream_thread;
pthread_mutex_t stream_mut, rec_mut;
pthread_cond_t  stream_cond, rec_cond;

PaStream *stream;

int snd_init()
{
    char info_buf[256];

    PaError p_err;
    if((p_err = Pa_Initialize()) != paNoError)
    {
        snprintf(info_buf, sizeof(info_buf),
				"Sound init failed:\n%s\n",
				Pa_GetErrorText(p_err));

      //  ALERT(info_buf);
        return 1;
    }

    reconnect = 0;
    buf_index = 0;
    return 0;
}

void snd_reinit()
{
    snd_close();
    snd_init();
    snd_open_stream();
}

int snd_open_stream()
{
    int samplerate;
    char info_buf[256];

    PaDeviceIndex pa_dev_id;
    PaStreamParameters pa_params;
    PaError pa_err;
    const PaDeviceInfo *pa_dev_info;

    if(cfg.audio.dev_count == 0)
    {
        print_info("ERROR: no sound device with input channels found", 1);
        return 1;
    }

    buf_size = PA_FRAMES * 4;
    framepacket_size = PA_FRAMES * cfg.audio.channel;

    pa_pcm_buf = (short*)malloc(buf_size * sizeof(short));
    encode_buf = (char*)malloc(buf_size * sizeof(char));

    rb_init(&rec_rb, 16 * buf_size * sizeof(short));
    rb_init(&stream_rb, 16 * buf_size * sizeof(short));

    samplerate = cfg.audio.samplerate;

    pa_dev_id = cfg.audio.pcm_list[cfg.audio.dev_num]->dev_id;

    pa_dev_info = Pa_GetDeviceInfo(pa_dev_id);
    if(pa_dev_info == NULL)
    {
        snprintf(info_buf, 127, "Error getting device Info (%d)", pa_dev_id);
        print_info(info_buf, 1);
        return 1;
    }

    pa_params.device = pa_dev_id;
    pa_params.channelCount = cfg.audio.channel;
    pa_params.sampleFormat = paInt16;
    pa_params.suggestedLatency = pa_dev_info->defaultHighInputLatency;
    pa_params.hostApiSpecificStreamInfo = NULL;

    pa_err = Pa_IsFormatSupported(&pa_params, NULL, samplerate);
    if(pa_err != paFormatIsSupported)
    {
        if(pa_err == paInvalidSampleRate)
        {
            snprintf(info_buf, sizeof(info_buf),
                    "Samplerate not supported: %dHz\n"
                    "Using default samplerate: %dHz",
                    samplerate, (int)pa_dev_info->defaultSampleRate);
            print_info(info_buf, 1);

            if(Pa_IsFormatSupported(&pa_params, NULL,
               pa_dev_info->defaultSampleRate) != paFormatIsSupported)
            {
                print_info("FAILED", 1);
                return 1;
            }
            else
            {
                samplerate = (int)pa_dev_info->defaultSampleRate;
                cfg.audio.samplerate = samplerate;
                cfg.rec.samplerate = samplerate;
                update_samplerates();
            }
        }
        else
        {
            snprintf(info_buf, sizeof(info_buf), "PA: Format not supported: %s\n",
                    Pa_GetErrorText(pa_err));
            print_info(info_buf, 1);
            return 1;
        }
    }

    pa_err = Pa_OpenStream(&stream, &pa_params, NULL,
                            samplerate, PA_FRAMES,
                            paNoFlag, snd_callback, NULL);

    if(pa_err != paNoError)
    {
        printf("error opening sound device: \n%s\n", Pa_GetErrorText(pa_err));
        return 1;
    }

    
    Pa_StartStream(stream);
    return 0;
}

void snd_start_stream()
{   
    pthread_mutex_init(&stream_mut, NULL);
    pthread_cond_init (&stream_cond, NULL);
    
    bytes_sent = 0;
    streaming = 1;
    pthread_create(&stream_thread, NULL, snd_stream_thread, NULL);
}

void snd_stop_stream()
{
    connected = 0;
    streaming = 0;

    pthread_cond_signal(&stream_cond);

    pthread_mutex_destroy(&stream_mut);
    pthread_cond_destroy(&stream_cond);

    print_info("Disconnected\n", 0);
}

void *snd_stream_thread(void *data)
{
    int sent;
    int rb_read_bytes;
	int encode_bytes_read;
	int bytes_to_read;

	bytes_to_read = 960 * 2*cfg.audio.channel;

    char *enc_buf = (char*)malloc(stream_rb.size * sizeof(char)*10);
    char *audio_buf = (char*)malloc(stream_rb.size * sizeof(short));

    int (*xc_send)(char *buf, int buf_len) = NULL;

    encode_bytes_read = 0;

    if(cfg.srv[cfg.selected_srv]->type == SHOUTCAST)
            xc_send = &sc_send;
    if(cfg.srv[cfg.selected_srv]->type == ICECAST)
        xc_send = &ic_send;

    while(connected)
    {
        pthread_cond_wait(&stream_cond, &stream_mut);
        if(!connected)
            break;

		while ((rb_filled(&stream_rb)) >= bytes_to_read) {

	
			rb_read_len(&stream_rb, audio_buf, bytes_to_read);





		    if(!strcmp(cfg.audio.codec, "opus"))
		        encode_bytes_read = opus_enc_encode(&opus_stream, (short int*)audio_buf, 
		                enc_buf, bytes_to_read/(2*cfg.audio.channel));

		    if((sent = xc_send(enc_buf, encode_bytes_read)) == -1)
		        connected = 0; //disconnected
		    else
		        bytes_sent += encode_bytes_read;
		        
		        
		}
    }
    


    free(enc_buf);
    free(audio_buf);

    return NULL;
}

void snd_start_rec()
{
    pthread_mutex_init(&rec_mut, NULL);
    pthread_cond_init (&rec_cond, NULL);

    bytes_written = 0;
    recording = 1;
	opus_enc_reinit(&opus_rec);
    pthread_create(&rec_thread, NULL, snd_rec_thread, NULL);

    print_info("recording to:", 0);
    print_info(cfg.rec.path, 0);
}

void snd_stop_rec()
{
    record = 0;
    recording = 0;

    pthread_cond_signal(&rec_cond);

    pthread_mutex_destroy(&rec_mut);
    pthread_cond_destroy(&rec_cond);

    print_info("recording stopped", 0);
}

//The recording stuff runs in its own thread
//this prevents dropouts in the recording, in case the
//bandwidth is smaller than the selected streaming bitrate
void* snd_rec_thread(void *data)
{
    int rb_read_bytes;
    int ogg_header_written;
    char *enc_buf = (char*)malloc(rec_rb.size * sizeof(char)*10);
    char *audio_buf = (char*)malloc(rec_rb.size * sizeof(short));
	int encode_bytes_read;
	int bytes_to_read;

    encode_bytes_read = 0;
    ogg_header_written = 0;

	bytes_to_read = 960 * 2*cfg.audio.channel;

    while(record)
    {
        pthread_cond_wait(&rec_cond, &rec_mut);
        
        
        if(!ogg_header_written)
        {
            opus_enc_write_header(&opus_rec);
            ogg_header_written = 1;
        }
        
		while ((rb_filled(&rec_rb)) >= bytes_to_read) {

	
			rb_read_len(&rec_rb, audio_buf, bytes_to_read);

		    encode_bytes_read = opus_enc_encode(&opus_rec, (short int*)audio_buf, 
		                enc_buf, bytes_to_read/(2*cfg.audio.channel));

		  bytes_written += fwrite(enc_buf, 1, encode_bytes_read, cfg.rec.fd);
		        
		        
		}

    }

    fclose(cfg.rec.fd);
    free(enc_buf);
    free(audio_buf);
    return NULL;
}

//this function is called by PortAudio when new audio data arrived
int snd_callback(const void *input,
                 void *output,
                 unsigned long frameCount,
                 const PaStreamCallbackTimeInfo* timeInfo,
                 PaStreamCallbackFlags statusFlags,
                 void *userData)
{
    memcpy(pa_pcm_buf, input, framepacket_size*sizeof(short));
	
    //tell vu_update() that there is new audio data
    pa_new_frames = 1;

	if(streaming)
	{
		rb_write(&stream_rb, (char*)input, framepacket_size*sizeof(short));
		pthread_cond_signal(&stream_cond);
	}
	if(recording)
	{
		rb_write(&rec_rb, (char*)input, framepacket_size*sizeof(short));
		pthread_cond_signal(&rec_cond);
	}

    return 0;
}

void snd_update_vu()
{
    int i;
    int lpeak = 0;
    int rpeak = 0;
    short *p;

    p = pa_pcm_buf;
    for(i = 0; i < framepacket_size; i += cfg.audio.channel)
    {
        if(p[i] > lpeak)
            lpeak = p[i];
        if(p[i+(cfg.audio.channel-1)] > rpeak)
            rpeak = p[i+(cfg.audio.channel-1)];
    }

    vu_meter(lpeak, rpeak);

    pa_new_frames = 0;
}

snd_dev_t **snd_get_devices(int *dev_count)
{
	int i, j;
    int devcount, sr_count, dev_num;
    bool sr_supported = 0;
    const PaDeviceInfo *p_di;
    char info_buf[256];
    PaStreamParameters pa_params;

    int sr[] = { 8000, 11025, 12000, 16000, 22050, 24000, 32000, 44100, 48000 };

    snd_dev_t **dev_list;

	dev_num = 0;

    dev_list = (snd_dev_t**)malloc(100*sizeof(snd_dev_t*));

	//100 sound devices should be enough
    for(i = 0; i < 100; i++)
        dev_list[i] = (snd_dev_t*)malloc(sizeof(snd_dev_t));

    dev_list[dev_num]->name = (char*) malloc(strlen("Default PCM device (default)")+1);
    strcpy(dev_list[dev_num]->name, "Default PCM device (default)");
    dev_list[dev_num]->dev_id = Pa_GetDefaultInputDevice();
    dev_num++;


    devcount = Pa_GetDeviceCount();
    if(devcount < 0)
    {
        snprintf(info_buf, sizeof(info_buf), "PaError: %s", Pa_GetErrorText(devcount));
        print_info(info_buf, 1);
    }

    for(i = 0; i < devcount && i < 100; i++)
    {
        sr_count = 0;
        p_di = Pa_GetDeviceInfo(i);
        if(p_di == NULL)
        {
            snprintf(info_buf, sizeof(info_buf), "Error getting device Info (%d)", i);
            print_info(info_buf, 1);
            continue;
        }


        //Save only devices which have input Channels
        if(p_di->maxInputChannels <= 0)
            continue;

        pa_params.device = i;
        pa_params.channelCount = cfg.audio.channel;
        pa_params.sampleFormat = paInt16;
        pa_params.suggestedLatency = p_di->defaultHighInputLatency;
        pa_params.hostApiSpecificStreamInfo = NULL;

        //add the supported samplerates to the device structure
        for(j = 0; j < 9; j++)
        {
            if(Pa_IsFormatSupported(&pa_params, NULL, sr[j]) != paInvalidSampleRate)
            {
                dev_list[dev_num]->sr_list[sr_count] = sr[j];
                sr_count++;
                sr_supported = 1;
            }
        }
        //Go to the next device if this one doesn't support one of our samplerates
        if(!sr_supported)
            continue;

        //Mark the end of the samplerate list for this device with a 0
        dev_list[dev_num]->sr_list[sr_count] = 0;

        dev_list[dev_num]->name = (char*) malloc(strlen(p_di->name)+1);
        strcpy(dev_list[dev_num]->name, p_di->name);
        dev_list[dev_num]->dev_id = i;

        //copy the sr_list from the device where the
        //virtual default device points to
        if(dev_list[0]->dev_id == dev_list[dev_num]->dev_id)
            memcpy(dev_list[0]->sr_list, dev_list[dev_num]->sr_list,
                    sizeof(dev_list[dev_num]->sr_list));


        //We need to escape every '/' in the device name
        //otherwise FLTK will add a submenu for every '/' in the dev list
        strrpl(&dev_list[dev_num]->name, "/", "\\/");

        dev_num++;
    }//for(i = 0; i < devcount && i < 100; i++)


    if(dev_num == 1)
        *dev_count = 0;
    else
        *dev_count = dev_num;

    return dev_list;
}

void snd_close()
{
    Pa_StopStream(stream);
    Pa_CloseStream(stream);
    Pa_Terminate();

    free(pa_pcm_buf);
    free(encode_buf);
}

