/********************************************************************************
  @file     App.c
  @brief    Application functions
  @author   N. Magliola, G. Davidov, F. Farall, J. Gaytán, L. Kammann, N. Trozzo
 *******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#define DEBUG_MODE
#include "drivers/HAL/WS2812/WS2812.h"
#include "drivers/MCAL/uart/uart.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define DISPLAY_ROW_SIZE    8
#define DISPLAY_COL_SIZE    8
#define DISPLAY_SIZE        DISPLAY_ROW_SIZE * DISPLAY_COL_SIZE

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/


/*******************************************************************************
 * VARIABLES TYPES DEFINITIONS
 ******************************************************************************/

// typedef int  my_int_t;

/*******************************************************************************
 * PRIVATE VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static pixel_t display[DISPLAY_SIZE];
static pixel_t colours;

static uart_cfg_t config = {
	.baudRate 		= UART_BAUD_RATE_9600,
	.length			= 0,
	.parityEnable	= 0,
	.parityMode		= 0,
	.stopMode		= 0
};


/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/* Called once at the beginning of the program */
void appInit (void)
{
    WS2812Init();

    WS2812SetDisplayBuffer(display, DISPLAY_SIZE);    

    //UART initialize
    uartInit(UART_INSTANCE_3, config);
}

/* Called repeatedly in an infinite loop */
void appRun (void)
{
	if (uartHasRxMsg(UART_INSTANCE_3))
	{
		uint8_t length = uartGetRxMsgLength(UART_INSTANCE_3);
		if(length == 3)
		{
			uint8_t lengthRead = uartReadMsg(UART_INSTANCE_3, &colours, length);
			for (uint8_t i = 0 ; i < DISPLAY_ROW_SIZE ; i++)
			    {
			        for (uint8_t j = 0 ; j < DISPLAY_COL_SIZE ; j++)
			        {
						display[i * DISPLAY_ROW_SIZE + j] = colours;
			        }
			    }
			    WS2812Update();
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
