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

// Select the testbench
#define TEST_XYZ
// #define TEST_ORIENTATION_CALLBACK
// #define TEST_ORIENTATION_POLLING

#define BUFFER_SIZE	50

#define LANDSCAPE_LEFT_BACK		"LANDSCAPE LEFT BACK\r\n"
#define LANDSCAPE_LEFT_FRONT	"LANDSPACE LEFT FRONT\r\n"
#define LANDSCAPE_RIGHT_BACK	"LANDSPACE RIGHT BACK\r\n"
#define LANDSCAPE_RIGHT_FRONT	"LANDSPACE RIGHT FRONT\r\n"
#define PORTRAIT_UP_BACK		"PORTRAIT UP BACK\r\n"
#define PORTRAIT_DOWN_BACK		"PORTRAIT DOWN BACK\r\n"
#define PORTRAIT_UP_FRONT		"PORTRAIT UP FRONT\r\n"
#define PORTRAIT_DOWN_FRONT		"PORTRAIT DOWN FRONT\r\n"

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

static void onOrientationChanged(void);

/*******************************************************************************
 * VARIABLES TYPES DEFINITIONS
 ******************************************************************************/

// typedef int  my_int_t;

/*******************************************************************************
 * PRIVATE VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static acc_vector_t			acceleration;
static acc_orientation_t	orientation;
static char					buffer[BUFFER_SIZE];

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
	FXOSInit(FXOS_INIT_DEFAULT);
#ifdef TEST_ORIENTATION_CALLBACK
	FXOSSubscribeOrientationChanged(onOrientationChanged);
#endif
}

/* Called repeatedly in an infinite loop */
void appRun (void)
{
#ifdef TEST_XYZ
	if (FXOSIsRunning())
	{
		if (FXOSMeasurementAvailable())
		{
			if (FXOSGetAcceleration(&acceleration))
			{
				sprintf(buffer, "Acceleration %d , %d , %d\r\n", acceleration.x, acceleration.y, acceleration.z);
				if (uartCanTx(UART_INSTANCE_0, strlen(buffer)))
				{
					uartWriteMsg(UART_INSTANCE_0, (const word_t*)buffer, strlen(buffer));
				}
			}
		}
	}
#endif

#ifdef TEST_ORIENTATION_POLLING
	if (FXOSIsRunning())
	{
		if (FXOSMeasurementAvailable())
		{
			if (FXOSGetOrientation(&orientation))
			{
				char* message;
				if (orientation.landscapePortrait == ACC_LANDSCAPE_LEFT && orientation.backFront == ACC_BACK)
				{
					message = LANDSCAPE_LEFT_BACK;
				}
				if (orientation.landscapePortrait == ACC_LANDSCAPE_LEFT && orientation.backFront == ACC_FRONT)
				{
					message = LANDSCAPE_LEFT_FRONT;
				}
				if (orientation.landscapePortrait == ACC_LANDSCAPE_RIGHT && orientation.backFront == ACC_BACK)
				{
					message = LANDSCAPE_RIGHT_BACK;
				}
				if (orientation.landscapePortrait == ACC_LANDSCAPE_RIGHT && orientation.backFront == ACC_FRONT)
				{
					message = LANDSCAPE_RIGHT_FRONT;
				}
				if (orientation.landscapePortrait == ACC_PORTRAIT_DOWN && orientation.backFront == ACC_BACK)
				{
					message = PORTRAIT_DOWN_BACK;
				}
				if (orientation.landscapePortrait == ACC_PORTRAIT_UP && orientation.backFront == ACC_BACK)
				{
					message = PORTRAIT_UP_BACK;
				}
				if (orientation.landscapePortrait == ACC_PORTRAIT_DOWN && orientation.backFront == ACC_FRONT)
				{
					message = PORTRAIT_DOWN_FRONT;
				}
				if (orientation.landscapePortrait == ACC_PORTRAIT_UP && orientation.backFront == ACC_FRONT)
				{
					message = PORTRAIT_UP_FRONT;
				}
				if (uartCanTx(UART_INSTANCE_0, strlen(buffer)))
				{
					uartWriteMsg(UART_INSTANCE_0, (const word_t*)message, strlen(message));
				}
			}
		}
	}
#endif
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

static void onOrientationChanged(void)
{
	if (FXOSGetOrientation(&orientation))
	{
		char* message;
		if (orientation.landscapePortrait == ACC_LANDSCAPE_LEFT && orientation.backFront == ACC_BACK)
		{
			message = LANDSCAPE_LEFT_BACK;
		}
		if (orientation.landscapePortrait == ACC_LANDSCAPE_LEFT && orientation.backFront == ACC_FRONT)
		{
			message = LANDSCAPE_LEFT_FRONT;
		}
		if (orientation.landscapePortrait == ACC_LANDSCAPE_RIGHT && orientation.backFront == ACC_BACK)
		{
			message = LANDSCAPE_RIGHT_BACK;
		}
		if (orientation.landscapePortrait == ACC_LANDSCAPE_RIGHT && orientation.backFront == ACC_FRONT)
		{
			message = LANDSCAPE_RIGHT_FRONT;
		}
		if (orientation.landscapePortrait == ACC_PORTRAIT_DOWN && orientation.backFront == ACC_BACK)
		{
			message = PORTRAIT_DOWN_BACK;
		}
		if (orientation.landscapePortrait == ACC_PORTRAIT_UP && orientation.backFront == ACC_BACK)
		{
			message = PORTRAIT_UP_BACK;
		}
		if (orientation.landscapePortrait == ACC_PORTRAIT_DOWN && orientation.backFront == ACC_FRONT)
		{
			message = PORTRAIT_DOWN_FRONT;
		}
		if (orientation.landscapePortrait == ACC_PORTRAIT_UP && orientation.backFront == ACC_FRONT)
		{
			message = PORTRAIT_UP_FRONT;
		}
		if (uartCanTx(UART_INSTANCE_0, strlen(buffer)))
		{
			uartWriteMsg(UART_INSTANCE_0, (const word_t*)message, strlen(message));
		}
	}
}

/*******************************************************************************
 ******************************************************************************/
