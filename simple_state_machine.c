/*Authoe : Abdullah
contact : abdullahansaric5@gmail.com*/

#include <stdio.h>
#define PERMITED 0


/*define the states*/
typedef enum {
    NORMAL,
    SUSPEND,
    SHUTDOWN,
    TOTAL_STATES,
    INVALID_STATE = -1
} e_states_config_t;


/*define the events*/
typedef enum {
    E1 = 0,
    E2,
    E3,
    E4,
    E5,
    E6,
    TOTAL_EVENTS,
    INVALID_EVENT = -1
} e_events_config_t;

typedef e_states_config_t e_state_t;
typedef e_events_config_t e_event_t;

/*handle to handle the event*/
typedef struct {
    e_event_t event;
    e_state_t next_state;
    int (*event_guard)(void);
    void (*event_handler)(void *);
} st_event_handle_t;

//event handler declaration
void handler_for_event_4(void *data);

//guard handler declarations
int guard_for_E3(void);
/*according to state and event definition configure the state maching

state_machine_config[NORMAL] -> 0
                      state_machine_config[NORMAL][0] = {event, next_state, event_guard, event_handler}
                      state_machine_config[NORMAL][1] = {event, next_state, event_guard, event_handler}
                      state_machine_config[NORMAL][2]  = {event, next_state, event_guard, event_handler}
                       .
                       .
                       .
state_machine_config[SUSPEND] -> 1
                      state_machine_config[SUSPEND][0] = {event, next_state, event_guard, event_handler}
                      state_machine_config[SUSPEND][1] = {event, next_state, event_guard, event_handler}
                      state_machine_config[SUSPEND][2]  = {event, next_state, event_guard, event_handler}
                      .
                      .
                      .
state_machine_config[SHUTDOWN] -> 2
                      state_machine_config[SHUTDOWN][0] = {event, next_state, event_guard, event_handler}
                      state_machine_config[SHUTDOWN][1] = {event, next_state, event_guard, event_handler}
                      state_machine_config[SHUTDOWN][2]  = {event, next_state, event_guard, event_handler}
                      .
                      .
                      .
Note : not necessary to write events configuration in sequence, may be random requence is also allowed

for example :
 state_machine_config[SHUTDOWN][0] = {E5, NORMAL, NULL, NULL,data_to_handler}
 state_machine_config[SHUTDOWN][1] = {E2, next_state, event_guard, event_handler,data_to_handler}
 state_machine_config[SHUTDOWN][2] = {E6, next_state, event_guard, event_handler,data_to_handler}

*/
st_event_handle_t state_machine_config[TOTAL_STATES][TOTAL_EVENTS] = {
    {
        /**/
        {E1, SUSPEND, NULL, NULL},
        {E2, SHUTDOWN, guard_for_E3, NULL},
        {E6, SHUTDOWN, guard_for_E3, NULL},
        {E3, SHUTDOWN, guard_for_E3, NULL},
        {INVALID_EVENT, INVALID_STATE, NULL, NULL},
        {INVALID_EVENT, INVALID_STATE, NULL, NULL}
    },
    {
        {E1, SHUTDOWN, NULL, NULL},
        {E4, NORMAL, NULL, handler_for_event_4},
        {E5, SHUTDOWN, NULL, NULL},
        {E1, SHUTDOWN, NULL, NULL},
        {E3, NORMAL, guard_for_E3, NULL},
        {E5, SHUTDOWN, NULL, NULL},
    },
    {
        {E6, NORMAL, NULL, NULL},
        {E1, SUSPEND, NULL, NULL},
        {E5, SUSPEND, NULL, NULL},
        {E2, NORMAL, NULL, NULL},
        {INVALID_EVENT, INVALID_STATE, NULL, NULL},
        {E3, NORMAL, guard_for_E3, NULL}
    }
};

typedef struct {
    void (*entry_handler)(void);
    void (*exit_handler)(void);
} st_state_handler_config_t;

st_state_handler_config_t state_handler_cfg[TOTAL_STATES];
e_state_t machine_state = INVALID_STATE;






// Entry and exit handlers for states
void normal_state_entry() {
    printf("Entering NORMAL state\n");
    machine_state = NORMAL;
}

