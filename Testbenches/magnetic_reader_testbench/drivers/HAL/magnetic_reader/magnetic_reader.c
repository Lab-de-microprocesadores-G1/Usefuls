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
#define BITS_PER_CHAR	    5
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

static void cardSwipe(void);
static void cardSwipeStopped(void);
static void dataRead(void);
static void dataCheck(void);


/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static bool     enableActive;
static bool     idReceibed;
static bool     error = true;
static uint32_t cardData[DATA_LENGTH];


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

  // Configure IRQ's
  gpioIRQ(MAG_CARD_ENABLE, GPIO_IRQ_MODE_INTERRUPT_FALLING_EDGE, cardSwipe);
  gpioIRQ(MAG_CARD_ENABLE, GPIO_IRQ_MODE_INTERRUPT_RISING_EDGE, cardSwipeStopped);
  gpioIRQ(MAG_CARD_CLK, GPIO_IRQ_MODE_INTERRUPT_RISING_EDGE, dataRead);

}

bool getCardNumber(uint32_t id[])
{
  if(!error)
  {
    uint8_t i;
    for(i = 0; i < 40; i++)
    {
      id[i] = cardData[i * BITS_PER_CHAR + 0] + (cardData[i * BITS_PER_CHAR + 1] << 1) + (cardData[i * BITS_PER_CHAR + 2] << 2) + (cardData[i * BITS_PER_CHAR + 3] << 3);
    }
  }
  return !error;
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

static void cardSwipe(void)
{
  enableActive = true;
}

static void cardSwipeStopped(void)
{
  enableActive = false;
  if(idReceibed)
  {
    error = false;
    dataCheck();
    idReceibed = false;
  }
  else
  {
    error = true;
  }
}

static void dataRead(void)
{
  static uint32_t i = 0;
  if(enableActive)
  {
    if(i < DATA_LENGTH && !idReceibed && !error)
    {
      cardData[i] = gpioRead(MAG_CARD_DATA);
      i++;
      idReceibed = false;
    }
    else if(error)
    {
      i = 0;
      cardData[i] = gpioRead(MAG_CARD_DATA);
      i++;
      idReceibed = false;

    }
    else
    {
      i = 0;
      idReceibed = true;
    }
  }
  else
  {
    i = 0;
    error = true;
  }
}

static void dataCheck(void)
{
    // Check for parity bit
    uint8_t  sum = 0;
    uint8_t  i = 0;
    uint8_t  j = 0;
    for(i = 0; i < 40; i++)
    {
    	for(j = 0; j < 4; i++)
		{
		  sum = (sum + cardData[i * BITS_PER_CHAR + j]);
		}
    	if((sum & (0x01)) == cardData[i * BITS_PER_CHAR + 4])			// Looking for the bit nº4 which is parity bit
		{
			error = true;
		}
		else
		{
			error = false;
		}
    	sum = 0;
    }
    bool finalerror;
    uint32_t id[40];
    finalerror = getCardNumber(id);
}

/******************************************************************************/
