#ifndef LIQUID_FILLING_H
#define LIQUID_FILLING_H

#include "global.h"

#define LIQUID_FILLING_IDLE 0
#define LIQUID_FILLING_CYCLE 1


typedef struct {
  char state;  
} liquid_filler_t;


void liquid_filling_init(liquid_filler_t*);
void liquid_filling_process(liquid_filler_t*);

#endif
