#ifndef MACHINE_H
#define MACHINE_H

#include "global.h"
#include "actions.h"
#include "blender.h"
#include "liquid_filling.h"
#include "input_button.h"
#include "NewPingCWrapper.h"

#define BUTTON_COUNT 9
#define BLEND_BUTTON 0
#define CLEAN_BUTTON 1
#define STOP_BUTTON 2
#define STEP_BUTTON 3
#define MOVE_UP 4
#define MOVE_DOWN 5
#define INITIALIZE 6
#define REBLEND_BUTTON 7
#define JOG_PUMP_BUTTON 8



typedef struct {
  char id;
  char is_initialized;
  char current_state;
  char cuurent_cycle_type;
  char current_step;
  blender_t blender;
  liquid_filler_t liquid_filler;
  unsigned long last_step_time;
  unsigned long last_jam_check_time;
  int last_jam_check_position;
  int cup_detect_reading;
  CNewPing* cup_detect_sensor;
  input_button_t buttons[BUTTON_COUNT];
  unsigned long last_cup_read_time;
  char keypad_enabled;
} machine_t;

void machine_init(machine_t*);
void machine_process(machine_t*);
void machine_change_state(machine_t*);
void machine_stop(machine_t*);

char machine_execute_action(machine_t*, action_t*);

char machine_check_safety_conditions(machine_t*);

char machine_wait_for(machine_t*, action_wait_for_t*);

void machine_check_for_jams(machine_t*);

#endif

