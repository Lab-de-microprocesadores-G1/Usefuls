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

// static void privateFunction(void);

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

	// Intialization of the uart
	uart_cfg_t uartSetting = { .baudRate = UART_BAUD_RATE_9600 };
	uartInit(UART_INSTANCE_0, uartSetting);
}

/* Called repeatedly in an infinite loop */
void appRun (void)
{
	for (uint32_t i = 0 ; i < 800000 ; i++);
	if (uartCanTx(UART_INSTANCE_0, BUFFER_SIZE))
	{
		joystick_position_t pos = joystickGetPosition();
		bool pressed = joystickIsPressed();
		sprintf((char*)message, "x: %d - y: %d - pressed: %d\r\n", pos.x, pos.y, pressed);
		uartWriteMsg(UART_INSTANCE_0, (const word_t*)message, strlen((char*)message));
	}
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/


/*******************************************************************************
 ******************************************************************************/
