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
#define  HDD4478_DISPLAY_CONTROL(d,c,b)	(((d<<2) | (c<<2) | (b<<0)) | HDD4478_DISPLAY_CONTROL_BIT)
#define HDD4478_ENTRY_MODE_SET(id,s)	(((id<<1) | (s<<0)) | HDD4478_ENTRY_MODE_SET_BIT)

#define HD4478_DISPLAY_ON		0x0C
#define HDD4478_DISPLAY_OFF		0x08

// HDD4478 Control pins
#define	HDD4478_RW_PIN			PIN_LCD_RW
#define HDD4478_RS_PIN			PIN_LCD_RS
#define HDD4478_E_PIN			PIN_LCD_E

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
static void writeInstruction(uint8_t instruction);
static void writeData(uint8_t data);
static void displayCounter(void);

static uint8_t	initStepCount;
static bool		lcdInitReady = false;
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

	// Initialization of the SPI driver
	spi_cfg_t config = {
		.baudRate = 1220,
		.frameSize = 8
	};
	spiInit(SPI_INSTANCE_0, config);

	// Clear Enable pin before setting as output
	gpioWrite(HDD4478_E_PIN, LOW); 
	gpioWrite(HDD4478_RW_PIN, LOW);

	// R/W, E, RS Pins setup
	gpioMode(HDD4478_RW_PIN, OUTPUT | DRIVESTRENGTH);
	gpioMode(HDD4478_RS_PIN, OUTPUT | DRIVESTRENGTH);
	gpioMode(HDD4478_E_PIN, OUTPUT | DRIVESTRENGTH);
}

/* Called repeatedly in an infinite loop */
void appRun (void)
{
	static bool init = false;
	if (!init)
	{
		// SUPER DELAY POST VCCC PAPU
		init = true;

		// Display configurations
		lcdCharacterFont = false;
		lcdDisplayLines = true;

		writeInstruction(HDD4478_FUNCTION_SET(1, 0, 0));
		while(!spiTransferComplete(SPI_INSTANCE_0));
		
		for (uint16_t i = 0 ; i < 25000 ; i++);
		gpioWrite(HDD4478_RW_PIN, HIGH);
		for (uint16_t i = 0 ; i < 10000 ; i++);
		gpioWrite(HDD4478_RW_PIN, LOW);

		for (uint16_t i = 0 ; i < 25000 ; i++);
		gpioWrite(HDD4478_E_PIN, HIGH);
		for (uint16_t i = 0 ; i < 10000 ; i++);
		gpioWrite(HDD4478_E_PIN, LOW);
		
		for (uint32_t i = 0 ; i < 500000 ; i++);

		writeInstruction(HDD4478_FUNCTION_SET(1, 0, 0));
		while(!spiTransferComplete(SPI_INSTANCE_0));
		
		for (uint16_t i = 0 ; i < 25000 ; i++);
		gpioWrite(HDD4478_RW_PIN, HIGH);
		for (uint16_t i = 0 ; i < 10000 ; i++);
		gpioWrite(HDD4478_RW_PIN, LOW);

		for (uint16_t i = 0 ; i < 25000 ; i++);
		gpioWrite(HDD4478_E_PIN, HIGH);
		for (uint16_t i = 0 ; i < 10000 ; i++);
		gpioWrite(HDD4478_E_PIN, LOW);
		
		for (uint32_t i = 0 ; i < 500000 ; i++);

		writeInstruction(HDD4478_FUNCTION_SET(1, 0, 0));
		while(!spiTransferComplete(SPI_INSTANCE_0));
		
		for (uint16_t i = 0 ; i < 25000 ; i++);
		gpioWrite(HDD4478_RW_PIN, HIGH);
		for (uint16_t i = 0 ; i < 10000 ; i++);
		gpioWrite(HDD4478_RW_PIN, LOW);

		for (uint16_t i = 0 ; i < 25000 ; i++);
		gpioWrite(HDD4478_E_PIN, HIGH);
		for (uint16_t i = 0 ; i < 10000 ; i++);
		gpioWrite(HDD4478_E_PIN, LOW);
		
		for (uint32_t i = 0 ; i < 500000 ; i++);

		writeInstruction(HDD4478_FUNCTION_SET(lcdDisplayLines ? 1 : 0,0, lcdCharacterFont ? 1 : 0));
		while(!spiTransferComplete(SPI_INSTANCE_0));
		
		for (uint16_t i = 0 ; i < 25000 ; i++);
		gpioWrite(HDD4478_RW_PIN, HIGH);
		for (uint16_t i = 0 ; i < 10000 ; i++);
		gpioWrite(HDD4478_RW_PIN, LOW);

		for (uint16_t i = 0 ; i < 25000 ; i++);
		gpioWrite(HDD4478_E_PIN, HIGH);
		for (uint16_t i = 0 ; i < 10000 ; i++);
		gpioWrite(HDD4478_E_PIN, LOW);

		for (uint32_t i = 0 ; i < 500000 ; i++);

		writeInstruction(HDD4478_DISPLAY_CONTROL(0,0,0));
		while(!spiTransferComplete(SPI_INSTANCE_0));
		
		for (uint16_t i = 0 ; i < 25000 ; i++);
		gpioWrite(HDD4478_RW_PIN, HIGH);
		for (uint16_t i = 0 ; i < 10000 ; i++);
		gpioWrite(HDD4478_RW_PIN, LOW);

		for (uint16_t i = 0 ; i < 25000 ; i++);
		gpioWrite(HDD4478_E_PIN, HIGH);
		for (uint16_t i = 0 ; i < 10000 ; i++);
		gpioWrite(HDD4478_E_PIN, LOW);

		for (uint32_t i = 0 ; i < 500000 ; i++);


		writeInstruction(HDD4478_CLEAR_DISPLAY);
		while(!spiTransferComplete(SPI_INSTANCE_0));
		
		for (uint16_t i = 0 ; i < 25000 ; i++);
		gpioWrite(HDD4478_RW_PIN, HIGH);
		for (uint16_t i = 0 ; i < 10000 ; i++);
		gpioWrite(HDD4478_RW_PIN, LOW);

		for (uint16_t i = 0 ; i < 25000 ; i++);
		gpioWrite(HDD4478_E_PIN, HIGH);
		for (uint16_t i = 0 ; i < 10000 ; i++);
		gpioWrite(HDD4478_E_PIN, LOW);

		for (uint32_t i = 0 ; i < 500000 ; i++);


		writeInstruction(HDD4478_ENTRY_MODE_SET(0, 0));
		while(!spiTransferComplete(SPI_INSTANCE_0));
		
		for (uint16_t i = 0 ; i < 25000 ; i++);
		gpioWrite(HDD4478_RW_PIN, HIGH);
		for (uint16_t i = 0 ; i < 10000 ; i++);
		gpioWrite(HDD4478_RW_PIN, LOW);

		for (uint16_t i = 0 ; i < 25000 ; i++);
		gpioWrite(HDD4478_E_PIN, HIGH);
		for (uint16_t i = 0 ; i < 10000 ; i++);
		gpioWrite(HDD4478_E_PIN, LOW);

		for (uint32_t i = 0 ; i < 500000 ; i++);


		writeInstruction(HDD4478_DISPLAY_CONTROL(1,0,0));
		while(!spiTransferComplete(SPI_INSTANCE_0));
		
		for (uint16_t i = 0 ; i < 25000 ; i++);
		gpioWrite(HDD4478_RW_PIN, HIGH);
		for (uint16_t i = 0 ; i < 10000 ; i++);
		gpioWrite(HDD4478_RW_PIN, LOW);

		for (uint16_t i = 0 ; i < 25000 ; i++);
		gpioWrite(HDD4478_E_PIN, HIGH);
		for (uint16_t i = 0 ; i < 10000 ; i++);
		gpioWrite(HDD4478_E_PIN, LOW);

		for (uint32_t i = 0 ; i < 500000 ; i++);


		writeData('0');
		while(!spiTransferComplete(SPI_INSTANCE_0));
		
		for (uint16_t i = 0 ; i < 25000 ; i++);
		gpioWrite(HDD4478_RW_PIN, HIGH);
		for (uint16_t i = 0 ; i < 10000 ; i++);
		gpioWrite(HDD4478_RW_PIN, LOW);

		for (uint16_t i = 0 ; i < 25000 ; i++);
		gpioWrite(HDD4478_E_PIN, HIGH);
		for (uint16_t i = 0 ; i < 10000 ; i++);
		gpioWrite(HDD4478_E_PIN, LOW);
	}
}

