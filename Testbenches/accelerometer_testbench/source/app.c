/********************************************************************************
  @file     App.c
  @brief    Application functions
  @author   N. Magliola, G. Davidov, F. Farall, J. Gaytán, L. Kammann, N. Trozzo
 *******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <string.h>

#include "board.h"

#include "drivers/MCAL/uart/uart.h"
#include "drivers/HAL/FXOS8700/fxos8700_accelerometer.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define BUFFER_SIZE	50

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

// static void privateFunction(void);

/*******************************************************************************
 * VARIABLES TYPES DEFINITIONS
 ******************************************************************************/

// typedef int  my_int_t;

/*******************************************************************************
 * PRIVATE VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static acc_vector_t acceleration;
static char			buffer[BUFFER_SIZE];

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/* Called once at the beginning of the program */
void appInit (void)
{
	// Initialize the board
	boardInit();

	// Initialize the uart
	uart_cfg_t uartSetting = { .baudRate = UART_BAUD_RATE_9600 };
	uartInit(UART_INSTANCE_0, uartSetting);

	// Initialize the accelerometer
	FXOSInit();
}

/* Called repeatedly in an infinite loop */
void appRun (void)
{
	if (FXOSIsRunning())
	{
		if (FXOSMeasurementAvailable())
		{
			if (FXOSGetAcceleration(&acceleration))
			{
				sprintf(buffer, "Measurement %d - %d - %d\r\n", acceleration.x, acceleration.y, acceleration.z);
				if (uartCanTx(UART_INSTANCE_0, strlen(buffer)))
				{
					uartWriteMsg(UART_INSTANCE_0, (const word_t*)buffer, strlen(buffer));
				}
			}
		}
	}
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/


/*******************************************************************************
 ******************************************************************************/
