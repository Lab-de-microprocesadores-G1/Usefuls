/********************************************************************************
  @file     App.c
  @brief    Application functions
  @author   N. Magliola, G. Davidov, F. Farall, J. Gaytán, L. Kammann, N. Trozzo
 *******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "board.h"

#include "drivers/HAL/button/button.h"
#include "drivers/HAL/led/led.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

// #define APP_TEST_SET_AND_CLEAR 	// OK!
// #define APP_TEST_TOGGLE			// OK!
// #define APP_TEST_STATUS			// OK!
// #define APP_TEST_BLINK			// OK!
// #define APP_TEST_BLINK_SOME		// OK!
// #define APP_TEST_BURST			// OK!
// #define APP_TEST_ONE_SHOT		// OK!

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

static void onButtonLeft(void);
static void onButtonRight(void);

/*******************************************************************************
 * VARIABLES TYPES DEFINITIONS
 ******************************************************************************/


/*******************************************************************************
 * PRIVATE VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/


/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/* Called once at the beginning of the program */
void appInit (void)
{
	boardInit();
	buttonInit();
	ledInit();

	buttonSubscribe(BUTTON_LEFT, BUTTON_PRESS, onButtonLeft);
	buttonSubscribe(BUTTON_RIGHT, BUTTON_PRESS, onButtonRight);
}

/* Called repeatedly in an infinite loop */
void appRun (void)
{
    // Application iterative tasks, every loop runs this function
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

static void onButtonLeft(void)
{
#ifdef APP_TEST_SET_AND_CLEAR
	ledSet(LED_RED);
#endif

#ifdef APP_TEST_TOGGLE
	ledToggle(LED_BLUE);
#endif

#ifdef APP_TEST_STATUS
	ledStatus(LED_GREEN, true);
#endif

#ifdef APP_TEST_BLINK
	ledBlink(LED_RED, LED_MS2TICK(1000));
#endif

#ifdef APP_TEST_BLINK_SOME
	ledBlinkSome(LED_RED, LED_MS2TICK(400), 5);
#endif

#ifdef APP_TEST_BURST
	ledBurst(LED_RED, LED_MS2TICK(2000), LED_MS2TICK(300), 5);
#endif

#ifdef APP_TEST_ONE_SHOT
	ledOneShot(LED_RED, LED_MS2TICK(1000));
#endif
}

static void onButtonRight(void)
{
#ifdef APP_TEST_SET_AND_CLEAR
	ledClear(LED_RED);
#endif

#ifdef APP_TEST_TOGGLE
	ledToggle(LED_RED);
#endif

#ifdef APP_TEST_STATUS
	ledStatus(LED_GREEN, false);
#endif

#ifdef APP_TEST_BLINK
	ledClear(LED_RED);
#endif

#ifdef APP_TEST_BLINK_SOME
	ledClear(LED_RED);
#endif

#ifdef APP_TEST_BURST
	ledClear(LED_RED);
#endif

#ifdef APP_TEST_ONE_SHOT
	ledClear(LED_RED);
#endif
}


/*******************************************************************************
 ******************************************************************************/