void normal_state_exit() {
    printf("Exiting NORMAL state\n");
}

void suspend_state_entry() {
    printf("Entering SUSPEND state\n");
     machine_state = SUSPEND;
}

void suspend_state_exit() {
    printf("Exiting SUSPEND state\n");
}

void shutdown_state_entry() {
    printf("Entering SHUTDOWN state\n");
     machine_state = SHUTDOWN;
}

void shutdown_state_exit() {
    printf("Exiting SHUTDOWN state\n");
}



/*example of guard function*/
int guard_for_E3(void)
{
    //loic to decide if the event operation is permitted (allowed at cureent state)
    int is_permited = PERMITED;
    if(machine_state == NORMAL)
    {
        is_permited = !is_permited;

    }
    printf("Event is %s permited\n", PERMITED != is_permited ?"not":"");
    return is_permited;
}


/*example of event handler function*/
void handler_for_event_4(void *data)
{
    printf("Event handler:%s\n",__FUNCTION__ );
    //implenet code to handle the event 
    //for example set any flage, so on...
}

void initialize_state_handlers(e_state_t curr_state) {
    state_handler_cfg[NORMAL].entry_handler = normal_state_entry;
    state_handler_cfg[NORMAL].exit_handler = normal_state_exit;

    state_handler_cfg[SUSPEND].entry_handler = suspend_state_entry;
    state_handler_cfg[SUSPEND].exit_handler = suspend_state_exit;

    state_handler_cfg[SHUTDOWN].entry_handler = shutdown_state_entry;
    state_handler_cfg[SHUTDOWN].exit_handler = shutdown_state_exit;
    
    machine_state = curr_state;

    // Initialize handlers for other states as needed
}

void dispatch(e_event_t curr_event, void *data) {
    e_state_t state_itr;
    e_event_t event_itr;

    for (state_itr = 0; state_itr < TOTAL_STATES; state_itr++) {
        if (machine_state == state_itr) {
            break;
        }
    }
    
    if(state_itr >= TOTAL_STATES)
    return;

    for (event_itr = 0; event_itr < TOTAL_EVENTS; event_itr++) {
        if (curr_event == state_machine_config[state_itr][event_itr].event) {
            if (INVALID_EVENT != state_machine_config[state_itr][event_itr].event) {
                
                if (NULL != state_machine_config[state_itr][event_itr].event_guard) {
                    if(PERMITED  != state_machine_config[state_itr][event_itr].event_guard())
                    {
                        return;
                    }
                    
                }
                
                if (NULL != state_machine_config[state_itr][event_itr].event_handler) {
                    state_machine_config[state_itr][event_itr].event_handler(data);
                }
                
                
                    if (machine_state != state_machine_config[state_itr][event_itr].next_state) {
                        if (INVALID_STATE != state_machine_config[state_itr][event_itr].next_state) {
                            if (NULL != state_handler_cfg[state_itr].exit_handler) {
                                state_handler_cfg[state_itr].exit_handler();
                            }
                            if (NULL != state_handler_cfg[state_machine_config[state_itr][event_itr].next_state].entry_handler) {
                                state_handler_cfg[state_machine_config[state_itr][event_itr].next_state].entry_handler();
                            }
                        }
                    }
                }
            
        }
    }
}

int main() {
    // Initialize state handlers
    initialize_state_handlers(NORMAL);

    int data = 123; // Example data
    
    
    for(e_event_t e = E1; e < TOTAL_EVENTS; e++)
    {
       // printf("STATE NOW : %d\n",machine_state);
        printf("Dispatching event E%d in state %d\n",e+1,machine_state);
        dispatch(e, &data);
    }
     for(e_event_t e = E1; e < TOTAL_EVENTS; e++)
    {
       // printf("STATE NOW : %d\n",machine_state);
        printf("Dispatching event E%d in state %d\n",e+1,machine_state);
        dispatch(e, &data);
    }
 for(e_event_t e = E1; e < TOTAL_EVENTS; e++)
    {
       // printf("STATE NOW : %d\n",machine_state);
        printf("Dispatching event E%d in state %d\n",e+1,machine_state);
        dispatch(e, &data);
    }


 
   
    return 0;
}
