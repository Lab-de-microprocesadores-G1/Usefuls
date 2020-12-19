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

#define SIGNAL_FREQ		1000
#define PI 				3.14159265
#define SAMPLE_RATE 	44100
#define PIT_CLOCK		50e6
#define BUFFER_SIZE 	(SAMPLE_RATE / SIGNAL_FREQ)

#define DAC_PEAK_VALUE	4096

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

static void onTimeout(void);
static void pitInit();
static __ISR__  PIT_IRQHandler(void);
static uint16_t adjustSample(double sample, uint32_t peakValue, bool bipolar);

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
		buffer[i] = adjustSample(sin(aux), 1, true);
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
	PIT->CHANNEL[0].LDVAL = (int)(PIT_CLOCK / (double)SAMPLE_RATE);

	// Enable interrupts
	// NVIC_EnableIRQ(PIT0_IRQn);

	// Enable timer and interrupts
	PIT->CHANNEL[0].TCTRL = PIT_TCTRL_TEN(1) | PIT_TCTRL_TIE(0);
}

__ISR__  PIT_IRQHandler(void)
{
	// Clear flag
	PIT->CHANNEL[0].TFLG = PIT_TFLG_TIF_MASK;

	onTimeout();
}

uint16_t adjustSample(double sample, uint32_t peakValue, bool bipolar)
{
	double result;

	// Scale to DAC range
	result = (sample * DAC_PEAK_VALUE) / peakValue;

	// If bipolar, peak2peak voltage is 2 * peakValue, divide by 2
	if (bipolar)
	{
		result /= 2;
	}

	// Adjust offset to go from 0 DAC_PEAK
	return	(uint16_t)(result + DAC_PEAK_VALUE / 2 - 1);
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/


/*******************************************************************************
 ******************************************************************************/
