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
          radio_temp[k] = bus_msg[i];
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
  radio_temp[3] ='\0';  
  DEBUG.println(radio_temp);
}

void radio_init()
{

  radio_temp[0] = 'N';
  radio_temp[1] = '/';
  radio_temp[2] = 'A';
  radio_temp[3] = '\0';
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

int32_t radio_get_hk(void)
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

int32_t radio_beacon_tx(void)
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
//        SICL.print(F("$TMLTM,"));
//        SICL.print(GPS_time[GPS_valid]);
//        SICL.print(F(","));
//        SICL.print(GPS_lati[GPS_valid]);
//        SICL.print(F(","));
//        SICL.print(GPS_long[GPS_valid]);
//        SICL.print(F(","));
//        //SICL.print(GPS_Alt_ch);
//        sprintf(buffer, "%05ld",GPS_Alt_tmp);
//        SICL.print(buffer);
//        SICL.print(F(","));
//        sprintf(buffer, "%04d",ext_temp[ext_temp_valid]);
//        SICL.print(buffer);
//        SICL.print(F(","));
//        sprintf(buffer, "%04d",pcb_temp[pcb_temp_valid]);
//        SICL.print(buffer);
  SICL.print(tmp_buffer);
  SICL.println(F("*47"));    

  sw_timer_enable_channel(BUS_TIMER);
  bus_sm(BUS_PROCESS);
  give_mutex();
  return 0;
}

/*void lowSpeedStartup(void)
{
      unsigned long timer=0;
     
//      SICL.begin(57600); 
      //////SICL.listen();
      radio_get_hk();
      
      
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
