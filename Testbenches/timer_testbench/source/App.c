/********************************************************************************
  @file     App.c
  @brief    Application functions
  @author   N. Magliola, G. Davidov, F. Farall, J. Gaytán, L. Kammann, N. Trozzo
 *******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "board/board.h"

#include "drivers/HAL/timer/timer.h"
#include "drivers/MCAL/gpio/gpio.h"
#include "drivers/MCAL/systick/SysTick.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

static tim_id_t id;

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

static void switchPressed(void);
static void toggleLed(void);

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/* Called once at the beginning of the program */
void App_Init (void)
{
	boardInit();
	gpioMode(PIN_LED_RED, OUTPUT);
	gpioMode(PIN_SW2, INPUT | PULLUP);
	gpioIRQ(PIN_SW2, GPIO_IRQ_MODE_INTERRUPT_FALLING_EDGE, switchPressed);

	timerInit();
	id = timerGetId();
	timerStart(id, TIMER_MS2TICKS(500), TIM_MODE_PERIODIC, toggleLed);
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

static void switchPressed(void)
{
	if (timerRunning(id))
	{
		timerPause(id);
	}
	else
	{
		timerResume(id);
	}
}

static void toggleLed(void)
{
	gpioToggle(PIN_LED_RED);
}

/*******************************************************************************
 ******************************************************************************/
