#include <SPI.h>
#include <SdFat.h>

#include <stdio.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <mcp_can.h>


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
#include "operation_modes.h"
#include "can.h"

/*
 * Timing macros
 */
 #define GETGPS       1   // activate GPS read every GETGPS)*5s                   (0-> never, 1-> every 5second)
 #define GETTEMP      15  // activate Temperature measurement every GETTEMP)*5s   (0-> never, 1-> every 5second)
 #define GETISD_CS       2  // activate ISD_CS capture every GETISD_CS)*5s                (0-> never, 1-> every 5second)
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



/*
 * Radio variables
 */

//char radio_handshake[65];



/*
 * Timing variables
 */

time_t obc_time;
can_ltm_t ltm_msg;
can_com_hk_t can_com_hk_msg;

unsigned long now;

//bool is_FTU_on=false;

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
  set_operation_mode(OP_MODE_INIT);
  
  sub_module_state = 0;

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

  can_init();
  
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
  set_operation_mode(OP_MODE_SAFE);
  wake_up_task(SAFE_MODE_TASK);
  start_scheduler();
}

void loop() 
{
  // put your main code here, to run repeatedly:

  DEBUG.println(F("[OBC] TASK MANAGER ERROR"));
  while(1);

}
