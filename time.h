#ifndef TIME_H
#define TIME_H


#define MODULE_OBC
//#define MODULE_SUBSYSTEM

typedef struct
{
  uint32_t hour;
  uint32_t minute;
  uint32_t second;
  uint32_t timestamp;
}time_t;

uint32_t gps_time_sync;

#endif
