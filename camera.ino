int processCMDTAmsg(void)
{
  int i, j, k, IntegerPart;
  char tmp[3];
  int32_t pic=0;
 
  // $TMHKR,C,,,,*47
  //       COM 
  //        1  
 
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
        tmp[k] = bus_msg[i];
        k++;
      }

    }
  }

  if( tmp[0] == 'S')
  {
    //SICL.println("save");
    pic = savePICsd();
    DEBUG.println(F(""));
    //TODO change to switch case
    if(pic == 0)
    {
      DEBUG.println(F("Success"));
      return 0;
    }
    else if(pic == -1)
    {
      DEBUG.println(F("Data overflow"));
    }
    else if(pic == -2)
    {
      DEBUG.println(F("CAM timeout"));
    }
    else if(pic == -3)
    {
      DEBUG.println(F("SD Error"));
    }
    return 0;
  }
  if( tmp[0] == 'E')
  {
    SICL.println(F("$TMCAM,,END*47"));
    return 0;
  }  
}

int32_t savePICsd(void)
{
  char str[10];
  byte buf[256];
  static int i = 0;
  static int k = 0;
  uint8_t temp = 0,temp_last=0;
  bool is_data=false;
  volatile int cam_wtchdg=0;
  int nowtime=0; 

  if( !sdcard_is_card_present())
  {
    return -3; //TODO error codes
  }
  
  //itoa(picnum, str, 10);
  str[0]='\0';
  strcat(str, GPS_time[GPS_valid]);
  strcat(str, ".jpg");

  //Open the new dataFile
  //dataFile = SD.open(str, O_WRITE | O_CREAT | O_TRUNC);
  if(!dataFile.open(str, O_RDWR | O_CREAT ))
  {
    DEBUG.println(F("open dataFile faild"));
    dataFile.close();
    return -3;
  }
  //picnum++;

  
 i = 0;
 cam_wtchdg = millis();
 while(!SICL.available())
 {
    nowtime=millis();
    if(nowtime - cam_wtchdg >6000)
    {        
      dataFile.close();
      return -2;
    }    
 }
/*  while (SICL.available() > 0)
  {
    temp = SICL.read();
  }*/
  temp=0; 
 //Read JPEG data from FIFO
 cam_wtchdg = millis();
 while ( (temp !=0xD9) | (temp_last !=0xFF))
 {
  nowtime=millis();
  if(nowtime - cam_wtchdg >3000)
  {        
    dataFile.close();
    return -2;
  }  
  if (SICL.available() > 0)
  {
    cam_wtchdg = millis();
    temp_last = temp;
    temp = SICL.read();  //Write image data to buffer if not full

      if( i < 256)
       buf[i++] = temp;
       else{
        //Write 256 bytes image data to dataFile
    
        dataFile.write(buf ,256);
        i = 0;

        //timeout in case of corrupt dadaflow; picture data larger than 80kB -> must be tested and calibrated
        if(k>320)
        {
          dataFile.close(); //close dataFile
          return -1;
        }
        
        buf[i++] = temp;
    
       }
 
     SICL.print(F("o")); 
     delay(0);  
  }
 }
 
 //Write the remain bytes in the buffer
 if(i > 0){

  dataFile.write(buf,i);
 }
 //Close the dataFile
 dataFile.close();
  

  return 0;  
}

void getPICuart()
{
  int timer=0;
  int nowtime=0;
  int getmsg=10;
  

  //SICL.listen();
  //SICL.println("d");
  setBusBusy();
  delay(500);  
  SICL.println(F("$TMCAM,1,CAP*47"));

  sw_timer_enable_channel(BUS_TIMER);
  bus_sm(BUS_PROCESS);
        
   // SICL.print(".");
 
  
}
