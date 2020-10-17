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

void onChannelInterruptIC(uint16_t count);

/*******************************************************************************
 * VARIABLES TYPES DEFINITIONS
 ******************************************************************************/
// typedef int my_int;


/*******************************************************************************
 * PRIVATE VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/


/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/* Called once at the beginning of the program */
void appInitInputCapture (void)
{
	// Default configuration of the development board
	boardInit();

	// Initializing the FTM, configuring the output compare mode,
	// subscribing to the channel match event, and starting the module
	ftmInit(FTM_INSTANCE_0, 5, 0xFFFF);

	ftmInputCaptureInit(FTM_INSTANCE_0, FTM_CHANNEL_0, FTM_IC_BOTH_EDGES);
	ftmChannelSubscribe(FTM_INSTANCE_0, FTM_CHANNEL_0, onChannelInterruptIC);

	ftmStart(FTM_INSTANCE_0);
}

/* Called repeatedly in an infinite loop */
void appRunInputCapture (void)
{
    // Things to do in an infinite loop.
}


/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void onChannelInterruptIC(uint16_t count)
{
	uint16_t bufferCount = count;
}

/*******************************************************************************
 ******************************************************************************/
