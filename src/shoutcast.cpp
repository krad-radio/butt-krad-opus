// shoutcast functions for butt
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

#ifdef _WIN32
 #include <winsock2.h>
 #define usleep(us) Sleep(us/1000)
 #define close(s) closesocket(s)
#else
 #include <sys/types.h>
 #include <sys/socket.h>
 #include <unistd.h>
 #include <netinet/in.h> //defines IPPROTO_TCP on BSD
 #include <netdb.h>
#endif

#include <errno.h>

#include "cfg.h"
#include "butt.h"
#include "timer.h"
#include "strfuncs.h"
#include "shoutcast.h"
#include "parseconfig.h"
#include "sockfuncs.h"

#include "config.h"

int sc_connect()
{
    int ret;
    char recv_buf[100];
    char send_buf[100];

    stream_socket = sock_connect(cfg.srv[cfg.selected_srv]->addr,
            cfg.srv[cfg.selected_srv]->port+1, CONN_TIMEOUT);

    if(stream_socket < 0)
    {
        switch(stream_socket)
        {
            case SOCK_ERR_CREATE:
                print_info("\nconnect: could not create network socket", 1);
                ret = 2;
                break;
            case SOCK_ERR_RESOLVE:
                print_info("\nconnect: error resolving server address", 1);
                ret = 1;
                break;
            case SOCK_TIMEOUT:
            case SOCK_INVALID:
                ret = 1;
                break;
            default:
                ret = 2;
        }

        sc_disconnect();
        return ret;
    }

    /*
    ret = sock_setbufsize(&stream_socket, 8192, 0);
    if(ret == SOCK_ERR_SET_SBUF)
        print_info("\nWarning: couldn't set socket SO_SNDBUF", 1);
    */
    sock_send(&stream_socket, cfg.srv[cfg.selected_srv]->pwd,
            strlen(cfg.srv[cfg.selected_srv]->pwd), SEND_TIMEOUT);
    sock_send(&stream_socket, "\n", 1, SEND_TIMEOUT);


    if((ret = sock_recv(&stream_socket, recv_buf, sizeof(recv_buf)-1, RECV_TIMEOUT)) == 0)
    {
        usleep(100000);
        sc_disconnect();
        return 1;
    }

    if( (recv_buf[0] != 'O') || (recv_buf[1] != 'K') || (ret <= 2) )
    {
        if(strstr(recv_buf, "invalid password") != NULL)
        {
            print_info("\nconnect: invalid password!\n", 1);
            sc_disconnect();
            return 2;
        }
        return 1;
    }

    sock_send(&stream_socket, "icy-name:", 9, SEND_TIMEOUT);
    if(cfg.main.num_of_icy > 0)
        if(cfg.icy[cfg.selected_icy]->desc != NULL)
            send(stream_socket, cfg.icy[cfg.selected_icy]->desc,
                    strlen(cfg.icy[cfg.selected_icy]->desc), 0);

    sock_send(&stream_socket, "\n", 1, SEND_TIMEOUT);

    sock_send(&stream_socket, "icy-genre:", 10, SEND_TIMEOUT);
    if(cfg.main.num_of_icy > 0)
        if(cfg.icy[cfg.selected_icy]->genre != NULL)
            send(stream_socket, cfg.icy[cfg.selected_icy]->genre,
                   strlen(cfg.icy[cfg.selected_icy]->genre), 0);

    sock_send(&stream_socket, "\n", 1, SEND_TIMEOUT);

    sock_send(&stream_socket, "icy-url:", 8, SEND_TIMEOUT);
    if(cfg.main.num_of_icy > 0)
        if(cfg.icy[cfg.selected_icy]->url != NULL)
        send(stream_socket, cfg.icy[cfg.selected_icy]->url,
                strlen(cfg.icy[cfg.selected_icy]->url), 0);

    sock_send(&stream_socket, "\n", 1, SEND_TIMEOUT);

    sock_send(&stream_socket, "icy-irc:", 8, SEND_TIMEOUT);
    if(cfg.main.num_of_icy > 0)
        if(cfg.icy[cfg.selected_icy]->irc != NULL)
            send(stream_socket, cfg.icy[cfg.selected_icy]->irc,
                    strlen(cfg.icy[cfg.selected_icy]->irc), 0);

    sock_send(&stream_socket, "\n", 1, SEND_TIMEOUT);

    sock_send(&stream_socket, "icy-icq:", 8, SEND_TIMEOUT);
    if(cfg.main.num_of_icy > 0)
        if(cfg.icy[cfg.selected_icy]->icq != NULL)
            send(stream_socket, cfg.icy[cfg.selected_icy]->icq,
                    strlen(cfg.icy[cfg.selected_icy]->icq), 0);

    sock_send(&stream_socket, "\n", 1, SEND_TIMEOUT);

    sock_send(&stream_socket, "icy-aim:", 8, SEND_TIMEOUT);
    if(cfg.main.num_of_icy > 0)
        if(cfg.icy[cfg.selected_icy]->aim != NULL)
            send(stream_socket, cfg.icy[cfg.selected_icy]->aim,
                    strlen(cfg.icy[cfg.selected_icy]->aim), 0);

    sock_send(&stream_socket, "\n", 1, SEND_TIMEOUT);

    sock_send(&stream_socket, "icy-pub:", 8, SEND_TIMEOUT);
    if(cfg.main.num_of_icy > 0)
        if(cfg.icy[cfg.selected_icy]->pub != NULL)
            send(stream_socket, cfg.icy[cfg.selected_icy]->pub,
                    strlen(cfg.icy[cfg.selected_icy]->pub), 0);

    sock_send(&stream_socket, "\n", 1, SEND_TIMEOUT);

    sock_send(&stream_socket, "icy-br:", 7, SEND_TIMEOUT);
    snprintf(send_buf, 100, "%u", cfg.audio.bitrate);
    sock_send(&stream_socket, send_buf, strlen(send_buf), SEND_TIMEOUT);
    sock_send(&stream_socket, "\n", 1, SEND_TIMEOUT);

    sock_send(&stream_socket, "content-type:", 13, SEND_TIMEOUT);

    if(!strcmp(cfg.audio.codec, "mp3"))
    {
        strcpy(send_buf, "audio/mpeg");
        sock_send(&stream_socket, send_buf, strlen(send_buf), SEND_TIMEOUT);
    }

    sock_send(&stream_socket, "\n\n", 2, SEND_TIMEOUT);

    connected = 1;

    timer_init(&stream_timer, 1);       //starts the "online" timer

    return 0;
}

