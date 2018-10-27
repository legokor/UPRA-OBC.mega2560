uint32_t channels;
uint32_t sw_tmr_enabled;
uint32_t sw_tmr_running;
void *sw_timer_callback[MAX_SW_TIMER_CH];
uint32_t sw_tmr_timeout[MAX_SW_TIMER_CH];
uint32_t sw_tmr_pause[MAX_SW_TIMER_CH];

void sw_timer_init()
{
  uint32_t i;
  timer_initstruct_t timer_InitStruct;
  cli();//stop interrupts

  timer_InitStruct.timer = TIMER_4;
  timer_InitStruct.period = 10; //[Hz]
  timer_InitStruct.callback = &sw_timer_callback_from_isr;

  timer_init(&timer_InitStruct);

  channels = 0;
  sw_tmr_enabled = 0;
  sw_tmr_running = 0;
  sw_timer_reset_channels();
  for(i = 0; i < MAX_SW_TIMER_CH; i++)
  {
    sw_timer_callback[i] = NULL;
    sw_tmr_timeout[i] = 0;
  }

  sei();//allow interrupts
  
}

void sw_timer_disable()
{
  uint32_t i;
  for(i = 0; i < MAX_SW_TIMER_CH; i++)
  {
    if( getbit(&channels, i) != 0)
    {
      clrbit(&sw_tmr_enabled, i);
    }
  }
}

void sw_timer_enable()
{
  uint32_t i;
  for(i = 0; i < MAX_SW_TIMER_CH; i++)
  {
    if( getbit(&sw_tmr_running, i) != 0)
    {
      setbit(&sw_tmr_enabled, i);
    }
  }
}

int32_t sw_timer_add_channel(uint32_t ch, uint32_t timeout, void (*callback)(void))
{
  if(ch > (MAX_SW_TIMER_CH - 1) )
  {
    return -1;
  }
  if( getbit(&channels, ch) != 0)
  {
    return -2;
  }
  
  sw_tmr_timeout[ch] = timeout / 100;
  sw_timer_callback[ch] = callback;
  setbit(&channels, ch);
  
  return 0;
}

int32_t sw_timer_enable_channel(uint32_t ch)
{
  if(ch > (MAX_SW_TIMER_CH - 1) )
  {
    return -1;
  }
  if( getbit(&channels, ch) == 0)
  {
    return -2;
  }

  setbit(&sw_tmr_enabled, ch);
  sw_tmr_running = sw_tmr_enabled;
  sw_timer_cntr[ch] = sw_tmr_timeout[ch];
}

int32_t sw_timer_disable_channel(uint32_t ch)
{
  if(ch > (MAX_SW_TIMER_CH - 1) )
  {
    return -1;
  }
  if( getbit(&channels, ch) == 0)
  {
    return -2;
  }

  clrbit(&sw_tmr_enabled, ch);
  sw_tmr_running = sw_tmr_enabled;
  sw_timer_cntr[ch] = 0;
}

bool sw_timer_is_channel_enabled(uint32_t ch)
{
  if(ch > (MAX_SW_TIMER_CH - 1) )
  {
    return false;
  }
  if( getbit(&channels, ch) == 0)
  {
    return false;
  }
  if( getbit(&sw_tmr_enabled, ch) == 0)
  {
    return false;
  }
  return true;
}

void sw_timer_reset_channels()
{
  uint32_t i;
  for(i = 0; i < MAX_SW_TIMER_CH; i++)
  {
    sw_timer_cntr[i] = 0;
  }
}

int32_t sw_timer_set_cntr(uint32_t ch, uint32_t cntr)
{
  if(ch > (MAX_SW_TIMER_CH - 1) )
  {
    return -1;
  }
  if( getbit(&channels, ch) == 0)
  {
    return -2;
  }
  
  sw_timer_cntr[ch] = cntr;
  return 0;
}

void sw_timer_callback_from_isr()
{
  uint32_t i;
  void (*callback)(void);
  for( i = 0; i < MAX_SW_TIMER_CH; i++)
  {
    if( sw_timer_cntr[i] != 0)
    {
      if( getbit(&sw_tmr_enabled, i) != 0)
      {
        sw_timer_cntr[i]--;
        if( sw_timer_cntr[i] == 0)
        {
          sw_timer_cntr[i] = sw_tmr_timeout[i];
          callback = sw_timer_callback[i];
          if( callback != NULL)
          {
            callback();
          }
        }
      }
    }
  }
}

void setbit(uint32_t* const reg, uint32_t bit_)
{
  *reg |= (1 << bit_);
}

void clrbit(uint32_t* const reg, uint32_t bit_)
{
  *reg &= ~(1 << bit_);
}

uint32_t getbit(uint32_t* reg, uint32_t bit_)
{
  return *reg & (1 << bit_);
}

