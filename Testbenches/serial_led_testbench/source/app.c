/********************************************************************************
  @file     App.c
  @brief    Application functions
  @author   N. Magliola, G. Davidov, F. Farall, J. Gaytán, L. Kammann, N. Trozzo
 *******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "board/board.h"
#include "drivers/HAL/button/button.h"
#include "drivers/HAL/serial_led/serial_led.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

// #define SOME_CONSTANT    20
// #define MACRO(x)         (x)

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

// static void privateFunction(void);

/*******************************************************************************
 * VARIABLES TYPES DEFINITIONS
 ******************************************************************************/

// typedef int  my_int_t;

/*******************************************************************************
 * PRIVATE VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

// SW2 is used to test serialLedWriteAll()
static void onSW2Pressed(void);
// SW3 is used to test serialLedSetNumber()
static void onSW3Pressed(void);

// Testing variables
uint8_t writeAllCount 		= 1;	// This will be incremented up to 7
uint8_t writeNumberCount 	= 0;  	// This will be incremented up to 3

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/* Called once at the beginning of the program */
void appInit (void)
{
	// Init board
	boardInit();

	// Init button driver
	buttonInit();

	// Subscribe for both board buttons press events
	buttonSubscribe(BUTTON_2, BUTTON_PRESS, onSW2Pressed);
	buttonSubscribe(BUTTON_1, BUTTON_PRESS, onSW3Pressed);


	// Init serial_led driver
	serialLedInit();
}

/* Called repeatedly in an infinite loop */
void appRun (void)
{

}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void onSW2Pressed(void)
{
	if (++writeAllCount == 8)
	{
		writeAllCount = 0;
	}
	serialLedWriteAll(writeAllCount);
}

void onSW3Pressed(void)
{
	if (++writeNumberCount == 4)
	{
		writeNumberCount = 0;
	}
	serialLedSetNumber(writeNumberCount);
}


/*******************************************************************************
 ******************************************************************************/
