#ifndef RADIO_H
#define RADIO_H

#define RADIO_ACK_TIMEOUT   5000  //[ms]
#define RADIO_ERROR_CNT     3

int16_t radio_temp;
uint32_t radio_msg_id;
uint32_t com_error_cntr;

#endif
