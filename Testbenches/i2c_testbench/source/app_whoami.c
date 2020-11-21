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
#include "drivers/MCAL/i2c/i2c_master.h"
#include "drivers/HAL/button/button.h"
#include "drivers/HAL/led/led.h"
#include "drivers/MCAL/gpio/gpio.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

// Address of slaves in the I2C bus
#define ACCELEROMETER_SLAVE_ADDRESS	0x1D
#define NODEMCU_SLAVE_ADDRESS		0x0A

// General settings
#define BAUD_RATE					10000U
#define BUFFER_SIZE					5

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

static void idle(void);
static void error(void);
static void ok(void);

/**
 * @brief Converts a byte to the string representation of the hex code
 * @param byte		Byte to be converted
 * @param string	String to place the conversion, need space for 4 chars
 */
void byte2HexString(uint8_t byte, char* string);

/**
 * @brief Converts the number to the hex code as a char
 * @param number	Number to be converted
 */
char number2HexChar(uint8_t number);

/*******************************************************************************
 * TEST BENCH DECLARATION
 ******************************************************************************/

/*******************************************************************************
 * VARIABLES TYPES DEFINITIONS
 ******************************************************************************/

/*******************************************************************************
 * PRIVATE VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static uint8_t	writeBuffer[BUFFER_SIZE];
static uint8_t	readBuffer[BUFFER_SIZE];

static char		MESSAGE_WRITE_READ_OK[64] = "I2C_WriteRead OK = 0xFF\r\n";

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

	// Initialize the led driver
	ledInit();
	idle();

	// Initialize the uart driver
	uart_cfg_t config = { .baudRate = 9600 };
	uartInit(UART_INSTANCE_0, config);

	// Tell the accelerometer you want to read the content of the register
	// 0x0D, which is the WHOAMI register.
	writeBuffer[0] = 0x0D;
	i2cStartTransaction(I2C_INSTANCE_0, ACCELEROMETER_SLAVE_ADDRESS, writeBuffer, 1, readBuffer, 1);
}

/* Called repeatedly in an infinite loop */
void appRun (void)
{
	switch (i2cQueryTransaction(I2C_INSTANCE_0))
	{
		case I2C_STATE_FINISHED:
			ok();
			byte2HexString(readBuffer[0], MESSAGE_WRITE_READ_OK + strlen(MESSAGE_WRITE_READ_OK) - 6);
			uartWriteMsg(UART_INSTANCE_0, (const word_t*)MESSAGE_WRITE_READ_OK, strlen(MESSAGE_WRITE_READ_OK));
			break;
		case I2C_STATE_ERROR:
			error();
			break;
		default:
			break;
	}
}


/*******************************************************************************
 *******************************************************************************
                        	PRIVATE FUNCTIONS
 *******************************************************************************
 ******************************************************************************/

char number2HexChar(uint8_t number)
{
	return number < 10 ? '0' + number : 'A' + (number - 10);
}

void byte2HexString(uint8_t byte, char* string)
{
	char msn = number2HexChar(byte / 16);
	char lsn = number2HexChar(byte % 16);
	string[0] = '0';
	string[1] = 'x';
	string[2] = msn;
	string[3] = lsn;
}

static void idle(void)
{
	ledClear(LED_GREEN);
	ledClear(LED_RED);
}

static void error(void)
{
	ledClear(LED_GREEN);
	ledSet(LED_RED);
}

static void ok(void)
{
	ledSet(LED_GREEN);
	ledClear(LED_RED);
}

/*******************************************************************************
 ******************************************************************************/
