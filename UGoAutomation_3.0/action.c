#include "actions.h"
#include "blender.h"
#include "machine.h"// add

#define MAX_ACTIONS 150

void blend_actions_init(char reinit) {
  int i, j = 0;
  blend_sequence.jam_counter_total = 0;
  // allocate space for actions_ptr
  if (!reinit) {
    blend_sequence.actions_ptr = (action_t*) malloc(MAX_ACTIONS * sizeof(action_t));
  }

  // STARTING OF BLENDING SEQUENCE
  blend_sequence.actions_ptr[i].type = ACTION_WAIT_FOR;
  blend_sequence.actions_ptr[i].wait_for.type = WAIT_FOR_CUP_IN_PLACE; // position
  blend_sequence.actions_ptr[i].wait_for.value = 15;
  blend_sequence.actions_ptr[i++].wait_for.comparer = WAIT_FOR_LESS_THAN;

  blend_sequence.actions_ptr[i].type = ACTION_WAIT;
  blend_sequence.actions_ptr[i++].wait.time_to_wait = 1000; //ms

  blend_sequence.actions_ptr[i].type = ACTION_ACTIVATE;
  blend_sequence.actions_ptr[i].activate.address = LIQUID_FILLING_VALVE_ADDRESS;
  blend_sequence.actions_ptr[i++].activate.state = ON;

  blend_sequence.actions_ptr[i].type = ACTION_ACTIVATE;
  blend_sequence.actions_ptr[i].activate.address = CLEANING_VALVE_ADDRESS;
  blend_sequence.actions_ptr[i++].activate.state = OFF;

  // wait for valve to activate before turing pump on
  blend_sequence.actions_ptr[i].type = ACTION_WAIT;
  blend_sequence.actions_ptr[i++].wait.time_to_wait = 500; //ms
  
  blend_sequence.actions_ptr[i].type = ACTION_ACTIVATE;
  blend_sequence.actions_ptr[i].activate.address = PUMP_ADDRESS;
  blend_sequence.actions_ptr[i++].activate.state = ON;

  blend_sequence.actions_ptr[i].type = ACTION_WAIT;
  blend_sequence.actions_ptr[i++].wait.time_to_wait = 3100; //ms 2750
  
  blend_sequence.actions_ptr[i].type = ACTION_ACTIVATE;
  blend_sequence.actions_ptr[i].activate.address = PUMP_ADDRESS;
  blend_sequence.actions_ptr[i++].activate.state = OFF;

  blend_sequence.actions_ptr[i].type = ACTION_WAIT;
  blend_sequence.actions_ptr[i++].wait.time_to_wait = 100; //ms

  // 1. Move the blender to above the cup
  blend_sequence.actions_ptr[i].type = ACTION_MTP;
  blend_sequence.actions_ptr[i].mtp.new_position = TOP_OF_CUP; // position TOP_OF_CUP+20， 35, 45
  blend_sequence.actions_ptr[i].mtp.move_direction = BLENDER_MOVEMENT_DOWN;
  blend_sequence.actions_ptr[i].mtp.time_out = 5000;
  blend_sequence.actions_ptr[i++].mtp.speed = MOTOR_SPEED_HALF;

  blend_sequence.actions_ptr[i].type = ACTION_WAIT;
  blend_sequence.actions_ptr[i++].wait.time_to_wait = 500; //ms

  blend_sequence.actions_ptr[i].type = ACTION_ACTIVATE;
  blend_sequence.actions_ptr[i].activate.address = BLENDER_SPEED_ADDRESS;
  blend_sequence.actions_ptr[i++].activate.state = ON;//ON
  
  blend_sequence.actions_ptr[i].type = ACTION_WAIT;
  blend_sequence.actions_ptr[i++].wait.time_to_wait = 100; //ms 100

  // 2. Turn blender on
  blend_sequence.actions_ptr[i].type = ACTION_ACTIVATE;
  blend_sequence.actions_ptr[i].activate.address = BLENDER_ADDRESS;
  blend_sequence.actions_ptr[i++].activate.state = ON; // on

  blend_sequence.actions_ptr[i].type = ACTION_WAIT;
  blend_sequence.actions_ptr[i++].wait.time_to_wait = 100; //ms
  //need half speed of blade to prevent from splattering

  /*blend_sequence.actions_ptr[i].type = ACTION_ACTIVATE;
  blend_sequence.actions_ptr[i].activate.address = BLENDER_SPEED_ADDRESS;
  blend_sequence.actions_ptr[i++].activate.state = OFF; //OFF*/

  blend_sequence.actions_ptr[i].type = ACTION_WAIT;
  blend_sequence.actions_ptr[i++].wait.time_to_wait = 100; //ms

  blend_sequence.actions_ptr[i].type = ACTION_ACTIVATE;
  blend_sequence.actions_ptr[i].activate.address = LIQUID_FILLING_VALVE_ADDRESS;
  blend_sequence.actions_ptr[i++].activate.state = ON;
  
  blend_sequence.actions_ptr[i].type = ACTION_ACTIVATE;
  blend_sequence.actions_ptr[i].activate.address = CLEANING_VALVE_ADDRESS;
  blend_sequence.actions_ptr[i++].activate.state = OFF;

  blend_sequence.actions_ptr[i].type = ACTION_WAIT;
  blend_sequence.actions_ptr[i++].wait.time_to_wait = 100; //ms

 

  //add main blending
  blend_sequence.actions_ptr[i].type = ACTION_MTP;
  blend_sequence.actions_ptr[i].mtp.new_position = BOTTOM_OF_CUP - 60; // position(20*J)
  blend_sequence.actions_ptr[i].mtp.move_direction = BLENDER_MOVEMENT_DOWN;
  blend_sequence.actions_ptr[i].mtp.time_out = 3000;
  blend_sequence.actions_ptr[i++].mtp.speed = MOTOR_SPEED_HALF; //full

  blend_sequence.actions_ptr[i].type = ACTION_WAIT;
  blend_sequence.actions_ptr[i++].wait.time_to_wait = 1250; //ms

  blend_sequence.actions_ptr[i].type = ACTION_MTP;
  blend_sequence.actions_ptr[i].mtp.new_position = BOTTOM_OF_CUP; // position(20*J)
  blend_sequence.actions_ptr[i].mtp.move_direction = BLENDER_MOVEMENT_DOWN;
  blend_sequence.actions_ptr[i].mtp.time_out = 3000;
  blend_sequence.actions_ptr[i++].mtp.speed = MOTOR_SPEED_HALF; //full

  /*blend_sequence.actions_ptr[i].type = ACTION_AGITATE;
  blend_sequence.actions_ptr[i].mtp.new_position = BOTTOM_OF_CUP; // position(20*J)
  blend_sequence.actions_ptr[i].mtp.move_direction = BLENDER_MOVEMENT_DOWN;
  blend_sequence.actions_ptr[i].mtp.time_out = 3000;
  blend_sequence.actions_ptr[i++].mtp.speed = MOTOR_SPEED_HALF; //full*/

  blend_sequence.actions_ptr[i].type = ACTION_WAIT;
  blend_sequence.actions_ptr[i++].wait.time_to_wait = 250; //ms

  
  //add: stir bottom - move slightly downwards each pulse to break any remaining fruit
  //full blender speed
  /*blend_sequence.actions_ptr[i].type = ACTION_ACTIVATE;
  blend_sequence.actions_ptr[i].activate.address = BLENDER_SPEED_ADDRESS;
  blend_sequence.actions_ptr[i++].activate.state = ON;*/

  blend_sequence.actions_ptr[i].type = ACTION_WAIT;
  blend_sequence.actions_ptr[i++].wait.time_to_wait = 250; //ms

  for (j = 0; j < 4; j++) {
    blend_sequence.actions_ptr[i].type = ACTION_MTP;
    blend_sequence.actions_ptr[i].mtp.new_position = BOTTOM_OF_CUP - 40; // position 595-30 BOTTOM_OF_CUP - 60
    blend_sequence.actions_ptr[i].mtp.move_direction = BLENDER_MOVEMENT_UP;
    blend_sequence.actions_ptr[i].mtp.time_out = 3000;
    blend_sequence.actions_ptr[i++].mtp.speed = MOTOR_SPEED_HALF;

    blend_sequence.actions_ptr[i].type = ACTION_WAIT;
    blend_sequence.actions_ptr[i++].wait.time_to_wait = 250; //ms
    
    blend_sequence.actions_ptr[i].type = ACTION_MTP;
    blend_sequence.actions_ptr[i].mtp.new_position = BOTTOM_OF_CUP; // position 595+5  BOTTOM_OF_CUP -40,BOTTOM_OF_CUP
    blend_sequence.actions_ptr[i].mtp.move_direction = BLENDER_MOVEMENT_DOWN;
    blend_sequence.actions_ptr[i].mtp.time_out = 3000;
    blend_sequence.actions_ptr[i++].mtp.speed = MOTOR_SPEED_HALF; //full half

    blend_sequence.actions_ptr[i].type = ACTION_WAIT;
    blend_sequence.actions_ptr[i++].wait.time_to_wait = 250; //ms    
  }


   for (j = 0; j < 2; j++) {
    blend_sequence.actions_ptr[i].type = ACTION_MTP;
    blend_sequence.actions_ptr[i].mtp.new_position = TOP_OF_CUP + 70; // position 595-30  TOP_OF_CUP + 15,40
    blend_sequence.actions_ptr[i].mtp.move_direction = BLENDER_MOVEMENT_UP;
    blend_sequence.actions_ptr[i].mtp.time_out = 3000;
    blend_sequence.actions_ptr[i++].mtp.speed = MOTOR_SPEED_HALF; //half

    blend_sequence.actions_ptr[i].type = ACTION_WAIT;
    blend_sequence.actions_ptr[i++].wait.time_to_wait = 250; //ms
    
    /*blend_sequence.actions_ptr[i].type = ACTION_ACTIVATE;
    blend_sequence.actions_ptr[i].activate.address = BLENDER_SPEED_ADDRESS;
    blend_sequence.actions_ptr[i++].activate.state = ON;
*/
    blend_sequence.actions_ptr[i].type = ACTION_WAIT;
    blend_sequence.actions_ptr[i++].wait.time_to_wait = 250; //ms
    
    blend_sequence.actions_ptr[i].type = ACTION_MTP;
    blend_sequence.actions_ptr[i].mtp.new_position = BOTTOM_OF_CUP + 20; // position 565+5 ,+15, BOTTOM_OF_CUP
    blend_sequence.actions_ptr[i].mtp.move_direction = BLENDER_MOVEMENT_DOWN;
    blend_sequence.actions_ptr[i].mtp.time_out = 3000;
    blend_sequence.actions_ptr[i++].mtp.speed = MOTOR_SPEED_HALF; //half

    blend_sequence.actions_ptr[i].type = ACTION_WAIT;
    blend_sequence.actions_ptr[i++].wait.time_to_wait = 750; //ms 1000
    
  }    
  
   //repeat
    //
    /*blend_sequence.actions_ptr[i].type = ACTION_ACTIVATE;
    blend_sequence.actions_ptr[i].activate.address = BLENDER_ADDRESS;
    blend_sequence.actions_ptr[i++].activate.state = OFF;

    blend_sequence.actions_ptr[i].type = ACTION_WAIT;
    blend_sequence.actions_ptr[i++].wait.time_to_wait = 250; //ms   

    blend_sequence.actions_ptr[i].type = ACTION_MTP;
    blend_sequence.actions_ptr[i].mtp.new_position = TOP_OF_CUP + 20; // position 595-30  TOP_OF_CUP + 15
    blend_sequence.actions_ptr[i].mtp.move_direction = BLENDER_MOVEMENT_UP;
    blend_sequence.actions_ptr[i].mtp.time_out = 3000;
    blend_sequence.actions_ptr[i++].mtp.speed = MOTOR_SPEED_HALF; //half */
    //

   /*for (j = 0; j < 2; j++) {
    blend_sequence.actions_ptr[i].type = ACTION_MTP;
    blend_sequence.actions_ptr[i].mtp.new_position = BOTTOM_OF_CUP - 40; // position 595-30 BOTTOM_OF_CUP - 60
    blend_sequence.actions_ptr[i].mtp.move_direction = BLENDER_MOVEMENT_UP;
    blend_sequence.actions_ptr[i].mtp.time_out = 3000;
    blend_sequence.actions_ptr[i++].mtp.speed = MOTOR_SPEED_QUARTER;

    blend_sequence.actions_ptr[i].type = ACTION_WAIT;
    blend_sequence.actions_ptr[i++].wait.time_to_wait = 250; //ms

    
    blend_sequence.actions_ptr[i].type = ACTION_MTP;
    blend_sequence.actions_ptr[i].mtp.new_position = BOTTOM_OF_CUP + 10; // position 595+5  BOTTOM_OF_CUP -40,BOTTOM_OF_CUP
    blend_sequence.actions_ptr[i].mtp.move_direction = BLENDER_MOVEMENT_DOWN;
    blend_sequence.actions_ptr[i].mtp.time_out = 3000;
    blend_sequence.actions_ptr[i++].mtp.speed = MOTOR_SPEED_QUARTER; //full half

    

    blend_sequence.actions_ptr[i].type = ACTION_WAIT;
    blend_sequence.actions_ptr[i++].wait.time_to_wait = 250; //ms  

    //
    blend_sequence.actions_ptr[i].type = ACTION_ACTIVATE;
    blend_sequence.actions_ptr[i].activate.address = BLENDER_ADDRESS;
    blend_sequence.actions_ptr[i++].activate.state = ON;
    //  
  }*/
  
  //refill
  blend_sequence.actions_ptr[i].type = ACTION_ACTIVATE;
  blend_sequence.actions_ptr[i].activate.address = LIQUID_FILLING_VALVE_ADDRESS;
  blend_sequence.actions_ptr[i++].activate.state = ON;

  blend_sequence.actions_ptr[i].type = ACTION_ACTIVATE;
  blend_sequence.actions_ptr[i].activate.address = CLEANING_VALVE_ADDRESS;
  blend_sequence.actions_ptr[i++].activate.state = OFF;

  blend_sequence.actions_ptr[i].type = ACTION_WAIT;
  blend_sequence.actions_ptr[i++].wait.time_to_wait = 100; //ms
  
  blend_sequence.actions_ptr[i].type = ACTION_ACTIVATE;
  blend_sequence.actions_ptr[i].activate.address = PUMP_ADDRESS;
  blend_sequence.actions_ptr[i++].activate.state = ON;
  

  blend_sequence.actions_ptr[i].type = ACTION_WAIT;
  blend_sequence.actions_ptr[i++].wait.time_to_wait = 1000; //ms 2750
  
  blend_sequence.actions_ptr[i].type = ACTION_ACTIVATE;
  blend_sequence.actions_ptr[i].activate.address = PUMP_ADDRESS;
  blend_sequence.actions_ptr[i++].activate.state = OFF;

  blend_sequence.actions_ptr[i].type = ACTION_WAIT;
  blend_sequence.actions_ptr[i++].wait.time_to_wait = 1000; //ms

  blend_sequence.actions_ptr[i].type = ACTION_ACTIVATE;
  blend_sequence.actions_ptr[i].activate.address = BLENDER_ADDRESS;
  blend_sequence.actions_ptr[i++].activate.state = ON;
  //
   for (j = 0; j < 4; j++) {
    blend_sequence.actions_ptr[i].type = ACTION_MTP;
    blend_sequence.actions_ptr[i].mtp.new_position = TOP_OF_CUP + 40; // position 595-30  TOP_OF_CUP + 15
    blend_sequence.actions_ptr[i].mtp.move_direction = BLENDER_MOVEMENT_UP;
    blend_sequence.actions_ptr[i].mtp.time_out = 3000;
    blend_sequence.actions_ptr[i++].mtp.speed = MOTOR_SPEED_QUARTER; //half


    blend_sequence.actions_ptr[i].type = ACTION_WAIT;
    blend_sequence.actions_ptr[i++].wait.time_to_wait = 250; //ms
    
    blend_sequence.actions_ptr[i].type = ACTION_MTP;
    blend_sequence.actions_ptr[i].mtp.new_position = BOTTOM_OF_CUP; // position 565+5 ,+15, BOTTOM_OF_CUP
    blend_sequence.actions_ptr[i].mtp.move_direction = BLENDER_MOVEMENT_DOWN;
    blend_sequence.actions_ptr[i].mtp.time_out = 3000;
    blend_sequence.actions_ptr[i++].mtp.speed = MOTOR_SPEED_QUARTER;//half

    blend_sequence.actions_ptr[i].type = ACTION_WAIT;
    blend_sequence.actions_ptr[i++].wait.time_to_wait = 700; //ms 1000 2750
    
  }    
  
  // 23. Move to top, stay in liquid
  blend_sequence.actions_ptr[i].type = ACTION_ACTIVATE;
  blend_sequence.actions_ptr[i].activate.address = BLENDER_ADDRESS;
  blend_sequence.actions_ptr[i++].activate.state = OFF;

  blend_sequence.actions_ptr[i].type = ACTION_WAIT;
  blend_sequence.actions_ptr[i++].wait.time_to_wait = 1000; //ms  

  blend_sequence.actions_ptr[i].type = ACTION_MTP;
  blend_sequence.actions_ptr[i].mtp.new_position = TOP_OF_CUP + 20; // position
  blend_sequence.actions_ptr[i].mtp.move_direction = BLENDER_MOVEMENT_UP;
  blend_sequence.actions_ptr[i].mtp.time_out = 5000;
  blend_sequence.actions_ptr[i++].mtp.speed = MOTOR_SPEED_HALF;

  blend_sequence.actions_ptr[i].type = ACTION_WAIT;
  blend_sequence.actions_ptr[i++].wait.time_to_wait = 100; //ms
  
  // SHAKE OFF above smoothie
  for (j = 0; j < 7; j++) {
    blend_sequence.actions_ptr[i].type = ACTION_MTP;
    blend_sequence.actions_ptr[i].mtp.new_position = TOP_OF_CUP - 15; // position TOP_OF_CUP - 15, 19，-15,+5
    blend_sequence.actions_ptr[i].mtp.move_direction = BLENDER_MOVEMENT_UP;
    blend_sequence.actions_ptr[i].mtp.time_out = 3000;
    blend_sequence.actions_ptr[i++].mtp.speed = MOTOR_SPEED_FULL;

    //blend_sequence.actions_ptr[i].type = ACTION_WAIT;
    //blend_sequence.actions_ptr[i++].wait.time_to_wait = 250; //ms
    
    blend_sequence.actions_ptr[i].type = ACTION_MTP;
    blend_sequence.actions_ptr[i].mtp.new_position = TOP_OF_CUP - 5; // position TOP_OF_CUP-10, 14，-10,+10
    blend_sequence.actions_ptr[i].mtp.move_direction = BLENDER_MOVEMENT_DOWN;
    blend_sequence.actions_ptr[i].mtp.time_out = 3000;
    blend_sequence.actions_ptr[i++].mtp.speed = MOTOR_SPEED_FULL;

    //blend_sequence.actions_ptr[i].type = ACTION_WAIT;
    //blend_sequence.actions_ptr[i++].wait.time_to_wait = 250; //ms
  }

  // 24. Turn blender off
  blend_sequence.actions_ptr[i].type = ACTION_WAIT;
  blend_sequence.actions_ptr[i++].wait.time_to_wait = 100; //ms

  //blend_sequence.actions_ptr[i].type = ACTION_ACTIVATE;
  //blend_sequence.actions_ptr[i].activate.address = BLENDER_ADDRESS;
  //blend_sequence.actions_ptr[i++].activate.state = OFF;

  //blend_sequence.actions_ptr[i].type = ACTION_WAIT;
  //blend_sequence.actions_ptr[i++].wait.time_to_wait = 1400; //ms

  blend_sequence.actions_ptr[i].type = ACTION_MTP;
  blend_sequence.actions_ptr[i].mtp.new_position = TOP_OF_CUP - 30; // position TOP_OF_CUP - 20
  blend_sequence.actions_ptr[i].mtp.move_direction = BLENDER_MOVEMENT_UP;
  blend_sequence.actions_ptr[i].mtp.time_out = 3000;
  blend_sequence.actions_ptr[i++].mtp.speed = MOTOR_SPEED_FULL;


  // SHAKE OFF above top
  for (j = 0; j < 7; j++) {
    blend_sequence.actions_ptr[i].type = ACTION_MTP;
    blend_sequence.actions_ptr[i].mtp.new_position = TOP_OF_CUP - 35; // position TOP_OF_CUP - 20
    blend_sequence.actions_ptr[i].mtp.move_direction = BLENDER_MOVEMENT_UP;
    blend_sequence.actions_ptr[i].mtp.time_out = 3000;
    blend_sequence.actions_ptr[i++].mtp.speed = MOTOR_SPEED_FULL;

    //blend_sequence.actions_ptr[i].type = ACTION_WAIT;
    //blend_sequence.actions_ptr[i++].wait.time_to_wait = 250; //ms
    
    blend_sequence.actions_ptr[i].type = ACTION_MTP;
    blend_sequence.actions_ptr[i].mtp.new_position = TOP_OF_CUP - 25; // position TOP_OF_CUP
    blend_sequence.actions_ptr[i].mtp.move_direction = BLENDER_MOVEMENT_DOWN;
    blend_sequence.actions_ptr[i].mtp.time_out = 3000;
    blend_sequence.actions_ptr[i++].mtp.speed = MOTOR_SPEED_FULL;

    //blend_sequence.actions_ptr[i].type = ACTION_WAIT;
    //blend_sequence.actions_ptr[i++].wait.time_to_wait = 250; //ms
  }
  
  // 25. Return home
  blend_sequence.actions_ptr[i].type = ACTION_MTP;
  blend_sequence.actions_ptr[i].mtp.new_position = TOP_POSITION; // position
  blend_sequence.actions_ptr[i].mtp.move_direction = BLENDER_MOVEMENT_UP;
  blend_sequence.actions_ptr[i].mtp.time_out = 10000;
  blend_sequence.actions_ptr[i++].mtp.speed = MOTOR_SPEED_FULL;

  blend_sequence.actions_ptr[i].type = ACTION_ACTIVATE;
  blend_sequence.actions_ptr[i].activate.address = LIQUID_FILLING_VALVE_ADDRESS;
  blend_sequence.actions_ptr[i++].activate.state = OFF;

  blend_sequence.actions_ptr[i].type = ACTION_WAIT;
  blend_sequence.actions_ptr[i++].wait.time_to_wait = 100; //ms

  blend_sequence.actions_ptr[i].type = ACTION_ACTIVATE;
  blend_sequence.actions_ptr[i].activate.address = CLEANING_VALVE_ADDRESS;
  blend_sequence.actions_ptr[i++].activate.state = OFF;

  blend_sequence.actions_ptr[i].type = ACTION_WAIT;
  blend_sequence.actions_ptr[i++].wait.time_to_wait = 100; //ms

  blend_sequence.actions_ptr[i].type = ACTION_ACTIVATE;
  blend_sequence.actions_ptr[i].activate.address = BLENDER_SPEED_ADDRESS;
  blend_sequence.actions_ptr[i++].activate.state = OFF;

  if (i > MAX_ACTIONS) {
    LOG_PRINT(LOGGER_ERROR, "Buffer overflow!!");
  } else {
    LOG_PRINT(LOGGER_ERROR, "total actions: %d", i);
  }
  
  blend_sequence.total_actions = i;
  blend_sequence.jam_counter_total = 0;
}

