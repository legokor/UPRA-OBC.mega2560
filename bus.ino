uint32_t bus_state;

void task_bus_process()
{
  take_mutex();
  GetBusMSG();
  give_mutex();
}

void task_slave_interrupt()
{
  if (digitalRead(BUSBUSY) != 1)
  {
    bus_sm(BUS_SLAVE_INTERRUPT);
  }
}

void bus_init(void)
{
  bus_state = BUS_ST_IDLE;
  sw_timer_add_channel(BUS_TIMER, BUS_ACK_TIMEOUT, &bus_timer_cb);

  clrBusBusy();
  
  DEBUG.begin(DEBUG_BAUD);
  SICL.begin(SICL_BAUD);
}

bool busBusy_interrupt(void)
{
  if (digitalRead(BUSBUSY) != 1)
  {
    return true;
  }
  return false;
}

void setBusBusy(void)
{
  pinMode(BUSBUSY, OUTPUT);
  digitalWrite(BUSBUSY, LOW);
}

void clrBusBusy(void)
{
  digitalWrite(BUSBUSY, HIGH);
  pinMode(BUSBUSY, INPUT);
}

int ProcessBUSmsg(void)
{
  if ((bus_msg[1] == 'T') && (bus_msg[2] == 'C') && (bus_msg[3] == 'H') && (bus_msg[4] == 'K') && (bus_msg[5] == 'D'))
  {
    processTCHKDmsg();
    bus_sm(BUS_ACK);
    hk_push_com_hk();
    sdcard_push_com_log();
    return 0;
  }
  if ((bus_msg[1] == 'T') && (bus_msg[2] == 'C') && (bus_msg[3] == 'A') && (bus_msg[4] == 'C') && (bus_msg[5] == 'K'))
  {
    bus_sm(BUS_ACK);
    return 0;
  }
  if ((bus_msg[1] == 'C') && (bus_msg[2] == 'M') && (bus_msg[3] == 'D') && (bus_msg[4] == 'T') && (bus_msg[5] == 'A'))
  {
    return processCMDTAmsg();
  }
  if ((bus_msg[1] == 'P') && (bus_msg[2] == 'C') && (bus_msg[3] == 'S') && (bus_msg[4] == 'E') && (bus_msg[5] == 'M'))
  {
    sw_timer_disable();
    service_mode();
    bus_sm(BUS_ACK);
    sw_timer_enable();
    return 0;
  }
  return 1;  
}


int GetBusMSG(void)
{
  char inByte;
  int error=10;
 // ////////SICL.listen(); 
  while (SICL.available() > 0)
  {
    inByte = SICL.read();
 
    if ((inByte =='$') || (MSGindex >= 30))
    {
      MSGindex = 0;
    }
 
    if (inByte != '\r')
    {
      bus_msg[MSGindex++] = inByte;
    }
 
    if (inByte == '\n')
    {
    //  SICL.println("endline");
      bus_sm(BUS_EOL);
    }
  }
  return error;  
}

int32_t bus_sm(const uint32_t input)
{
  int32_t ret;
  switch(bus_state)
  {
    case BUS_ST_IDLE:
    {
      ret = b_idle_state_executor(input);
      break;
    }
    case BUS_ST_PROCESS:
    {
      take_mutex();
      ret = b_process_state_executor(input);
      give_mutex();
      break;
    }
    case BUS_ST_GET_COM_HK:
    {
      ret = b_get_com_hk_state_executor(input);
      break;
    }
    case BUS_ST_BEACON_TX:
    {
      ret = b_beacon_tx_state_executor(input);
      break;
    }
    case BUS_ST_INTERRUPT:
    {
      ret = b_interrupt_state_executor(input);
      break;
    }
    default:
    {
      ret = -1;
      break;
    }
  }
  return ret;
}

int32_t b_idle_state_executor(const uint32_t input)
{
  switch(input)
  {
    case BUS_GET_COM_HK:
    {
      bus_state = BUS_ST_GET_COM_HK;
      sw_timer_enable_channel(BUS_TIMER);
      radio_get_hk();
      return 0;
    }
    case BUS_BEACON_TX:
    {
      bus_state = BUS_ST_BEACON_TX;
      sw_timer_enable_channel(BUS_TIMER);
      radio_beacon_tx();
      return 0;
    }
    case BUS_SLAVE_INTERRUPT:
    {
      bus_state = BUS_ST_INTERRUPT;
      sleep_task(SLAVE_INT_TASK);
      sw_timer_enable_channel(BUS_TIMER);
      DEBUG.println(F("[OBC] SLAVE INTERRUPT ON SICL"));
      b_slave_interrupt_handler();
      return 0;
    }
    default: 
    {
//      DEBUG.println("error_idle");
      return -2;
    }
  }
}

int32_t b_process_state_executor(const uint32_t input)
{
  switch(input)
  {
    case BUS_EOL:
    {
      sleep_task(BUS_PROCESS_TASK);
      ProcessBUSmsg();
      return 0;
    }
    case BUS_ACK:
    {
      bus_state = BUS_ST_IDLE;
      MSGindex = 0;
      clrBusBusy();
      wake_up_task(SLAVE_INT_TASK);
      return 0;
    }
    case BUS_TIMEOUT:
    {
      bus_state = BUS_ST_IDLE;
      sleep_task(BUS_PROCESS_TASK);
      MSGindex = 0;
      clrBusBusy();
      wake_up_task(SLAVE_INT_TASK);
      return 0;
    }
    default: 
    {
      //DEBUG.println("error_process");
      return -2;
    }
  }
}

int32_t b_get_com_hk_state_executor(const uint32_t input)
{
  switch(input)
  {
    case BUS_PROCESS:
    {
      bus_state = BUS_ST_PROCESS;
      wake_up_task(BUS_PROCESS_TASK);
      return 0;
    }
    case BUS_TIMEOUT:
    {
 //     bus_state = BUS_ST_IDLE;
      clrBusBusy();
      return 0;
    }
    default: 
    {
      DEBUG.println("error_com_hk");
      return -2;
    }
  }
}

int32_t b_beacon_tx_state_executor(const uint32_t input)
{
  switch(input)
  {
    case BUS_PROCESS:
    {
      bus_state = BUS_ST_PROCESS;
      sw_timer_enable_channel(BUS_TIMER);
      wake_up_task(BUS_PROCESS_TASK);
      return 0;
    }
    case BUS_TIMEOUT:
    {
      bus_state = BUS_ST_IDLE;
      clrBusBusy();
      return 0;
    }
    default: 
    {
//      DEBUG.println("error_beacon");
      return -2;
    }
  }
}

int32_t b_interrupt_state_executor(const uint32_t input)
{
  switch(input)
  {
    case BUS_PROCESS:
    {
      bus_state = BUS_ST_PROCESS;
      sw_timer_enable_channel(BUS_TIMER);
      wake_up_task(BUS_PROCESS_TASK);
      return 0;
    }
    case BUS_TIMEOUT:
    {
      bus_state = BUS_ST_IDLE;
      clrBusBusy();
      return 0;
    }
    default: 
    {
//      DEBUG.println("error_beacon");
      return -2;
    }
  }
}

void b_slave_interrupt_handler()
{
  SICL.println(F("$TMINT*4A"));
  bus_sm(BUS_PROCESS);
}

void bus_timer_cb()
{
  sw_timer_disable_channel(BUS_TIMER);
  bus_sm(BUS_TIMEOUT);
}

