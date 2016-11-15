/***************************************************
* Mediator                              <mediator.c>
* Authored By: Steven McKeown
*
* The mediator file is designed to be an event-handler
* system. 
* 
* Function pointers will be stored in an array
* associated with an event type. When a message is
* sent, all functions that are registered against
* the event are triggered.
*
* Any function registered with the mediator needs to
* accept a pointer to generic args.
* 
* EXAMPLE: Registering an event
* mediator_register(MEDIATOR_EVENT_X, function_pointer);
*
* EXAMPLE: Triggering events
* mediator_send_message(MEDIATOR_EVENT_X, pointer_to_args);
*
* SM040716
***************************************************/
#include "mediator.h"

#define MAX_EVENTS 10
#define MAX_ACTIONS_PER_EVENT 10

typedef struct 
{
    int event_id;
    int total_actions;
    ACTION_PTR actions[MAX_ACTIONS_PER_EVENT];
} event_callbacks_t;

// array that stores all the event types, and their function pointers
event_callbacks_t callbacks[MAX_EVENTS];

/* START FUNCTION DESCRIPTION *********************
mediator_init                          <mediator.c>

SYNTAX: void mediator_init( void );

DESCRIPTION:
Initializes the mediator.

RETURN VALUE:  null

SM040716
END DESCRIPTION ***********************************/
void mediator_init()
{
    memset(callbacks, 0, sizeof(callbacks));
}


/* START FUNCTION DESCRIPTION *********************
mediator_register                      <mediator.c>

SYNTAX: void mediator_register(int event_id, ACTION_PTR action);

DESCRIPTION:
Registers a function to execute when a message is sent.

PARAMETER1: The defined event id to listen for
PARAMETER2: The function to execute on the event
  
RETURN VALUE:  null

SM040716
END DESCRIPTION ***********************************/
void mediator_register(int event_id, ACTION_PTR action)
{
    callbacks[event_id].actions[callbacks[event_id].total_actions++] = action;
}

/* START FUNCTION DESCRIPTION *********************
mediator_unregister                    <mediator.c>

SYNTAX: void mediator_unregister(int event_id, ACTION_PTR action);

DESCRIPTION:
Unregisters a function to prevent execution when an
event is triggered

PARAMETER1: The defined event id unregister an
            function from
PARAMETER2: The function to remove

RETURN VALUE:  null

SM040716
END DESCRIPTION ***********************************/
void mediator_unregister(int event_id, ACTION_PTR action)
{
    // TODO...    
}

/* START FUNCTION DESCRIPTION *********************
mediator_send_message                    <mediator.c>

SYNTAX: void mediator_send_message(int event_id, uint8* message);

DESCRIPTION:
Unregisters a function to prevent execution when an
event is triggered

PARAMETER1: The event to execute
PARAMETER2: Any additional data related to the message

RETURN VALUE:  null

SM040716
END DESCRIPTION ***********************************/
void mediator_send_message(int event_id, char* message)
{
  LOG_PRINT(LOGGER_VERBOSE, "Executing event id: %d , total actions: %d", event_id, callbacks[event_id].total_actions); 
    int i;
    
    for (i = 0; i < callbacks[event_id].total_actions; i++)
    {
        callbacks[event_id].actions[i](message);
    }
}
