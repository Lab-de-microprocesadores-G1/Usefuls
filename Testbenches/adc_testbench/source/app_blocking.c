/********************************************************************************
  @file     App.c
  @brief    Application functions
  @author   N. Magliola, G. Davidov, F. Farall, J. Gaytán, L. Kammann, N. Trozzo
 *******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "drivers/MCAL/adc/adc.h"
#include "drivers/MCAL/uart/uart.h"

#include "board.h"

#include <string.h>

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define BUFFER_SIZE	30

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

static uint32_t sample;
static char buffer[BUFFER_SIZE];

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

	// Initialize the UART peripheral
	uart_cfg_t uartSettings = { .baudRate = UART_BAUD_RATE_9600 };
	uartInit(UART_INSTANCE_0, uartSettings);

	// Initialize the ADC peripheral
	adcInit();
}

/* Called repeatedly in an infinite loop */
void appRun (void)
{
    // Application iterative tasks, every loop runs this function
	sample = adcBlockingConversion(ADC_POTENTIOMETER);
	sprintf(buffer, "ADC Measuremente %d", sample);
	if (uartCanTx(UART_INSTANCE_0, strlen(buffer)))
	{
		uartWriteMsg(UART_INSTANCE_0, buffer, strlen(buffer));
	}

	// Delay... ¿why? there is no why
	for (uint32_t i = 0 ; i < 500000 ; i++);
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/


/*******************************************************************************
 ******************************************************************************/
