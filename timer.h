#ifndef TIMER_H
#define TIMER_H

typedef enum {
  TIMER_1   = 0,
  TIMER_2   = 1,
  TIMER_3   = 2,
  TIMER_4   = 3,
  TIMER_5   = 4,
}timer_e;

typedef struct {
  timer_e   timer;
  uint16_t  period;
  void      (*callback)(void);
}timer_initstruct_t;

#endif
