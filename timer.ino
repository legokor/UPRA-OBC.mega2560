void (*timer1_callback)(void);
void (*timer3_callback)(void);
void (*timer4_callback)(void);
void (*timer5_callback)(void);


void timer_init(timer_initstruct_t* InitStruct)
{
  cli();//stop interrupts
  uint16_t period;

//  period = InitStruct->period;
  period = (uint16_t)( ( 16000000 / (InitStruct->period)) / (1024)); 

  switch(InitStruct->timer)
  {
    case TIMER_1:
    {
      TCCR1A = 0;// set entire TCCR1A register to 0
      TCCR1B = 0;// same for TCCR1B
      TCNT1  = 0;//initialize counter value to 0
      // set compare match register for 1kHzincrements
      OCR1A =  (uint16_t)period; 
      // turn on CTC mode
      TCCR1B |= (1 << WGM12);
      // Set CS10 and CS12 bits for 1024 prescaler
      TCCR1B |= (1 << CS12) | (1 << CS10);  
      // enable timer compare interrupt
      timer1_callback = InitStruct->callback;
      TIMSK1 |= (1 << OCIE1A);  
      break;
    }
    case TIMER_3:
    {
      TCCR3A = 0;// set entire TCCR1A register to 0
      TCCR3B = 0;// same for TCCR1B
      TCNT3  = 0;//initialize counter value to 0
      // set compare match register for 1kHzincrements
      OCR3A =  (uint16_t)period;
      // turn on CTC mode
      TCCR3B |= (1 << WGM12);
      // Set CS10 and CS12 bits for 1024 prescaler
      TCCR3B |= (1 << CS12) | (1 << CS10);  
      // enable timer compare interrupt
      timer3_callback = InitStruct->callback;
      TIMSK3 |= (1 << OCIE1A);  
      break;
    }
    case TIMER_4:
    {
      TCCR4A = 0;// set entire TCCR1A register to 0
      TCCR4B = 0;// same for TCCR1B
      TCNT4  = 0;//initialize counter value to 0
      // set compare match register for 1kHzincrements
      OCR4A =  (uint16_t)period;
      // turn on CTC mode
      TCCR4B |= (1 << WGM12);
      // Set CS10 and CS12 bits for 1024 prescaler
      TCCR4B |= (1 << CS12) | (1 << CS10);  
      // enable timer compare interrupt
      timer4_callback = InitStruct->callback;
      TIMSK4 |= (1 << OCIE1A);  
      break;
    }
    case TIMER_5:
    {
      TCCR5A = 0;// set entire TCCR1A register to 0
      TCCR5B = 0;// same for TCCR1B
      TCNT5  = 0;//initialize counter value to 0
      // set compare match register for 1kHzincrements
      OCR5A =  (uint16_t)period;
      // turn on CTC mode
      TCCR5B |= (1 << WGM12);
      // Set CS10 and CS12 bits for 1024 prescaler
      TCCR5B |= (1 << CS12) | (1 << CS10);  
      // enable timer compare interrupt
      timer5_callback = InitStruct->callback;
      TIMSK5 |= (1 << OCIE1A);  
      break;
    }
    default: break;
  }

sei();//allow interrupts

}

#ifndef TMR1
ISR(TIMER1_COMPA_vect)
{
  timer1_callback();
}
#endif

#ifndef TMR3
ISR(TIMER3_COMPA_vect)
{
  timer3_callback();
}
#endif

#ifndef TMR4
ISR(TIMER4_COMPA_vect)
{
  timer4_callback();
}
#endif

#ifndef TMR5
ISR(TIMER5_COMPA_vect)
{
  timer5_callback();
}
#endif
