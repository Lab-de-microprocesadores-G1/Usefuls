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

#define NO_EVENTS				NULL
#define	MAX_EVENT_GENERATORS	15
#define	OUT_OF_GENERATORS		MAX_EVENT_GENERATORS

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

// The implementation of the Event Queue uses one element to distinguish
// between a full and an empty queue, so the queueSize should be always
// one more than the actual maximum size desired.
struct 	event_queue_t;

typedef	void * (*event_generator_t)(void);
typedef	uint8_t	generator_id_t;


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
 * @brief Registers an event generator. Returns the generator id if
 * 		  succeed or OUT_OF_GENERATORS on error.
 * @param queue			Pointer to the Event Queue instance
 * @param generator 	Callback to the generator
 */
generator_id_t registerEventGenerator(event_queue_t* queue, event_generator_t generator);

/**
 * @brief Enables/Disables generator. Returns true if the generator existed,
 * 		  false if not.
 * @param queue			Pointer to the Event Queue instance
 * @param id			generator id
 * @param enable		enable/disable status
 */
bool setEnable(event_queue_t* queue, generator_id_t id, bool enable);

/**
 * @brief Returns next event from the queue, returns NO_EVENTS if there are
 * 		  no events.
 * @param queue			Pointer to the Event Queue instance
 */
void* getNextEvent(event_queue_t* queue);


/*******************************************************************************
 ******************************************************************************/

#endif
