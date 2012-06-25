// ringbuffer functions for butt
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
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "ringbuffer.h"

pthread_mutex_t rb_mutex = PTHREAD_MUTEX_INITIALIZER;

int rb_init(struct ringbuf *rb, unsigned int len)
{
	rb->buf = (char*)malloc(len * sizeof(char));
	if(!rb->buf)
		return -1;

	rb->r_ptr = rb->buf;
	rb->w_ptr = rb->buf;
	rb->size = len;
    rb->full = 0;

	return 0;
}

int rb_filled(struct ringbuf *rb)

{
	int filled;
	char *end_ptr;

    if(rb->w_ptr == rb->r_ptr && rb->full)
        filled = rb->size;
    else if(rb->w_ptr == rb->r_ptr && !rb->full)
        filled = 0;
    else if(rb->w_ptr > rb->r_ptr)
		filled = rb->w_ptr - rb->r_ptr;
	else {
		end_ptr = rb->buf + rb->size;
		filled = end_ptr - rb->r_ptr;
		filled += rb->w_ptr - rb->buf;
	}

	return filled;
}

int rb_space(struct ringbuf *rb)
{
	int space;
	char *end_ptr;

	if(rb->r_ptr == rb->w_ptr && rb->full)
		space = 0;
    else if(rb->r_ptr == rb->w_ptr && !rb->full)
		space = rb->size;
	else if(rb->r_ptr > rb->w_ptr)
		space = rb->r_ptr - rb->w_ptr;
	else {
		end_ptr = rb->buf + rb->size;
		space = end_ptr - rb->w_ptr;
		space += rb->r_ptr - rb->buf;
	}

	return space;
}

unsigned int rb_read(struct ringbuf *rb, char *dest)
{
	unsigned int len = 0;
	char *end_ptr;


	if(!dest || !rb->buf)
		return 0;
	if(len > rb->size)
		return 0;

    pthread_mutex_lock(&rb_mutex);

	end_ptr = rb->buf + rb->size;
	len = rb_filled(rb);

	if(rb->r_ptr + len < end_ptr ) {
		memcpy(dest, rb->r_ptr, len);
		rb->r_ptr += len;
	}
	/*buf content crosses the start point of the ring*/
	else { 
		/*copy from r_ptr to start of ringbuffer*/
		memcpy(dest, rb->r_ptr, end_ptr - rb->r_ptr);
		/*copy from start of ringbuffer to w_ptr*/
		memcpy(dest + (end_ptr - rb->r_ptr), rb->buf, len - (end_ptr - rb->r_ptr));
		rb->r_ptr = rb->buf + (len - (end_ptr - rb->r_ptr));
	}

    pthread_mutex_unlock(&rb_mutex);

    if(rb->w_ptr == rb->r_ptr)
        rb->full = 0;

	return len;
}

int rb_write(struct ringbuf *rb, char* src, unsigned int len)
{
	char *end_ptr;

	if(!src || !rb->buf)
		return -1;
	if(len > rb->size)
		return -1;
    if(len == 0)
        return 0;
    
    pthread_mutex_lock(&rb_mutex);

	end_ptr = rb->buf + rb->size;

	if(rb->w_ptr + len < end_ptr ) {
		memcpy(rb->w_ptr, src, len);
		rb->w_ptr += len;
	}
	else {
		memcpy(rb->w_ptr, src, end_ptr - rb->w_ptr);
		memcpy(rb->buf, src + (end_ptr - rb->w_ptr), len - (end_ptr - rb->w_ptr));
		rb->w_ptr = rb->buf + (len - (end_ptr - rb->w_ptr));
	}

    if(rb->w_ptr == rb->r_ptr)
        rb->full = 1;

    pthread_mutex_unlock(&rb_mutex);

	return 0;
}

int rb_free(struct ringbuf *rb)
{
	free(rb->buf);
	return 0;
}