void clean_actions_init() {
  int i, j = 0;
  
  clean_sequence.actions_ptr = (action_t*) malloc(50 * sizeof(action_t));
  
  clean_sequence.actions_ptr[i].type = ACTION_WAIT_FOR;
  clean_sequence.actions_ptr[i].wait_for.type = WAIT_FOR_CUP_IN_PLACE; // position
  clean_sequence.actions_ptr[i].wait_for.value = 18;
  clean_sequence.actions_ptr[i++].wait_for.comparer = WAIT_FOR_GREATER_THAN;

  clean_sequence.actions_ptr[i].type = ACTION_WAIT;
  clean_sequence.actions_ptr[i++].wait.time_to_wait = 2000; //ms
  
  
  // add move the blender directly to the bottom of the cup
  clean_sequence.actions_ptr[i].type = ACTION_MTP;
  clean_sequence.actions_ptr[i].mtp.new_position = BOTTOM_OF_CLEANING - 60; // position really bottom
  clean_sequence.actions_ptr[i].mtp.move_direction = BLENDER_MOVEMENT_DOWN;
  clean_sequence.actions_ptr[i].mtp.time_out = 5000;
  clean_sequence.actions_ptr[i++].mtp.speed = MOTOR_SPEED_HALF;

  //ONLY turn the top valve
  clean_sequence.actions_ptr[i].type = ACTION_ACTIVATE;
  clean_sequence.actions_ptr[i].activate.address = LIQUID_FILLING_VALVE_ADDRESS;
  clean_sequence.actions_ptr[i++].activate.state = ON; 

  // turn the bottom valve off
  clean_sequence.actions_ptr[i].type = ACTION_ACTIVATE;
  clean_sequence.actions_ptr[i].activate.address = CLEANING_VALVE_ADDRESS;
  clean_sequence.actions_ptr[i++].activate.state = OFF; 

  // wait for valve to activate before turing pump on
  clean_sequence.actions_ptr[i].type = ACTION_WAIT;
  clean_sequence.actions_ptr[i++].wait.time_to_wait = 500; //ms

 
  // 2. Turn blender on
  clean_sequence.actions_ptr[i].type = ACTION_ACTIVATE;
  clean_sequence.actions_ptr[i].activate.address = BLENDER_SPEED_ADDRESS;
  clean_sequence.actions_ptr[i++].activate.state = ON;
  
  // wait for valve to activate before turing pump on
  clean_sequence.actions_ptr[i].type = ACTION_WAIT;
  clean_sequence.actions_ptr[i++].wait.time_to_wait = 500; //ms
  //full speed of blade
  clean_sequence.actions_ptr[i].type = ACTION_ACTIVATE;
  clean_sequence.actions_ptr[i].activate.address = BLENDER_ADDRESS;
  clean_sequence.actions_ptr[i++].activate.state = ON;
    
  //turn on the pump
  clean_sequence.actions_ptr[i].type = ACTION_WAIT;
  clean_sequence.actions_ptr[i++].wait.time_to_wait = 100; //ms

  clean_sequence.actions_ptr[i].type = ACTION_ACTIVATE;
  clean_sequence.actions_ptr[i].activate.address = PUMP_ADDRESS;
  clean_sequence.actions_ptr[i++].activate.state = ON; //should be on

  clean_sequence.actions_ptr[i].type = ACTION_WAIT;
  clean_sequence.actions_ptr[i++].wait.time_to_wait = 100; //ms
     
  clean_sequence.actions_ptr[i].type = ACTION_ACTIVATE;
  clean_sequence.actions_ptr[i].activate.address = LIQUID_FILLING_VALVE_ADDRESS;
  clean_sequence.actions_ptr[i++].activate.state = ON;
    
  clean_sequence.actions_ptr[i].type = ACTION_WAIT;
  clean_sequence.actions_ptr[i++].wait.time_to_wait = 1500; //ms
  //ADD
  clean_sequence.actions_ptr[i].type = ACTION_ACTIVATE;
  clean_sequence.actions_ptr[i].activate.address = LIQUID_FILLING_VALVE_ADDRESS;
  clean_sequence.actions_ptr[i++].activate.state = OFF;

  clean_sequence.actions_ptr[i].type = ACTION_WAIT;
  clean_sequence.actions_ptr[i++].wait.time_to_wait = 100; //ms


  clean_sequence.actions_ptr[i].type = ACTION_ACTIVATE;
  clean_sequence.actions_ptr[i].activate.address = PUMP_ADDRESS;
  clean_sequence.actions_ptr[i++].activate.state = OFF; 

  //IF ADD THIS, THE BLENDER CANT GO UP, NO IDEA
  /*blend_sequence.actions_ptr[i].type = ACTION_WAIT;
  blend_sequence.actions_ptr[i++].wait.time_to_wait = 100; //ms*/

  clean_sequence.actions_ptr[i].type = ACTION_MTP;
  clean_sequence.actions_ptr[i].mtp.new_position = CLEANING_LEVEL - 20;//at the surface of plastic 555
  clean_sequence.actions_ptr[i].mtp.move_direction = BLENDER_MOVEMENT_UP;
  clean_sequence.actions_ptr[i].mtp.time_out = 5000;
  clean_sequence.actions_ptr[i++].mtp.speed = MOTOR_SPEED_HALF;

  
  
  clean_sequence.actions_ptr[i].type = ACTION_ACTIVATE;
  clean_sequence.actions_ptr[i].activate.address = LIQUID_FILLING_VALVE_ADDRESS;
  clean_sequence.actions_ptr[i++].activate.state = OFF;

  //add more time to turn off the top valve
  clean_sequence.actions_ptr[i].type = ACTION_WAIT;
  clean_sequence.actions_ptr[i++].wait.time_to_wait = 100; //ms

  clean_sequence.actions_ptr[i].type = ACTION_ACTIVATE;
  clean_sequence.actions_ptr[i].activate.address = CLEANING_VALVE_ADDRESS;
  clean_sequence.actions_ptr[i++].activate.state = ON;// should be on

  clean_sequence.actions_ptr[i].type = ACTION_WAIT;
  clean_sequence.actions_ptr[i++].wait.time_to_wait = 1000; //ms

   // use remaining speed of blade to clean
  clean_sequence.actions_ptr[i].type = ACTION_ACTIVATE;
  clean_sequence.actions_ptr[i].activate.address = BLENDER_ADDRESS;
  clean_sequence.actions_ptr[i++].activate.state = OFF;

  clean_sequence.actions_ptr[i].type = ACTION_WAIT;
  clean_sequence.actions_ptr[i++].wait.time_to_wait = 100; //ms

  clean_sequence.actions_ptr[i].type = ACTION_ACTIVATE;
  clean_sequence.actions_ptr[i].activate.address = PUMP_ADDRESS;
  clean_sequence.actions_ptr[i++].activate.state = ON;
    
  clean_sequence.actions_ptr[i].type = ACTION_WAIT;
  clean_sequence.actions_ptr[i++].wait.time_to_wait = 1000; //ms
  
  clean_sequence.actions_ptr[i].type = ACTION_ACTIVATE;
  clean_sequence.actions_ptr[i].activate.address = PUMP_ADDRESS;
  clean_sequence.actions_ptr[i++].activate.state = OFF;
  //add
  clean_sequence.actions_ptr[i].type = ACTION_WAIT;
  clean_sequence.actions_ptr[i++].wait.time_to_wait = 1000; //ms
  
  clean_sequence.actions_ptr[i].type = ACTION_ACTIVATE;
  clean_sequence.actions_ptr[i].activate.address = PUMP_ADDRESS;
  clean_sequence.actions_ptr[i++].activate.state = ON;

  clean_sequence.actions_ptr[i].type = ACTION_WAIT;
  clean_sequence.actions_ptr[i++].wait.time_to_wait = 1000; //ms
  
  clean_sequence.actions_ptr[i].type = ACTION_ACTIVATE;
  clean_sequence.actions_ptr[i].activate.address = PUMP_ADDRESS;
  clean_sequence.actions_ptr[i++].activate.state = OFF;
  //
  clean_sequence.actions_ptr[i].type = ACTION_WAIT;
  clean_sequence.actions_ptr[i++].wait.time_to_wait = 1000; //ms

  clean_sequence.actions_ptr[i].type = ACTION_ACTIVATE;
  clean_sequence.actions_ptr[i].activate.address = LIQUID_FILLING_VALVE_ADDRESS;
  clean_sequence.actions_ptr[i++].activate.state = OFF;
  
  clean_sequence.actions_ptr[i].type = ACTION_WAIT;
  clean_sequence.actions_ptr[i++].wait.time_to_wait = 1000; //ms
  
  clean_sequence.actions_ptr[i].type = ACTION_ACTIVATE;
  clean_sequence.actions_ptr[i].activate.address = CLEANING_VALVE_ADDRESS;
  clean_sequence.actions_ptr[i++].activate.state = OFF;

  clean_sequence.actions_ptr[i].type = ACTION_WAIT;
  clean_sequence.actions_ptr[i++].wait.time_to_wait = 1000; //ms

  clean_sequence.actions_ptr[i].type = ACTION_ACTIVATE;
  clean_sequence.actions_ptr[i].activate.address = BLENDER_SPEED_ADDRESS;
  clean_sequence.actions_ptr[i++].activate.state = OFF;

  /*blend_sequence.actions_ptr[i].type = ACTION_WAIT;
  blend_sequence.actions_ptr[i++].wait.time_to_wait = 100; //ms*/


  
  // add shake of as above here because when the blender goes up, there is still water driping from blade
  /*for (j = 0; j < 5; j++) {
    clean_sequence.actions_ptr[i].type = ACTION_MTP;
    clean_sequence.actions_ptr[i].mtp.new_position = CLEANING_LEVEL - 30; // position
    clean_sequence.actions_ptr[i].mtp.move_direction = BLENDER_MOVEMENT_UP;
    clean_sequence.actions_ptr[i].mtp.time_out = 3000;
    clean_sequence.actions_ptr[i++].mtp.speed = MOTOR_SPEED_FULL;

    clean_sequence.actions_ptr[i].type = ACTION_WAIT;
    clean_sequence.actions_ptr[i++].wait.time_to_wait = 250; //ms
    
    clean_sequence.actions_ptr[i].type = ACTION_MTP;
    clean_sequence.actions_ptr[i].mtp.new_position = CLEANING_LEVEL+10; // position
    clean_sequence.actions_ptr[i].mtp.move_direction = BLENDER_MOVEMENT_DOWN;
    clean_sequence.actions_ptr[i].mtp.time_out = 3000;
    clean_sequence.actions_ptr[i++].mtp.speed = MOTOR_SPEED_FULL;

    clean_sequence.actions_ptr[i].type = ACTION_WAIT;
    clean_sequence.actions_ptr[i++].wait.time_to_wait = 250; //ms
  }*/

  

  clean_sequence.actions_ptr[i].type = ACTION_MTP;
  clean_sequence.actions_ptr[i].mtp.new_position = TOP_POSITION; // position
  clean_sequence.actions_ptr[i].mtp.move_direction = BLENDER_MOVEMENT_UP;
  clean_sequence.actions_ptr[i].mtp.time_out = 10000;
  clean_sequence.actions_ptr[i++].mtp.speed = MOTOR_SPEED_FULL; 

  if (i >= 50) {
    LOG_PRINT(LOGGER_ERROR, "Buffer overflow!!");
  }  
  clean_sequence.total_actions = i;
}

void initializing_action_init() {

  initializing_action.type = ACTION_MTP;
  initializing_action.mtp.new_position = TOP_POSITION;
  initializing_action.mtp.move_direction = BLENDER_MOVEMENT_UP;
  initializing_action.mtp.speed = MOTOR_SPEED_FULL;
  initializing_action.mtp.time_out = 5000;


}
