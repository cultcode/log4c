static const char version[] = "$Id: layout_type_userloc.c,v 1.6 2006/09/27 21:25:12 legoater Exp $";

/*
 * layout.c
 *
 * Copyright 2001-2003, Meiosys (www.meiosys.com). All rights reserved.
 *
 * See the COPYING file for the terms of usage and distribution.
 */

#include <log4c/layout.h>
#include <log4c/priority.h>
#include <sd/sprintf.h>
#include <sd/sd_xplatform.h>
#include <stdio.h>
#include <string.h>
#include <sd/error.h>

/*******************************************************************************/
static const char* userloc_format(
    const log4c_layout_t*       a_layout,
    const log4c_logging_event_t*a_event)
{
    static char buffer_time[1024]={0};
    static char buffer[1024]={0};
    char* code = NULL;

#ifdef LOG4C_POSIX_TIMESTAMP
    struct tm tm;
    time_t t;

    t = a_event->evt_timestamp.tv_sec;
    localtime_r(&t, &tm);
    snprintf(buffer, sizeof(buffer), "%04d-%02d-%02dT%02d:%02d:%02d.%03ld+%d[%s]",
             tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
             tm.tm_hour, tm.tm_min, tm.tm_sec,
             a_event->evt_timestamp.tv_usec / 1000,
             a_event->evt_timezone.tz_minuteswest / -60,
             log4c_priority_to_string(a_event->evt_priority));
#else
    SYSTEMTIME stime, ltime;

    if (FileTimeToSystemTime(&a_event->evt_timestamp, &stime) &&
        SystemTimeToTzSpecificLocalTime(NULL, &stime, &ltime)) {
        snprintf(buffer, sizeof(buffer), "%04d-%02d-%02d %02d:%02d:%02d.%03ld+%d[%s]",
                 ltime.wYear, ltime.wMonth , ltime.wDay,
                 ltime.wHour, ltime.wMinute, ltime.wSecond,
                 ltime.wMilliseconds,
                 a_event->evt_timezone.Bias / -60,
                 log4c_priority_to_string(a_event->evt_priority));
    }
#endif

    strcpy(buffer_time, buffer);

    sd_debug("Formatter s13_userloc checking location info for userdata %p",a_event->evt_loc->loc_data);
    if (a_event->evt_loc->loc_data != NULL)
    {
      sd_debug("Formatter s13_userloc getting a valid user location info pointer");
      code = (char*) a_event->evt_loc->loc_data;
      sprintf(buffer, "%s[%s+%i][%i][%s][%s]\n",
        buffer_time,
        a_event->evt_loc->loc_file, a_event->evt_loc->loc_line,
        getpid(),
        code,
        a_event->evt_msg);
    }
    return buffer;
}

/*******************************************************************************/
const log4c_layout_type_t log4c_layout_type_userloc = {
    "userloc",
    userloc_format,
};

