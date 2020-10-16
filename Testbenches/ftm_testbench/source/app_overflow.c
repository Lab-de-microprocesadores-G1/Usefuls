/********************************************************************************
  @file     App.c
  @brief    Application functions
  @author   N. Magliola, G. Davidov, F. Farall, J. Gaytán, L. Kammann, N. Trozzo
 *******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "board.h"

#include "drivers/MCAL/gpio/gpio.h"
#include "drivers/MCAL/ftm/ftm.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/


/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

void onTimerOverflow(void);

/*******************************************************************************
 * VARIABLES TYPES DEFINITIONS
 ******************************************************************************/
// typedef int my_int;


/*******************************************************************************
 * PRIVATE VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/
// static int myVar;


/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/* Called once at the beginning of the program */
void App_Init (void)
{
	// Default configuration of the development board
	boardInit();

	// Setting up the GPIO output used for the led
	gpioWrite(PIN_LED_BLUE, !LED_ACTIVE);
	gpioMode(PIN_LED_BLUE, OUTPUT);

	// Initializing the FTM, configuring the overflow mode,
	// subscribing to the overflow event, and starting the module
	ftmInit(FTM_INSTANCE_0, 5, 15625);
	ftmOverflowSubscribe(FTM_INSTANCE_0, onTimerOverflow);
	ftmStart(FTM_INSTANCE_0);
}

/* Called repeatedly in an infinit loop */
void App_Run (void)
{
    // Things to do in an infinit loop.
}


/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void onTimerOverflow(void)
{
	static uint16_t count = 0;
	if (++count >= 50)
	{
		count = 0;
		gpioToggle(PIN_LED_BLUE);
	}
}

/*******************************************************************************
 ******************************************************************************/
