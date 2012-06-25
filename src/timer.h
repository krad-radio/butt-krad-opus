// timer related functions
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

#ifndef TIMER_H
#define TIMER_H

#include <time.h>

struct sec_timer
{
    time_t start_time;
    time_t new_time;
    int duration;
};


void timer_init(sec_timer *t, int duration);
int timer_is_elapsed(sec_timer *t);
char *timer_get_time_str(sec_timer *t);

#endif

