/********************************************************************************
  @file     App.c
  @brief    Application functions
  @author   N. Magliola, G. Davidov, F. Farall, J. Gaytán, L. Kammann, N. Trozzo
 *******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "board/board.h"

#include "drivers/HAL/led/led.h"
#include "drivers/MCAL/gpio/gpio.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/


/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

static void onToggleBlink(void);
static void onToggleLed(void);

/*******************************************************************************
 * PRIVATE VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static bool enabled;

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/* Called once at the beginning of the program */
void App_Init (void)
{
	ledInit();
	boardInit();

	gpioMode(PIN_SW2, INPUT | PULLUP);
	gpioIRQ(PIN_SW2, GPIO_IRQ_MODE_INTERRUPT_FALLING_EDGE, onToggleLed);

	gpioMode(PIN_SW3, INPUT | PULLUP);
	gpioIRQ(PIN_SW3, GPIO_IRQ_MODE_INTERRUPT_FALLING_EDGE, onToggleBlink);
}

/* Called repeatedly in an infinit loop */
void App_Run (void)
{
}


/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

static void onToggleBlink(void)
{
	enabled = !enabled;
	if (enabled)
	{
		ledBurst(LED_BLUE, 3000, 100, 5);
	}
	else
	{
		ledTurn(LED_BLUE, OFF);
	}
}

static void onToggleLed(void)
{
	enabled = !enabled;
	if (enabled)
	{
		ledBurst(LED_BLUE, 3000, 100, 5);
	}
	else
	{
		ledTurn(LED_BLUE, OFF);
	}
}

/*******************************************************************************
 ******************************************************************************/
