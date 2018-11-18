#ifndef GPS_H
#define GPS_H

#define GPS                     Serial2
#define GPS_BAUD                4800    //NEO6: 9600, PC: 9600, TYCO(for test): 4800
#define GPS_MAX_RETRIES         ((uint32_t)(512))
#define GPS_PROCESS_TIMEOUT     ((uint32_t)5000)  //[ms]


typedef enum {
  GPS_ST_IDLE           = 0,
  GPS_ST_RX             = 1,
  GPS_ST_PROCESS        = 2,
  GPS_ST_INVALID        = 3,
}gps_state_e;

typedef enum {
  GPS_READ              = 0,
  GPS_EOL               = 1,
  GPS_GGA               = 2,
  GPS_NO_GGA            = 3,
  GPS_TIMEOUT           = 4,
}gps_input_e;


char              GPS_time[2][10];
char              GPS_lati[2][12];
char              GPS_long[2][13];
long              GPS_Altitude[2];
uint8_t           GPS_Satellites;
uint8_t           GPS_Fix;
uint8_t           GPS_first_fix;


uint8_t           GPS_valid;


#endif
