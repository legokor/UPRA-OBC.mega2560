time_t *subtimestamp;

void time_init(time_t* device)
{
  timer_initstruct_t timer_InitStruct;
  cli();//stop interrupts

  timer_InitStruct.timer = TIMER_1;
  timer_InitStruct.period = 1; //[Hz]
  timer_InitStruct.callback = &time_refresh_from_isr;

  timer_init(&timer_InitStruct);

  subtimestamp = device;
  subtimestamp->hour   = 0;
  subtimestamp->minute = 0;
  subtimestamp->second = 0;
  subtimestamp->timestamp = 0;

  gps_time_sync = 0;

  sei();//allow interrupts
}

void time_set_gps_time(time_t *device)
{
  enter_atomic();
  device->hour   = ((GPS_time[GPS_valid][0] - '0') * 10 ) +  (GPS_time[GPS_valid][1] - '0');
  device->minute = ((GPS_time[GPS_valid][2] - '0') * 10 ) +  (GPS_time[GPS_valid][3] - '0');
  device->second = ((GPS_time[GPS_valid][4] - '0') * 10 ) +  (GPS_time[GPS_valid][4] - '0');
  device->timestamp = (device->hour * 3600 ) + (device->minute * 60 ) + device->second;
  exit_atomic();
}

int32_t time_set(time_t* device, uint32_t hour, uint32_t minute, uint32_t second)
{
  if( hour > 23) return -1;
  if( minute > 59) return -1;
  if( second > 59) return -1;

  enter_atomic();
  device->hour   = hour;
  device->minute = minute;
  device->second = second;
  device->timestamp = (device->hour * 3600 ) + (device->minute * 60 ) + device->second;
  exit_atomic();
  return 0;
}

int32_t time_set_timestamp(time_t* device, uint32_t timestamp)
{
  if(timestamp > 86399) return -1;
  
  enter_atomic();
  device->timestamp = timestamp;
  device->hour   = device->timestamp / 3600;
  device->minute = (device->timestamp - (device->hour * 3600 ) ) / 60;
  device->second = (device->timestamp - (device->hour * 3600 ) ) - (device->minute * 60 ) ;
  exit_atomic();
  return 0;
}

void time_refresh_from_isr(void)
{
  task_toggle_led();
  gps_time_sync++;
  if( (subtimestamp->timestamp++) > 86399)
  {
    subtimestamp->timestamp = 0;
  }
  subtimestamp->hour   = subtimestamp->timestamp / 3600;
  subtimestamp->minute = (subtimestamp->timestamp - (subtimestamp->hour * 3600 ) ) / 60;
  subtimestamp->second = (subtimestamp->timestamp - (subtimestamp->hour * 3600 ) ) - (subtimestamp->minute * 60 ) ;
//  DEBUG.print(subtimestamp->hour);
//  DEBUG.print(subtimestamp->minute);
//  DEBUG.println(subtimestamp->second);
//  DEBUG.println(subtimestamp->timestamp);
}

uint32_t time_get(uint32_t* hour, uint32_t* minute, uint32_t* second)
{
  enter_atomic();
  *hour = subtimestamp->hour;
  *minute = subtimestamp->minute;
  *second = subtimestamp->second;
  exit_atomic();
}

//ISR(TIMER1_COMPA_vect)
//{
//  task_toggle_led();
//  time_refresh_from_isr();
////  DEBUG.print(subtimestamp->hour);
////  DEBUG.print(subtimestamp->minute);
////  DEBUG.println(subtimestamp->second);
////  DEBUG.println(subtimestamp->timestamp);
//
//
//}
