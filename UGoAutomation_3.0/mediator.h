#ifndef __MEDIATOR_H
#define __MEDIATOR_H

#include "global.h"

#define MEDIATOR_AUTO_CYCLE_START 0
#define MEDIATOR_STOP_REQUEST 1
#define MEDIATOR_CLEAN_CYCLE_START 2
#define MEDIATOR_INITIALIZE 3
#define MEDIATOR_MOVE_UP 4
#define MEDIATOR_MOVE_DOWN 5
#define MEDIATOR_JOG_TOP 6
#define MEDIATOR_JOG_BOTTOM 7
#define MEDIATOR_REBLEND 8
#define MEDIATOR_DISABLE_KEYPAD 9

typedef void (* ACTION_PTR)(char*);

void mediator_init();
void mediator_register(int event_id, ACTION_PTR action);
void mediator_unregister(int event_id, ACTION_PTR action);
void mediator_send_message(int event_id, char* message);

#endif
