/***************************************************
  Machine                                <machine.c>
  Authored By: Steven McKeown

  The machine file controls the operation of a
  machine.

  A machine controls:
  - A blender
  - Input buttons

  NOTE: When implementing multiple machines, address
  changes will be needed, for now the addresses are:

  **** INPUTS ****
  1 - BLEND BUTTON
  2 - CLEAN BUTTON
  3 - STOP BUTTON
  4 - STEP BUTTON
  11 - LED READY
  12 - LED IN USE
  A1 - MOTOR POTENTIOMETER

  **** OUTPUTS ****
  5 - MOTOR FORWARD
  6 - MOTOR REVERSE
  7 - FORWARD ENABLED
  8 - REVERSE ENABLED
  9 - BLENDER
  10 - PUMP

  TODO: Implement stepping

  SM070716
***************************************************/
#include "machine.h"
#include "actions.h"


char step_request;
int jam_counter = 0;//add
const int PUSH_BLEND_BUTTON_1 = 24;

void machine_init(machine_t* machine_ptr) {
  pinMode(13, OUTPUT);
  machine_ptr->is_initialized = 0;
  machine_ptr->keypad_enabled = 1;
  machine_ptr->current_state = MACHINE_STATE_IDLE;
  machine_ptr->last_cup_read_time = millis();

  // MAGIC NUMBERS FOR NOW, DEFINE AFTER......
  machine_ptr->cup_detect_sensor = new_ping_c_wrapper_init(12,11);
  
  // initialize the blender
  blender_init(&machine_ptr->blender);

  input_button_init(&machine_ptr->buttons[BLEND_BUTTON], 41);
  input_button_init(&machine_ptr->buttons[CLEAN_BUTTON], 39);
  input_button_init(&machine_ptr->buttons[STOP_BUTTON], 37);
  input_button_init(&machine_ptr->buttons[STEP_BUTTON], 0); // NOT IMPLIMENTED
  
  input_button_init(&machine_ptr->buttons[MOVE_UP], 47);
  input_button_init(&machine_ptr->buttons[MOVE_DOWN], 45);
  input_button_init(&machine_ptr->buttons[INITIALIZE], 43);
  
  input_button_init(&machine_ptr->buttons[REBLEND_BUTTON], 33);
  input_button_init(&machine_ptr->buttons[JOG_PUMP_BUTTON], 31);

  step_request = 0;

  //implement the push button
 /* input_button_init(&machine_ptr->buttons[PUSH_BLEND_BUTTON], 24);
  input_button_init(&machine_ptr->buttons[PUSH_CLEAN_BUTTON], 26);
  input_button_init(&machine_ptr->buttons[PUSH_INITIALIZE], 28);*/
  
}

