/********************************************************************************
  @file     App.c
  @brief    Application functions
  @author   N. Magliola, G. Davidov, F. Farall, J. Gaytán, L. Kammann, N. Trozzo
 *******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <string.h>
#include <stdlib.h>
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

// Register address in the accelerometer
#define ACC_WHOAMI_ADDRESS			0x0D
#define ACC_SYSMODE_ADDRESS			0x0B
#define ACC_OUT_X_MSB_ADDRESS		0x01
#define ACC_OUT_Y_MSB_ADDRESS		0x03
#define ACC_OUT_Z_MSB_ADDRESS		0x05
#define ACC_CTRL_REG_1_ADDRESS		0x2A

// General settings
#define BAUD_RATE					100000U
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

void readFromAddress(uint8_t address);
void printMessageAndHexByte(const char* msg, uint8_t byte);
void readAndLog(const char* msg, uint8_t address);
void writeAndLog(uint8_t address, uint8_t value);

/*******************************************************************************
 * TEST BENCH DECLARATION
 ******************************************************************************/

/*******************************************************************************
 * VARIABLES TYPES DEFINITIONS
 ******************************************************************************/

typedef struct {
	int8_t	x;
	int8_t 	y;
	int8_t 	z;
} acceleration_t;

/*******************************************************************************
 * PRIVATE VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static uint8_t			writeBuffer[BUFFER_SIZE];
static uint8_t			readBuffer[BUFFER_SIZE];
static acceleration_t 	acc;

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
	uart_cfg_t config = { .baudRate = UART_BAUD_RATE_9600 };
	uartInit(UART_INSTANCE_0, config);
}

/* Called repeatedly in an infinite loop */
void appRun (void)
{
	static bool onlyOnce = true;
	if (onlyOnce)
	{
		onlyOnce = false;
		writeAndLog(ACC_CTRL_REG_1_ADDRESS, 0x01);

		for(uint32_t i = 0 ; i < 500000 ; i++);

		readAndLog("Respuesta a CTRL_REG_1 ", ACC_CTRL_REG_1_ADDRESS);
	}

	for(uint32_t i = 0 ; i < 500000 ; i++);

	readFromAddress(ACC_OUT_X_MSB_ADDRESS);
	while(i2cQueryTransaction(I2C_INSTANCE_0) != I2C_STATE_IDLE);
	acc.x = readBuffer[0];

	for(uint32_t i = 0 ; i < 500000 ; i++);

	readFromAddress(ACC_OUT_Y_MSB_ADDRESS);
	while(i2cQueryTransaction(I2C_INSTANCE_0) != I2C_STATE_IDLE);
	acc.y = readBuffer[0];

	for(uint32_t i = 0 ; i < 500000 ; i++);

	readFromAddress(ACC_OUT_Z_MSB_ADDRESS);
	while(i2cQueryTransaction(I2C_INSTANCE_0) != I2C_STATE_IDLE);
	acc.z = readBuffer[0];

	for(uint32_t i = 0 ; i < 500000 ; i++);

	char buffer[50];
	sprintf(buffer, "X = %d Y = %d Z = %d\r\n", acc.x, acc.y, acc.z);
	if (uartCanTx(UART_INSTANCE_0, 50))
	{
		uartWriteMsg(UART_INSTANCE_0, (const word_t*)buffer, strlen(buffer));
	}
}


/*******************************************************************************
 *******************************************************************************
                        	PRIVATE FUNCTIONS
 *******************************************************************************
 ******************************************************************************/

void writeAndLog(uint8_t address, uint8_t value)
{
	bool finished = false;
	writeBuffer[0] = address;
	writeBuffer[1] = value;
	i2cStartTransaction(I2C_INSTANCE_0, ACCELEROMETER_SLAVE_ADDRESS, writeBuffer, 2, NULL, 0);
	while (!finished)
	{
		switch (i2cQueryTransaction(I2C_INSTANCE_0))
		{
			case I2C_STATE_FINISHED:
				ok();
				finished = true;
				break;
			case I2C_STATE_IDLE:
			case I2C_STATE_ERROR:
				error();
				finished = true;
				break;
			default:
				break;
		}
	}
}

void readAndLog(const char* msg, uint8_t address)
{
	bool finished = false;
	readFromAddress(address);
	while (!finished)
	{
		switch (i2cQueryTransaction(I2C_INSTANCE_0))
		{
			case I2C_STATE_FINISHED:
				ok();
				printMessageAndHexByte(msg, readBuffer[0]);
				finished = true;
				break;
			case I2C_STATE_IDLE:
			case I2C_STATE_ERROR:
				error();
				finished = true;
				break;
			default:
				break;
		}
	}
}

void printMessageAndHexByte(const char* msg, uint8_t byte)
{
	char buffer[] = "0x00";
	byte2HexString(byte, buffer);
	uartWriteMsg(UART_INSTANCE_0, (const word_t*)msg, strlen(msg));
	uartWriteMsg(UART_INSTANCE_0, (const word_t*)buffer, strlen(buffer));
	uartWriteMsg(UART_INSTANCE_0, (const word_t*)"\r\n", 2);
}

void readFromAddress(uint8_t address)
{
	writeBuffer[0] = address;
	i2cStartTransaction(I2C_INSTANCE_0, ACCELEROMETER_SLAVE_ADDRESS, writeBuffer, 1, readBuffer, 1);
}

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

