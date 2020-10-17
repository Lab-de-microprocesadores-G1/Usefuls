/***************************************************************************//**
  @file     array.h
  @brief    Array algorithms library
  @author   G. Davidov, F. Farall, J. Gaytán, L. Kammann, N. Trozzo
 ******************************************************************************/

#ifndef ARRAY_ARRAY_H_
#define ARRAY_ARRAY_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdint.h>

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/**
 * @brief Removes an element of the array by swap all elements to optimize the use
 * 	  	  of spacing by removing empty positions.
 * 	  	  [1, 2, 3, 4, 5, 6] index: 3 -> [1, 2, 3, 5, 6]
 * @param array			Pointer of the array
 * @param index			Index of the element to be removed
 * @param arraySize 	Size of the array in elements
 * @param elementSize	Size in bytes of the element
 */
void arrayRemove(void* array, uint32_t index, size_t arraySize, size_t elementSize);

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 ******************************************************************************/


#endif /* ARRAY_ARRAY_H_ */
