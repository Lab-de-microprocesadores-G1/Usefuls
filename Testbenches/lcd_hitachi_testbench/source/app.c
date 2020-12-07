/********************************************************************************
  @file     App.c
  @brief    Application functions
  @author   N. Magliola, G. Davidov, F. Farall, J. Gaytán, L. Kammann, N. Trozzo
 *******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "drivers/MCAL/spi/spi_master.h"
#include "drivers/MCAL/gpio/gpio.h"
#include "drivers/HAL/timer/timer.h"
#include "drivers/HAL/button/button.h"
#include "board/board.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

// HDD4478 Instructions without parameters
#define HDD4478_CLEAR_DISPLAY	0x01
#define HDD4478_RETURN_HOME		0x02

// HDD4478 Fixed Bits for Instructions with parameters
#define HDD4478_FUNCTION_SET_BIT		0x20
#define HDD4478_DISPLAY_CONTROL_BIT		0x08
#define HDD4478_ENTRY_MODE_SET_BIT		0x04


// HDD4478 Instructions with parameters
#define HDD4478_FUNCTION_SET(dl,n,f) 	(((dl<<4) | (n<<3) | (f<<2)) | HDD4478_FUNCTION_SET_BIT)
#define HDD4478_DISPLAY_CONTROL(d,c,b)	(((d<<2) | (c<<2) | (b<<0)) | HDD4478_DISPLAY_CONTROL_BIT)
#define HDD4478_ENTRY_MODE_SET(id,s)	(((id<<1) | (s<<0)) | HDD4478_ENTRY_MODE_SET_BIT)

#define HDD4478_DISPLAY_ON		0x0C
#define HDD4478_DISPLAY_OFF		0x08

// Spi constants
#define HDD4478_SPI_INSTANCE	SPI_INSTANCE_0
#define HDD4478_SPI_SLAVE		1
#define HDD4478_SPI_FRAME_SIZE	6	// 4 data bits, 1 for E and 1 for RS

// Parallel output port positions
#define HDD4478_RS_POS		5
#define HDD4478_E_POS		4
#define HDD4478_VALUE_POS 	0


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

// Internal functions declarations
static void timerCallback(void);

/**
 * @brief Writes the given instruction to the LCD
 * @param instruction	LCD instruction
 */
static void writeInstruction(uint8_t instruction);

/**
 * @brief Writes the given data to the LCD
 * @param data	Data byte to be written
 */
static void writeData(uint8_t data);

/**
 * @brief Writes the given instruction nybble to the LCD
 * @param instruction	Instruction nybble to be written
 */
static void writeInstructionNybble(uint8_t instruction);

/**
 * @brief Writes the given data nybble to the LCD
 * @param data	Data nybble to be written
 */
static void writeDataNybble(uint8_t data);

/**
 * @brief Writes the given values to the parallel output port
 * @param enable	1 bit for LCD E pin
 * @param rs		1 bit for LCD RS pin
 * @param value		4 bits for LCD D4-D7
 */
static void parallelOutput4bit(uint8_t enable, uint8_t rs, uint8_t value);

/**
 * @brief Updates one of the 8 bits of the output port
 * @param pos	Bit number
 * @param value Bit value
 */
static void updateOutputBit(uint8_t pos, bool value);

/**
 * @brief Updates the value of the output port
 */
static bool updateOutput(void);

static void enablePulse(void);

static void displayCounter(void);

static uint8_t	initStepCount;
static bool		lcdInitReady = false;

// Parallel output port context variables
static uint8_t currentPortValue = 0;

// Flags for display configurations
static bool lcdDisplayLines;
static bool lcdCharacterFont;

static tim_id_t	idTimer;

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/* Called once at the beginning of the program */
void appInit (void)
{
	// Initialization of the timer driver
	timerInit();
	idTimer = timerGetId();
	timerStart(idTimer, TIMER_MS2TICKS(15), TIM_MODE_SINGLESHOT, timerCallback);

	// Button driver
	buttonInit();
	buttonSubscribe(BUTTON_1, BUTTON_PRESS, displayCounter);

	// Initialization of the SPI driver
	spi_cfg_t config = {
		.baudRate = 1220,
		.frameSize = HDD4478_SPI_FRAME_SIZE,
		.slaveSelectPolarity = SPI_SS_INACTIVE_HIGH,
		.continuousPcs = SPI_CONTINUOUS_PCS_DIS,
		.clockPhase = SPI_CPHA_FIRST_CAPTURE,
		.clockPolarity = SPI_CPOL_INACTIVE_LOW,
		.endianness = SPI_ENDIANNESS_MSB_FIRST
	};
	spiInit(HDD4478_SPI_INSTANCE, HDD4478_SPI_SLAVE, config);
}

