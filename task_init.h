#ifndef TASK_INIT_H
#define TASK_INIT_H


#define BUS_PROCESS_TASK      0
#define BUS_PROCESS_PERIOD    ((uint32_t)10)

#define GPS_TASK              1
#define GPS_TASK_PERIOD       ((uint32_t)5000)

#define GPS_RX_TASK           2
#define GPS_RX_TASK_PERIOD    ((uint32_t)10)

#define BEACON_TX_TASK        3
#define BECAON_TX_PERIOD      ((uint32_t)15000)

#define GET_COM_HK_TASK       4
#define GET_COM_HK_PERIOD     ((uint32_t)5000)

#define GET_OBC_HK_TASK       5
#define GET_OBC_HK_PERIOD     ((uint32_t)5000)

#define SLAVE_INT_TASK        6
#define SLAVE_INT_PERIOD      ((uint32_t)50)

#endif
