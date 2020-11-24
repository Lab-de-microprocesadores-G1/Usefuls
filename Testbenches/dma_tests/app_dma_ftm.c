/********************************************************************************
  @file     App.c
  @brief    Application functions
  @author   N. Magliola, G. Davidov, F. Farall, J. Gaytán, L. Kammann, N. Trozzo
 *******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdio.h>
#include <stdint.h>

#include "MK64F12.h"
#include "hardware.h"

#include "board.h"
#include "drivers/MCAL/gpio/gpio.h"
#include "drivers/HAL/led/led.h"
#include "drivers/MCAL/ftm/ftm.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define BUFFER_SIZE		256
#define SAMPLE_SIZE		2

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

static uint16_t	sourceBuffer[BUFFER_SIZE] = {10000, 40000};
static uint16_t destinationBuffer[BUFFER_SIZE];

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/* Called once at the beginning of the program */
void appInit (void)
{
	// Initialization of the board and led
	boardInit();
	ledInit();

	// 0°) Configuration of the FTM driver to be used with DMA
	ftmInit(FTM_INSTANCE_0, 1, 0xFFFF);
	ftmPwmInit(FTM_INSTANCE_0, FTM_CHANNEL_0, FTM_PWM_EDGE_ALIGNED, FTM_PWM_HIGH_PULSES, 25000, 50000);

	FTM0->CONTROLS[0].CnSC |= FTM_CnSC_DMA(1) | FTM_CnSC_CHIE(1);
	FTM0->MODE = (FTM0->MODE & ~FTM_MODE_FTMEN_MASK) | FTM_MODE_FTMEN(0);

	// 1°) Clock gating of the DMA and DMAMUX peripheral
	SIM->SCGC6 |= SIM_SCGC6_DMAMUX_MASK;
	SIM->SCGC7 |= SIM_SCGC7_DMA_MASK;

	// 2°) Configures the DMAMUX, selects which source will be used
	// for the DMA channel to raise requests, and enables it.
	// Enable the channel, without triggering option, routing the 51 source which is the PORTC.
	DMAMUX->CHCFG[0] = DMAMUX_CHCFG_ENBL(1) | DMAMUX_CHCFG_TRIG(0) | DMAMUX_CHCFG_SOURCE(20);

	// 3°) Complete the transfer control descriptor, to describe how the DMA transfer should be done.
	DMA0->TCD[0].SADDR = (uint32_t)(sourceBuffer);
	DMA0->TCD[0].SOFF = 2;
	DMA0->TCD[0].DADDR = (uint32_t)(&(FTM0->CONTROLS[0].CnV));
	DMA0->TCD[0].DOFF = 0;
	DMA0->TCD[0].ATTR = DMA_ATTR_SSIZE(1) | DMA_ATTR_DSIZE(1);
	DMA0->TCD[0].NBYTES_MLNO = ( 0x01 ) * ( 0x02 );
	DMA0->TCD[0].SLAST = - SAMPLE_SIZE * 0x02;
	DMA0->TCD[0].DLAST_SGA = 0;
	DMA0->TCD[0].CSR = 0;
	DMA0->TCD[0].BITER_ELINKNO = SAMPLE_SIZE;
	DMA0->TCD[0].CITER_ELINKNO = SAMPLE_SIZE;

	// 4°) Enable NVIC interruption
	NVIC_EnableIRQ(DMA0_IRQn);

	// 5°) Enable DMA requests from the corresponding channel
	DMA0->ERQ = DMA_ERQ_ERQ0(1);

	ftmStart(FTM_INSTANCE_0);
}

/* Called repeatedly in an infinite loop */
void appRun (void)
{
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

__ISR__ DMA0_IRQHandler(void)
{
	if (DMA0->INT & DMA_INT_INT0_MASK)
	{
		DMA0->INT |= DMA_INT_INT0_MASK;
		ledBlinkSome(LED_GREEN, LED_MS2TICK(50), 6);
	}
}

/*******************************************************************************
 ******************************************************************************/
