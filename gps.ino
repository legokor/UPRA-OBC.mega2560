byte              GPSBuffer[82];
byte              GPSIndex=0;
uint32_t          gps_state;
uint32_t          gps_retries;
int32_t           gps_first_time_fix;


void task_gps()
{
  gps_sm(GPS_READ);
}

void task_read_gps()
{
  take_mutex();
  CheckGPS();
  give_mutex();
}



int32_t gps_init()
{
  int GPS_stat;

  gps_state = GPS_ST_IDLE;
  gps_retries = GPS_MAX_RETRIES;
  GPS_valid = 0;

  GPS_Fix = 0;
  GPS_Satellites = 0;
  gps_first_time_fix = 1;
  
  GPS_time[GPS_valid][0] = '3';
  GPS_time[GPS_valid][1] = '3';
  GPS_time[GPS_valid][2] = '6';
  GPS_time[GPS_valid][3] = '6';
  GPS_time[GPS_valid][4] = '7';
  GPS_time[GPS_valid][5] = '7';
  GPS_time[GPS_valid][6] = '\0';

  GPS_lati[GPS_valid][0] = '+';
  GPS_lati[GPS_valid][1] = '9';
  GPS_lati[GPS_valid][2] = '5';
  GPS_lati[GPS_valid][3] = '0';
  GPS_lati[GPS_valid][4] = '0';
  GPS_lati[GPS_valid][5] = '.';
  GPS_lati[GPS_valid][6] = '0';
  GPS_lati[GPS_valid][7] = '0';
  GPS_lati[GPS_valid][8] = '0';
  GPS_lati[GPS_valid][9] = '\0';

  GPS_long[GPS_valid][0] = '+';
  GPS_long[GPS_valid][1] = '1';
  GPS_long[GPS_valid][2] = '8';
  GPS_long[GPS_valid][3] = '8';
  GPS_long[GPS_valid][4] = '8';
  GPS_long[GPS_valid][5] = '8';
  GPS_long[GPS_valid][6] = '.';
  GPS_long[GPS_valid][7] = '0';
  GPS_long[GPS_valid][8] = '0';
  GPS_long[GPS_valid][9] = '0';
  GPS_long[GPS_valid][10] = '\0';  


  GPS.begin(GPS_BAUD);
  GPS_stat = SetupUBLOX();

  sw_timer_add_channel(GPS_TIMER, GPS_PROCESS_TIMEOUT, &gps_timer_cb);

  
  if(GPS_stat == 1)
  {
    //SICL.listen();
    return -1;
  }
  return 0;
}

uint8_t SetupUBLOX(void)
{
  uint8_t GPS_ERROR=0;
//  GPS.listen();
   _delay_ms(1000);
  if( setGPS_DynamicModel6() != 0)
  {
    GPS_ERROR += 1;
  }
  //else GPS_ERROR = 0;

  return GPS_ERROR;  
}

uint8_t CheckGPS()
{
  uint8_t inByte;
  uint8_t error=10;
  while (GPS.available() > 0)
  {
    inByte = GPS.read();
 
    if ((inByte =='$') || (GPSIndex >= 80))
    {
      gps_time_sync = 0;
      GPSIndex = 0;
    }
 
    if (inByte != '\r')
    {
      GPSBuffer[GPSIndex++] = inByte;
    }
 
    if (inByte == '\n')
    {
      gps_sm(GPS_EOL);
//      error=process_nmea();
    }
  }
  return error;
}
uint8_t process_nmea()
{
  if ((GPSBuffer[1] == 'G') && (GPSBuffer[2] == 'P') && (GPSBuffer[3] == 'G') && (GPSBuffer[4] == 'G') && (GPSBuffer[5] == 'A'))
  {
    ProcessGPGGACommand();
    gps_sm(GPS_GGA);
    sdcard_push_track_log();
    return 0;
  }
//  Serial.println("no gga");
  gps_sm(GPS_NO_GGA);
  return 1;
}

