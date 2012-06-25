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

#include <stdio.h>
#include <stdlib.h>

#include "timer.h"

char time_str[10];

void timer_init(sec_timer *t, int duration)
{
    t->start_time = time(NULL);
    t->new_time = t->start_time;
    t->duration = duration;
}

int timer_is_elapsed(sec_timer *t)
{

    if(time(NULL) >= t->new_time + t->duration)
    {
        t->new_time = time(NULL); //reset the timer
        return 1;
    }
    else
        return 0;
}

char *timer_get_time_str(sec_timer *t)
{
    int hour = 0, min = 0, sec = 0;
    time_t cur_time = time(NULL);

    sec = cur_time - t->start_time;

    min = sec / 60;
    hour = min / 60;
    min %= 60;
    sec %= 60;

    snprintf(time_str, 10, "%02d:%02d:%02d", hour, min, sec);

    return time_str;
}

