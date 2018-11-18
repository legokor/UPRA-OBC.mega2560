/*
 * SD Card variables
 */

bool card_present = false;
bool obc_log_present = false;
bool track_log_present = false;
bool com_log_present = false;


int32_t sdcard_init()
{
  DEBUG.print(F("[OBC] SD Card init..."));

  pinMode(SD_CS, OUTPUT);
  digitalWrite(SD_CS, HIGH);
  
  if (!sd.begin(SD_CS, SPI_HALF_SPEED)) 
  {
    DEBUG.println(F("NO SD Card present"));
    card_present = false;
    return -1; //TODO error codes
  }
  DEBUG.println(F("OK"));      
  
  DEBUG.println(F("[OBC] create log files:"));
  sd_init_obc_log();
  sd_init_com_log();
  sd_init_track_log();
  
  card_present = true;
  DEBUG.println(F("DONE"));      
  return 0;
}

int32_t sd_init_obc_log()
{
  DEBUG.print(F("...init OBC log..."));

  if (dataFile.open("obc_hk.csv", O_RDWR | O_CREAT | O_AT_END))
  {
    dataFile.println(F("time,ext_temp,pcb_temp"));
    dataFile.close();
    DEBUG.println(F("OK"));      
    dataFile.close();
    obc_log_present = true;
    return 0;
  }   
  
  dataFile.close();
  DEBUG.println(F("File error!"));
  obc_log_present = false;
  return -1; //TODO error codes
}

int32_t sd_init_com_log()
{
  DEBUG.print(F("...init COM log..."));

  if (dataFile.open("com_hk.csv", O_RDWR | O_CREAT | O_AT_END))
  {
    dataFile.println(F("time,COM_temp,sent_msg"));
    dataFile.close();
    DEBUG.println(F("OK"));      
    dataFile.close();
    com_log_present = true;
    return 0;
  }   
  
  dataFile.close();
  DEBUG.println(F("File error!"));
  com_log_present = false;
  return -1; //TODO error codes
}

int32_t sd_init_track_log()
{
  DEBUG.print(F("...init TRACK log..."));

  if (dataFile.open("gps.csv", O_RDWR | O_CREAT | O_AT_END))
  {
    dataFile.println(F("GPS time,latitude,longitude,altitude,GPS fix,satellites"));
    dataFile.close();
    DEBUG.println(F("OK"));      
    dataFile.close();
    track_log_present = true;
    return 0;
  }   
  
  dataFile.close();
  DEBUG.println(F("File error!"));
  track_log_present = false;
  return -1; //TODO error codes
}

bool sdcard_is_card_present()
{
  return card_present;
}

int sdcard_push_obc_log(void)
{
  if( !obc_log_present)
  {
    return -1;
  }

  take_mutex();
  DEBUG.print(F("[OBC] STORE OBC HK..."));
  // if the file is available, write to it:
  if (dataFile.open("obc_hk.csv", O_RDWR | O_CREAT | O_AT_END)) 
  {
    dataFile.println(obc_hk_buff);      

    dataFile.close();

    DEBUG.println(F("OK"));
    give_mutex();
    return 0;
  }
  dataFile.close();
  DEBUG.println(F("file error!"));
  give_mutex();
  return -2;
}

int sdcard_push_com_log(void)
{
  if( !com_log_present)
  {
    return -1;
  }

  take_mutex();
  DEBUG.print(F("[OBC] STORE COM HK..."));
  // if the file is available, write to it:
  if (dataFile.open("com_hk.csv", O_RDWR | O_CREAT | O_AT_END)) 
  {
    dataFile.println(radio_hk_buff);      
    dataFile.close();

    DEBUG.println(F("OK"));
    give_mutex();
    return 0;
  }
  dataFile.close();
  DEBUG.println(F("file error!"));
  give_mutex();
  return -2;
}

int sdcard_push_track_log(void)
{
  if( !track_log_present)
  {
    return -1;
  }

  take_mutex();
  DEBUG.print(F("[OBC] STORE GPS TRACK..."));
  // if the file is available, write to it:
  if (dataFile.open("gps.csv", O_RDWR | O_CREAT | O_AT_END)) 
  {
    dataFile.print(GPS_time[GPS_valid]);
    dataFile.print(F(","));
    dataFile.print(GPS_lati[GPS_valid]);
    dataFile.print(F(","));
    dataFile.print(GPS_long[GPS_valid]);
    dataFile.print(F(","));
    dataFile.print(GPS_Altitude[GPS_valid]);
    dataFile.print(F(","));
    dataFile.print(GPS_Fix);
    dataFile.print(F(","));
    dataFile.println(GPS_Satellites);
    dataFile.close();

    DEBUG.println(F("OK"));
    give_mutex();
    return 0;
  }
  dataFile.close();
  DEBUG.println(F("file error!"));
  give_mutex();
  return -2;
}

/*
int read_config(void)
{
  File dataFile = SD.open("config1.cfg");

  if (dataFile) 
  {
    // read from the file until there's nothing else in it:
    int i=0;
    while (dataFile.available()) 
    {
      config_data[i]=dataFile.read();
      i++;
    }
    // close the file:
    dataFile.close();
    return 0;
  } else 
  {
    return 2;
    // if the file didn't open, print an error:
    
  }
  return 1;
}

void parseConfig(void)
{
    TERMINATION=0;
    callsign = "";
    int j=0;
    for(int i=0; config_data[i] != 'e'; i++)
    {
      if( config_data[i] == '\r')
      {
        j++;
        i+=2;
      }
      if( j == 0)
      {
        callsign += String(config_data[i]);        
      }
      else if( j == 1)
      {
        TERMINATION = TERMINATION * 10;
        TERMINATION += (config_data[i] - '0');
      }
    }  
}*/