/* Called repeatedly in an infinite loop */
void appRun (void)
{
	static bool init = false;
	if (false)
	{
		init = true;

		// Clear Enable pin
		currentPortValue = 0;
		updateOutput();

		// Display configurations
		lcdCharacterFont = false;
		lcdDisplayLines = true;

		for (uint32_t i = 0 ; i < 500000 ; i++);

		writeInstructionNybble(HDD4478_FUNCTION_SET(1, 0, 0) >> 4);
		enablePulse();
		
		for (uint32_t i = 0 ; i < 500000 ; i++);

		writeInstructionNybble(HDD4478_FUNCTION_SET(1, 0, 0) >> 4);
		enablePulse();
		
		for (uint32_t i = 0 ; i < 500000 ; i++);

		writeInstructionNybble(HDD4478_FUNCTION_SET(1, 0, 0) >> 4);
		enablePulse();
		
		for (uint32_t i = 0 ; i < 500000 ; i++);

		writeInstructionNybble(HDD4478_FUNCTION_SET(0, 0, 0) >> 4);
		enablePulse();

		writeInstruction(HDD4478_FUNCTION_SET(0, lcdDisplayLines ? 1 : 0, lcdCharacterFont ? 1 : 0));

		writeInstruction(HDD4478_DISPLAY_CONTROL(0,0,0));

		writeInstruction(HDD4478_CLEAR_DISPLAY);
		
		writeInstruction(HDD4478_ENTRY_MODE_SET(1, 0)); // increment no shift

		writeInstruction(HDD4478_DISPLAY_CONTROL(1,0,0));

		writeInstruction(HDD4478_RETURN_HOME);

		writeData('0');
	}
}

void writeInstruction(uint8_t instruction)
{
	for (uint32_t i = 0 ; i < 500000 ; i++);

	// E=0
	updateOutputBit(HDD4478_E_POS, 0);
	for (uint32_t i = 0 ; i < 500000 ; i++);

	// Write first nybble, MSN first
	writeInstructionNybble( (instruction & 0xF0) >> 4);

	for (uint32_t i = 0 ; i < 500000 ; i++);

	// E=1 for some time, then back to 0
	updateOutputBit(HDD4478_E_POS, 1);
	for (uint32_t i = 0 ; i < 500000 ; i++);
	updateOutputBit(HDD4478_E_POS, 0);

	for (uint32_t i = 0 ; i < 500000 ; i++);

	// Write second nybble, LSN
	writeInstructionNybble(instruction & 0x0F);

	for (uint32_t i = 0 ; i < 500000 ; i++);

	// E=1 for some time, then back to 0
	updateOutputBit(HDD4478_E_POS, 1);
	for (uint16_t i = 0 ; i < 10000 ; i++);
	updateOutputBit(HDD4478_E_POS, 0);
}

void writeData(uint8_t data)
{
	for (uint32_t i = 0 ; i < 500000 ; i++);

	// E=0
	updateOutputBit(HDD4478_E_POS, 0);

	for (uint32_t i = 0 ; i < 500000 ; i++);

	// Write first nybble, MSN first
	writeDataNybble( (data & 0xF0) >> 4);

	for (uint32_t i = 0 ; i < 500000 ; i++);

	// E=1 for some time, then back to 0
	updateOutputBit(HDD4478_E_POS, 1);
	for (uint32_t i = 0 ; i < 500000 ; i++);
	updateOutputBit(HDD4478_E_POS, 0);

	for (uint32_t i = 0 ; i < 50000 ; i++);

	// Write second nybble, LSN
	writeDataNybble(data & 0x0F);

	for (uint32_t i = 0 ; i < 500000 ; i++);

	// E=1 for some time, then back to 0
	enablePulse();
}

