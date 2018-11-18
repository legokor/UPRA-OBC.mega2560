uint32_t radio_state;

void task_beacon_tx()
{
  bus_sm(BUS_BEACON_TX);
}

void processTCHKDmsg()
{
  int i, j, k, IntegerPart;
 
  // $TCHKD,0123,336*47
  //        temp  vcc
  //           1   2
 
  IntegerPart = 1;
  radio_msg_id = 0;
  radio_temp = 0;
 
  for (i=0, j=0, k=0; (i<MSGindex) && (j<10); i++) // We start at 7 so we ignore the '$GPGGA,'
  {
    if ((bus_msg[i] == ',') || (bus_msg[i] == '*'))
    {
      j++;    // Segment index
      k=0;    // Index into target variable
      IntegerPart = 1;
    }
    else
    {
      if (j == 1)
      {
        if (((bus_msg[i] >= '0') && (bus_msg[i] <= '9')) ||(bus_msg[i] == '-') )
        {        
          radio_temp = radio_temp * 10;
          radio_temp += (unsigned int)(bus_msg[i] - '0');
          k++;
        }
      }
      else if (j == 2)
      {
        k++;
      }
      else if (j == 3)
      {
        if (((bus_msg[i] >= '0') && (bus_msg[i] <= '9')) )
        {        
          radio_msg_id = radio_msg_id * 10;
          radio_msg_id += (unsigned int)(bus_msg[i] - '0');
          k++;
        }
      }
    }
  }
  DEBUG.println(radio_temp);
}

void radio_init()
{
  com_error_cntr = RADIO_ERROR_CNT;
  radio_temp = 0;
  radio_msg_id = 0;
//  sw_timer_add_channel(RADIO_TIMER, RADIO_ACK_TIMEOUT, &radio_timer_cb);
}


void serialFlush(){
  while(SICL.available() > 0) {
    char t = SICL.read();
  }
  for(int i=0; i<30; i++)
  {
    bus_msg[i]=0;
  }
} 



int parseRequestHandshake(void)
{
 
}

int parseEOTHandshake(void)
{

}




int SendRadioTelemetry(void)
{
 
}


/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////

int32_t radio_sicl_get_hk(void)
{
  ////////////SICL.listen();  
  DEBUG.println(F("[OBC] Get Radio Housekeeping"));  
  delay(100);
  setBusBusy();
  delay(500);
  SICL.println(F("$TMHKR,C,,,,*47"));

  // TODO bus_sm(BUS_RX);
  bus_sm(BUS_PROCESS);
  return 0;
}

int32_t radio_sicl_beacon_tx(void)
{
  long  GPS_Alt_tmp;
  char tmp_buffer[64];

      ////////////SICL.listen(); 
  take_mutex();
        
  delay(500);
//      Serial.println("");
//      debugLOG();
  setBusBusy();
  delay(500);
  
  GPS_Alt_tmp = GPS_Altitude[GPS_valid];
  sprintf(tmp_buffer, "$TMLTM,%s,%s,%s,%05ld,%04lu,%04lu",  GPS_time[GPS_valid], 
                                                              GPS_lati[GPS_valid], 
                                                                GPS_long[GPS_valid], 
                                                                 GPS_Alt_tmp, 
                                                                   ext_temp, 
                                                                      pcb_temp );

  DEBUG.print(F("[OBC] ALTITUDE: "));  
  DEBUG.println(GPS_Alt_tmp);
  DEBUG.println(tmp_buffer);
  delay(200);

  SICL.print(tmp_buffer);
  SICL.println(F("*47"));    

  sw_timer_enable_channel(BUS_TIMER);
  bus_sm(BUS_PROCESS);
  give_mutex();
  return 0;
}

int32_t radio_micl_get_hk(void)
{
  DEBUG.println(F("[OBC] Get Radio Housekeeping"));  

  can_tx_msg[0] = 0;
  can_tx_msg[1] = CAN_GET_COM_HK;

  if( can_send_msg(CAN_ID, 2) != 0)
  {
    bus_sm(BUS_CAN_ERROR);
    return -1;
  }
  bus_sm(BUS_PROCESS);
  return 0;
}

int32_t radio_micl_beacon_tx(can_ltm_t* msg)
{
  byte ltm[8];
  byte chksm = 0;
  byte sndStat;
  int i;

  can_set_beacon_msg(msg);

  can_tx_msg[0] = 0;
  can_tx_msg[1] = msg->id;
  memcpy(can_tx_msg + 2, &(msg->gps_time), 4);
  memcpy(can_tx_msg + 6, &(msg->latitude1), 2);

  for(i = 2; i < 8; i++)
  {
    chksm ^= can_tx_msg[i];
  }
 
  if( can_send_msg(CAN_ID, 8) != 0)
  {
    bus_sm(BUS_CAN_ERROR);
    return -1;
  }

  can_tx_msg[0]++;
  memcpy(can_tx_msg + 2, &(msg->latitude2), 2);
  memcpy(can_tx_msg + 4, &(msg->longitude1), 2);
  memcpy(can_tx_msg + 6, &(msg->longitude2), 2);

  for(i = 2; i < 8; i++)
  {
    chksm ^= can_tx_msg[i];
  }
 
  if( can_send_msg(CAN_ID, 8) != 0)
  {
    bus_sm(BUS_CAN_ERROR);
    return -1;
  }

  can_tx_msg[0]++;
  memcpy(can_tx_msg + 2, &(msg->altitude), 2);
  memcpy(can_tx_msg + 4, &(msg->ext_temp), 2);
  memcpy(can_tx_msg + 6, &(msg->obc_temp), 2);

  for(i = 2; i < 8; i++)
  {
    chksm ^= can_tx_msg[i];
  }

  if( can_send_msg(CAN_ID, 8) != 0)
  {
    bus_sm(BUS_CAN_ERROR);
    return -1;
  }

  can_tx_msg[0]++;
  can_tx_msg[2] = chksm;

  if( can_send_msg(CAN_ID, 3) != 0)
  {
    bus_sm(BUS_CAN_ERROR);
    return -1;
  }
  bus_sm(BUS_PROCESS);
}

/*void lowSpeedStartup(void)
{
      unsigned long timer=0;
     
//      SICL.begin(57600); 
      //////SICL.listen();
      radio_sicl_get_hk();
      
      
      delay(1000);
//      SICL.println("");
//      debugLOG();
      
      if ( SendRadioTelemetry() != 0)
      {
        SICL.print("##");
        SICL.print(callsign);
        SICL.print(",MATeF-STARTUP------------------------------------------");   

        timer=millis();       
        while( GetEOTHandshake() == 0)
        {
            if( millis()-timer > 6000)
           {
              break;
           }   
        }
      }
      SICL.println("");    
}*/
