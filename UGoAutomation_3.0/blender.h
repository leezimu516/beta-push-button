#ifndef BLENDER_H
#define BLENDER_H

#include "global.h"
#include "actions.h"

#define BLENDER_MOVEMENT_DOWN 0
#define BLENDER_MOVEMENT_UP 1
#define BLENDER_MOVEMENT_IDLE 2

#define BLENDER_ON 1
#define BLENDER_OFF 0

#define PUMP_ON 1
#define PUMP_OFF 0

#define BLENDER_ADDRESS 9
#define PUMP_ADDRESS 51
#define LIQUID_FILLING_VALVE_ADDRESS 49
#define CLEANING_VALVE_ADDRESS 53
#define BLENDER_SPEED_ADDRESS 35


typedef struct{
  int position;
  char movement;  
  char blade;
  char water_pump;
  int actuator_up_address;
  int actuator_down_address;
  int blender_ssr_address;
  int water_pump_address;
  int encoder_address; 
  int actuator_up_enabled_address;
  int actuator_down_enabled_address;
  int liquid_filling_valve_address;
  int cleaning_valve_address;
  char blender_speed;  
  char blender_speed_address;
} blender_t;

void blender_init(blender_t*);
void blender_move(blender_t*, char, char);
void update_current_position(blender_t*);

char move_to_position(blender_t*, unsigned long, action_move_to_position_t*);
char wait(blender_t*, unsigned long, action_wait_t*);
char activate(blender_t*, action_activate_t*);
char agitate(blender_t*, action_agitate_t*);

#endif


