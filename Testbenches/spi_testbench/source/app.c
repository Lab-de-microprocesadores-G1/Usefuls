/********************************************************************************
  @file     App.c
  @brief    Application functions
  @author   N. Magliola, G. Davidov, F. Farall, J. Gaytán, L. Kammann, N. Trozzo
 *******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "drivers/MCAL/spi/spi_master.h"
#include "drivers/HAL/button/button.h"

#include <string.h>

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define BUFFER_SIZE		10

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

static void onSW2Pressed(void);

/*******************************************************************************
 * VARIABLES TYPES DEFINITIONS
 ******************************************************************************/

// typedef int  my_int_t;

/*******************************************************************************
 * PRIVATE VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static const uint16_t message[BUFFER_SIZE] = {'H', 'o', 'l', 'a', 'h', 'u', 'e', 'v', 'o'};
static uint16_t buffer[BUFFER_SIZE];

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/* Called once at the beginning of the program */
void appInit (void)
{
	// Initialization of the Button driver
	buttonInit();
	buttonSubscribe(BUTTON_2, BUTTON_PRESS, onSW2Pressed);

	// Initialization of the SPI driver
	spi_cfg_t config = {
		.baudRate = 1220,
		.frameSize = 8
	};
	spiInit(SPI_INSTANCE_0, SPI_SLAVE_0, config);
}

/* Called repeatedly in an infinite loop */
void appRun (void)
{
	if (spiTransferComplete(SPI_INSTANCE_0))
	{
		size_t count = spiGetReceiveCount(SPI_INSTANCE_0);
		if (count)
		{
			spiRead(SPI_INSTANCE_0, buffer, count);
		}
	}
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

static void onSW2Pressed(void)
{
	if (spiCanSend(SPI_INSTANCE_0, 9))
	{
		spiSend(SPI_INSTANCE_0, SPI_SLAVE_0, message, 9);
	}
}

/*******************************************************************************
 ******************************************************************************/
