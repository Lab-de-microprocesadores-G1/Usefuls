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
#include "drivers/HAL/button/button.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define PULSE_WIDTH		10	// 6.4us

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

void buttonPressed(void);

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
void appInitOutputCompareSingleShot (void)
{
	// Default configuration of the development board
	boardInit();

	// Initializing FTM
	ftmInit(FTM_INSTANCE_0, 7, 0xFFFF);
	ftmOutputCompareInit(FTM_INSTANCE_0, FTM_CHANNEL_0, FTM_OC_CLEAR, true);
	ftmStart(FTM_INSTANCE_0);

	// Init Button
	buttonInit();
	buttonSubscribe(BUTTON_1, BUTTON_PRESS, buttonPressed);
}

/* Called repeatedly in an infinite loop */
void appRunOutputCompareSingleShot (void)
{

}


/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void buttonPressed(void)
{
	ftmOutputCompareStart(FTM_INSTANCE_0, FTM_CHANNEL_0, 0xFFFF);
}

/*******************************************************************************
 ******************************************************************************/
