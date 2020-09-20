/*******************************************************************************
  @file     App.c
  @brief    Application functions
  @author   N. Magliola, G. Davidov, F. Farall, J. Gaytán, L. Kammann, N. Trozzo
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "board/board.h"

#include "drivers/MCAL/gpio/gpio.h"
#include "drivers/MCAL/systick/SysTick.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/


/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

static void blinkRed(void);
static void blinkBlue(void);

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/* Called once at the beginning of the program */
void App_Init (void)
{
	gpioMode(PIN_LED_RED, OUTPUT);
	gpioMode(PIN_LED_BLUE, OUTPUT);

	gpioMode(PIN_SW2, INPUT | PULLUP);

	SysTick_Init(blinkRed);
	SysTick_Init(blinkBlue);
}

/* Called repeatedly in an infinite loop */
static bool isEnabled = false;
void App_Run (void)
{
	static bool isPressed = false;
	if (isPressed)
	{
		if (gpioRead(PIN_SW2) != SW_ACTIVE)
		{
			isPressed = false;
		}
	}
	else
	{
		if (gpioRead(PIN_SW2) == SW_ACTIVE)
		{
			isPressed = true;
			isEnabled = isEnabled ? false : true;
		}
	}
}


/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

static void blinkRed(void)
{
	static uint32_t count = 0;
	if (isEnabled)
	{
		count++;
		if (count >= 500)
		{
			count = 0;
			gpioToggle(PIN_LED_RED);
		}
	}
}

static void blinkBlue(void)
{
	static uint32_t count = 0;
	if (isEnabled)
	{
		count++;
		if (count >= 200)
		{
			count = 0;
			gpioToggle(PIN_LED_BLUE);
		}
	}
}

/*******************************************************************************
 ******************************************************************************/
