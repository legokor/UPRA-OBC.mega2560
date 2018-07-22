#include <SPI.h>
#include <SdFat.h>

//#include <SoftwareSerial.h>
//#include <AltSoftSerial.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/pgmspace.h>

/*
 * Timing macros
 */
 #define GETGPS       1   // activate GPS read every GETGPS)*5s                   (0-> never, 1-> every 5second)
 #define GETTEMP      15  // activate Temperature measurement every GETTEMP)*5s   (0-> never, 1-> every 5second)
 #define GETICS       2  // activate ICS capture every GETICS)*5s                (0-> never, 1-> every 5second)
 #define RADIO        3   // activate telemetry downlink every RADIO*5s           (0-> never, 1-> every 5second)
 //#define TERMINATION  cut_down_alt

/*
 * Temperature macros
 */

#define AVRG 10
#define INTERNALOFFSET 0
#define INTERNALCALIB 102.71
#define INTERNALGAIN 2.69

/*
 * PIN macros
 */

#define BUZZ    5         //BUZZER
#define FTU     4         //FLIGHT TERMINATION ENABLE
#define GPS_RX  17         //GPS TO OBC            
#define GPS_TX  16         //OBC TO GPS
#define SDA     18        //I2C SDA        
#define SCL     19        //I2C SCL

//EXTERNAL CAMERA
#define MENU    6
#define SHTR    7

//SD CARD - SPI
#define MOSI    51        //MOSI
#define MISO    50        //MISO
#define CLK     52        //CLK        
#define CS      26        //CHIP SELECT

//UPRA BUS
#define DEBUG_BAUD  9600
#define SICL_BAUD   9600
#define GPS_BAUD    4800    //NEO6: 9600, PC: 9600, TYCO(for test): 4800
#define SICLRX      0
#define SICLTX      1
#define BUSBUSY     A0

#define GPS       Serial2
#define SICL   Serial    //later Serial1
#define DEBUG     Serial


/*
 * GPS variables
 */

//SoftwareSerial GPS(GPS_RX, GPS_TX); // RX, TX
//SoftwareSerial SICL(SICLRX,SICLTX);
//AltSoftSerial GPS;

byte              GPSBuffer[82];
byte              GPSIndex=0;
uint8_t           GPS_Satellites=0;
long              GPS_Altitude=0;

uint8_t           GPS_Fix=0;
char              GPS_time[10];
char              GPS_lati[12];
char              GPS_long[13];

/*
 * SD Card variables
 */

bool card_present=false;

SdFat sd;
SdFile dataFile;

/*
 * Temperature variables
 */

volatile float ext_temp = 0.0;
volatile double int_temp =0.0;

/*
 * Radio variables
 */

byte MSGindex=0;
char bus_msg[30];
//char radio_handshake[65];

char radio_temp[4];


/*
 * Timing variables
 */

typedef struct
{
  uint32_t hour;
  uint32_t minute;
  uint32_t second;
  uint32_t OBC_time;
}time_t;

time_t OBC_time;

unsigned long now;
uint8_t is_gps=0;
uint8_t is_temp=0;
uint8_t is_radio=0;
uint8_t is_ics=0;

uint8_t ecam_timer=0;
bool is_ecam_on=false;

bool is_climb=false;
bool is_landing=false;
//bool is_FTU_on=false;

bool is_com_present=true;

void timing()
{
  now = millis();
  if( !is_landing )
  {
    is_gps++;
    is_temp++;
    is_radio++;
    is_ics++;
  }
}

/*
 * Logging variables
 */

//String dataString = "";
//String radioString = "";
//const String callsign PROGMEM = "HAxUPRA";
//File dataFile;
/*
 * Configuration variables
 */

//bool is_config_read=false;
//char config_data[100];
 
