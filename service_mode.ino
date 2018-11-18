String s_input_string = "";         // a string to hold incoming data
boolean s_string_complete = false;  // whether the string is complete
uint32_t service_state;
time_t s_timeout_buffer;

void service_mode()
{
  time_t old_time;

  
  take_mutex();
  service_state = SER_ST_IDLE;
  
  time_set_timestamp(&old_time, obc_time.timestamp);
  time_set_timestamp(&obc_time, 0);

  s_help();
    
  while(1)
  {
    switch(service_state)
    {
      case SER_ST_IDLE:
      {
        if(s_string_complete)
        {
          if(s_input_string == "#FORMAT\r")
          {
            DEBUG.println(F("[OBC][SERVICE MODE] ALL DATA WILL LOST! CONTINUE? (Y/N)"));
            service_state = SER_ST_FORMAT;
          }
          else if(s_input_string == "#GET_TIME\r")
          {
            DEBUG.println(F("[OBC][SERVICE MODE] SYSTEM TIME ("));
            service_state = SER_ST_TIME;
          }
          else if(s_input_string == "#HELP\r")
          {
            s_help();            
          }
          else if(s_input_string == "#EXIT\r")
          {
            service_state = SER_ST_EXIT;
          }
          s_input_string = "";
          s_string_complete = false;
        }
        break;              
      }
      case SER_ST_FORMAT:
      {
        break;              
      }
      case SER_ST_TIME:
      {
        if(s_string_complete)
        {
          if(s_input_string == "#MAIN\r")
          {
            service_state = SER_ST_IDLE;
          }
          s_input_string = "";
          s_string_complete = false;
        }
        DEBUG.println("time");
        break;              
      }
      case SER_ST_EXIT:
      {
        time_set_timestamp(&obc_time, ((obc_time.timestamp) + (s_timeout_buffer.timestamp) + (old_time.timestamp)));
        give_mutex();
        return;
      }
      default: break;
    }
    service_timeout();
    service_serialEvent();
  }
  give_mutex();
}

void s_help()
{
  DEBUG.println(F("#########################################"));
  DEBUG.println(F("[OBC] SERVICE MODE"));
  DEBUG.println(F(""));
  DEBUG.println(F("#FORMAT      - format SD Card"));
  DEBUG.println(F("#GET_TIME    - get system time"));
  DEBUG.println(F("#MAIN        - back to main menu"));
  DEBUG.println(F("#HELP        - help"));
  DEBUG.println(F("#EXIT        - leave service mode"));
  DEBUG.println(F("#########################################"));
  DEBUG.println(F(""));
}

void service_timeout()
{
  if(obc_time.timestamp > SERVICE_TIMEOUT)
  {
    service_state = SER_ST_EXIT;
  }
}

void service_timeout_reset()
{
  time_set_timestamp(&s_timeout_buffer, obc_time.timestamp);
  time_set_timestamp(&obc_time, 0);
}

void service_serialEvent() 
{
  while (DEBUG.available()) 
  {
    service_timeout_reset();
    // get the new byte:
    char inChar = (char)DEBUG.read();
    // add it to the inputString:
    if (inChar == '\n') 
    {
      s_string_complete = true;
      return;
    }
    s_input_string += inChar;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
  }
}
