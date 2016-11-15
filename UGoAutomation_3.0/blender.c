#include "blender.h"

#define number_of_readings 25

typedef struct {
  int readings[number_of_readings];      // the readings from the analog input
  int readIndex;              // the index of the current reading
  int total;                  // the running total
  int average;                // the average
} blender_position_smoother_t;

blender_position_smoother_t blender_smoother;

void blender_init(blender_t* blender){
  blender->position = 0;
  blender->movement = BLENDER_MOVEMENT_IDLE;  
  blender->blade = BLENDER_OFF;
  blender->water_pump = PUMP_OFF;
  blender->actuator_up_address = 3;
  blender->actuator_down_address = 4;
  blender->blender_ssr_address = BLENDER_ADDRESS;
  blender->water_pump_address = PUMP_ADDRESS; 
  blender->encoder_address = A2;
  blender->actuator_up_enabled_address = 7;
  blender->actuator_down_enabled_address = 8;
  blender->blender_speed_address = BLENDER_SPEED_ADDRESS;
  blender->liquid_filling_valve_address = LIQUID_FILLING_VALVE_ADDRESS;
  blender->cleaning_valve_address = CLEANING_VALVE_ADDRESS;
  pinMode(blender->actuator_up_address, OUTPUT);
  pinMode(blender->actuator_down_address, OUTPUT);
  pinMode(blender->blender_ssr_address, OUTPUT);
  pinMode(blender->water_pump_address, OUTPUT);
  pinMode(blender->actuator_up_enabled_address, OUTPUT);
  pinMode(blender->actuator_down_enabled_address, OUTPUT);
  pinMode(blender->liquid_filling_valve_address, OUTPUT);
  pinMode(blender->cleaning_valve_address, OUTPUT);
  pinMode(blender->blender_speed_address, OUTPUT);
  // for now we are going to activate motor enabled. later on we
  // will check safety and activated as needed
  digitalWrite(blender->actuator_up_enabled_address, ON);
  digitalWrite(blender->actuator_down_enabled_address, ON);
  
  digitalWrite(blender->water_pump_address, ON);
  digitalWrite(blender->liquid_filling_valve_address, ON);
  digitalWrite(blender->cleaning_valve_address, ON);
  digitalWrite(blender->blender_speed_address, ON);

  for (int thisReading = 0; thisReading < number_of_readings; thisReading++) {
    blender_smoother.readings[thisReading] = 0;
  }
}

void blender_move(blender_t* blender, char direction, char speed){
  blender->movement = direction;
  
  switch (direction) {
    case BLENDER_MOVEMENT_DOWN:
      analogWrite(blender->actuator_down_address, 0);   
      analogWrite(blender->actuator_up_address, speed);
    break;
    case BLENDER_MOVEMENT_UP:
      analogWrite(blender->actuator_up_address, 0);
      analogWrite(blender->actuator_down_address, speed);
    break;
    case BLENDER_MOVEMENT_IDLE:
      analogWrite(blender->actuator_up_address, 0);
      analogWrite(blender->actuator_down_address, 0);
    break;
  }
}

void update_current_position(blender_t* blender) {
    // subtract the last reading:
  blender_smoother.total = blender_smoother.total - blender_smoother.readings[blender_smoother.readIndex];
  // read from the sensor:
  blender_smoother.readings[blender_smoother.readIndex] = analogRead(blender->encoder_address);
  // add the reading to the total:
  blender_smoother.total = blender_smoother.total + blender_smoother.readings[blender_smoother.readIndex];
  // advance to the next position in the array:
  blender_smoother.readIndex = blender_smoother.readIndex + 1;

  // if we're at the end of the array...
  if (blender_smoother.readIndex >= number_of_readings) {
    // ...wrap around to the beginning:
    blender_smoother.readIndex = 0;
  }
  
  blender->position = blender_smoother.total / number_of_readings;
}

