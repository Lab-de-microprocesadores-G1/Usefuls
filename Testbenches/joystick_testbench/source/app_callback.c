/********************************************************************************
  @file     App.c
  @brief    Application functions
  @author   N. Magliola, G. Davidov, F. Farall, J. Gaytán, L. Kammann, N. Trozzo
 *******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "drivers/HAL/joystick/joystick.h"
#include "drivers/MCAL/uart/uart.h"

#include <string.h>

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define	BUFFER_SIZE		64

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

static void onButtonPressed(void);
static void onFixedDirection(joystick_fixed_direction_t direction);
static void onAnyDirection(uint16_t angle);

/*******************************************************************************
 * VARIABLES TYPES DEFINITIONS
 ******************************************************************************/

// typedef int  my_int_t;

/*******************************************************************************
 * PRIVATE VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static char*	message[BUFFER_SIZE];

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/* Called once at the beginning of the program */
void appInit (void)
{
	// Initialization of the joystick
	joystickInit();

	joystickOnAnyDirection(onAnyDirection, 100);
	joystickOnFixedDirection(onFixedDirection, 100);
	joystickOnPressed(onButtonPressed);

	// Intialization of the uart
	uart_cfg_t uartSetting = { .baudRate = UART_BAUD_RATE_9600 };
	uartInit(UART_INSTANCE_0, uartSetting);
}

/* Called repeatedly in an infinite loop */
void appRun (void)
{
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

static void onButtonPressed(void)
{
	if (uartCanTx(UART_INSTANCE_0, BUFFER_SIZE))
	{
		sprintf((char*)message, "Button pressed!\r\n");
		uartWriteMsg(UART_INSTANCE_0, (const word_t*)message, strlen((char*)message));
	}
}

static void onFixedDirection(joystick_fixed_direction_t direction)
{
	if (uartCanTx(UART_INSTANCE_0, BUFFER_SIZE))
	{
		sprintf((char*)message, "Direction: %d\r\n", direction);
		uartWriteMsg(UART_INSTANCE_0, (const word_t*)message, strlen((char*)message));
	}
}

static void onAnyDirection(uint16_t angle)
{
	if (uartCanTx(UART_INSTANCE_0, BUFFER_SIZE))
	{
		sprintf((char*)message, "Direction angle: %d\r\n", angle);
		uartWriteMsg(UART_INSTANCE_0, (const word_t*)message, strlen((char*)message));
	}
}



/*******************************************************************************
 ******************************************************************************/
