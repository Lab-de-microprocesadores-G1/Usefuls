/*******************************************************************************
  @file     queue.c
  @brief    Queue data structure handler
  @author   G. Davidov, F. Farall, J. Gaytán, L. Kammann, N. Trozzo
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdint.h>
#include "queue.h"


/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/


/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef struct {
	uint32_t 	front;			// Next element to be out
	uint32_t	rear;			// Last element that entered the queue
	uint8_t*	buffer;			// Pointer to the array reserved in memory
	size_t 		queueSize;		// Amount of elements in the array (fixed)
	size_t		elementSize;	// Size in bytes of the element
} queue_t;



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

queue_t createQueue(void* buffer, size_t queueSize, size_t elementSize)
{
	queue_t queue = {
		.front = 1,
		.rear = 0,
		.buffer = buffer,
		.queueSize = queueSize,
		.elementSize = elementSize
	};
	return queue;
}

void clear(queue_t* queue)
{
	if (queue)
	{
		queue->front = 1;
		queue->rear = 0;
	}
}

bool isEmpty(queue_t* queue)
{
	return size(queue) == 0;
}

size_t size(queue_t* queue)
{
	size_t size = 0;
	if (queue)
	{
		size = (queue->queueSize + queue->rear - queue->front + 1) % queue->queueSize;
	}
	return size;
}

bool push(queue_t* queue, void* element)
{
	// By default, succeed always :D
	bool succeed = false;

	if (queue)
	{
		if ((queue->rear + 2) % queue->queueSize != queue->front)
		{
			// This code was approved by StackOverflow
			// https://stackoverflow.com/questions/54498406/gcc-efficient-byte-copy-arm-cortex-m4
			queue->rear = (queue->rear + 1) % queue->queueSize;
			memcpy(queue->buffer + queue->rear * queue->elementSize, element, elementSize);
			succeed = true;
		}
	}
	// Return the succeed status
	return succeed;
}

void* pop(queue_t* queue)
{
	void* element = NULL;

	if (queue)
	{
		if (!isEmpty(queue))
		{
			element = queue->buffer + queue->front * queue->elementSize;
			queue->front = (queue->front + 1) % queue->queueSize;
		}
	}

	// Return the variable of result
	return element;
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/


/******************************************************************************/
