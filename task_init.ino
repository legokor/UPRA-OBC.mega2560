void task_init()
{
  task_mngr_init();
  
  task_mngr_add_task( GPS_TASK, 
                        &task_gps, 
                          GPS_TASK_PERIOD, 
                            TASK_SLEEP);
                      
  task_mngr_add_task( GPS_RX_TASK, 
                        &task_read_gps, 
                          GPS_RX_TASK_PERIOD, 
                            TASK_SLEEP);

  task_mngr_add_task( BUS_PROCESS_TASK, 
                        &task_bus_process, 
                          BUS_PROCESS_PERIOD, 
                            TASK_SLEEP);
                            
  task_mngr_add_task( GET_COM_HK_TASK, 
                        &task_get_com_hk, 
                          GET_COM_HK_PERIOD, 
                            TASK_SLEEP);
                            
  task_mngr_add_task( BEACON_TX_TASK, 
                        &task_beacon_tx, 
                          BECAON_TX_PERIOD, 
                            TASK_SLEEP);
                            
  task_mngr_add_task( GET_OBC_HK_TASK, 
                        &task_get_obc_hk, 
                          GET_OBC_HK_PERIOD, 
                            TASK_SLEEP);

  task_mngr_add_task( SLAVE_INT_TASK, 
                        &task_slave_interrupt, 
                          SLAVE_INT_PERIOD, 
                            TASK_IDLE);

  task_mngr_add_task( LANDING_TASK, 
                        &task_landing_becon_mode, 
                          LANDING_PERIOD, 
                            TASK_SLEEP);

  task_mngr_add_task( SAFE_MODE_TASK, 
                        &task_safe_mode, 
                          SAFE_MODE_PERIOD, 
                            TASK_SLEEP);

  task_mngr_add_task( OP_MODE_TASK, 
                        &task_operation_mode_control, 
                          OP_MODE_PERIOD, 
                            TASK_IDLE);
}


