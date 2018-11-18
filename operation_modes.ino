uint32_t operation_mode;
uint8_t op_safe_switcher;

int32_t task_operation_mode_control()
{
  switch(operation_mode)
  {
    case OP_MODE_INIT:
    {
      return 0;
    }
    case OP_MODE_SAFE:
    {
      if( (sub_module_state & SUBMODULES_MASK) == SUBMODULES_MASK )
      {
        DEBUG.println(F("[OBC] ENTER NORMAL MODE"));
        operation_mode = OP_MODE_NORMAL;
        sleep_task(SAFE_MODE_TASK);
        op_mode_wake_up_normal();
      }
      return 0;
    }
    case OP_MODE_NORMAL:
    {
      if( is_landing == true)
      {
        DEBUG.println(F("[OBC] ENTER LANDING MODE"));
        operation_mode = OP_MODE_LANDING;
        op_mode_sleep_normal();
        wake_up_task(LANDING_TASK);
        return 0;
      }
      if( (sub_module_state & SUBMODULES_MASK) != SUBMODULES_MASK )
      {
        DEBUG.println(F("[OBC] ENTER SAFE MODE"));
        operation_mode = OP_MODE_SAFE;
        op_safe_switcher = 0;
        op_mode_sleep_normal();
        wake_up_task(SAFE_MODE_TASK);
        return 0;
      }
      return 0;      
    }
    case OP_MODE_LANDING:
    {
      if( is_landing == false)
      {
        DEBUG.println(F("[OBC] ENTER NORMAL MODE"));
        operation_mode = OP_MODE_NORMAL;
        op_mode_wake_up_normal();
        sleep_task(LANDING_TASK);
      }
      return 0;
    }
    default: return -1;
  }
}

void task_safe_mode()
{
  DEBUG.println(F("[OBC] SAFE MODE"));
  gps_sm(GPS_READ);
  if( op_safe_switcher == 0)
  {
    bus_sm(BUS_BEACON_TX);
  }
  else
  {
    bus_sm(BUS_GET_COM_HK);
    
  }
  task_get_obc_hk();
  op_safe_switcher = 1 - op_safe_switcher;  
}

void task_landing_becon_mode()
{
  gps_sm(GPS_READ);
  bus_sm(BUS_GET_COM_HK);
  task_get_obc_hk();
  bus_sm(BUS_BEACON_TX);
}

void set_operation_mode(uint32_t mode)
{
  operation_mode = mode;
}

void op_mode_wake_up_normal()
{
  wake_up_task(GPS_TASK);
  wake_up_task(BEACON_TX_TASK);
  wake_up_task(GET_COM_HK_TASK);
  wake_up_task(GET_OBC_HK_TASK);
}

void op_mode_sleep_normal()
{
  sleep_task(GPS_TASK);
  sleep_task(BEACON_TX_TASK);
  sleep_task(GET_COM_HK_TASK);
  sleep_task(GET_OBC_HK_TASK);
}

int32_t op_mode_check_altitude()
{
  if( is_min_altitude_reached == false )
  {
    if( GPS_Altitude[GPS_valid] > MIN_CLIMB_ALTITUDE )
    {
      is_min_altitude_reached = true;
      return 2; 
    }
    return 1;
  }
  if( (GPS_Altitude[GPS_valid] < LANDING_ON_ALT ) && (is_landing == false ))
  {
    is_landing = true;
    return 3; //TODO define/enum for return codes  
  }
  if( (GPS_Altitude[GPS_valid] > LANDING_OFF_ALT ) && (is_landing == true ))
  {
    is_landing = false;
    return 4; //TODO define/enum for return codes  
  }
  return 0;
}

int32_t op_mode_set_submodule(uint32_t submodule)
{
  if( submodule >= SUBMODULES_MAX)
  {
    return -1;
  }
  sub_module_state |= (1 << submodule);
  return 0;
}

int32_t op_mode_clr_submodule(uint32_t submodule)
{
  if( submodule >= SUBMODULES_MAX)
  {
    return -1;
  }
  sub_module_state &= ~(1 << submodule);
  return 0;
}

int32_t op_mode_get_submodule(uint32_t submodule)
{
  if( submodule >= SUBMODULES_MAX)
  {
    return -1;
  }
  return (sub_module_state & (1 << submodule));
}

