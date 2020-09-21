/*******************************************************************************
  @file     led.h
  @brief    LED Driver
  @author   G. Davidov, F. Farall, J. Gaytán, L. Kammann, N. Trozzo
 ******************************************************************************/

#ifndef HAL_LED_LED_H_
#define HAL_LED_LED_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "drivers/HAL/timer/timer.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

// Declaring label for each led to be handled by top
typedef enum {
	LED_RED,
	LED_BLUE,
	AMOUNT_OF_LEDS
} led_id_t;

// Declaring modes of operation for the led
typedef enum {
	STATIC,
	BLINK,
	BURST
} led_mode_t;

// Declaring the turn options of the static mode
typedef enum {
	OFF,
	ON
} led_status_t;


/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/**
 * @brief Initializes the LED Driver
 */
void ledInit(void);

/**
 * @brief Toggle the current state of the LED
 * @param id		ID of the led to toggle
 */
void ledToggle(led_id_t id);

/**
 * @brief Turns on or off the LED
 * @param id		ID of the led to set
 * @param status	Current status to update the led
 */
void ledTurn(led_id_t id, led_status_t status);

/**
 * @brief Sets the blinking mode of the LED
 * @param id		ID of the led to start blinking
 * @param period	Period of the blinking
 */
void ledBlink(led_id_t id, ttick_t period);

/**
 * @brief Sets the burst mode of the LED
 * @param id			ID of the led to start burst
 * @param burstPeriod	Period of the burst
 * @param blinkPeriod	Period of the blink
 * @param burst			Amount of blinks at the beginning of each period
 */
void ledBurst(led_id_t id, ttick_t burstPeriod, ttick_t blinkPeriod, uint8_t burst);

/*******************************************************************************
 ******************************************************************************/

#endif
