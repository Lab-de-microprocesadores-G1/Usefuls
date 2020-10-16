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

#define PERIOD			10000
#define SEMI_PERIOD		((PERIOD) / 2)

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

void onChannelInterrupt(uint16_t count);

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

	// Initializing the FTM, configuring the output compare mode,
	// subscribing to the channel match event, and starting the module
	ftmInit(FTM_INSTANCE_0, 5, 0xFFFF);
	ftmOutputCompareInit(FTM_INSTANCE_0, FTM_CHANNEL_0, FTM_OC_TOGGLE, dutyDelta);
	ftmChannelSubscribe(FTM_INSTANCE_0, FTM_CHANNEL_0, onChannelInterrupt);
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
	ftmSetChannelCount(
			FTM_INSTANCE_0,
			FTM_CHANNEL_0,
			gpioRead(PIN_FTM_CHANNEL_0) ? count + dutyDelta : count + (PERIOD - dutyDelta)
	);
}

/*******************************************************************************
 ******************************************************************************/
