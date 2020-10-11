/******************************************************************************
  @file     fsm.h
  @brief    FSM Library
  @author   G. Davidov, F. Farall, J. Gaytan, L. Kammann, N. Trozzo
 ******************************************************************************/

#ifndef FSM_FSM_H_
#define FSM_FSM_H

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

// Event's ID definition
// You can create your own event type, as a structure (for example), BUT
// the first element MUST always be an event_id_t.
// ¡See Action definition to use this!
// For example,
//
// typedef struct {
// 	 event_id_t		id;
//	 // ... more elements you need
// } custom_event_t;
typedef uint32_t event_id_t;

// Action definition
// The action receives as parameter the pointer to the event
// and you can cast it to your custom structure of events.
typedef void (*action_t)(void*);

// Edge definition
typedef struct edge edge_t;

// State definition
typedef edge_t*  state_t;

// Edge structure
struct edge {
    event_id_t      event;
    state_t       	nextState;
    action_t       	action;
};

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

// Definition of the end of the state, when no edge found for the current event
#define END_OF_STATE						(uint32_t)0xFFFFFFFFF

// Definition of an empty action routine, do nothing
void 	__do_nothing__(void* event);
#define DO_NOTHING	(__do_nothing__)

// Definition of the default state
#define DEFAULT_EDGE(state, callback)			{END_OF_STATE, (state), (callback)}
#define DEFAULT_NO_ACTION_EDGE(state)			{END_OF_STATE, (state), DO_NOTHING}

// Get the ID of an event's pointer
#define EVENT_ID(ev) 							(*(event_id_t*)(ev))

// Verify current state of the machine state
#define IS_CURRENT_STATE(fsm, state)			((fsm) == state)

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/**
 * @brief Cycles the current state of the FSM
 * @param state		Current state of the FSM
 * @param event		Current event received
 */
void fsmCycle(state_t* state, void* event);

#endif /* FSM_FSM_H_ */