void setup() 
{
  // put your setup code here, to run once:

  GPS_time[0] = '3';
  GPS_time[1] = '3';
  GPS_time[2] = '6';
  GPS_time[3] = '6';
  GPS_time[4] = '7';
  GPS_time[5] = '7';
  GPS_time[6] = '\0';

  GPS_lati[0] = '+';
  GPS_lati[1] = '9';
  GPS_lati[2] = '5';
  GPS_lati[3] = '0';
  GPS_lati[4] = '0';
  GPS_lati[5] = '.';
  GPS_lati[6] = '0';
  GPS_lati[7] = '0';
  GPS_lati[8] = '0';
  GPS_lati[9] = '\0';

  GPS_long[0] = '+';
  GPS_long[1] = '1';
  GPS_long[2] = '8';
  GPS_long[3] = '8';
  GPS_long[4] = '8';
  GPS_long[5] = '8';
  GPS_long[6] = '.';
  GPS_long[7] = '0';
  GPS_long[8] = '0';
  GPS_long[9] = '0';
  GPS_long[10] = '\0';

  radio_temp[0] = 'N';
  radio_temp[1] = '/';
  radio_temp[2] = 'A';
  radio_temp[3] = '\0';
  
  //UART port for COM module
  SICL.begin(SICL_BAUD);
//  DEBUG.begin(DEBUG_BAUD); // TODO: UNCOMMENT THIS FOR RELEASE
  DEBUG.println(F("OBC: startup"));

  ecam_init();
  ecam_ON();

  time_init();
  time_set();

  
  DEBUG.print(F("OBC: init COM..."));
  //SICL.listen();
  
  //wait for COM startup or timeout
  now = millis();
  while(!SICL.available())
  {
    if((millis() - now) > 10000)
    {
      is_com_present=false;
      break;
    }
  }
  now=0;
  if(is_com_present)
  {
    delay(1000);
    DEBUG.println(F("OK"));
  }
  else
  {
    DEBUG.println(F("COM TIMEOUT"));
  }

  
  DEBUG.print(F("OBC: init GPS..."));

  GPS.begin(GPS_BAUD);
  int GPS_stat = SetupUBLOX();
  if(GPS_stat == 1)
  {
    //SICL.listen();
    DEBUG.println(F("ERROR!: Airborne mode"));
  }
  else
  {
    //SICL.listen();
    DEBUG.println(F("OK"));
  }
  
  delay(500);
  //SICL.listen();
  //Pin configurations
  pinMode(BUSBUSY, INPUT);
  pinMode(BUZZ, OUTPUT);
  pinMode(FTU, OUTPUT);
  pinMode(CS, OUTPUT);
  digitalWrite(CS, HIGH);
  digitalWrite(FTU, LOW);

  delay(500);

  //Test Buzzer
  DEBUG.print(F("OBC: BUZZER TEST..."));
  buzzerTest();
  DEBUG.println(F("OK"));
  //Debug FTU
/*        digitalWrite(FTU, HIGH);
        delay(1000);
        digitalWrite(FTU, LOW);  
  while(1);
  */
  //SD CARD init
  if (!sd.begin(CS, SPI_HALF_SPEED)) 
  {
    DEBUG.println(F("OBC: NO SD Card"));
    card_present=false;
  }
  else
  {
    DEBUG.print(F("OBC: create log file..."));
    card_present=true;
    if (dataFile.open("data.csv", O_RDWR | O_CREAT | O_AT_END))
    {
      dataFile.println(F("time,latitude,longitude,altitude,ext_temp,OBC_temp,COM_temp"));
      dataFile.close();
      DEBUG.println(F("OK"));      
    }   
    else 
    {
      card_present=false;
      dataFile.close();
      DEBUG.println(F("File error!"));
    }    
  }
  dataFile.close();

  
  //send startup msg
  DEBUG.println(F("OBC: Init done"));

  //start external camera video
  DEBUG.println(F("OBC: ECAM START"));
  ecam_PictureMode();
  ecam_pressSHTR(500);

   
  //set startup time
  now = millis();  

  //Startup measurement and radio
  getTemperatures();
  getGPSMeasurement();
  delay(50);
  lowSpeedTelemetry();
}

void loop() 
{
  // put your main code here, to run repeatedly:
  if(busBusy_interrupt())
  {
//    SICL.println(F("OBC: BUS INTERRUPT"));
    //send ACK message
    //get BUS message
  }
  
  if( ((millis() - now) > 5000) && (!is_landing))
  {
    ecam_timer=0;
    timing();
    if(is_temp == GETTEMP)
    {
      getTemperatures();
      is_temp = 0;
    }

    if(is_gps == GETGPS)
    {
      //getTemperatures();
 
      getGPSMeasurement();
      delay(50);

//---------------datalog---------------

      dumpLog();
          
//---------------datalogend------------

      ecam_pressSHTR(500);
      
      is_gps=0;
    }
    if(is_radio == RADIO)
    {
      lowSpeedTelemetry();
      is_radio=0;
    }

    if(is_ics == GETICS)
    {
//      DEBUG.print(OBC_time.hour);
//      DEBUG.print(OBC_time.minute);
//      DEBUG.println(OBC_time.second);
//      DEBUG.println(OBC_time.OBC_time);
      getPICuart();
      is_ics = 0;
    }
    
/*    if(( GPS_Altitude > TERMINATION) && (!is_FTU_on))
    {
        digitalWrite(FTU, HIGH);
        delay(1000);
        digitalWrite(FTU, LOW);
        is_FTU_on = true;
    }*/
   // timing();
  }
  
  if(is_landing)
  {
    DEBUG.println(F("OBC: BEACON MODE"));
    buzzer();
    getGPSMeasurement();
    dumpLog();
    lowSpeedTelemetry();
    if( ((millis() - now) > 5000) && (is_ecam_on))
    {
      timing();
      ecam_pressSHTR(500);
      ecam_timer++;
    }
    if( (ecam_timer > 120 ) && (is_ecam_on)) //120 for 10mins
    {
      ecam_OFF();
    }
   // delay(1000);
  }


}