void writeInstructionNybble(uint8_t instruction)
{
	// Write nybble with RS=0 (instruction), E=0
	parallelOutput4bit(0, 0, instruction);
}

void writeDataNybble(uint8_t data)
{
	// Write nybble with RS=1 (data), E=0
	parallelOutput4bit(0, 1, data);
}

void parallelOutput4bit(uint8_t enable, uint8_t rs, uint8_t value)
{
	currentPortValue = ((rs & 1) << HDD4478_RS_POS) | ((enable & 1) << HDD4478_E_POS) | ((value & 0xF) << HDD4478_VALUE_POS);
	updateOutput();
}

void enablePulse(void)
{
	for (uint32_t i = 0 ; i < 500000 ; i++);
	updateOutputBit(HDD4478_E_POS, 1);
	for (uint32_t i = 0 ; i < 500000 ; i++);
	updateOutputBit(HDD4478_E_POS, 0);
}

void updateOutputBit(uint8_t pos, bool value)
{
	currentPortValue = (currentPortValue & ~(1 << pos)) | ((value ? 1 : 0) << pos);
	updateOutput();
}

bool updateOutput(void)
{
	bool ret = false;
	uint16_t message = currentPortValue;

	// Write if SPI available
	if ( spiCanSend(HDD4478_SPI_INSTANCE, 1) )
	{
		spiSend(HDD4478_SPI_INSTANCE, HDD4478_SPI_SLAVE, &message, 1);

		// Block until transfer is completed
		// while(!spiTransferComplete(HDD4478_SPI_INSTANCE));
		ret = true;
	}
	else
	{
		ret = false;
	}

	return ret;
}

static void timerCallback(void)
{
	initStepCount++;

	switch(initStepCount - 1)
	{
	case 0: // Function set 8 bit, dummy DL and F
		writeInstructionNybble(HDD4478_FUNCTION_SET(1, 0, 0) >> 4);
		timerStart(idTimer, TIMER_MS2TICKS(4), TIM_MODE_SINGLESHOT, timerCallback);
		break;
	case 1: // Function set 8 bit, dummy DL and F
		writeInstructionNybble(HDD4478_FUNCTION_SET(1, 0, 0) >> 4);
		timerStart(idTimer, 1, TIM_MODE_SINGLESHOT, timerCallback);
		break;
	case 2: // Function set 8 bit, dummy DL and F
		writeInstructionNybble(HDD4478_FUNCTION_SET(1, 0, 0) >> 4);
		timerStart(idTimer, 1, TIM_MODE_SINGLESHOT, timerCallback);
		break;
	case 3: // Function set 4 bit
		writeInstructionNybble(HDD4478_FUNCTION_SET(0, 0, 0) >> 4);
		timerStart(idTimer, 1, TIM_MODE_SINGLESHOT, timerCallback);
		break;
	case 4:	// Function set 4 bit, specify display lines and character font
		writeInstruction(HDD4478_FUNCTION_SET(0, lcdDisplayLines ? 1 : 0, lcdCharacterFont ? 1 : 0));
		timerStart(idTimer, 1, TIM_MODE_SINGLESHOT, timerCallback);
		break;
	case 5: // Display Off
		writeInstruction(HDD4478_DISPLAY_CONTROL(0,0,0));
		timerStart(idTimer, 1, TIM_MODE_SINGLESHOT, timerCallback);
		break;
	case 6:	// Display Clear
		writeInstruction(HDD4478_CLEAR_DISPLAY);
		timerStart(idTimer, 1, TIM_MODE_SINGLESHOT, timerCallback);
		break;
	case 7:	// Entry Mode Set, increment no shift
		writeInstruction(HDD4478_ENTRY_MODE_SET(1, 0));
		timerStart(idTimer, TIMER_MS2TICKS(100), TIM_MODE_PERIODIC, displayCounter);
		lcdInitReady = true;
		break;
	}
}

void displayCounter(void)
{
	static uint8_t counter = 0;
	
	if (counter++ >= 9)
	{
		counter = 0;
	}

	writeData('0' + counter);
}
/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/


/*******************************************************************************
 ******************************************************************************/
