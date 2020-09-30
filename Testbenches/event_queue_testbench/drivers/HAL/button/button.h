/***************************************************************************//**
  @file     button.h
  @brief    Buttons driver
  @author   Nicolás Trozzo
 ******************************************************************************/

#ifndef BUTTON_BUTTON_H_
#define BUTTON_BUTTON_H_


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

typedef enum {
	BUTTON_RELEASE,
	BUTTON_PRESS,
	BUTTON_LKP,
	BUTTON_TYPEMATIC,
	BUTTON_EV_COUNT
} button_events_t;


typedef enum {
	BUTTON_1,
	BUTTON_2,
	BUTTON_COUNT
} button_id_t;

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/**
 * @brief Initialize button and corresponding peripheral
 */
void buttonInit(void);

// Non-Blocking services //

/**
 * @brief Suscribe to button event
 */
void buttonSubscribe(button_id_t id, button_events_t event, void (*callback) (void));

/**
 * @brief Check if button is pressed
 * @param id ID of the button
 */
bool isButtonPressed(button_id_t id);


#endif /* BUTTON_BUTTON_H_ */
