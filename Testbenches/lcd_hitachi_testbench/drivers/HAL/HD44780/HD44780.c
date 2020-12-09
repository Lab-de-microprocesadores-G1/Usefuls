/***************************************************************************//**
  @file     HD44780.c
  @brief    Hitachi HD44780 LCD Display Low-Level Driver using SPI
  @author   G. Davidov, F. Farall, J. Gaytán, L. Kammann, N. Trozzo
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "drivers/HAL/timer/timer.h"
#include "drivers/MCAL/spi/spi_master.h"
#include "HD44780.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

// Parallel output port positions
#define HD44780_RS_POS		5
#define HD44780_E_POS		4
#define HD44780_VALUE_POS 	0

// Delay constants
#define HD44780_LONG_DELAY	10000
#define HD44780_E_DELAY		5000

// SPI Serial2Parallel port definitions
#define SPORT_SPI_INSTANCE	SPI_INSTANCE_0
#define SPORT_SPI_SLAVE		1
#define SPORT_SPI_BAUD_RATE	100000
#define SPORT_SPI_FRAME_SIZE	6	// 4 data bits, 1 for E and 1 for RS

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef enum {
	HD44780_NOT_INIT,
	HD44780_INITIALIZING_8BIT,
	HD44780_INITIALIZING_4BIT,	// Changes when MPU is set to 4bit operation
	HD44780_INITIALIZED
} hd44780_init_state_t;

// Driver context
typedef struct {

	// Initialization-related variables
	uint8_t initState;		// Indicates if init has started, is in progress or ended
	uint8_t initStepCount;	// Keeps track of initialization steps

	// LCD configuration given by the user
	hd44780_cfg_t config;

	// Output port context
	uint8_t currentPortValue;

	// Timer ID given by the driver
	tim_id_t	idTimer;

} hd44780_context_t;

/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/


/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static hd44780_context_t lcdContext;

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

// HD44780 helper functions

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

static void enablePulse(void);

// TIMER CALLBACK

/**
 * @brief Performs the LCD initialization step-by-step when called on timeout
 */
static void cycleInitialization(void);

// SERIAL TO PARALLEL PORT INTERFACE

/**
 * @brief Updates the whole output port
 * @param value		Output value
 */
static void portWriteOutput(uint8_t value);

/**
 * @brief Updates one of the 8 bits of the output port
 * @param pos	Bit number
 * @param value Bit value
 */
static void portWriteOutputBit(uint8_t pos, bool value);

/**
 * @brief Updates the value of the output port
 * Note:  Blocking function! Waits for SPI transfer to be completed
 * Note2: this function should only be used by portWrite...()
 */
static bool portUpdateOutput(void);


/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void HD44780Init(hd44780_cfg_t config)
{
	if (lcdContext.initState == HD44780_NOT_INIT)
	{
		// Set state
		lcdContext.initState = HD44780_INITIALIZING_8BIT;

		// Set initialization step
		lcdContext.initStepCount = 0;

		// Output starts at 0x00
		lcdContext.currentPortValue = 0;

		// Save user configuration
		lcdContext.config = config;

		// Initialization of the timer driver
		timerInit();
		lcdContext.idTimer = timerGetId();

		// Initialization of the SPI driver
		spi_cfg_t spiConfig = {
			.baudRate = SPORT_SPI_BAUD_RATE,
			.frameSize = SPORT_SPI_FRAME_SIZE,
			.slaveSelectPolarity = SPI_SS_INACTIVE_HIGH,
			.continuousPcs = SPI_CONTINUOUS_PCS_DIS,
			.clockPhase = SPI_CPHA_FIRST_CAPTURE,
			.clockPolarity = SPI_CPOL_INACTIVE_LOW,
			.endianness = SPI_ENDIANNESS_MSB_FIRST
		};
		spiInit(SPORT_SPI_INSTANCE, SPORT_SPI_SLAVE, spiConfig);

		// Schedule initialization start. Controller needs 15ms since power-up.
		timerStart(lcdContext.idTimer, TIMER_MS2TICKS(15), TIM_MODE_SINGLESHOT, cycleInitialization);
	}
}

bool HD44780InitReady(void)
{
	return (lcdContext.initState == HD44780_INITIALIZED);
}

void HD44780WriteInstruction(uint8_t instruction)
{
	if (lcdContext.initState >= HD44780_INITIALIZING_4BIT)
	{
		for (uint32_t i = 0 ; i < HD44780_LONG_DELAY ; i++);

		// Write first nybble, MSN first, E = 0
		writeInstructionNybble( (instruction & 0xF0) >> 4);

		for (uint32_t i = 0 ; i < HD44780_LONG_DELAY ; i++);

		// E=1 for some time, then back to 0
		enablePulse();

		for (uint32_t i = 0 ; i < HD44780_LONG_DELAY ; i++);

		// Write second nybble, LSN
		writeInstructionNybble(instruction & 0x0F);

		// E=1 for some time, then back to 0
		enablePulse();
	}
}

