/********************************************************************************
  @file     App.c
  @brief    Application functions
  @author   N. Magliola, G. Davidov, F. Farall, J. Gaytán, L. Kammann, N. Trozzo
 *******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdio.h>

#include "hardware.h"
#include "MK64F12.h"
#include "board/board.h"
#include "drivers/MCAL/gpio/gpio.h"
#include "drivers/HAL/led/led.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

// #define SOME_CONSTANT    20
// #define MACRO(x)         (x)

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

__ISR__ DMA0_IRQHandler(void);

/*******************************************************************************
 * VARIABLES TYPES DEFINITIONS
 ******************************************************************************/

typedef enum {
    DMA_MUX_PORTC = 51
} dma_mux_sources_t;

typedef enum {
    DMA_TRANSFER_SIZE_8BIT,
    DMA_TRANSFER_SIZE_16BIT,
    DMA_TRANSFER_SIZE_32BIT,
    DMA_TRANSFER_SIZE_16BYTE_BURST = 0b100,
    DMA_TRANSFER_SIZE_32BYTE_BURST
} dma_data_transfer_size_t;

/*******************************************************************************
 * PRIVATE VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

uint8_t sourceBuffer[50];
uint16_t destinationBuffer[50];

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/* Called once at the beginning of the program */
void appInit (void)
{
	// Initialize LED driver
	ledInit();

    // Filling sourceBuffer
    for (int i=0; i<50; i++)
    {
        sourceBuffer[i] = i;
    }

    // Clock gating for PORTC
	SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;

    // Configure PORTC (SW2) to trigger DMA requests
	PORTC->PCR[PIN2NUM(PIN_SW2)] = PORT_PCR_MUX(1) | PORT_PCR_PE(1) | PORT_PCR_PS(1) | PORT_PCR_IRQC(GPIO_IRQ_MODE_DMA_FALLING_EDGE) ; 

    // Clock Gating for eDMA and DMAMux
    SIM->SCGC7 |= SIM_SCGC7_DMA_MASK;
	SIM->SCGC6 |= SIM_SCGC6_DMAMUX_MASK;

    // Enable DMAMUX for DMA0 and select PORTC (SW2 PORT) as DMA source
    DMAMUX->CHCFG[0] = DMAMUX_CHCFG_ENBL(1) | DMAMUX_CHCFG_TRIG(0) | DMAMUX_CHCFG_SOURCE(DMA_MUX_PORTC);

    // Enable NVIC for DMA channel 0
    NVIC_EnableIRQ(DMA0_IRQn);

    // Filling TCD...
    // Source and destination addresses
    DMA0->TCD[0].SADDR = (uint32_t)(sourceBuffer);
    DMA0->TCD[0].DADDR = (uint32_t)(destinationBuffer);

    // Source and destination offsets
    DMA0->TCD[0].SOFF = sizeof(uint8_t);
    DMA0->TCD[0].DOFF = sizeof(uint16_t);

    // Source and destination last address adjustment
    DMA0->TCD[0].SLAST = -5*4*sizeof(uint8_t);
    DMA0->TCD[0].DLAST_SGA = -5*4*sizeof(uint16_t);

    // Transfer atributes, not circular buffer
    DMA0->TCD[0].ATTR = DMA_ATTR_SSIZE(DMA_TRANSFER_SIZE_8BIT) | DMA_ATTR_DSIZE(DMA_TRANSFER_SIZE_8BIT);

    // Number of bytes per transfer
    DMA0->TCD[0].NBYTES_MLNO = 0x05;

    // Minor Loop Beggining Value
    DMA0->TCD[0].BITER_ELINKNO = 0x04;

    // Minor Loop Current Value must be set to the beggining value the first time
    DMA0->TCD[0].CITER_ELINKNO = 0x04;

    // Enable Major Loop End Interrupts
    DMA0->TCD[0].CSR |= DMA_CSR_INTMAJOR(1);

    // Enable DMA Request for channel 0
    DMA0->ERQ |= DMA_ERQ_ERQ0(1);
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
	uint16_t status = DMA0->INT;

	if (status & 0x01)
	{
		// Clear flag
		DMA0->INT = 0x01;

		ledBlinkSome(LED_GREEN, LED_MS2TICK(100), 5);
	}
	else
	{
		ledBlinkSome(LED_RED, LED_MS2TICK(100), 5);
	}
}

/*******************************************************************************
 ******************************************************************************/
