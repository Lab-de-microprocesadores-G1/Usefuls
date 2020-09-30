/***************************************************************************//**
  @file     board.h
  @brief    Board management
  @author   N. Magliola, G. Davidov, F. Farall, J. Gaytán, L. Kammann, N. Trozzo
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include "board.h"


/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/


/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void boardInit(void)
{
  // By default, unable interrupts of the SW3
  gpioMode(PIN_Sw3, INPUT);
  gpioIrq(PIN_SW3, GPIO_IRQ_MODE_DISABLE, null);
}