void machine_process(machine_t* machine_ptr) {
  int i;
  update_current_position(&machine_ptr->blender);

  if ((machine_ptr->last_cup_read_time + 500) < millis()) {
    machine_ptr->cup_detect_reading = new_ping_c_wrapper_sonar_ping(machine_ptr->cup_detect_sensor);    
    //LOG_PRINT(LOGGER_VERBOSE, "Reading: %d address:%d", machine_ptr->cup_detect_reading, &machine_ptr->cup_detect_reading);
    machine_ptr->last_cup_read_time = millis();
  }

  // ---------- BEGING INPUT BUTTON SECTION ----------
  if (machine_ptr->keypad_enabled) {
    for (i = 0; i < BUTTON_COUNT; i++) {
      input_button_read(&machine_ptr->buttons[i]);
    }
  }

  if (machine_ptr->current_state == MACHINE_STATE_IDLE) {
    if (machine_ptr->buttons[BLEND_BUTTON].current_state ) {
      LOG_PRINT(LOGGER_VERBOSE, "Blender button pushed, starting blending, total actions: %d", blend_sequence.total_actions );
      machine_ptr->current_state = MACHINE_STATE_BLENDING;
    } else if (machine_ptr->buttons[CLEAN_BUTTON].current_state) {
      LOG_PRINT(LOGGER_VERBOSE, "Cleaning button pushed, starting cleaning");
      machine_ptr->current_state = MACHINE_STATE_CLEANING;
    } else if (machine_ptr->buttons[REBLEND_BUTTON].current_state) {
      LOG_PRINT(LOGGER_VERBOSE, "Reblender button pushed, starting reblending, total actions: %d", blend_sequence.total_actions );
      blend_sequence.actions_ptr[5].activate.state = OFF;
      blend_sequence.actions_ptr[53].activate.state = OFF;
      machine_ptr->current_state = MACHINE_STATE_BLENDING;
    }
  }
  
  if (machine_ptr->buttons[INITIALIZE].current_state) {
    LOG_PRINT(LOGGER_VERBOSE, "Initializing");
    blend_actions_init(1);
    machine_ptr->current_state = MACHINE_STATE_INITIALIZING;
    //add: solve the initialization that blade and actuator stop asynchronous
    //machine_stop(machine_ptr);
  }
  
  if (machine_ptr->buttons[STOP_BUTTON].current_state && machine_ptr->current_state != MACHINE_STATE_IDLE) {
    LOG_PRINT(LOGGER_VERBOSE, "Stop button pushed, stopping machine");
    machine_stop(machine_ptr);
    blend_actions_init(1);
    machine_ptr->current_state = MACHINE_STATE_IDLE;
    
  }

//  if (machine_ptr->buttons[STEP_BUTTON].current_state) {
//    if (machine_ptr->current_state != MACHINE_STATE_STEPPING) {
//      machine_ptr->current_state = MACHINE_STATE_STEPPING;
//    }
//    step_request = 1;
//  }

  LOG_PRINT(LOGGER_DEBUG , "Machine Current State: %d - Button States: B%d - C%d - S%d - MU%d - MD%d - I%d - MP%d - CS%d", 
    machine_ptr->current_state,
    machine_ptr->buttons[BLEND_BUTTON].current_state,   
    machine_ptr->buttons[CLEAN_BUTTON].current_state,   
    machine_ptr->buttons[STOP_BUTTON].current_state,
    machine_ptr->buttons[MOVE_UP].current_state,
    machine_ptr->buttons[MOVE_DOWN].current_state,
    machine_ptr->buttons[INITIALIZE].current_state,
    machine_ptr->blender.position,
    machine_ptr->cup_detect_reading);

  // ---------- END INPUT BUTTON SECTION ----------

  // change to mediator rather than switch
  switch (machine_ptr->current_state) {
    case MACHINE_STATE_IDLE:    
      if (machine_ptr->buttons[MOVE_UP].current_state) {
        LOG_PRINT(LOGGER_VERBOSE, "MOVING UP, current position:%d, speed:%d", machine_ptr->blender.position, MOTOR_SPEED_HALF);
        blender_move(&machine_ptr->blender, BLENDER_MOVEMENT_UP, MOTOR_SPEED_HALF);
      } else if (machine_ptr->buttons[MOVE_DOWN].current_state ) {
        LOG_PRINT(LOGGER_VERBOSE, "MOVING DOWN, current position:%d, speed:%d", machine_ptr->blender.position, MOTOR_SPEED_HALF);
        blender_move(&machine_ptr->blender, BLENDER_MOVEMENT_DOWN, MOTOR_SPEED_HALF);
      } else {
        //LOG_PRINT(LOGGER_VERBOSE, "IDLE");
        machine_stop(machine_ptr);
        blender_move(&machine_ptr->blender, BLENDER_MOVEMENT_IDLE, 0);
      }
      //jog pump top
      if (machine_ptr->buttons[JOG_PUMP_BUTTON].current_state) {
        //LOG_PRINT(LOGGER_VERBOSE, "MOVING DOWN, current position:%d, speed:%d", machine_ptr->blender.position, MOTOR_SPEED_HALF);
        digitalWrite(PUMP_ADDRESS, 0);
        digitalWrite(LIQUID_FILLING_VALVE_ADDRESS, 0);  
        
      } else if (!machine_ptr->buttons[JOG_PUMP_BUTTON].current_state) {
        //LOG_PRINT(LOGGER_VERBOSE, "MOVING DOWN, current position:%d, speed:%d", machine_ptr->blender.position, MOTOR_SPEED_HALF);
        digitalWrite(PUMP_ADDRESS, 1);
        //digitalWrite(LIQUID_FILLING_VALVE_ADDRESS, 1);
        
      }

      // temp hack for now, just to keep valves closed
      if (digitalRead(CLEANING_VALVE_ADDRESS) != 1) {
         digitalWrite(CLEANING_VALVE_ADDRESS, 1);
      }
      
      
      machine_ptr->current_step = 0;
      machine_ptr->last_step_time = millis();
      machine_ptr->last_jam_check_position = millis();
      break;
    case MACHINE_STATE_BLENDING:
      if (machine_execute_action(machine_ptr, &blend_sequence.actions_ptr[machine_ptr->current_step])) {
        // reset jam issue
        machine_ptr->last_jam_check_position = machine_ptr->blender.position;
        machine_ptr->last_jam_check_time = millis();

        // we finished the last action, let's move to the next action.
        LOG_PRINT(LOGGER_VERBOSE, "Bending step %d completed, percent complete:%d", machine_ptr->current_step, (100*machine_ptr->current_step+1)/blend_sequence.total_actions);
        if (blend_sequence.actions_ptr[machine_ptr->current_step].type == ACTION_MTP) {
          LOG_PRINT(LOGGER_VERBOSE, "current position:%d, desired position:%d, direction:%d", machine_ptr->blender.position, blend_sequence.actions_ptr[machine_ptr->current_step].mtp.new_position, blend_sequence.actions_ptr[machine_ptr->current_step].mtp.move_direction);
        } else if (blend_sequence.actions_ptr[machine_ptr->current_step].type == ACTION_ACTIVATE) {
          LOG_PRINT(LOGGER_VERBOSE, "toggling output:%d, desired state:%d", blend_sequence.actions_ptr[machine_ptr->current_step].activate.address, blend_sequence.actions_ptr[machine_ptr->current_step].activate.state);
        }
        machine_ptr->current_step++;
        machine_ptr->last_step_time = millis();

        if (machine_ptr->current_step == blend_sequence.total_actions) {
          LOG_PRINT(LOGGER_VERBOSE, "Blending complete, cleaning machine");
          
          machine_ptr->current_step = 0;
          machine_ptr->current_state = MACHINE_STATE_CLEANING;
         
          // Jam detection changes the actual action array, reinitialize when done
          blend_actions_init(1);
        }
      } else {
        // we need to check if we are actually moving properly
        if (blend_sequence.actions_ptr[machine_ptr->current_step].type == ACTION_MTP) {
          machine_check_for_jams(machine_ptr);
        }
      }
      break;
    case MACHINE_STATE_CLEANING:
      if (machine_execute_action(machine_ptr, &clean_sequence.actions_ptr[machine_ptr->current_step])) {
        LOG_PRINT(LOGGER_VERBOSE, "Cleaning step %d completed, percent complete:%d", machine_ptr->current_step, (100*machine_ptr->current_step+1)/clean_sequence.total_actions);
        if (clean_sequence.actions_ptr[machine_ptr->current_step].type == ACTION_MTP) {
          LOG_PRINT(LOGGER_VERBOSE, "current position:%d, desired position:%d, direction:%d", machine_ptr->blender.position, clean_sequence.actions_ptr[machine_ptr->current_step].mtp.new_position, clean_sequence.actions_ptr[machine_ptr->current_step].mtp.move_direction);
        } else if (blend_sequence.actions_ptr[machine_ptr->current_step].type == ACTION_ACTIVATE) {
          LOG_PRINT(LOGGER_VERBOSE, "toggling output:%d, desired state:%d", blend_sequence.actions_ptr[machine_ptr->current_step].activate.address, blend_sequence.actions_ptr[machine_ptr->current_step].activate.state);
        }
        // we finished the last action, let's move to the next action.
        machine_ptr->current_step++;
        machine_ptr->last_step_time = millis();

        if (machine_ptr->current_step == clean_sequence.total_actions) {
          machine_ptr->current_state = MACHINE_STATE_IDLE;
        }
      }
      break;
    case MACHINE_STATE_STEPPING:
      if (step_request) {
        if (machine_execute_action(machine_ptr, &blend_sequence.actions_ptr[machine_ptr->current_step])) {
          // we finished the last action, let's move to the next action.
          LOG_PRINT(LOGGER_VERBOSE, "Bending step %d completed, percent complete:%d", machine_ptr->current_step, (100*machine_ptr->current_step)/blend_sequence.total_actions);
          if (blend_sequence.actions_ptr[machine_ptr->current_step].type == ACTION_MTP) {
            LOG_PRINT(LOGGER_VERBOSE, "current position:%d, desired position:%d, direction:%d", machine_ptr->blender.position, blend_sequence.actions_ptr[machine_ptr->current_step].mtp.new_position, blend_sequence.actions_ptr[machine_ptr->current_step].mtp.move_direction);
          }
          machine_ptr->current_step++;
          machine_ptr->last_step_time = millis();
  
          if (machine_ptr->current_step == blend_sequence.total_actions) {
            LOG_PRINT(LOGGER_VERBOSE, "Blending complete, stopping machine");
            machine_ptr->current_state = MACHINE_STATE_IDLE;
          }

          step_request = 0;
        }  
      }
      break;
    case MACHINE_STATE_INITIALIZING:
      //add: solve the initialization that blade and actuator stop asynchronous
      machine_stop(machine_ptr);
      //led off
      digitalWrite(13, LOW);  
      //add to solve moving up slowly sometimes
      blender_move(&machine_ptr->blender, BLENDER_MOVEMENT_IDLE, 0);

      if (machine_execute_action(machine_ptr, &initializing_action)) {
        machine_ptr->current_state = MACHINE_STATE_IDLE;
        machine_ptr->is_initialized = 1;
        LOG_PRINT(LOGGER_VERBOSE, "Machine Initialized");
      }
  }
}

