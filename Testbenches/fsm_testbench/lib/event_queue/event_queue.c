/*******************************************************************************
  @file     event_queue.c
  @brief    [...]
  @author   G. Davidov, F. Farall, J. Gaytán, L. Kammann, N. Trozzo
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdint.h>
#include "../queue/queue.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define	MAX_EVENT_GENERATORS	15

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef struct {
	queue_t				queue;									// Queue to save events
	event_generator_t	eventGenerators[MAX_EVENT_GENERATORS];	// Registered event generators
	uint8_t				generatorsCount;						// Amount of registered event generators
} event_queue_t;


/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/



/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/


/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

event_queue_t createEventQueue(void* buffer, size_t queueSize, size_t elementSize)
{
	event_queue_t newQueue = {
			.queue = createQueue(buffer, queueSize, elementSize),
			.generatorsCount = 0
	};
	return newQueue;
}

bool registerEventGenerator(event_queue_t* queue, event_generator_t generator)
{
	bool succeed = false;

	if (queue)
	{
		if (queue->generatorsCount < MAX_EVENT_GENERATORS)
		{
			queue->eventGenerators[queue->generatorsCount++] = generator;
			succeed = true;
		}
	}

	// Return the succeed status
	return succeed;
}

void* getNextEvent(event_queue_t* queue)
{
	void* element = NO_EVENTS;

	if (queue)
	{
		// Look for all events from generators
		uint8_t	i;
		void* ev;
		for ( i = 0 ; i < queue->generatorsCount ; i++ )
		{
			ev = queue->eventGenerators[i]();
			if (ev != NO_EVENTS)
			{
				push(queue->queue, ev);
			}
		}
		element = pop(queue->queue);
	}

	// Return the next event
	return element;
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/


/******************************************************************************/
