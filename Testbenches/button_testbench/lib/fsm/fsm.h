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
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define END_OF_STATE						0xFFFFFFFFF

// All states MUST have a default edge!
#define DEFAULT_EDGE(state, callback)		{END_OF_STATE, (state), (callback)}

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
typedef edge_t *state_t;

// Edge structure
struct edge {
    event_id_t      event;
    state_t       	nextState;
    action_t       	action;
};

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