void writeInstruction(uint8_t instruction)
{
	uint16_t message = instruction;

	// Write Instruction if SPI available
	if ( spiCanSend(SPI_INSTANCE_0, 1) )
	{
		spiSend(SPI_INSTANCE_0, 1, &message, 1);
	}

	// Instruction
	gpioWrite(HDD4478_RS_PIN, LOW);
}

void writeData(uint8_t data)
{
	uint16_t message = data;

	// Write data if SPI available
	if ( spiCanSend(SPI_INSTANCE_0, 1) )
	{
		spiSend(SPI_INSTANCE_0, 1, &message, 1);
	}
	
	gpioWrite(HDD4478_RS_PIN, HIGH);
}

static void timerCallback(void)
{
	uint8_t msDelay = 0;
	uint8_t aux;


	initStepCount++;

	switch(initStepCount - 1)
	{
	case 0: // Function set 8 bit, dummy DL and F
		aux = HDD4478_FUNCTION_SET(1, 0, 0);
		writeInstruction(HDD4478_FUNCTION_SET(1, 0, 0));
		timerStart(idTimer, TIMER_MS2TICKS(4), TIM_MODE_SINGLESHOT, timerCallback);
		break;
	case 1: // Function set 8 bit, dummy DL and F
		writeInstruction(HDD4478_FUNCTION_SET(1, 0, 0));
		timerStart(idTimer, TIMER_MS2TICKS(1), TIM_MODE_SINGLESHOT, timerCallback);
		break;
	case 2: // Function set 8 bit, dummy DL and F
		writeInstruction(HDD4478_FUNCTION_SET(1, 0, 0));
		break;
	case 3:	// Function set 8 bit, specify display lines and character font
		writeInstruction(HDD4478_FUNCTION_SET(1, lcdDisplayLines ? 1 : 0, lcdCharacterFont ? 1 : 0));
		break;
	case 4: // Display Off
		writeInstruction(HDD4478_DISPLAY_CONTROL(0,0,0));
		break;
	case 5:	// Display Clear
		writeInstruction(HDD4478_CLEAR_DISPLAY);
		break;
	case 6:	// Entry Mode Set,
		writeInstruction(HDD4478_ENTRY_MODE_SET(0, 0));
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
