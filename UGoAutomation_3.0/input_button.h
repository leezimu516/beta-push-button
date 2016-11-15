#ifndef INPUT_BUTTON_H
#define INPUT_BUTTON_H

#include "global.h"

#define DEBOUNCING_TIME 150

typedef struct {
  char address;
  char current_state;
  char last_state;
  long last_debounce_time;
} input_button_t;

void input_button_init( input_button_t*, char );
void input_button_read( input_button_t* );

#endif