int sc_send(char *buf, int buf_len)
{
    int ret;
    ret = sock_send(&stream_socket, buf, buf_len, SEND_TIMEOUT);

    if(ret == SOCK_TIMEOUT)
        ret = -1;

    return ret;
}

int sc_update_song()
{
    int ret;
    int web_socket;
    char send_buf[1024];
    char *song_buf;

    web_socket = sock_connect(cfg.srv[cfg.selected_srv]->addr,
            cfg.srv[cfg.selected_srv]->port, CONN_TIMEOUT);

    if(web_socket < 0)
    {
        switch(web_socket)
        {
            case SOCK_ERR_CREATE:
                print_info("\nupdate_song: could not create network socket", 1);
                ret = 1;
                break;
            case SOCK_ERR_RESOLVE:
                print_info("\nupdate_song: error resolving server address", 1);
                ret = 1;
                break;
            case SOCK_TIMEOUT:
            case SOCK_INVALID:
                ret = 1;
                break;
            default:
                ret = 1;
        }

        return ret;
    }

    song_buf = strdup(cfg.main.song);

    strrpl(&song_buf, " ", "%20");
    strrpl(&song_buf, "&", "%26");

    snprintf(send_buf, 500, "GET /admin.cgi?pass=%s&mode=updinfo&song=%s&url= HTTP/1.0\n"
                      "User-Agent: ShoutcastDSP (Mozilla Compatible)\n\n",
                      cfg.srv[cfg.selected_srv]->pwd,
                      song_buf
                     );

    sock_send(&web_socket, send_buf, strlen(send_buf), SEND_TIMEOUT);
    close(web_socket);
    free(song_buf);

    return 0;
}

void sc_disconnect()
{
    close(stream_socket);

#ifdef _WIN32
    WSACleanup();
#endif

}