void HD44780WriteData(uint8_t data)
{
	if (lcdContext.initState >= HD44780_INITIALIZING_4BIT)
	{
		for (uint32_t i = 0 ; i < HD44780_LONG_DELAY ; i++);

		// Write first nybble, MSN first, E = 0
		writeDataNybble( (data & 0xF0) >> 4);

		for (uint32_t i = 0 ; i < HD44780_LONG_DELAY ; i++);

		// E=1 for some time, then back to 0
		enablePulse();

		for (uint32_t i = 0 ; i < HD44780_LONG_DELAY ; i++);

		// Write second nybble, LSN
		writeDataNybble(data & 0x0F);

		for (uint32_t i = 0 ; i < HD44780_LONG_DELAY ; i++);

		// E=1 for some time, then back to 0
		enablePulse();
	}
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

// HD44780 helper functions

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
	uint8_t newPortValue = ((rs & 1) << HD44780_RS_POS) | ((enable & 1) << HD44780_E_POS) | ((value & 0xF) << HD44780_VALUE_POS);
	portWriteOutput(newPortValue);
}

void enablePulse(void)
{
	for (uint32_t i = 0 ; i < HD44780_E_DELAY ; i++);
	portWriteOutputBit(HD44780_E_POS, 1);
	for (uint32_t i = 0 ; i < HD44780_E_DELAY ; i++);
	portWriteOutputBit(HD44780_E_POS, 0);
}

// TIMER CALLBACK

void cycleInitialization(void)
{
	uint8_t initStepCount = ++lcdContext.initStepCount;
	hd44780_cfg_t config = lcdContext.config;

	if (initStepCount - 1 < 4)
	{
		for (uint32_t i = 0 ; i < HD44780_LONG_DELAY ; i++);

		// E = 0
		portWriteOutputBit(HD44780_E_POS, false);
		for (uint32_t i = 0 ; i < HD44780_LONG_DELAY ; i++);
	}

	switch(initStepCount - 1)
	{
	case 0: // Function set 8 bit, dummy DL and F
		writeInstructionNybble(HD44780_FUNCTION_SET(1, 0, 0) >> 4);
		timerStart(lcdContext.idTimer, TIMER_MS2TICKS(4), TIM_MODE_SINGLESHOT, cycleInitialization);
		break;
	case 1: // Function set 8 bit, dummy DL and F
		writeInstructionNybble(HD44780_FUNCTION_SET(1, 0, 0) >> 4);
		timerStart(lcdContext.idTimer, 1, TIM_MODE_SINGLESHOT, cycleInitialization);
		break;
	case 2: // Function set 8 bit, dummy DL and F
		writeInstructionNybble(HD44780_FUNCTION_SET(1, 0, 0) >> 4);
		timerStart(lcdContext.idTimer, 1, TIM_MODE_SINGLESHOT, cycleInitialization);
		break;
	case 3: // Function set 4 bit
		writeInstructionNybble(HD44780_FUNCTION_SET(0, 0, 0) >> 4);
		timerStart(lcdContext.idTimer, 1, TIM_MODE_SINGLESHOT, cycleInitialization);
		lcdContext.initState = HD44780_INITIALIZING_4BIT;
		break;
	case 4:	// Function set 4 bit, specify display lines and character font
		HD44780WriteInstruction(HD44780_FUNCTION_SET(0, config.bothLines, config.charFont));
		timerStart(lcdContext.idTimer, 1, TIM_MODE_SINGLESHOT, cycleInitialization);
		break;
	case 5: // Display Off
		HD44780WriteInstruction(HD44780_DISPLAY_CONTROL(0,0,0));
		timerStart(lcdContext.idTimer, 1, TIM_MODE_SINGLESHOT, cycleInitialization);
		break;
	case 6:	// Display Clear
		HD44780WriteInstruction(HD44780_CLEAR_DISPLAY);
		timerStart(lcdContext.idTimer, 1, TIM_MODE_SINGLESHOT, cycleInitialization);
		break;
	case 7:	// Entry Mode Set, specify increment and shift
		HD44780WriteInstruction(HD44780_ENTRY_MODE_SET(config.inc, config.shift));
		timerStart(lcdContext.idTimer, 1, TIM_MODE_SINGLESHOT, cycleInitialization);
		break;
	case 8: // Display On, specify cursor and cursor position blink
		HD44780WriteInstruction(HD44780_DISPLAY_CONTROL(1, config.cursor, config.blink));
		timerStart(lcdContext.idTimer, 1, TIM_MODE_SINGLESHOT, cycleInitialization);
		break;
	case 9: // Return Home
		HD44780WriteInstruction(HD44780_RETURN_HOME);
		lcdContext.initState = HD44780_INITIALIZED;
	}

	if (initStepCount - 1 < 4)
	{
		for (uint32_t i = 0 ; i < HD44780_LONG_DELAY ; i++);
		// E = 1
		portWriteOutputBit(HD44780_E_POS, true);

		for (uint32_t i = 0 ; i < HD44780_LONG_DELAY ; i++);
	}

}


// SERIAL TO PARALLEL PORT INTERFACE

void portWriteOutput(uint8_t value)
{
	lcdContext.currentPortValue = value;
	portUpdateOutput();
}

void portWriteOutputBit(uint8_t pos, bool value)
{
	lcdContext.currentPortValue = (lcdContext.currentPortValue & ~(1 << pos)) | ((value ? 1 : 0) << pos);
	portUpdateOutput();
}

bool portUpdateOutput(void)
{
	bool ret = false;
	uint16_t message = lcdContext.currentPortValue;

	// Write if SPI available
	if ( spiCanSend(SPORT_SPI_INSTANCE, 1) )
	{
		spiSend(SPORT_SPI_INSTANCE, SPORT_SPI_SLAVE, &message, 1);

		// Block until transfer is completed
		while(!spiTransferComplete(SPORT_SPI_INSTANCE));
		ret = true;
	}

	return ret;
}


/******************************************************************************/
