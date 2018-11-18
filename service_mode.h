#ifndef SERVICE_MODE_H
#define SERVICE_MODE_H

//$PSD_CSEM*48

#define SERVICE_TIMEOUT   20 //[s]

typedef enum {
  SER_ST_IDLE       = 0,
  SER_ST_FORMAT     = 1,
  SER_ST_TIME       = 2,
  SER_ST_EXIT       = 3,
}service_state_e;

#endif
