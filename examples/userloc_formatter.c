/******************************************************************************
 *
 * Part of the log4c examples.
 *
 * Along with example_appenders.c this file is used to create a small
 * library of custom appenders and formatters.
 *
 * This library is excercised using application_3 and a sample log4crc
 * config file.
 *
 *****************************************************************************/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <string.h>
#include <log4c.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
/* using internal log4c debug function here,
 * this is not belong to examples :-) */
#include <sd/error.h>
#include "application_3.h"

/**********************************************************************
 *
 * Formatted to look for extended user location info
 *
 **********************************************************************/
static const char* userloc_format(
    const log4c_layout_t*       a_layout,
    const log4c_logging_event_t*a_event)
{
    static char buffer_time[1024]={0};
    static char buffer[4096]={0};
    user_locinfo_t* uloc = NULL;

#ifdef LOG4C_POSIX_TIMESTAMP
    struct tm tm;
    time_t t;

    t = a_event->evt_timestamp.tv_sec;
    localtime_r(&t, &tm);
    snprintf(buffer, sizeof(buffer), "%04d%02d%02d %02d:%02d:%02d.%03ld[%-8s]",
             tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
             tm.tm_hour, tm.tm_min, tm.tm_sec,
             a_event->evt_timestamp.tv_usec / 1000,
             log4c_priority_to_string(a_event->evt_priority));
#else
    SYSTEMTIME stime, ltime;

    if (FileTimeToSystemTime(&a_event->evt_timestamp, &stime) &&
        SystemTimeToTzSpecificLocalTime(NULL, &stime, &ltime)) {
        snprintf(buffer, sizeof(buffer), "%04d%02d%02d %02d:%02d:%02d.%03ld[%-8s]",
                 ltime.wYear, ltime.wMonth , ltime.wDay,
                 ltime.wHour, ltime.wMinute, ltime.wSecond,
                 ltime.wMilliseconds,
                 log4c_priority_to_string(a_event->evt_priority));
    }
#endif

    strcpy(buffer_time, buffer);

    sd_debug("Formatter s13_userloc checking location info for userdata %p",a_event->evt_loc->loc_data);
    if (a_event->evt_loc->loc_data != NULL)
    {
      sd_debug("Formatter s13_userloc getting a valid user location info pointer");
      uloc = (user_locinfo_t*) a_event->evt_loc->loc_data;
      sprintf(buffer, "%s[%s+%i][%i][%s][%s]",
        buffer_time,
        a_event->evt_loc->loc_file, a_event->evt_loc->loc_line,
        getpid(),
        uloc->code,
        a_event->evt_msg);
    }
    return buffer;
}

const log4c_layout_type_t log4c_layout_type_userloc  = {
   "s13_userloc",
   userloc_format,
};

/*****************************/
/*
 * Here provide an init routine for this lib 
 *
******************************/
static const log4c_layout_type_t * const layout_types[] = {
    &log4c_layout_type_userloc,
};
static int nlayout_types =
	(int)(sizeof(layout_types) / sizeof(layout_types[0]));


int init_userloc_formatters(){

  int rc = 0; int i = 0;
	
  for (i = 0; i < nlayout_types; i++) 
     log4c_layout_type_set(layout_types[i]);

  return(rc);

}


