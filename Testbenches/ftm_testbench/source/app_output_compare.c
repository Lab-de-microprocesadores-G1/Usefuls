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

#define PERIOD			(uint16_t)10000
#define SEMI_PERIOD		((PERIOD) / 2)

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

void onChannelInterrupt(uint16_t count);
void onSW2Pressed(void);
void onSW3Pressed(void);

/*******************************************************************************
 * VARIABLES TYPES DEFINITIONS
 ******************************************************************************/
// typedef int my_int;


/*******************************************************************************
 * PRIVATE VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static uint16_t	dutyDelta = SEMI_PERIOD;

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/* Called once at the beginning of the program */
void appInitOutputCompare (void)
{
	// Default configuration of the development board
	boardInit();

	// GPIO
	gpioMode(PIN_SW3, INPUT | PULLUP);
	gpioMode(PIN_SW2, INPUT | PULLUP);
	gpioIRQ(PIN_SW2, GPIO_IRQ_MODE_INTERRUPT_FALLING_EDGE, onSW2Pressed);
	gpioIRQ(PIN_SW3, GPIO_IRQ_MODE_INTERRUPT_FALLING_EDGE, onSW3Pressed);

	// Initializing the FTM, configuring the output compare mode,
	// subscribing to the channel match event, and starting the module
	ftmInit(FTM_INSTANCE_0, 5, 0xFFFF);
	ftmOutputCompareInit(FTM_INSTANCE_0, FTM_CHANNEL_0, FTM_OC_TOGGLE, false);
	ftmChannelSubscribe(FTM_INSTANCE_0, FTM_CHANNEL_0, onChannelInterrupt);
	ftmOutputCompareStart(FTM_INSTANCE_0, FTM_CHANNEL_0, dutyDelta);
	ftmStart(FTM_INSTANCE_0);
}

/* Called repeatedly in an infinite loop */
void appRunOutputCompare (void)
{
    // Things to do in an infinite loop.
}


/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void onChannelInterrupt(uint16_t count)
{
	ftmChannelSetCount(
			FTM_INSTANCE_0,
			FTM_CHANNEL_0,
			gpioRead(PIN_FTM_CHANNEL_0) ? count + dutyDelta : count + (PERIOD - dutyDelta)
	);
}

void onSW2Pressed(void)
{
	static bool active = true;
	if (active)
	{
		active = false;
		ftmOutputCompareStop(FTM_INSTANCE_0, FTM_CHANNEL_0);
	}
	else
	{
		active = true;
		ftmOutputCompareStart(FTM_INSTANCE_0, FTM_CHANNEL_0, dutyDelta);
	}
}

void onSW3Pressed(void)
{
	dutyDelta += 100;
	if (dutyDelta >= 4000)
	{
		dutyDelta = 100;
	}
}

/*******************************************************************************
 ******************************************************************************/