char move_to_position(blender_t* blender, unsigned long start_time, action_move_to_position_t* action_move_to_position) {
  update_current_position(blender);

  // make sure we are actually moving
  if (blender->movement != action_move_to_position->move_direction) {
    LOG_PRINT(LOGGER_VERBOSE, "Activating the motor to move %s", action_move_to_position->move_direction == BLENDER_MOVEMENT_DOWN ? "down" : "up");
    blender_move(blender, action_move_to_position->move_direction, action_move_to_position->speed);
  }

  // add a timeout in case it gets jammed  // time_out bigger means when jam detected, the actuator will react faster
  if (start_time + (action_move_to_position->time_out) < millis()) { //(start_time + action_move_to_position->time_out < millis()) {
    LOG_PRINT(LOGGER_VERBOSE, "Movement timeout");
    return true;
  }
  
  switch (action_move_to_position->move_direction) {
    case BLENDER_MOVEMENT_DOWN:
      if(blender->position >= action_move_to_position->new_position) {
        // destination reached
        blender_move(blender, BLENDER_MOVEMENT_IDLE, 0);
        return true;
      } else {
        // destination not reached
        return false;
      }
    break;
    case BLENDER_MOVEMENT_UP:
      if(blender->position <= action_move_to_position->new_position) {
        // destination reached
        blender_move(blender, BLENDER_MOVEMENT_IDLE, 0);
        return true;
      } else {
        // destination not reached
        return false;
      }
    break;
    default:
      // error
        blender_move(blender, BLENDER_MOVEMENT_IDLE, 0);
      return true; 
    break;
  }
}

char wait(blender_t* blender, unsigned long start_wait_time, action_wait_t* action_wait) {
  return ((start_wait_time + action_wait->time_to_wait) < millis());
}

char activate(blender_t* blender, action_activate_t* action_activate) {
  if (action_activate->address == BLENDER_ADDRESS) {
    digitalWrite(action_activate->address, action_activate->state);
  } else {
    digitalWrite(action_activate->address, !action_activate->state);
  }
  return 1;
}

char agitate(blender_t* blender_ptr, action_agitate_t* action_agitate) {
  // agitate is a series of move_to_positions, this is where we set it up
  LOG_PRINT(LOGGER_ERROR, "Agitating");
  action_move_to_position_t* action_move_to_position_ptr;

  if(!action_agitate->is_running) {
    action_agitate->is_running = 1;
    action_agitate->start_position = blender_ptr->position;
  }
  
  if ((!action_agitate->current_step && action_agitate->start_direction) || (action_agitate->current_step && !action_agitate->start_direction)) {
    
  LOG_PRINT(LOGGER_ERROR, "DOWN");
    // LOWER
    action_move_to_position_ptr->new_position = action_agitate->start_position + action_agitate->lowering_distance ;
    action_move_to_position_ptr->move_direction = BLENDER_MOVEMENT_DOWN;
    action_move_to_position_ptr->speed = MOTOR_SPEED_FULL;
    action_move_to_position_ptr->time_out = 5000;
  } else {
    
  LOG_PRINT(LOGGER_ERROR, "UP");
    // RAISE
    action_move_to_position_ptr->new_position = action_agitate->start_position - action_agitate->rising_distance ;
    action_move_to_position_ptr->move_direction = BLENDER_MOVEMENT_UP;
    action_move_to_position_ptr->speed = MOTOR_SPEED_FULL;
    action_move_to_position_ptr->time_out = 5000;
  }

  if (move_to_position(blender_ptr, millis(), action_move_to_position_ptr)) {
    // we are in the right position
    
  LOG_PRINT(LOGGER_ERROR, "MTP DONE");
    action_agitate->start_position = blender_ptr->position;
    
    if (action_agitate->current_step) {
      action_agitate->current_cycle++;
    }
    
    action_agitate->current_step = !action_agitate->current_step;
  }
  
  // if we have ran the required repetitions return true
  if (action_agitate->current_cycle == action_agitate->repeat_cycles) {
    action_agitate->is_running = 0;
    action_agitate->current_cycle = 0;
    action_agitate->current_step = 0;
    return 1;
  }

  return 0;
}
