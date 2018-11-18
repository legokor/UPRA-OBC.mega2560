#ifndef CAN_H
#define CAN_H

#define CAN_CS    43
#define CAN_INT   41

#define CAN_ID    0x0001

MCP_CAN CAN0(CAN_CS);

typedef enum {
  CAN_BEACON_TX     = 0x01,
  CAN_GET_COM_HK    = 0x02,
  CAN_COM_HK        = 0x03,
}can_msg_id_e;

typedef enum {
  CAN_OBC_ID      = 0x0001,
  CAN_COM_ID      = 0x0002,
  CAN_EPS_ID      = 0x0003,
  CAN_DAU_ID      = 0x0004,
}can_module_id_e;

typedef struct {
  uint8_t id;
  uint32_t gps_time;
  uint16_t latitude1;
  uint16_t latitude2;
  uint16_t longitude1;
  uint16_t longitude2;
  uint16_t altitude;
  int16_t ext_temp;
  int16_t obc_temp;
  uint8_t checksum;
}can_ltm_t;

typedef struct {
  uint8_t id;
  uint16_t com_temp;
  uint16_t msg_num;
  uint8_t checksum;
}can_com_hk_t;

byte can_tx_msg[8];

long unsigned int can_rx_id;
unsigned char can_rx_len = 0;
byte can_rx_msg[8];

#endif
