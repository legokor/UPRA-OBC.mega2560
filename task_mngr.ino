void *t_call_stack[MAX_TASK_NUMBER];
uint32_t t_call_cntr[MAX_TASK_NUMBER];
uint32_t t_task_period[MAX_TASK_NUMBER];
task_state_e t_task_state[MAX_TASK_NUMBER];
uint8_t t_mutex;
uint8_t t_scheduler_tick;

void init_scheduler(void)
{
  cli();//stop interrupts

#ifdef TMR1
//set timer1 interrupt at 1Hz
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0
  // set compare match register for 1kHzincrements
  OCR1A =  156; 
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS10 and CS12 bits for 1024 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10);  
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);  
#endif

#ifdef TMR3
//set timer1 interrupt at 1Hz
  TCCR3A = 0;// set entire TCCR1A register to 0
  TCCR3B = 0;// same for TCCR1B
  TCNT3  = 0;//initialize counter value to 0
  // set compare match register for 1kHzincrements
  OCR3A =  156;
  // turn on CTC mode
  TCCR3B |= (1 << WGM12);
  // Set CS10 and CS12 bits for 1024 prescaler
  TCCR3B |= (1 << CS12) | (1 << CS10);  
  // enable timer compare interrupt
  TIMSK3 |= (1 << OCIE1A);  
#endif

#ifdef TMR4
//set timer1 interrupt at 1Hz
  TCCR4A = 0;// set entire TCCR1A register to 0
  TCCR4B = 0;// same for TCCR1B
  TCNT4  = 0;//initialize counter value to 0
  // set compare match register for 1kHzincrements
  OCR4A =  156;
  // turn on CTC mode
  TCCR4B |= (1 << WGM12);
  // Set CS10 and CS12 bits for 1024 prescaler
  TCCR4B |= (1 << CS12) | (1 << CS10);  
  // enable timer compare interrupt
  TIMSK4 |= (1 << OCIE1A);  
#endif

#ifdef TMR5
//set timer1 interrupt at 1Hz
  TCCR5A = 0;// set entire TCCR1A register to 0
  TCCR5B = 0;// same for TCCR1B
  TCNT5  = 0;//initialize counter value to 0
  // set compare match register for 1kHzincrements
  OCR5A =  156;
  // turn on CTC mode
  TCCR5B |= (1 << WGM12);
  // Set CS10 and CS12 bits for 1024 prescaler
  TCCR5B |= (1 << CS12) | (1 << CS10);  
  // enable timer compare interrupt
  TIMSK5 |= (1 << OCIE1A);  
#endif

//sei();//allow interrupts

}

void task_mngr_init()
{
  uint32_t i;

  cli();
  for( i = 0; i < MAX_TASK_NUMBER; i++)
  {
    t_call_stack[i] = NULL;
    t_call_cntr[i] = 0;
    t_task_period[i] = 0;
  }
  t_mutex = 0;
  t_scheduler_tick = 0;
  
  init_scheduler();
}

int32_t task_mngr_add_task( uint32_t id, void (*callback)(void), uint32_t period, task_state_e state) // task priority is set by id, if period == 0 ----> task called only once by the scheduler
{
  if( id > (MAX_TASK_NUMBER - 1))
  {
    return -1; // TODO: Define error codes #define STACK_OVERFLOW (int32_t)(-1)
  }
  if( t_call_stack[id] != NULL )
  {
    return -2;  // TODO: Define error codes #define ID_EXISTS (int32_t)(-1)
  }
  
  t_call_stack[id] = callback;
  switch(state)
  {
    case TASK_SLEEP:
    {
      t_call_cntr[id] = 0;
      t_task_period[id] = (period / 10);
      t_task_state[id] = TASK_SLEEP;
      break;
    }
    case TASK_IDLE:
    default:
    {
      if( period > 9)
      {
        t_call_cntr[id] = (period / 10);
        t_task_period[id] = (period / 10);
      }
      else
      {
          t_call_cntr[id] = 1;
          t_task_period[id] = 0;
      }
      t_task_state[id] = TASK_IDLE;
      break;    
    }
  }
  
  return 0;
}

void start_scheduler()
{
  sei();//allow interrupts
  task_mngr(); 
}

int32_t sleep_task(uint32_t id)
{
  if( t_call_stack[id] == NULL)
  {
    return -1; //TODO ERRORS
  }
  if( t_task_state[id] == TASK_SLEEP )
  {
    return -2; //TODO errors
  }
  t_call_cntr[id] = 0;
  t_task_state[id] = TASK_SLEEP;
  return 0;
}

int32_t wake_up_task(uint32_t id)
{
  if( t_call_stack[id] == NULL)
  {
    return -1; //TODO ERRORS
  }
  if( t_task_state[id] != TASK_SLEEP )
  {
    return -2; //TODO errors
  }

  t_call_cntr[id] = 1;
  t_task_state[id] = TASK_IDLE;
  return 0;
}

int32_t take_mutex(void)
{
  if(t_mutex == 0)
  {
    t_mutex = 1;
    return 0;
  }
  return -1; //TODO ERRORS
}

int32_t give_mutex(void)
{
  t_mutex = 0;
  return 0;
}

int32_t enter_atomic(void)
{
  cli();
  return take_mutex();
}

int32_t exit_atomic(void)
{
  give_mutex();
  sei();
  return 0;
}

void task_mngr(void)
{
  uint32_t i;
  void (*call)(void);

  while(1)
  {
    if(t_scheduler_tick == 1)
    {
      for( i = 0; i < MAX_TASK_NUMBER; i++)
      {
        if(t_call_cntr[i] != 0)
        {
          t_call_cntr[i]--;
          if(t_call_cntr[i] == 0)
          {
            if(t_mutex == 0)
            {
              call = t_call_stack[i];
              t_call_cntr[i] = t_task_period[i];
              if(call != NULL)
              {
                t_task_state[i] = TASK_RUNNING;
                call();
                t_task_state[i] = TASK_IDLE;
              }
            }
            else
            {
              t_call_cntr[i] = 1;
            }
          }
        }
      }
      t_scheduler_tick = 0;
    }
    delay(1);
  }
}

#ifdef TMR1
ISR(TIMER1_COMPA_vect)
{
  t_scheduler_tick = 1;
}
#endif

#ifdef TMR3
ISR(TIMER3_COMPA_vect)
{
  t_scheduler_tick = 1;
}
#endif

#ifdef TMR4
ISR(TIMER4_COMPA_vect)
{
  t_scheduler_tick = 1;
}
#endif

#ifdef TMR5
ISR(TIMER5_COMPA_vect)
{
  t_scheduler_tick = 1;
}
#endif

