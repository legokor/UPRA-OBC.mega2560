void getGPSMeasurement(void)
{
  uint8_t gps_error=10;
  volatile int gps_wtchdg=0;
  int nowtime=0;

/*#ifdef NEO6_GPS  
  GPS.begin(9600);
  SetupUBLOX(void);
#elif defined(TYCO)
  GPS.begin(4800);
#elif defined(Soft_GPS)
  GPS.begin(9600);

#endif
*/
  gps_wtchdg = millis();
//  Serial.println(gps_wtchdg);
  DEBUG.print("OBC: GPS read...");
  ////GPS.listen(); 
  GPS.begin(GPS_BAUD);
  while((gps_error !=0))// || (gps_wtchdg < 10000))
  {
//    Serial.print(".");
    gps_error=CheckGPS();
    //gps_wtchdg++;
    nowtime=millis();
    if(nowtime - gps_wtchdg >3000)
    {
      DEBUG.print("timeout");
      break;
    }

  //  Serial.println(nowtime);
  }
  DEBUG.println("...end");
  gps_wtchdg = 0;  
  if( GPS_Altitude > 1000)
  {
    is_climb = true; 
    is_landing = false;
  }
  if( (GPS_Altitude < 500) && (is_climb))
  {
    is_landing = true;
  }
  if( (GPS_Altitude > 500) && (is_landing))
  {
    is_landing = false;
  }

  UDR0=0;
  UCSR0A &= !(1<<UDRE0);
  SICL.begin(SICL_BAUD);
//  DEBUG.begin(DEBUG_BAUD); // TODO: UNCOMMENT THIS FOR RELEASE
}

void getTemperatures(void)
{
  DEBUG.println(F("OBC: COLLECT SENSOR DATA"));
  //getGPSMeasurement();
  
  ext_temp = getExtTemp();
  int_temp = getIntTemp();
}

void debugLOG(void)
{
  DEBUG.print("time:      ");DEBUG.println(GPS_time);
  DEBUG.print("latitude:  ");DEBUG.println(GPS_lati);
  DEBUG.print("longitude: ");DEBUG.println(GPS_long);
  DEBUG.print("altitude:  ");DEBUG.println(GPS_Altitude);
  DEBUG.print("external:  ");DEBUG.println(ext_temp);
  DEBUG.print("internal:  ");DEBUG.println(int_temp);
  DEBUG.print("radio:  ");DEBUG.println(radio_temp);
}