char machine_execute_action(machine_t* machine_ptr, action_t* action) {
  switch (action->type) {
    case  ACTION_MTP:
      return move_to_position(&machine_ptr->blender, machine_ptr->last_step_time, &action->mtp);
      break;
    case ACTION_WAIT:
      return wait(&machine_ptr->blender, machine_ptr->last_step_time, &action->wait);
      break;
    case ACTION_ACTIVATE:
      return activate(&machine_ptr->blender, &action->activate);
      break;
    case ACTION_AGITATE:
      return agitate(&machine_ptr->blender, &action->agitate);
      break;
    case ACTION_WAIT_FOR:
      return machine_wait_for(machine_ptr, &action->wait_for);
      break;
  }
  // error...
  LOG_PRINT(LOGGER_WARNING, "machine_execute_action - invalid action type: %d", action->type);
  return 0;
}

// function to check if the machine is in an unsafe state, and take action
char machine_check_safety_conditions(machine_t* machine_ptr) {
  // TODO
  //if (machine_ptr->current_state == MACHINE_STATE_CLEANING || machine_ptr->current_state == MACHINE_STATE_BLENDING) { 
  if (machine_ptr->current_state == MACHINE_STATE_CLEANING) {    
    if (machine_ptr->cup_detect_reading < 8) {
      // something is in the machine
      //machine_ptr->current_state = MACHINE_STATE_IDLE;
      LOG_PRINT(LOGGER_ERROR, "SAFETY TIGGERED");
      // hack for now, cancel it
      //machine_stop(machine_ptr);
      //led check    
      digitalWrite(13, HIGH);  
    }
  }
  // to blink led
  /*if(machine_ptr->current_state == MACHINE_STATE_IDLE && machine_ptr->cup_detect_reading < 8){
    digitalWrite(13, HIGH);
    delay(100);
    digitalWrite(13, LOW);
    delay(100);
  }*/
  
  return 1;
}

