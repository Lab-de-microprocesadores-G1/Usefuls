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

void pressCallback(void);
void releaseCallback(void);
void typematicCallback(void);
void lkpCallback(void);

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/* Función que se llama 1 vez, al comienzo del programa */
void App_Init (void)
{
    gpioMode(PIN_LED_BLUE, OUTPUT);
    gpioMode(PIN_LED_RED, OUTPUT);
	gpioToggle(PIN_LED_BLUE);
	gpioToggle(PIN_LED_RED);
    buttonInit();
    buttonSubscribe(BUTTON_1, BUTTON_PRESS, pressCallback);
    buttonSubscribe(BUTTON_1, BUTTON_RELEASE, releaseCallback);
    buttonSubscribe(BUTTON_1, BUTTON_TYPEMATIC, typematicCallback);
    buttonSubscribe(BUTTON_1, BUTTON_LKP, lkpCallback);
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

void pressCallback(void)
{
	gpioToggle(PIN_LED_BLUE);
}

void releaseCallback(void)
{
	gpioWrite(PIN_LED_BLUE, HIGH);
	gpioWrite(PIN_LED_RED, HIGH);
}

void lkpCallback(void)
{
	gpioToggle(PIN_LED_BLUE);
}

void typematicCallback(void)
{
	gpioToggle(PIN_LED_RED);
}



/*******************************************************************************
 ******************************************************************************/
