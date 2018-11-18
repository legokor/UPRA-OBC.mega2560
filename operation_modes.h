#ifndef OPERATION_MODES_H
#define OPERATION_MODES_H

#define MIN_CLIMB_ALTITUDE      1000  //[m]
#define LANDING_ON_ALT          500   //[m]
#define LANDING_OFF_ALT         800   //[m]
#define SUBMODULES_MAX          2
#define SUBMODULES_MASK         ((uint32_t)((1 << SUBMODULES_MAX) - 1))

typedef enum {
  MODULE_MAIN_GPS = 0,
  MODULE_COM      = 1,
  MODULE_DAU      = 2,
  MODULE_EPS      = 3,
  MODULE_ICS      = 4,
}submodules_e;

typedef enum {
  OP_MODE_INIT        = 0,
  OP_MODE_SAFE        = 1,
  OP_MODE_NORMAL      = 2,
  OP_MODE_LANDING     = 3,
}operation_modes_e;

bool is_min_altitude_reached=false;
bool is_landing=false;
uint32_t sub_module_state;

#endif

