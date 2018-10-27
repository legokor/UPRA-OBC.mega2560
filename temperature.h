#ifndef TEMPERATURE_H
#define TEMPERATURE_H

#define AVRG                  100
#define INTERNALCALIB         ((double)(90.0 ))
#define INTERNALGAIN          ((double)(1.51))
#define CALIBRATION_OFFSET    ((double)(-20.18))
#define CALIBRATION_GAIN      ((double)(1.56))

#define V_REF                 ((double)5.0) //BB: 5v0, EM,FM: 3v3

int32_t ext_temp;

int32_t pcb_temp;

#endif
