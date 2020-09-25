/********************************************************************************
  @file     App.c
  @brief    Application functions
  @author   N. Magliola, G. Davidov, F. Farall, J. Gaytán, L. Kammann, N. Trozzo
 *******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "drivers/HAL/button/button.h"

// Estos no corresponden, solo para el test-bench
#include "board/board.h"
#include "drivers/MCAL/gpio/gpio.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/


/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

void boardRedOn(void);
void boardRedOff(void);
void protoRedOn(void);
void protoRedOff(void);
void toggleGreen(void);
void toggleBlue(void);

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/* Función que se llama 1 vez, al comienzo del programa */
void App_Init (void)
{
	// Configure LED pins for output
    gpioMode(PIN_LED_BLUE, OUTPUT);
    gpioMode(PIN_LED_RED, OUTPUT);
    gpioMode(PIN_LED_GREEN, OUTPUT);
    gpioMode(PIN_LED_OUT, OUTPUT);
	gpioToggle(PIN_LED_BLUE);
	gpioToggle(PIN_LED_RED);
	gpioToggle(PIN_LED_GREEN);
	gpioToggle(PIN_LED_OUT);

	// Configure buttons drivers
    buttonInit();

    // Board button
    buttonSubscribe(BUTTON_1, BUTTON_PRESS, boardRedOn);
    buttonSubscribe(BUTTON_1, BUTTON_RELEASE, boardRedOff);
    buttonSubscribe(BUTTON_1, BUTTON_TYPEMATIC, toggleBlue);

    // Protoboard button
    buttonSubscribe(BUTTON_2, BUTTON_PRESS, protoRedOn);
    buttonSubscribe(BUTTON_2, BUTTON_RELEASE, protoRedOff);
    buttonSubscribe(BUTTON_2, BUTTON_TYPEMATIC, toggleGreen);
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

void boardRedOn(void)
{
	gpioWrite(PIN_LED_RED, LOW);
}

void boardRedOff(void)
{
	gpioWrite(PIN_LED_RED, HIGH);
	gpioWrite(PIN_LED_BLUE, HIGH);
}

void protoRedOn(void)
{
	gpioWrite(PIN_LED_OUT, LOW);
}

void protoRedOff(void)
{
	gpioWrite(PIN_LED_OUT, HIGH);
	gpioWrite(PIN_LED_GREEN, HIGH);
}

void toggleGreen(void)
{
	gpioToggle(PIN_LED_GREEN);
}

void toggleBlue(void)
{
	gpioToggle(PIN_LED_BLUE);
}

/*******************************************************************************
 ******************************************************************************/
