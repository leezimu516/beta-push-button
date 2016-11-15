#ifndef ACTIONS_H
#define ACTIONS_H

#include "global.h"

#define ACTION_MTP 0
#define ACTION_WAIT 1
#define ACTION_ACTIVATE 2
#define ACTION_AGITATE 3
#define ACTION_WAIT_FOR 4

#define MOTOR_SPEED_FULL 0xFF
#define MOTOR_SPEED_HALF (MOTOR_SPEED_FULL / 2)
#define MOTOR_SPEED_THIRD 0x55
#define MOTOR_SPEED_OFF 0x00
#define MOTOR_SPEED_QUARTER (MOTOR_SPEED_FULL / 4)
#define MOTOR_SPEED_CERTIAN 0x64

#define WAIT_FOR_CUP_IN_PLACE 0

#define WAIT_FOR_LESS_THAN 0 
#define WAIT_FOR_GREATER_THAN 1
#define WAIT_FOR_EQUALS 2
#define CANCEL_SENSOR 3

typedef struct __attribute__((__packed__, aligned(1))) {
  /* The new position to move to */
  int new_position;
  /* The direction to move */
  char move_direction;
  /* The speed of the motor (0 -255) */
  char speed;
  /* How long to try before giving up */
  int time_out;
} action_move_to_position_t;

typedef struct __attribute__((__packed__, aligned(1))) {
  /* How long to wait */
  int time_to_wait;
} action_wait_t;

typedef struct __attribute__((__packed__, aligned(1))) {
  /* What to change */
  char address;
  /* On or off */
  char state; // 0:OFF; 1:ON;
} action_activate_t;

typedef struct __attribute__((__packed__, aligned(1))) {
  char type;
  char value;
  char comparer;
} action_wait_for_t;

// TODO: This needs to change when there is more than 1 machine
typedef struct __attribute__((__packed__, aligned(1))) {
  /* how many units to raise */
  char rising_distance;
  /* how many units to lower */
  char lowering_distance;
  /* how many times to repeat */
  char repeat_cycles;
  /* start by raising or lowering */
  char start_direction; // 0: LOWER; 1:RAISE
  /* internal variable to track number of times repeated */
  char current_cycle;
  /* internal variable first or second step */
  char current_step;
  /* internal variable to track where we started */
  char start_position;
  /* internal variable to see if we have started */
  char is_running;
} action_agitate_t;

typedef struct __attribute__((__packed__, aligned(1))) {
  char type;
  union {
    action_move_to_position_t mtp;
    action_wait_t wait;
    action_activate_t activate;
    action_agitate_t agitate;
    action_wait_for_t wait_for;
  };
} action_t;

typedef struct __attribute__((__packed__, aligned(1))) {
  action_t* actions_ptr;
  int total_actions;
  int jam_counter_total; //add
} sequence_t;

sequence_t blend_sequence;
sequence_t clean_sequence;
//add
//sequence_t initializing_sequence;
action_t initializing_action;

void blend_actions_init(char reinit);
void clean_actions_init();
void initializing_action_init();

#endif
