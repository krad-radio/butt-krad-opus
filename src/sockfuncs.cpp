// socket functions for butt
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
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#ifdef _WIN32
 #include <winsock2.h>
 #define errno WSAGetLastError()
 #define EWOULDBLOCK WSAEWOULDBLOCK
#else
 #include <sys/types.h>
 #include <sys/socket.h>
 #include <unistd.h>
 #include <netinet/in.h> //defines IPPROTO_TCP on BSD
 #include <netdb.h>
 #include <sys/select.h>
 #include <errno.h>
#endif

#include "sockfuncs.h"

#ifdef _WIN32
 typedef int socklen_t;
#endif


int sock_connect(char *addr, short port, int msec)
{

    int sock;
    struct hostent *host_ptr;
    struct sockaddr_in hostname;

#ifdef _WIN32
    WSADATA wsa;
    WSAStartup(MAKEWORD(2,2),&wsa);
#endif

    sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(sock == -1)
          return SOCK_ERR_CREATE;

    host_ptr = gethostbyname(addr);
    if(host_ptr == NULL)
    {
        host_ptr = gethostbyaddr(addr, strlen(addr), AF_INET);
        if(host_ptr == NULL)
        {
            sock_close(&sock);
            return SOCK_ERR_RESOLVE;
        }
    }

    hostname.sin_family = AF_INET;
    hostname.sin_port = htons(port);

    memcpy(&hostname.sin_addr, host_ptr->h_addr, host_ptr->h_length);

    
    sock_nonblock(&sock);

    connect(sock, (struct sockaddr*) &hostname, sizeof(hostname));

    if(sock_select(&sock, msec, WRITE) <= 0)
    {
        sock_close(&sock);
        return SOCK_TIMEOUT;
    }

    if(!sock_isvalid(&sock))
    {
        sock_close(&sock);
        return SOCK_INVALID;
    }

    return sock;
}

int sock_setbufsize(int *s, int send_size, int recv_size)
{
    int ret;
    socklen_t len = sizeof(send_size);

    if(send_size > 0)
    {
        ret = setsockopt(*s, SOL_SOCKET, SO_SNDBUF, (char*)(&send_size), len);
        if(ret)
            return SOCK_ERR_SET_SBUF;
    }
    if(recv_size > 0)
    {
        ret = setsockopt(*s, SOL_SOCKET, SO_RCVBUF, (char*)(&recv_size), len);
        if(ret)
            return SOCK_ERR_SET_RBUF;
    }

    return 0;
}

int sock_send(int *s, char *buf, int len, int msec)
{
    int rc;
    int sent = 0;
    int error;

    while(sent < len)
    {
        rc = sock_select(s, 60000, WRITE); //check if socket is connected
        if(rc == 0)
        {
            printf("select returned 0\n");
            fflush(stdout);
            return SOCK_TIMEOUT;
        }

        if(rc == -1)
        {
            printf("select returned -1\n");
            fflush(stdout);
            return SOCK_TIMEOUT;
        }

        if((rc = send(*s, buf+sent, len-sent, 0)) < 0)
        {
            error = errno;
            if(error != EWOULDBLOCK)
            {
                printf("send errno: %d\n", error);
                fflush(stdout);
                return SOCK_TIMEOUT;
            }
            rc = 0;
        }

        sent += rc;
    }
    return sent;
}

int sock_recv(int *s, char *buf, int len, int msec)
{
    int rc;

    if(sock_select(s, msec, READ) <= 0)
        return SOCK_TIMEOUT;

    rc = recv(*s, buf, len, 0);

    return rc;
}

int sock_nonblock(int *s)
{
#ifndef _WIN32
    long arg;

    arg = fcntl(*s, F_GETFL);
    arg |= O_NONBLOCK;

    return fcntl(*s, F_SETFL, arg);
#else
    unsigned long arg = 1;
    return ioctlsocket(*s, FIONBIO, &arg);
#endif
}

int sock_block(int *s)
{
#ifndef _WIN32
    long arg;

    arg = fcntl(*s, F_GETFL);
    arg &= ~O_NONBLOCK;

    return fcntl(*s, F_SETFL, arg);
#else
    unsigned long arg = 0;
    return ioctlsocket(*s, FIONBIO, &arg);
#endif
}

int sock_select(int *s, int msec, int mode)
{
   struct timeval tv;
   fd_set fd_wr, fd_rd;

   FD_ZERO(&fd_wr);
   FD_ZERO(&fd_rd);
   FD_SET(*s, &fd_wr);
   FD_SET(*s, &fd_rd);

   tv.tv_sec = msec/1000;
   tv.tv_usec = (msec%1000)*1000;

   switch(mode)
   {
       case READ:
           return select(*s+1, &fd_rd, NULL, NULL, &tv);
           break;
       case WRITE:
           return select(*s+1, NULL, &fd_wr, NULL, &tv);
           break;
       case RW:
           return select(*s+1, &fd_rd, &fd_wr, NULL, &tv);
           break;
       default:
           return SOCK_NO_MODE;
   }
}

int sock_isvalid(int *s)
{
    int optval;
    socklen_t len = sizeof(optval);

    getsockopt(*s, SOL_SOCKET, SO_ERROR, (char*)(&optval), &len);

    if (optval)
        return 0;

    return 1;
}

void sock_close(int *s)
{
#ifdef _WIN32
    closesocket(*s);
    WSACleanup();
#else
    close(*s);
#endif
}
