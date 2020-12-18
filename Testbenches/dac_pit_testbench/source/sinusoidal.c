/********************************************************************************
  @file     App.c
  @brief    Application functions
  @author   N. Magliola, G. Davidov, F. Farall, J. Gaytán, L. Kammann, N. Trozzo
 *******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdint.h>
#include <math.h>
#include "drivers/MCAL/systick/systick.h"
#include "drivers/MCAL/dac/dac.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define BUFFER_SIZE 2000
#define PI 			3.14159265

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

static void onTimeout(void);


/*******************************************************************************
 * VARIABLES TYPES DEFINITIONS
 ******************************************************************************/


/*******************************************************************************
 * PRIVATE VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static uint16_t 	buffer[BUFFER_SIZE];
static uint32_t 	bufferIndex = 0;

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/* Called once at the beginning of the program */
void appInit (void)
{
	// Setup DAC
	dac_cfg_t config = { .swTrigger = 1 };
	dacInit(DAC_0, config);

	// Fill buffer
	for (uint32_t i = 0 ; i < BUFFER_SIZE ; i++)
	{
		double aux = 2 * PI * i / BUFFER_SIZE;
		buffer[i] = ( sin(aux) * (4096/2) ) + 2047;
	}

	// Initialize systick for periodic interrupts
	systickInit(onTimeout);
}

/* Called repeatedly in an infinite loop */
void appRun (void)
{

}

void onTimeout(void)
{
	// Write buffer to output
	dacWrite(DAC_0, buffer[bufferIndex++]);

	bufferIndex %= BUFFER_SIZE;
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/


/*******************************************************************************
 ******************************************************************************/
