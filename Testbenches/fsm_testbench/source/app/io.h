/******************************************************************************
  @file     io.h
  @brief    I/O Abstraction Layer for the Application Layer
  @author   G. Davidov, F. Farall, J. Gaytán, L. Kammann, N. Trozzo
 ******************************************************************************/

#ifndef APP_IO_H_
#define APP_IO_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdbool.h>
#include <stdint.h>

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef enum {
	IO_EVENT_NONE,
	IO_EVENT_LEFT_KEY_PRESSED,
	IO_EVENT_LEFT_KEY_RELEASED,
	IO_EVENT_RIGHT_KEY_PRESSED,
	IO_EVENT_RIGHT_KEY_RELEASED,
	IO_EVENT_TIMEOUT
} io_event_id_t;

typedef struct {
	uint32_t 	id;
	uint32_t	dummy;
} io_event_t;

typedef enum {
	IO_LED_RED,
	IO_LED_BLUE,
	IO_LED_GREEN
} io_led_t;

/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/**
 * @brief Initialize devices, drivers, and all hardware resources needed
 */
void ioInit(void);

/**
 * @brief Returns the next event in the queue
 */
io_event_t ioGetNextEvent(void);

/*
 * @brief Sends commands to the output leds
 * @param led		Id of the target led
 * @param status	New status for that led
 */
void ioLed(io_led_t led, bool status);

/*******************************************************************************
 ******************************************************************************/

#endif
