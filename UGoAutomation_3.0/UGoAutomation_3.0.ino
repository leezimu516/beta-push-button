//kaikk, 1
//2016.11.15
//code the push button in the setup and loop
//automation works fine, slove the splash when third jam moving up
//remount the blender, the height of cleaning cycle need to be changed------->>>changed back
//cancel safety trigger in machine.c, and cancel the sensor when using push button

/*
 * TODO: 
 * LEDs not implelemted
 * 
 */
#include "global.h"

#ifdef __cplusplus 
extern "C" {
#endif
  #include "actions.h"
  #include "machine.h"
#ifdef __cplusplus 
}
#endif

#ifdef USB_COMMUNICATION
#include "usb_comm.h"
#endif

machine_t machines[NUMBER_OF_MACHINES];

void auto_cycle_start(char*);
void clean_cycle_start(char*);
void initialize(char*);
void stop_machine(char*);
void machine_reblend(char*);
void disable_keypad(char* message);

hmi_message_t heartbeat_msg;

long last_heartbeat;

const int PUSH_BLEND_BUTTON = 24;
const int PUSH_CLEAN_BUTTON = 26;
const int PUSH_INITIALIZE = 28;

void setup() {
  int i;
#ifdef USB_COMMUNICATION
  // set up the serial port for communucation
  Serial.begin(115200);
#endif

  // initialize the patterns
  blend_actions_init(0);
  clean_actions_init();
  initializing_action_init();
 
  // initialize the machine
  for (i = 0; i < NUMBER_OF_MACHINES; i++) {
    machine_init(&machines[i]);
  }
  
  // initialize the mediator
  mediator_init();

  // initialize the logger
  logger_init();

  // For the time being, explicitly initialize the machine
  //machines[0].current_state = MACHINE_STATE_INITIALIZING;
  
  mediator_register(MEDIATOR_AUTO_CYCLE_START, auto_cycle_start);
  mediator_register(MEDIATOR_CLEAN_CYCLE_START, clean_cycle_start);
  mediator_register(MEDIATOR_INITIALIZE, initialize);
  mediator_register(MEDIATOR_STOP_REQUEST, stop_machine);
  mediator_register(MEDIATOR_REBLEND, machine_reblend);
  mediator_register(MEDIATOR_JOG_TOP,machine_jog_top);
  mediator_register(MEDIATOR_JOG_BOTTOM, machine_jog_bottom);
  mediator_register(MEDIATOR_MOVE_UP, machine_move_up);
  mediator_register(MEDIATOR_MOVE_DOWN, machine_move_down);
  mediator_register(MEDIATOR_DISABLE_KEYPAD, disable_keypad);

  heartbeat_msg.message_id = MSG_HEARTBEAT;

  LOG_PRINT(LOGGER_INFO, "Setup complete");
  
  last_heartbeat = millis();
  machines[0].last_step_time = millis();


  pinMode(PUSH_BLEND_BUTTON,INPUT_PULLUP);
  pinMode(PUSH_CLEAN_BUTTON,INPUT_PULLUP);
  pinMode(PUSH_INITIALIZE,INPUT_PULLUP);

  //add if arduino restart, blender will go to top. temp hack for now
   machines[0].current_state = MACHINE_STATE_INITIALIZING;

}

void loop() {
  int i = 0;
#ifdef USB_COMMUNICATION
  // check if there are any messages to process
  usb_communication_process();
#endif

  // initialize the machine
  for (i = 0; i < NUMBER_OF_MACHINES; i++) {
    machine_check_safety_conditions(&machines[i]);
    machine_process(&machines[i]);
  } 

  // send out heartbeat message
  if (millis() > last_heartbeat + 1000) {
    last_heartbeat = millis(); 
    usb_communication_send_message(heartbeat_msg, 0);
  }

  //button implementation
  if (digitalRead(PUSH_BLEND_BUTTON) == LOW)
  {
    delay(200);//debounce
    if (digitalRead(PUSH_BLEND_BUTTON) == LOW){
      machines[0].current_state = MACHINE_STATE_BLENDING;
    }
  }

  if (digitalRead(PUSH_CLEAN_BUTTON) == LOW)
  {
    delay(200);
      if (digitalRead(PUSH_CLEAN_BUTTON) == LOW)
    {
      clean_sequence.actions_ptr[0].wait_for.comparer = CANCEL_SENSOR;
      clean_sequence.actions_ptr[1].wait.time_to_wait = 0; //ms
      machines[0].current_state = MACHINE_STATE_CLEANING;
    }
  }

  if (digitalRead(PUSH_INITIALIZE) == LOW)
  {
    delay(150);
      if (digitalRead(PUSH_INITIALIZE) == LOW)
    {
      machines[0].current_state = MACHINE_STATE_INITIALIZING;
    }
  }

  

}

// TODO: seriously, we need to add validation to this, otherwise
// we can change from blending to cleaning without stopping.....
void auto_cycle_start(char* args) {
  LOG_PRINT(LOGGER_INFO, "Starting auto cycle");
  machines[0].current_state = MACHINE_STATE_BLENDING;
}

void clean_cycle_start(char*) {
  LOG_PRINT(LOGGER_INFO, "Starting clean cycle");
  machines[0].current_state = MACHINE_STATE_CLEANING;
}

void initialize(char*){
  LOG_PRINT(LOGGER_INFO, "Initializing");
  machines[0].current_state = MACHINE_STATE_INITIALIZING;
}

void stop_machine(char* args) {
  LOG_PRINT(LOGGER_INFO, "Stopping machine");
  machines[0].current_state = MACHINE_STATE_IDLE;
}

void machine_reblend(char* message){
  LOG_PRINT(LOGGER_VERBOSE, "Starting reblending");
  blend_sequence.actions_ptr[5].activate.state = OFF;
  blend_sequence.actions_ptr[51].activate.state = OFF;
  machines[0].current_state = MACHINE_STATE_BLENDING;
}


void machine_jog_top(char* message){
  LOG_PRINT(LOGGER_VERBOSE, "Jogging top");
  machines[0].buttons[JOG_PUMP_BUTTON].current_state = !machines[0].buttons[JOG_PUMP_BUTTON].current_state;
}

void machine_jog_bottom(char* message){
  LOG_PRINT(LOGGER_VERBOSE, "Jogging bottom");
  digitalWrite(PUMP_ADDRESS, 0);
  digitalWrite(CLEANING_VALVE_ADDRESS, 0);
}

void machine_move_up(char* message){
  LOG_PRINT(LOGGER_VERBOSE, "Moving up");
  machines[0].buttons[MOVE_UP].current_state = !machines[0].buttons[MOVE_UP].current_state;
}

void machine_move_down(char* message){
  send_status("Moving down");
  machines[0].buttons[MOVE_DOWN].current_state = !machines[0].buttons[MOVE_DOWN].current_state;
}

void disable_keypad(char* message) {
  send_status("Keypad toggled");
  machines[0].keypad_enabled = ! machines[0].keypad_enabled;
}

