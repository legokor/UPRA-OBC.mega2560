void serialFlush(){
  while(SICL.available() > 0) {
    char t = SICL.read();
  }
  for(int i=0; i<30; i++)
  {
    bus_msg[i]=0;
  }
} 

void parseRadioHK()
{
  int i, j, k, IntegerPart;


 
  // $TCHKD,0123,336*47
  //        temp  vcc
  //           1   2
 
  IntegerPart = 1;

 
  for (i=0, j=0, k=0; (i<MSGindex) && (j<10); i++) // We start at 7 so we ignore the '$GPGGA,'
  {
    if (bus_msg[i] == ',')
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
    }
    
   // GPS_Altitude = Altitude;
  }
  radio_temp[3] ='\0';  

}

int parseRequestHandshake(void)
{
 
}

int parseEOTHandshake(void)
{

}


int GetRadioHousekeeping(void)
{
  int inByte;
  int i=0;
  int msg_index=0;
  int msg_code = 0;
  int timer=0;
  int nowtime=0;
  int getmsg=10;
  
  ////////////SICL.listen();  
  DEBUG.println(F("OBC: Get Radio Housekeeping"));  
  delay(100);
  setBusBusy();
  delay(500);
  SICL.println(F("$TMHKR,C,,,,*47"));

  timer=millis();
  //SICL.println(timer);

  ////////////SICL.listen();  
 // serialFlush();
  while(getmsg !=0)
  {
    getmsg=GetBusMSG();
    
    nowtime=millis();
    if(nowtime - timer >4000)
    {
      DEBUG.println(F("OBC: COM HK timeout"));
      break;
    }
    
        
   // SICL.print(".");
  }
  clrBusBusy();
}

int SendRadioTelemetry(void)
{
 
}


/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////

void lowSpeedTelemetry(void)
{
      unsigned long timer=0;
      long  GPS_Alt_tmp;
      char buffer[10];
      GetRadioHousekeeping();
      DEBUG.print(F("OBC: COM temp: "));
      DEBUG.println(radio_temp);
//      Serial.begin(57600);
      ////////////SICL.listen(); 
      //getTemperatures();
      
      delay(500);
//      Serial.println("");
//      debugLOG();
        setBusBusy();
        delay(500);
      

        GPS_Alt_tmp = GPS_Altitude;
        delay(200);
        SICL.print(F("$TMLTM,"));
        SICL.print(GPS_time);
        SICL.print(F(","));
        SICL.print(GPS_lati);
        SICL.print(F(","));
        SICL.print(GPS_long);
        SICL.print(F(","));
        //SICL.print(GPS_Alt_ch);
        sprintf(buffer, "%05ld",GPS_Alt_tmp);
        SICL.print(buffer);
        SICL.print(F(","));
        sprintf(buffer, "%04d",(int)(ext_temp*10.0));
        SICL.print(buffer);
        SICL.print(F(","));
        sprintf(buffer, "%04d",(int)(int_temp*10.0));
        SICL.print(buffer);
        SICL.println(F("*47"));    

        clrBusBusy();
}

/*void lowSpeedStartup(void)
{
      unsigned long timer=0;
     
//      SICL.begin(57600); 
      //////SICL.listen();
      GetRadioHousekeeping();
      
      
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
