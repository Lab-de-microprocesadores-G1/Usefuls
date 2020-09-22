/*******************************************************************************
  @file     queue
  @brief    Queue data structure handler
  @author   G. Davidov, F. Farall, J. Gaytán, L. Kammann, N. Trozzo
 ******************************************************************************/

#ifndef QUEUE_H_
#define QUEUE_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/


/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

// The implementation of the Queue uses one element to distinguish between
// a full and an empty queue, so the queueSize should be always one more
// than the actual maximum size desired.
typedef struct {
	uint32_t 	front;			// Next element to be out
	uint32_t	rear;			// Last element that entered the queue
	uint8_t*	buffer;			// Pointer to the array reserved in memory
	size_t 		queueSize;		// Amount of elements in the array (fixed)
	size_t		elementSize;	// Size in bytes of the element
} queue_t;

/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/**
 * @brief Creates a Queue instance from the buffer and size specified by user
 * @param buffer		Pointer to the array reserved in memory
 * @param queueSize		Amount of elements in the array (fixed)
 * @param elementSize	Size in bytes of the element
 */
queue_t createQueue(void* buffer, size_t queueSize, size_t elementSize);

/**
 * @brief Returns whether the queue is empty or not
 * @param queue		Pointer to the Queue instance
 */
bool isEmpty(queue_t* queue);

/*
 * @brief Returns the current size of the queue
 * @param queue		Pointer to the Queue instance
 */
size_t size(queue_t* queue);

/**
 * @brief Clear the Queue instance
 * @param queue		Pointer to the Queue instance
 */
void clear(queue_t* queue);

/**
 * @brief Push a new element to the queue, and returns true if succeed
 * 		  or false if the queue was full
 * @param queue		Pointer to the Queue instance
 * @param element	Pointer to the new element to be pushed
 */
bool push(queue_t* queue, void* element);

/**
 * @brief Pop the next element from the queue. If no elements left it returns
 * null, else the pointer to the next element, REMEMBER to copy the element!
 * ¡The persistance of the element is temporary! Use it at your own risk.
 */
void* pop(queue_t* queue);

/*******************************************************************************
 ******************************************************************************/

#endif
