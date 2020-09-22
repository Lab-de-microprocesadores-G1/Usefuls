/***************************************************************************//**
  @file     event_queue.h
  @brief    [...]
  @author   G. Davidov, F. Farall, J. Gaytán, L. Kammann, N. Trozzo
 ******************************************************************************/

#ifndef EVENT_QUEUE_H_
#define EVENT_QUEUE_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdio.h>
#include <stdbool.h>

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define NO_EVENTS		NULL

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

// The implementation of the Event Queue uses one element to distinguish
// between a full and an empty queue, so the queueSize should be always
// one more than the actual maximum size desired.
struct 		event_queue_t;

typedef		void * (*event_generator_t)(void);



/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/**
 * @brief Creates an Event Queue instance from the buffer and size specified by user
 * @param buffer		Pointer to the array reserved in memory
 * @param queueSize		Amount of elements in the array (fixed)
 * @param elementSize	Size in bytes of the element
 */
event_queue_t createEventQueue(void* buffer, size_t queueSize, size_t elementSize);

/**
 * @brief Registers an event generator. Returns true if succeeded or
 * 		  false if there already were MAX_EVENT_GENERATORS.
 * @param queue			Pointer to the Event Queue instance
 * @param generator 	Callback to the generator
 */
bool registerEventGenerator(event_queue_t* queue, event_generator_t generator);

/**
 * @brief Returns next event from the queue, returns NO_EVENTS if there are
 * 		  no events.
 * @param queue			Pointer to the Event Queue instance
 */
void* getNextEvent(event_queue_t* queue);


/*******************************************************************************
 ******************************************************************************/

#endif
