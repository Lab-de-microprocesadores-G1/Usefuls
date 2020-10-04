/*******************************************************************************
  @file     magnetic_reader.c
  @brief    [...]
  @author   G. Davidov, F. Farall, J. Gaytán, L. Kammann, N. Trozzo
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "drivers/MCAL/gpio/gpio.h"
#include "board/board.h"
#include "magnetic_reader.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define MAG_CARD_ENABLE   PIN_MAG_CARD_ENABLE
#define MAG_CARD_DATA     PIN_MAG_CARD_DATA
#define MAG_CARD_CLK      PIN_MAG_CARD_CLK

#define DATA_CHARS        40
#define BITS_PER_CHAR	  5
#define DATA_LENGTH       DATA_CHARS * BITS_PER_CHAR

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/


/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/


/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/


static void edgesHandler(void);
static void cardSwipe(void);
static void cardSwipeStopped(void);
static void dataRead(void);
static bool getUsefulData(void);
static uint16_t searchSS(void);
static uint16_t searchES(uint16_t ss);
static bool dataParse(void);
static void getCardNumber(void);
static void clearData(void);

/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static bool     enableActive;
static bool     restart = false;
static uint8_t 	cardData[DATA_LENGTH*2];
static uint8_t	usefulCardData[DATA_LENGTH];
static uint8_t 	finalId[40];

static void 	(*dataCallback)(uint8_t data[]);
static void 	(*errorCallback)(void);

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void magneticReaderInit(void)
{
	// Configure pins as input
	gpioMode(MAG_CARD_ENABLE, INPUT);
	gpioMode(MAG_CARD_DATA, INPUT);
	gpioMode(MAG_CARD_CLK, INPUT);
	gpioMode(PIN_TEST1, OUTPUT);
	gpioMode(PIN_TEST2, OUTPUT);
	gpioMode(PIN_TEST3, OUTPUT);

	// Configure IRQ's
	gpioIRQ(MAG_CARD_ENABLE, GPIO_IRQ_MODE_INTERRUPT_BOTH_EDGES, edgesHandler);
	gpioIRQ(MAG_CARD_CLK, GPIO_IRQ_MODE_INTERRUPT_FALLING_EDGE, dataRead);

}

void magneticReaderSubscribe(void (*dataCb) (uint8_t data[]), void (*errorCb) (void))
{
	dataCallback = dataCb;
	errorCallback = errorCb;
}


/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

static void edgesHandler(void)
{
	gpioWrite(PIN_TEST1, true);
	bool state;
	state = gpioRead(MAG_CARD_ENABLE);
	if(state)
	{
		cardSwipeStopped();
	}
	else
	{
		cardSwipe();
	}
	gpioWrite(PIN_TEST1, false);
}

static void cardSwipe(void)
{
	enableActive = true;
}

static void cardSwipeStopped(void)
{
	if(enableActive)
	{
		enableActive = false;
		restart = true;

		gpioWrite(PIN_TEST3, true);
		if(!dataParse())
		{
			gpioWrite(PIN_TEST3, false);
		    getCardNumber();
		    dataCallback(finalId);
		    clearData();
		}
		else
		{
			errorCallback();
		}
	}
}

static void dataRead(void)
{
	gpioWrite(PIN_TEST2, true);
	static uint32_t bitCounter = 0;
	if(restart)
	{
		bitCounter = 0;
		restart = false;
	}
	if(enableActive)
	{
		cardData[bitCounter] = (!gpioRead(MAG_CARD_DATA));
		bitCounter++;
	}
	else
	{
		bitCounter = 0;
	}
	gpioWrite(PIN_TEST2, false);
}

static bool getUsefulData(void)
{
	uint8_t  i = 0;
	bool error = false;
	uint16_t ss = searchSS();
	uint16_t es = searchES(ss);
	if(es == 0 || ss == 0)
	{
		error = true;
	}
	if(!error)
	{
		for(i = 0; i <= es - ss; i++)
		{
			usefulCardData[i] = cardData[ss + i];
		}
	}
	return error;
}

static uint16_t searchSS(void)
{
	uint16_t i;
	uint8_t incomingData[5] = {0, 0, 0, 0, 0};
	uint8_t sum = 0;
	for(i = 0; i < 400; i++)
	{
	    incomingData[0] = cardData[i];
		sum = incomingData[4] + (incomingData[3] << 1) + (incomingData[2] << 2) + (incomingData[1] << 3) + (incomingData[0] << 4);
		if(sum == 11)
		{
			return i - 4;
		}
		else
		{
			uint8_t bit;
		    for(bit = 4; bit >= 1; bit--)
		    {
		        incomingData[bit] = incomingData[bit - 1];
		    }
		}
	}
	return false;
}

static uint16_t searchES(uint16_t ss)
{
	uint16_t i, es = 0;
	for(i = 0; i < 40; i++)
	{
		es = cardData[i * BITS_PER_CHAR + ss] + (cardData[i * BITS_PER_CHAR + ss + 1] << 1) + (cardData[i * BITS_PER_CHAR + ss + 2] << 2) + (cardData[i * BITS_PER_CHAR + ss + 3] << 3);
		if(es == 0xF)
		{
			return i * BITS_PER_CHAR + ss + BITS_PER_CHAR;
		}
	}
	return false;
}


static bool dataParse(void)
{
 // Check for parity bit
	bool error = false;
	uint8_t  i = 0;
	uint8_t  j = 0;
	uint8_t sum = 0;
	error = getUsefulData();

	if(!error)
	{
		for(i = 0; i < 40; i++)
			{
				for(j = 0; j < 4; j++)
				{
				  sum = (sum + usefulCardData[i * BITS_PER_CHAR + j]);
				}
				if((sum & (0x01)) == usefulCardData[i * BITS_PER_CHAR + 4])			// Looking for the bit nº4 which is parity bit
				{
					error = true;
				}
				sum = 0;
			}
	}
    return error;
}

static void getCardNumber(void)
{
	uint8_t i;
	for(i = 0; i < 40; i++)
	{
		finalId[i] =  usefulCardData[i * BITS_PER_CHAR + 0] + (usefulCardData[i * BITS_PER_CHAR + 1] << 1) + (usefulCardData[i * BITS_PER_CHAR + 2] << 2) + (usefulCardData[i * BITS_PER_CHAR + 3] << 3);
	}
}

static void clearData(void)
{
	uint16_t i;
	for(i = 0; i < 400; i++)
	{
		cardData[i] = 0;
	}
}

/******************************************************************************/
