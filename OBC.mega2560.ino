#include <SPI.h>
#include <SdFat.h>

#include <stdio.h>
#include <util/delay.h>
#include <avr/pgmspace.h>

#include "service_mode.h"
#include "task_mngr.h"
#include "sdcard.h"
#include "time.h"
#include "gps.h"
#include "task_init.h"
#include "temperature.h"
#include "timer.h"
#include "radio.h"
#include "sw_timer.h"
#include "bus.h"
#include "get_hk.h"

/*
 * Timing macros
 */
 #define GETGPS       1   // activate GPS read every GETGPS)*5s                   (0-> never, 1-> every 5second)
 #define GETTEMP      15  // activate Temperature measurement every GETTEMP)*5s   (0-> never, 1-> every 5second)
 #define GETICS       2  // activate ICS capture every GETICS)*5s                (0-> never, 1-> every 5second)
 #define RADIO        3   // activate telemetry downlink every RADIO*5s           (0-> never, 1-> every 5second)
 //#define TERMINATION  cut_down_alt

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


/*
 * Radio variables
 */

//char radio_handshake[65];



/*
 * Timing variables
 */

time_t obc_time;

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

int ledon = 1;
void task_toggle_led()
{
  if(ledon == 0)
  {
    digitalWrite(7, LOW);
    ledon = 1;
  }
  else
  {
    digitalWrite(7, HIGH);
    ledon = 0;
  }
}

 
void setup() 
{
  // put your setup code here, to run once:
  pinMode(7, OUTPUT);
  digitalWrite(7, LOW);

  sw_timer_init();
  
  //UART port for COM module
  bus_init();
//  DEBUG.begin(DEBUG_BAUD); // TODO: UNCOMMENT THIS FOR RELEASE
  DEBUG.println(F("[OBC] startup"));

//  ecam_init();
//  ecam_ON();
  time_init(&obc_time);

  
  DEBUG.println(F("[OBC] init COM..."));
  //SICL.listen();
  radio_init();
  
  //wait for COM startup or timeout

  
  DEBUG.print(F("[OBC] init GPS..."));
  
  int32_t gps_error;
  gps_error = gps_init();

  switch(gps_error)
  {
    case 0 :
    {
      DEBUG.println(F("OK"));
      break;
    }
    case -1 :
    {
      DEBUG.println(F("ERROR!: Airborne mode"));
      break;    
    }
    default :
    {
      DEBUG.println(F("ERROR!"));
      break;    
    }
  }
  
  delay(500);
  //SICL.listen();
  //Pin configurations
  pinMode(BUZZ, OUTPUT);
  pinMode(FTU, OUTPUT);
  pinMode(CS, OUTPUT);
  digitalWrite(CS, HIGH);
  digitalWrite(FTU, LOW);

  delay(500);

  //Test Buzzer
  buzzerTest();
  //Debug FTU
/*        digitalWrite(FTU, HIGH);
        delay(1000);
        digitalWrite(FTU, LOW);  
  while(1);
  */
  //SD CARD init
  sdcard_init();

  task_init();
  //send startup msg
  DEBUG.println(F("[OBC] Init done"));
  start_scheduler();
}

void loop() 
{
  // put your main code here, to run repeatedly:

  DEBUG.println(F("[OBC] TASK MANAGER ERROR"));
  while(1);

//  if(busBusy_interrupt())
//  {
////    SICL.println(F("[OBC] BUS INTERRUPT"));
//    //send ACK message
//    //get BUS message
//  }
//  
//  if( ((millis() - now) > 5000) && (!is_landing))
//  {
//    ecam_timer=0;
//    timing();
//    if(is_temp == GETTEMP)
//    {
//      getTemperatures();
//      is_temp = 0;
//    }
//
//    if(is_gps == GETGPS)
//    {
//      //getTemperatures();
// 
//      getGPSMeasurement();
//      delay(50);
//
////---------------datalog---------------
//
//      sdcard_push_obc_log();
//          
////---------------datalogend------------
//
//      
//      is_gps=0;
//    }
//    if(is_radio == RADIO)
//    {
//      radio_beacon_tx();
//      is_radio=0;
//    }
//
//    if(is_ics == GETICS)
//    {
////      DEBUG.print(obc_time.hour);
////      DEBUG.print(obc_time.minute);
////      DEBUG.println(obc_time.second);
////      DEBUG.println(obc_time.obc_time);
//      getPICuart();
//      is_ics = 0;
//    }
//    
///*    if(( GPS_Altitude > TERMINATION) && (!is_FTU_on))
//    {
//        digitalWrite(FTU, HIGH);
//        delay(1000);
//        digitalWrite(FTU, LOW);
//        is_FTU_on = true;
//    }*/
//   // timing();
//  }
//  
//  if(is_landing)
//  {
//    DEBUG.println(F("[OBC] BEACON MODE"));
//    buzzer();
//    getGPSMeasurement();
//    sdcard_push_obc_log();
//    radio_beacon_tx();
//   // delay(1000);
//  }


}
