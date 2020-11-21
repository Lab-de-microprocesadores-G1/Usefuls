/********************************************************************************
  @file     App.c
  @brief    Application functions
  @author   N. Magliola, G. Davidov, F. Farall, J. Gaytán, L. Kammann, N. Trozzo
 *******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "board.h"

#include "drivers/MCAL/i2c/i2c_master.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define ACCELEROMETER_SLAVE_ADDRESS	0x1D
#define BAUD_RATE					10000U
#define BUFFER_SIZE					5
#define REGISTER_COUNT				6

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

static uint8_t	writeBuffer[BUFFER_SIZE];
static uint8_t	readBuffer[BUFFER_SIZE];
static uint8_t  currentRegister = 0x00;
static int16_t	stableAcceleration[3];
static int16_t	readingAcceleration[3];

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/* Called once at the beginning of the program */
void appInit (void)
{
	// Default board initialization
	boardInit();

	// I2C master driver initialization
	i2cMasterInit(I2C_INSTANCE_0, BAUD_RATE);
}

/* Called repeatedly in an infinite loop */
void appRun (void)
{
	switch (i2cQueryTransaction(I2C_INSTANCE_0))
	{
		// Every time the I2C driver is idle, read the next register of the accelerometer
		case I2C_STATE_IDLE:
			writeBuffer[0] = ++currentRegister;
			i2cStartTransaction(I2C_INSTANCE_0, ACCELEROMETER_SLAVE_ADDRESS, writeBuffer, 1, readBuffer, 1);

		case I2C_STATE_FINISHED:
			readingAcceleration[(currentRegister - 1) / 2] |= (currentRegister - 1) % 2 ? (readBuffer[0] & 0xFC) : readBuffer[0] << 8;
			if (currentRegister == 0x06)
			{
				currentRegister = 0x00;
				stableAcceleration[0] = readingAcceleration[0] >> 2;
				stableAcceleration[1] = readingAcceleration[1] >> 2;
				stableAcceleration[2] = readingAcceleration[2] >> 2;
				readingAcceleration[0] = 0;
				readingAcceleration[1] = 0;
				readingAcceleration[2] = 0;
			}
			break;

		default:
			break;
	}
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/


/*******************************************************************************
 ******************************************************************************/
