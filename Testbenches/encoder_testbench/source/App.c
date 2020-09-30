/***************************************************************************//**
  @file     App.c
  @brief    Application functions
  @author   N. Magliola, G. Davidov, F. Farall, J. Gaytán, L. Kammann, N. Trozzo
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include <stdbool.h>

/* Include drivers */
#include "drivers/MCAL/gpio/gpio.h"
#include "drivers/HAL/encoder/encoder.h"
#include "board/board.h"


/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/


/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/
static void rotate_clockwise(void);
static void rotate_counter_clockwise(void);
static void change_colour(void);


/*******************************************************************************
 * VARIABLES TYPES DEFINITIONS
 ******************************************************************************/
typedef enum {RED, GREEN, BLUE};
typedef int8_t colour_t;


/*******************************************************************************
 * PRIVATE VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/
static colour_t colour;
static bool colour_changed;


/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/* Called once at the beginning of the program */
void App_Init (void)
{
	gpioWrite(PIN_LED_RED, HIGH);
	gpioWrite(PIN_LED_GREEN, HIGH);
	gpioWrite(PIN_LED_BLUE, HIGH);

    gpioMode(PIN_LED_RED, OUTPUT);
    gpioMode(PIN_LED_GREEN, OUTPUT);
    gpioMode(PIN_LED_BLUE, OUTPUT);

    encoderInit();
    registerCallbacks(ENCODER_0, rotate_clockwise, rotate_counter_clockwise);

    gpioWrite(PIN_LED_RED, LOW);
}

/* Called repeatedly in an infinit loop */
void App_Run (void)
{
    if (colour_changed)
    {
    	change_colour();
    }
}


/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
void rotate_clockwise(void)
{
	colour++;
	colour_changed = true;
}

void rotate_counter_clockwise(void)
{
	colour--;
	colour_changed = true;
}

void change_colour(void)
{
	colour_changed = false;
	if (colour > 2)
	{
		colour = 0;
	}
	else if (colour < 0)
	{
		colour = 2;
	}

	switch (colour)
	{
		case RED:
		{
			gpioWrite(PIN_LED_RED, LOW);
			gpioWrite(PIN_LED_GREEN, HIGH);
			gpioWrite(PIN_LED_BLUE, HIGH);
			break;
		}
		case GREEN:
		{
			gpioWrite(PIN_LED_RED, HIGH);
			gpioWrite(PIN_LED_GREEN, LOW);
			gpioWrite(PIN_LED_BLUE, HIGH);
			break;
		}
		case BLUE:
		{
			gpioWrite(PIN_LED_RED, HIGH);
			gpioWrite(PIN_LED_GREEN, HIGH);
			gpioWrite(PIN_LED_BLUE, LOW);
			break;
		}
	}
}


/*******************************************************************************
 ******************************************************************************/
