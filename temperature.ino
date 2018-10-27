int32_t get_external_temperature(void)
{
  double temp     = 0.0;
  double temp_avg = 0.0;
  
  for (int  i = 0; i < AVRG; i++)
  {
    temp = ((double)analogRead(A1) * V_REF / 1023.0) - 0.5;
    temp /= 0.01;
    delay(10);
    temp_avg += temp;  
  }
  
  temp = temp_avg/((double)AVRG);

  ext_temp = (int32_t)(temp * 10.0);
  return 0;
}


int32_t get_pcb_temperature(void)
{
  double temp     = 0.0;
  double temp_avg = 0.0;
  
  for (int  i = 0; i < AVRG; i++)
  {
    temp = ((double)analogRead(A0) * V_REF / 1023.0) - 0.5;
    temp /= 0.01;
    delay(10);
    temp_avg += temp;  
  }
  
  temp = temp_avg/((double)AVRG);

  pcb_temp = (int32_t)(temp * 10.0);
  DEBUG.print(F("[OBC] PCB TEMPERATURE: "));
  DEBUG.println(pcb_temp);
  return 0;

}




