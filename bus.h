#ifndef BUS_H
#define BUS_H

#define BUS_ACK_TIMEOUT   5000  //[ms]

//UPRA BUS
#define DEBUG_BAUD  9600
#define SICL_BAUD   9600
#define SICLRX      0
#define SICLTX      1
#define BUSBUSY     A8

#define SICL      Serial1    //later Serial1
#define DEBUG     Serial

typedef enum {
  BUS_ST_IDLE           = 0,
  BUS_ST_PROCESS        = 1,
  BUS_ST_GET_COM_HK     = 2,
  BUS_ST_BEACON_TX      = 3,
  BUS_ST_INTERRUPT      = 4,
}bus_state_e;

typedef enum {
  BUS_IDLE              = 0,
  BUS_PROCESS           = 1,
  BUS_TIMEOUT           = 2,
  BUS_ACK               = 3,
  BUS_EOL               = 4,
  BUS_GET_COM_HK        = 5,
  BUS_BEACON_TX         = 6,
  BUS_SLAVE_INTERRUPT   = 7,
  BUS_CAN_ERROR         = 8,
}bus_input_e;

byte MSGindex=0;
char bus_msg[30];


#endif