void ProcessGPGGACommand()
{
  int i, j, k, IntegerPart;
  long Altitude;
  int time_msec = 0;
  uint8_t buff_to_write;

  enter_atomic();

  buff_to_write = 1 - GPS_valid;

  IntegerPart = 1;
  GPS_Satellites = 0;
  Altitude = 0;
  GPS_Fix =0;
  GPS_Altitude[buff_to_write] = 0;
  
  for (i=0, j=0, k=0; (i<GPSIndex) && (j<10); i++) // We start at 7 so we ignore the '$GPGGA,'
  {
    if (GPSBuffer[i] == ',')
    {
      j++;    // Segment index
      k=0;    // Index into target variable
      IntegerPart = 1;
    }
    else
    {
      if (j == 1)
      {
        if ((GPSBuffer[i] >= '0') && (GPSBuffer[i] <= '9') && (time_msec == 0))
        {        
          GPS_time[buff_to_write][k] = GPSBuffer[i];
          k++;
        }
        if(GPSBuffer[i] == '.')
        {
          time_msec = 1;
        }
      }
      else if (j == 2)
      {
        k++;
        GPS_lati[buff_to_write][k] = GPSBuffer[i];
      }
      else if (j == 3)
      {
        GPS_lati[buff_to_write][0] = CheckNSEW(GPSBuffer[i]);
      }
      else if (j == 4)
      {
        k++;
        GPS_long[buff_to_write][k] = GPSBuffer[i];
      }
      else if (j == 5)
      {
        GPS_long[buff_to_write][0] = CheckNSEW(GPSBuffer[i]);
      }
      else if (j == 6)
      {
        // Fix read
        if ((GPSBuffer[i] >= '0') && (GPSBuffer[i] <= '9'))
        {
          GPS_Fix = GPS_Fix * 10;
          GPS_Fix += (unsigned int)(GPSBuffer[i] - '0');
          //GPS_Fix=1; //debug
        }
        //GPS_Fix=1; //debug
      }
      else if (j == 7)
      {
        // Satellite Count
        if ((GPSBuffer[i] >= '0') && (GPSBuffer[i] <= '9'))
        {
          GPS_Satellites = GPS_Satellites * 10;
          GPS_Satellites += (unsigned int)(GPSBuffer[i] - '0');
        }
      }
      else if (j == 9)
      {
        // Altitude
        if ((GPSBuffer[i] >= '0') && (GPSBuffer[i] <= '9') && IntegerPart)
        {
          GPS_Altitude[buff_to_write] = GPS_Altitude[buff_to_write] * 10;
          GPS_Altitude[buff_to_write] += (unsigned int)(GPSBuffer[i] - '0');
        }
        else
        {
          IntegerPart = 0;
        }
      }
    }
  }

  GPS_time[buff_to_write][7] ='\0';    
  gps_first_time_fix = gps_sync_time();
  GPS_lati[buff_to_write][11] ='\0';
  GPS_long[buff_to_write][12] ='\0';
  
  if( GPS_Fix != 0)
  {
    GPS_valid = buff_to_write;  
  }
  GPSIndex = 0;
  exit_atomic();
}

int32_t gps_sync_time()
{
  time_t gps_time;
  int32_t delta_time;
  int32_t ret = 0;

  time_set_gps_time(&gps_time);

  gps_time.timestamp = (gps_time.timestamp) + gps_time_sync;
  
  delta_time = (int32_t)(gps_time.timestamp - obc_time.timestamp);

  if( gps_first_time_fix != 0)
  {
    ret = time_set_timestamp(&obc_time, gps_time.timestamp);
  }

  if( (delta_time <  (-30)) || ( (delta_time > 0) && (delta_time < 120) ) )
  {
    ret = time_set_timestamp(&obc_time, gps_time.timestamp);
  }

  return ret;
}

char CheckNSEW(char nsew)
{
  if(( nsew == 'N') or ( nsew == 'E'))
  {
    return '+';
  }
  else if(( nsew == 'S') or ( nsew == 'W'))
  {
    return '-';
  }
  else return 0;
}


int32_t gps_sm(uint32_t input)
{
  int32_t ret = 0; 
  switch(gps_state)
  {
    case GPS_ST_IDLE:
    {
      ret = g_idle_state_executor(input);
      break;
    }
    case GPS_ST_RX:
    {
      ret = g_rx_state_executor(input);
      break;
    }
    case GPS_ST_PROCESS:
    {
      ret = g_process_state_executor(input);
      break;
    }
    default:
    {
      ret = -1;
      break;
    }
  }
  return ret;
}

int32_t g_idle_state_executor(uint32_t input)
{
  switch(input)
  {
    case GPS_READ:
    {
      DEBUG.println(F("[OBC] GPS READ START"));
      sw_timer_enable_channel(GPS_TIMER);
      gps_state = GPS_ST_RX;
      wake_up_task(GPS_RX_TASK);
      return 0;
    }
    default: return -2;
  }
}