void machine_stop(machine_t* machine_prt) {
  // turn off blender and pump!!
  digitalWrite(PUMP_ADDRESS, HIGH);
  digitalWrite(BLENDER_ADDRESS, LOW);
  digitalWrite(BLENDER_SPEED_ADDRESS,HIGH);
  digitalWrite(CLEANING_VALVE_ADDRESS,HIGH);
  digitalWrite(LIQUID_FILLING_VALVE_ADDRESS,HIGH);
}

char machine_wait_for(machine_t* machine_ptr, action_wait_for_t* wait_for) {
  LOG_PRINT(LOGGER_VERBOSE, "waiting for T:%d C:%d R:%d V%d", wait_for->type, wait_for->comparer, machine_ptr->cup_detect_reading, wait_for->value);
  switch (wait_for->type) {
    case WAIT_FOR_CUP_IN_PLACE:
      switch (wait_for->comparer) {
        case WAIT_FOR_LESS_THAN:
          if (machine_ptr->cup_detect_reading < wait_for->value) {
            return true;
          }
          break;
        case WAIT_FOR_GREATER_THAN:
          if (machine_ptr->cup_detect_reading > wait_for->value) {
            return true;
          }
          break;
        case WAIT_FOR_EQUALS:
          if (machine_ptr->cup_detect_reading == wait_for->value) {
            return true;
          }
          break;
        case CANCEL_SENSOR:
          return true;
          break;
      }
      break;
  }

  return false;
}

