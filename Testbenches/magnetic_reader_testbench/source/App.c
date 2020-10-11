/********************************************************************************
  @file     App.c
  @brief    Application functions
  @author   N. Magliola, G. Davidov, F. Farall, J. Gaytán, L. Kammann, N. Trozzo
 *******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "drivers/HAL/magnetic_reader/magnetic_reader.h"

// Estos no corresponden, solo para el test-bench
#include "board/board.h"
#include "drivers/MCAL/gpio/gpio.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/


/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/
static void dataCallback(uint8_t array[]);
static void errorCallback(void);

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/* Función que se llama 1 vez, al comienzo del programa */
void App_Init (void)
{
    magneticReaderInit();
    magneticReaderSubscribe(dataCallback, errorCallback);
    gpioMode(PIN_LED_GREEN, OUTPUT);
    gpioToggle(PIN_LED_RED);
    gpioToggle(PIN_LED_GREEN);
    gpioMode(PIN_LED_RED, OUTPUT);
}

/* Función que se llama constantemente en un ciclo infinito */
void App_Run (void)
{

}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
static void dataCallback(uint8_t array[])
{
	uint64_t i = 0;
	gpioWrite(PIN_LED_GREEN, true);
//	for(i=0; i<1000000;i++)
//	{
//
//	}
	gpioWrite(PIN_LED_GREEN, false);
	gpioWrite(PIN_LED_RED, true);
}

static void errorCallback(void)
{
	uint64_t i = 0;
	gpioWrite(PIN_LED_RED, true);
//	for(i=0; i<1000000;i++)
//	{
//
//	}
	gpioWrite(PIN_LED_RED, false);
	gpioWrite(PIN_LED_GREEN, true);
}

/*******************************************************************************
 ******************************************************************************/
