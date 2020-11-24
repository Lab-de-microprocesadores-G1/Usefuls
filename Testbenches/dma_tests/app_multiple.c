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

#include "board.h"
#include "drivers/MCAL/gpio/gpio.h"
#include "drivers/HAL/led/led.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define BUFFER_SIZE		32

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

static uint8_t	sourceBuffer[BUFFER_SIZE] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20};
static uint8_t	destinationBuffer[BUFFER_SIZE];

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

	// 0°) Sets the GPIO peripheral to raise DMA requests on
	// falling edge events detected in the port input.
	SIM->SCGC5 |= SIM_SCGC5_PORTC(1);
	PORTC->PCR[PIN2NUM(PIN_SW2)] = PORT_PCR_PS(1) | PORT_PCR_PE(1) | PORT_PCR_MUX(0x001) | PORT_PCR_IRQC(GPIO_IRQ_MODE_DMA_FALLING_EDGE);

	// 1°) Clock gating of the DMA and DMAMUX peripheral
	SIM->SCGC6 |= SIM_SCGC6_DMAMUX_MASK;
	SIM->SCGC7 |= SIM_SCGC7_DMA_MASK;

	// 2°) Configures the DMAMUX, selects which source will be used
	// for the DMA channel to raise requests, and enables it.
	// Enable the channel, without triggering option, routing the 51 source which is the PORTC.
	DMAMUX->CHCFG[0] = DMAMUX_CHCFG_ENBL(1) | DMAMUX_CHCFG_TRIG(0) | DMAMUX_CHCFG_SOURCE(51);

	// 3°) Complete the transfer control descriptor, to describe how the DMA transfer should be done.
	DMA0->TCD[0].SADDR = (uint32_t)(sourceBuffer);
	DMA0->TCD[0].SOFF = 1;
	DMA0->TCD[0].DADDR = (uint32_t)(destinationBuffer);
	DMA0->TCD[0].DOFF = 1;
	DMA0->TCD[0].ATTR = DMA_ATTR_SSIZE(0) | DMA_ATTR_DSIZE(0);
	DMA0->TCD[0].NBYTES_MLNO = ( 0x02 ) * ( 0x01 );
	DMA0->TCD[0].SLAST = 0;
	DMA0->TCD[0].DLAST_SGA = 0;
	DMA0->TCD[0].CSR = 0;
	DMA0->TCD[0].BITER_ELINKNO = 5;
	DMA0->TCD[0].CITER_ELINKNO = 5;

	// 4°) Enable DMA requests from the corresponding channel
	DMA0->ERQ = DMA_ERQ_ERQ0(1);
}

/* Called repeatedly in an infinite loop */
void appRun (void)
{
	if (DMA0->TCD[0].CSR & DMA_CSR_DONE_MASK)
	{
		ledSet(LED_GREEN);
		DMA0->TCD[0].CSR = 0;
	}
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/


/*******************************************************************************
 ******************************************************************************/
