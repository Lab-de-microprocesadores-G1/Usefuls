/***************************************************************************//**
  @file     magnetic_reader.h
  @brief    [...]
  @author   G. Davidov, F. Farall, J. Gaytán, L. Kammann, N. Trozzo
 ******************************************************************************/

#ifndef MAGNETIC_READER_MAGNETIC_READER_H_
#define MAGNETIC_READER_MAGNETIC_READER_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/
// Magnetic card structure
typedef struct {
	uint8_t PAN[19];
	uint8_t PANLength;
	uint8_t extraData[7];
	uint8_t discretionaryData[8];
} mag_card_t;

/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

 /**
 * @brief Initialize magnetic reader and corresponding peripheral
 */
void magneticReaderInit(void);

/**
* @brief Callbacks to the card information and error.
* @param dataCb callback to get new card event.
* @param errorCb callback to check for error.
*/
void magneticReaderSubscribe(void (*dataCb) (mag_card_t data), void (*errorCb) (void));



/*******************************************************************************
 ******************************************************************************/

#endif
