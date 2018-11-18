
int32_t can_init()
{
  if(CAN0.begin(MCP_ANY, CAN_250KBPS, MCP_8MHZ) != CAN_OK) 
  {
    DEBUG.println(F("[OBC] CAN INIT FAILED"));  
    return -1;
  }
  CAN0.setMode(MCP_NORMAL);   // Change to normal mode to allow messages to be transmitted
  pinMode(CAN_INT, INPUT); 
  return 0;
}

int32_t get_micl_msg(void)
{
  if(!digitalRead(CAN_INT))                         // If CAN0_INT pin is low, read receive buffer
  {
    CAN0.readMsgBuf(&can_rx_id, &can_rx_len, can_rx_msg);      // Read data: len = data length, buf = data byte(s)
    
    switch(can_rx_id & 0x00000FFF)
    {
      case CAN_COM_ID: 
      {
        process_com_micl_msg();
        break;
      }
      default: break;
    }
  }
  return 0;
}

int32_t process_com_hk(can_com_hk_t* msg)
{
  int i;
  msg->checksum = 0;
  memcpy(&(msg->com_temp), can_rx_msg + 2, 2);
  memcpy(&(msg->msg_num), can_rx_msg + 4, 2);
  for(i = 2; i < 6; i++)
  {
    msg->checksum ^= can_rx_msg[i];
  }
  
  if( msg->checksum != can_rx_msg[6])
  {
    DEBUG.println(F("[OBC] COM HK CAN CHECKSUM ERROR"));
    return -1;
  }
  return 0;
}

int32_t process_com_micl_msg(void)
{
  switch(can_rx_msg[1])
  {
    case CAN_COM_HK:
    {
      if( process_com_hk(&can_com_hk_msg) == 0)
      {
        sleep_task(BUS_PROCESS_TASK);
        DEBUG.println(F("[OBC] COM HK DATA RECEIVED ON CAN"));
        radio_temp = can_com_hk_msg.com_temp;
        radio_msg_id = can_com_hk_msg.msg_num;
        DEBUG.print(F("[OBC] COM HK: TEMP: "));
        DEBUG.print(radio_temp);
        DEBUG.print(F("  |  MSG NUM: "));
        DEBUG.println(radio_msg_id);
        bus_sm(BUS_ACK);
      }
      break;
    }
    default: break;
  }
  return 0;
}

void c_set_ns(can_ltm_t* msg)
{
  if(GPS_lati[GPS_valid][0] == '+')
  {
    msg->latitude1 |= 0x8000;
  }
  else
  {
    msg->latitude1 &= 0x7FFF;
  }
}

void c_set_we(can_ltm_t* msg)
{
  if(GPS_long[GPS_valid][0] == '+')
  {
    msg->longitude1 |= 0x8000;
  }
  else
  {
    msg->longitude1 &= 0x7FFF;
  }
}

int32_t can_set_beacon_msg(can_ltm_t* msg)
{
  int i, j, IntegerPart;

  uint32_t hh;
  uint32_t mm;
  uint32_t ss;  
   
  IntegerPart = 1;
  radio_msg_id = 0;
  radio_temp = 0;
  msg->id = CAN_BEACON_TX;
  msg->gps_time = 0;
  msg->latitude1 = 0;
  msg->latitude2 = 0;
  msg->longitude1 = 0;
  msg->longitude2 = 0;
  msg->altitude = 0;
  msg->ext_temp = 0;
  msg->obc_temp = 0;

  // set time
  hh = (unsigned int)(GPS_time[GPS_valid][0] - '0') * 10;
  hh += (unsigned int)(GPS_time[GPS_valid][1] - '0');

  mm = (unsigned int)(GPS_time[GPS_valid][2] - '0') * 10;
  mm += (unsigned int)(GPS_time[GPS_valid][3] - '0');
  
  ss = (unsigned int)(GPS_time[GPS_valid][4] - '0') * 10;
  ss += (unsigned int)(GPS_time[GPS_valid][5] - '0');

  msg->gps_time = (hh * 3600 ) + (mm * 60 ) + ss;
 
  // set latitude
  for( i = 1, j = 0; i < 8; i++)
  {
    if (GPS_lati[GPS_valid][i] == '.')
    {
      j++;    // Segment index
      IntegerPart = 1;
    }
    else
    {
      if( j == 0)
      {
        msg->latitude1 = msg->latitude1 * 10;
        msg->latitude1 += (unsigned int)(GPS_lati[GPS_valid][i] - '0'); 
      }
      if( j == 1)
      {
        msg->latitude2 = msg->latitude2 * 10;
        msg->latitude2 += (unsigned int)(GPS_lati[GPS_valid][i] - '0'); 
      }
    }
  }
  c_set_ns(msg);

  // set longitude
  for( i = 1, j = 0; i < 9; i++)
  {
    if (GPS_long[GPS_valid][i] == '.')
    {
      j++;    // Segment index
      IntegerPart = 1;
    }
    else
    {
      if( j == 0)
      {
        msg->longitude1 = msg->longitude1 * 10;
        msg->longitude1 += (unsigned int)(GPS_long[GPS_valid][i] - '0'); 
      }
      if( j == 1)
      {
        msg->longitude2 = msg->longitude2 * 10;
        msg->longitude2 += (unsigned int)(GPS_long[GPS_valid][i] - '0'); 
      }
    }
  }
  c_set_we(msg);
  
  // set altitude
  msg->altitude = GPS_Altitude[GPS_valid];

  // set temperatures
  msg->ext_temp = ext_temp;
  msg->obc_temp = pcb_temp;
  

}

int32_t can_send_msg(uint32_t id, uint32_t len)
{
  byte sndStat = CAN0.sendMsgBuf(id, 0, len, can_tx_msg);
  if(sndStat != CAN_OK)
  {
    return sndStat;
  }
  return 0;  
}

