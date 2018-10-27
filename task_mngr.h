#include "conf_task_mngr.h"

typedef enum {
  TASK_IDLE     = 0,
  TASK_RUNNING  = 1,
  TASK_SLEEP    = 2,  
}task_state_e;

typedef struct {
  void* call_stack[MAX_TASK_NUMBER];
  uint32_t call_cntr[MAX_TASK_NUMBER];
  uint32_t task_period[MAX_TASK_NUMBER];
  uint32_t next_free_slot;
  uint8_t mutex;
}task_manager;

