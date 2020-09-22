/*******************************************************************************
  @file     queue.c
  @brief    Queue data structure handler
  @author   G. Davidov, F. Farall, J. Gaytán, L. Kammann, N. Trozzo
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "queue.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/


/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/



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
}

void clear(queue_t* queue)
{
	queue->front = 1;
	queue->rear = 0;
}

bool isEmpty(queue_t* queue)
{
	return size(queue) == 0;
}

size_t size(queue_t* queue)
{
	return (queue->queueSize + queue->rear - queue->front + 1) % queue->queueSize;
}

bool push(queue_t* queue, void* element)
{
	// By default, succeed always :D
	bool succeed = true;

	if ((queue->rear + 2) % queue->queueSize == queue->front)
	{
		succeed = false;
	}
	else
	{
		// This code was approved by StackOverflow
		// https://stackoverflow.com/questions/54498406/gcc-efficient-byte-copy-arm-cortex-m4
		queue->rear = (queue->rear + 1) % queue->queueSize;
		memcpy(queue->buffer + queue->rear * queue->elementSize, element, elementSize);
	}

	// Return the succeed status
	return succeed;
}

void* pop(queue_t* queue)
{
	void* element = NULL;

	if (!isEmpty(queue))
	{
		element = queue->buffer + queue->front * queue->elementSize;
		queue->front = (queue->front + 1) % queue->queueSize;
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
