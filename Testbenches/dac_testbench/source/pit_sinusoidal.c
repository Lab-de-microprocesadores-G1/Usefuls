/********************************************************************************
  @file     App.c
  @brief    Application functions
  @author   N. Magliola, G. Davidov, F. Farall, J. Gaytán, L. Kammann, N. Trozzo
 *******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdint.h>
#include <math.h>
#include "drivers/MCAL/systick/systick.h"
#include "drivers/MCAL/dac/dac.h"

#include "MK64F12.h"
#include "hardware.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define BUFFER_SIZE 2000
#define PI 			3.14159265

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

static void onTimeout(void);
static void pitInit();
static __ISR__  PIT_IRQHandler(void);


/*******************************************************************************
 * VARIABLES TYPES DEFINITIONS
 ******************************************************************************/


/*******************************************************************************
 * PRIVATE VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static uint16_t 	buffer[BUFFER_SIZE];
static uint32_t 	bufferIndex = 0;

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/* Called once at the beginning of the program */
void appInit (void)
{
	// Setup DAC
	dac_cfg_t config = { .swTrigger = 1 };
	dacInit(DAC_0, config);

	// Fill buffer
	for (uint32_t i = 0 ; i < BUFFER_SIZE ; i++)
	{
		double aux = 2 * PI * i / BUFFER_SIZE;
		buffer[i] = ( sin(aux) * (4096/2) ) + 2047;
	}

	// Initialize PIT for periodic interrupts
	pitInit();
}

/* Called repeatedly in an infinite loop */
void appRun (void)
{
	if (PIT->CHANNEL[0].TFLG & PIT_TFLG_TIF_MASK)
	{
		// Clear flag
		PIT->CHANNEL[0].TFLG = PIT_TFLG_TIF_MASK;

		onTimeout();
	}

}

void onTimeout(void)
{
	// Write buffer to output
	dacWrite(DAC_0, buffer[bufferIndex++]);

	bufferIndex %= BUFFER_SIZE;
}

void pitInit()
{
	// Clock gating for PIT peripheral
	SIM->SCGC6 |= SIM_SCGC6_PIT(1);

	// Enable module
	PIT->MCR = PIT_MCR_MDIS(0);

	// Configure desired sample rate
	PIT->CHANNEL[0].LDVAL = 1133;

	// Enable interrupts
	NVIC_EnableIRQ(PIT0_IRQn);

	// Enable timer and interrupts
	PIT->CHANNEL[0].TCTRL = PIT_TCTRL_TEN(1) | PIT_TCTRL_TIE(1);
}

__ISR__  PIT_IRQHandler(void)
{
	// Clear flag
	PIT->CHANNEL[0].TFLG = PIT_TFLG_TIF_MASK;

	onTimeout();
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/


/*******************************************************************************
 ******************************************************************************/
