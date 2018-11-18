void task_get_obc_hk()
{
    get_external_temperature();
    get_pcb_temperature();
    hk_push_obc_hk();
    sdcard_push_obc_log();   
}

void task_get_com_hk()
{
  bus_sm(BUS_GET_COM_HK);
}


void hk_push_com_hk()
{
  uint32_t hh;
  uint32_t mm;
  uint32_t ss;

  char time_tmp[9];

  time_get(&hh, &mm, &ss);
  DEBUG.print(F("[OBC] COM HK DATA: "));
  sprintf(radio_hk_buff, "%02ld:%02ld:%02ld,%4u,%05ld", hh, mm, ss, radio_temp, radio_msg_id);
  DEBUG.println(radio_hk_buff);
  
}

void hk_push_obc_hk()
{
  uint32_t hh;
  uint32_t mm;
  uint32_t ss;

  char time_tmp[9];

  time_get(&hh, &mm, &ss);
  DEBUG.print(F("[OBC] OBC HK DATA: "));
  sprintf(obc_hk_buff, "%02ld:%02ld:%02ld,%04lu,%04lu", hh, mm, ss, ext_temp, pcb_temp);
  DEBUG.println(obc_hk_buff);
  
}