void machine_check_for_jams(machine_t* machine_ptr) {
  // we we are supposed to be moving, let's validate that we are actually moving
  if (blend_sequence.actions_ptr[machine_ptr->current_step].mtp.new_position == TOP_POSITION) {return;}
  if (machine_ptr->last_jam_check_time + 250 < millis()) { //+500 jam react time: number bigger means actuator will keep going for longer time
    int where_should_we_be = 0;
    //blend_sequence.jam_counter_total = 0; // add for count the totoal jam, because the shake function in jam should not work for the first 3 times.


    switch (blend_sequence.actions_ptr[machine_ptr->current_step].mtp.move_direction) {
      // change to ABS calc instead of switch
      case BLENDER_MOVEMENT_UP:
          where_should_we_be = machine_ptr->last_jam_check_position - 2;
          if (where_should_we_be < machine_ptr->blender.position) {
            // JAMMED
            LOG_PRINT(LOGGER_ERROR, "Jammed moving up: should be:%d is:%d", where_should_we_be, machine_ptr->blender.position);
            blend_sequence.actions_ptr[machine_ptr->current_step - 4].type = ACTION_WAIT;
            blend_sequence.actions_ptr[machine_ptr->current_step - 4].wait.time_to_wait = 750; //ms

            blend_sequence.actions_ptr[machine_ptr->current_step - 3].type = ACTION_MTP;
            blend_sequence.actions_ptr[machine_ptr->current_step - 3].mtp.new_position = (machine_ptr->blender.position + 50 > BOTTOM_OF_CUP) ? BOTTOM_OF_CUP : machine_ptr->blender.position + 50; // position
            blend_sequence.actions_ptr[machine_ptr->current_step - 3].mtp.move_direction = BLENDER_MOVEMENT_DOWN;
            blend_sequence.actions_ptr[machine_ptr->current_step - 3].mtp.time_out = 3000;
            blend_sequence.actions_ptr[machine_ptr->current_step - 3].mtp.speed = MOTOR_SPEED_FULL;// MOTOR_SPEED_HALF
            
            blend_sequence.actions_ptr[machine_ptr->current_step - 2].type = ACTION_ACTIVATE;
            blend_sequence.actions_ptr[machine_ptr->current_step - 2].activate.address = BLENDER_SPEED_ADDRESS;
            blend_sequence.actions_ptr[machine_ptr->current_step - 2].activate.state = ON;
            
            blend_sequence.actions_ptr[machine_ptr->current_step - 1].type = ACTION_WAIT;
            blend_sequence.actions_ptr[machine_ptr->current_step - 1].wait.time_to_wait = 2000; //ms
            machine_ptr->current_step = machine_ptr->current_step - 4;
          }
        break;
      case BLENDER_MOVEMENT_DOWN:
          if ((machine_ptr->last_jam_check_position + 5) > machine_ptr->blender.position) {
            where_should_we_be = machine_ptr->last_jam_check_position;
            if (abs(machine_ptr->blender.position - where_should_we_be) <=  5) {  //3 more sensitive for smaller number
              jam_counter +=1;
              blend_sequence.jam_counter_total +=1;
              LOG_PRINT(LOGGER_ERROR, "jam_counter:%d", jam_counter);
              //LOG_PRINT(LOGGER_ERROR, "jam_counter_total:%d", blend_sequence.jam_counter_total);
            }
            else{
              jam_counter -=1;
              blend_sequence.jam_counter_total -=1;
            }

            

            LOG_PRINT(LOGGER_ERROR, "Jammed moving down: should be:%d is:%d", where_should_we_be, machine_ptr->blender.position);
            blend_sequence.actions_ptr[machine_ptr->current_step - 4].type = ACTION_ACTIVATE;
            blend_sequence.actions_ptr[machine_ptr->current_step - 4].activate.address = BLENDER_SPEED_ADDRESS;
            blend_sequence.actions_ptr[machine_ptr->current_step - 4].activate.state = ON; //OFF

            blend_sequence.actions_ptr[machine_ptr->current_step - 4].type = ACTION_WAIT;
            blend_sequence.actions_ptr[machine_ptr->current_step - 4].wait.time_to_wait = 250; //ms 750, 1250

            blend_sequence.actions_ptr[machine_ptr->current_step - 3].type = ACTION_MTP;           
            blend_sequence.actions_ptr[machine_ptr->current_step - 3].mtp.new_position = machine_ptr->blender.position - 40; // position
            blend_sequence.actions_ptr[machine_ptr->current_step - 3].mtp.move_direction = BLENDER_MOVEMENT_UP;
            blend_sequence.actions_ptr[machine_ptr->current_step - 3].mtp.time_out = 3000;
            blend_sequence.actions_ptr[machine_ptr->current_step - 3].mtp.speed = MOTOR_SPEED_CERTIAN; //half
              
            blend_sequence.actions_ptr[machine_ptr->current_step - 2].type = ACTION_WAIT;
            blend_sequence.actions_ptr[machine_ptr->current_step - 2].wait.time_to_wait = 250; //ms

            blend_sequence.actions_ptr[machine_ptr->current_step - 1].type = ACTION_WAIT;
            blend_sequence.actions_ptr[machine_ptr->current_step - 1].wait.time_to_wait = 250; //ms

            machine_ptr->current_step = machine_ptr->current_step - 4;

            
            
            if(jam_counter == 5)
            {
              //if(machine_ptr->blender.position > 560){
              
                //for (int j = 0; j < 2; j++) {
                  //votex
                  blend_sequence.actions_ptr[machine_ptr->current_step - 4].type = ACTION_MTP;
                  blend_sequence.actions_ptr[machine_ptr->current_step - 4].mtp.new_position = 495;//machine_ptr->blender.position - 60; // position TOP_OF_CUP ,TOP_OF_SMOOTHIE  + 50
                  blend_sequence.actions_ptr[machine_ptr->current_step - 4].mtp.move_direction = BLENDER_MOVEMENT_UP;
                  blend_sequence.actions_ptr[machine_ptr->current_step - 4].mtp.time_out = 3000;
                  blend_sequence.actions_ptr[machine_ptr->current_step - 4].mtp.speed = MOTOR_SPEED_HALF;


                  blend_sequence.actions_ptr[machine_ptr->current_step - 3].type = ACTION_WAIT;
                  blend_sequence.actions_ptr[machine_ptr->current_step - 3].wait.time_to_wait = 3350; //ms

                  blend_sequence.actions_ptr[machine_ptr->current_step - 2].type = ACTION_MTP;
                  blend_sequence.actions_ptr[machine_ptr->current_step - 2].mtp.new_position = machine_ptr->blender.position - 5; // position TOP_OF_CUP - 20, TOP_OF_SMOOTHIE  + 45
                  blend_sequence.actions_ptr[machine_ptr->current_step - 2].mtp.move_direction = BLENDER_MOVEMENT_DOWN;
                  blend_sequence.actions_ptr[machine_ptr->current_step - 2].mtp.time_out = 3000;
                  blend_sequence.actions_ptr[machine_ptr->current_step - 2].mtp.speed = MOTOR_SPEED_QUARTER;

                  
                  blend_sequence.actions_ptr[machine_ptr->current_step - 1].type = ACTION_WAIT;
                  blend_sequence.actions_ptr[machine_ptr->current_step - 1].wait.time_to_wait = 350; //ms

                  machine_ptr->current_step = machine_ptr->current_step - 4;
                //}
              //}
              
              /*else{
                blend_sequence.actions_ptr[machine_ptr->current_step - 3].type = ACTION_WAIT;
                blend_sequence.actions_ptr[machine_ptr->current_step - 3].wait.time_to_wait = 350; //ms

              
                blend_sequence.actions_ptr[machine_ptr->current_step - 2].type = ACTION_MTP;           
                blend_sequence.actions_ptr[machine_ptr->current_step - 2].mtp.new_position = TOP_OF_SMOOTHIE + 45; // position  TOP_OF_SMOOTHIE 30
                blend_sequence.actions_ptr[machine_ptr->current_step - 2].mtp.move_direction = BLENDER_MOVEMENT_UP;
                blend_sequence.actions_ptr[machine_ptr->current_step - 2].mtp.time_out = 3000;
                blend_sequence.actions_ptr[machine_ptr->current_step - 2].mtp.speed = MOTOR_SPEED_THIRD; //half

                blend_sequence.actions_ptr[machine_ptr->current_step - 1].type = ACTION_WAIT;
                blend_sequence.actions_ptr[machine_ptr->current_step - 1].wait.time_to_wait = 100; //ms
                machine_ptr->current_step = machine_ptr->current_step - 3;
              }*/
              

              jam_counter = 0;
            }


          }


          
          break;
    }
    machine_ptr->last_jam_check_position = machine_ptr->blender.position;
    machine_ptr->last_jam_check_time = millis();
  }
}