int32_t g_rx_state_executor(uint32_t input)
{
  switch(input)
  {
    case GPS_EOL:
    {
      sleep_task(GPS_RX_TASK);
      sw_timer_enable_channel(GPS_TIMER);
      gps_state = GPS_ST_PROCESS;
      process_nmea();
      return 0;
    }
    case GPS_TIMEOUT:
    {
      sleep_task(GPS_RX_TASK);
      give_mutex();
      GPS_Fix = 0;
      GPS_Satellites = 0;
      DEBUG.println(F("[OBC] GPS READ TIMEOUT"));
      gps_state = GPS_ST_IDLE;
      return 0;
    }
    default: return -2;
  }
}

int32_t g_process_state_executor(uint32_t input)
{
  switch(input)
  {
    case GPS_NO_GGA:
    {
      gps_state = GPS_ST_RX;
      g_retry_cntr();
      return 0;
    }
    case GPS_GGA:
    {
      DEBUG.println(F("[OBC] GPS READ FINISHED"));
      gps_state = GPS_ST_IDLE;
      return 0;
    }
    case GPS_TIMEOUT:
    {
      DEBUG.println(F("[OBC] GPS READ TIMEOUT"));
      GPS_Fix = 0;
      GPS_Satellites = 0;
      gps_state = GPS_ST_IDLE;
      return 0;
    }
    default: return -2;
  }
}

void g_retry_cntr()
{
  if(gps_retries != 0)
  {
    gps_retries--;
    if(gps_retries == 0)
    {
      gps_sm(GPS_TIMEOUT);
    }
  }
}

void gps_timer_cb()
{
  sw_timer_disable_channel(GPS_TIMER);
  give_mutex();
  gps_sm(GPS_TIMEOUT);
}

/*********************************************************************************************/
uint8_t setGPS_DynamicModel6()
{
 int gps_set_sucess=0;
 int wtchdg=0;
 uint8_t setdm6[] = {
 0xB5, 0x62, 0x06, 0x24, 0x24, 0x00, 0xFF, 0xFF, 0x06,
 0x03, 0x00, 0x00, 0x00, 0x00, 0x10, 0x27, 0x00, 0x00,
 0x05, 0x00, 0xFA, 0x00, 0xFA, 0x00, 0x64, 0x00, 0x2C,
 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x16, 0xDC };
 
 while(!gps_set_sucess)
 {
   sendUBX(setdm6, sizeof(setdm6)/sizeof(uint8_t));
   //delay(100);
   gps_set_sucess=getUBX_ACK(setdm6);
   wtchdg++;
   if( wtchdg > 3) //32700 
   {
//      SICL.print("d_error...");
      return 1;
   }
   //delay(100);  
 }
// SICL.print("d_ok...");
  return 0;
}


void sendUBX(uint8_t *MSG, uint8_t len) 
{
// GPS.flush();
 GPS.write(0xFF);
 _delay_ms(500);
 for(int i=0; i<len; i++) 
 {
   GPS.write(MSG[i]);
 }
}

boolean getUBX_ACK(uint8_t *MSG) 
{
 int wtchdg=0; 
 uint8_t b;
 uint8_t ackByteID = 0;
 uint8_t ackPacket[10];
 unsigned long startTime = millis();
 
// Construct the expected ACK packet
 ackPacket[0] = 0xB5; // header
 ackPacket[1] = 0x62; // header
 ackPacket[2] = 0x05; // class
 ackPacket[3] = 0x01; // id
 ackPacket[4] = 0x02; // length
 ackPacket[5] = 0x00;
 ackPacket[6] = MSG[2]; // ACK class
 ackPacket[7] = MSG[3]; // ACK id
 ackPacket[8] = 0; // CK_A
 ackPacket[9] = 0; // CK_B
 
// Calculate the checksums
 for (uint8_t ubxi=2; ubxi<8; ubxi++) 
 {   
   ackPacket[8] = ackPacket[8] + ackPacket[ubxi];
   ackPacket[9] = ackPacket[9] + ackPacket[8];
 }
 
 while (1) 
 {
// Test for success
   if (ackByteID > 9) 
   {
     // All packets in order!
     return true;
   }
   
    // Timeout if no valid response in 3 seconds
   if (millis() - startTime > 3000) 
   {
     return false;
   }
   
  // Make sure data is available to read
   if (GPS.available()) 
   {
     b = GPS.read();
   
  // Check that bytes arrive in sequence as per expected ACK packet
     if (b == ackPacket[ackByteID]) 
     {
       ackByteID++;
     }
     else 
     {
       ackByteID = 0; // Reset and look again, invalid order
     }
   }
 }
}
