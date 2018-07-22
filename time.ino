void time_init(void)
{
  cli();//stop interrupts

//set timer1 interrupt at 1Hz
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0
  // set compare match register for 1hz increments
  OCR1A = 15624;// = (16*10^6) / (1*1024) - 1 (must be <65536)
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS10 and CS12 bits for 1024 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10);  
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);  

  sei();//allow interrupts
}

void time_set(void)
{
  OBC_time.hour   = ((GPS_time[0] - '0') * 10 ) +  (GPS_time[1] - '0');
  OBC_time.minute = ((GPS_time[2] - '0') * 10 ) +  (GPS_time[3] - '0');
  OBC_time.second = ((GPS_time[4] - '0') * 10 ) +  (GPS_time[4] - '0');
  OBC_time.OBC_time = (OBC_time.hour * 3600 ) + (OBC_time.minute * 60 ) + OBC_time.second;
}

void time_refresh(void)
{
  OBC_time.OBC_time++;
  OBC_time.hour   = OBC_time.OBC_time / 3600;
  OBC_time.minute = (OBC_time.OBC_time - (OBC_time.hour * 3600 ) ) / 60;
  OBC_time.second = (OBC_time.OBC_time - (OBC_time.hour * 3600 ) ) - (OBC_time.minute * 60 ) ;
}

uint32_t time_get(void)
{
    
}

ISR(TIMER1_COMPA_vect)
{
  time_refresh();
//  DEBUG.print(OBC_time.hour);
//  DEBUG.print(OBC_time.minute);
//  DEBUG.println(OBC_time.second);
//  DEBUG.println(OBC_time.OBC_time);


}
