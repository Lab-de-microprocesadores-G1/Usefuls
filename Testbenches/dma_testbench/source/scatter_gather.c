/********************************************************************************
  @file     App.c
  @brief    Application functions
  @author   N. Magliola, G. Davidov, F. Farall, J. Gaytán, L. Kammann, N. Trozzo
 *******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdio.h>
#include <string.h>

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
__ISR__ DMA_Error_IRQHandler(void);


/*******************************************************************************
 * VARIABLES TYPES DEFINITIONS
 ******************************************************************************/

typedef struct {
    uint32_t SADDR;                                 /**< TCD Source Address, array offset: 0x1000, array step: 0x20 */
    uint16_t SOFF;                                  /**< TCD Signed Source Address Offset, array offset: 0x1004, array step: 0x20 */
    uint16_t ATTR;                                  /**< TCD Transfer Attributes, array offset: 0x1006, array step: 0x20 */
    union {                                          /* offset: 0x1008, array step: 0x20 */
        uint32_t NBYTES_MLNO;                       /**< TCD Minor Byte Count (Minor Loop Disabled), array offset: 0x1008, array step: 0x20 */
        uint32_t NBYTES_MLOFFNO;                    /**< TCD Signed Minor Loop Offset (Minor Loop Enabled and Offset Disabled), array offset: 0x1008, array step: 0x20 */
        uint32_t NBYTES_MLOFFYES;                   /**< TCD Signed Minor Loop Offset (Minor Loop and Offset Enabled), array offset: 0x1008, array step: 0x20 */
    };
    uint32_t SLAST;                                 /**< TCD Last Source Address Adjustment, array offset: 0x100C, array step: 0x20 */
    uint32_t DADDR;                                 /**< TCD Destination Address, array offset: 0x1010, array step: 0x20 */
    uint16_t DOFF;                                  /**< TCD Signed Destination Address Offset, array offset: 0x1014, array step: 0x20 */
    union {                                          /* offset: 0x1016, array step: 0x20 */
        uint16_t CITER_ELINKNO;                     /**< TCD Current Minor Loop Link, Major Loop Count (Channel Linking Disabled), array offset: 0x1016, array step: 0x20 */
        uint16_t CITER_ELINKYES;                    /**< TCD Current Minor Loop Link, Major Loop Count (Channel Linking Enabled), array offset: 0x1016, array step: 0x20 */
    };
    uint32_t DLAST_SGA;                             /**< TCD Last Destination Address Adjustment/Scatter Gather Address, array offset: 0x1018, array step: 0x20 */
    uint16_t CSR;                                   /**< TCD Control and Status, array offset: 0x101C, array step: 0x20 */
    union {                                          /* offset: 0x101E, array step: 0x20 */
        uint16_t BITER_ELINKNO;                     /**< TCD Beginning Minor Loop Link, Major Loop Count (Channel Linking Disabled), array offset: 0x101E, array step: 0x20 */
        uint16_t BITER_ELINKYES;                    /**< TCD Beginning Minor Loop Link, Major Loop Count (Channel Linking Enabled), array offset: 0x101E, array step: 0x20 */
    };
} eDMA_TCD_t;

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

static int8_t sourceBuffers[2][50];
static int8_t destinationBuffer[50];


static eDMA_TCD_t tcds[2] __attribute__ ((aligned(32)));

static uint16_t description;

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
        sourceBuffers[0][i] = i;
        sourceBuffers[1][i] = -i;
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
    NVIC_EnableIRQ(DMA_Error_IRQn);

    DMA0->EEI = 1;

    // Filling TCD...
    // Source and destination addresses
    tcds[0].SADDR = (uint32_t)(sourceBuffers[0]);
    tcds[0].DADDR = (uint32_t)(destinationBuffer);

    // Source and destination offsets
    tcds[0].SOFF = sizeof(uint8_t);
    tcds[0].DOFF = sizeof(uint8_t);

    // Source and destination last address adjustment
    tcds[0].SLAST = 0;
    tcds[0].DLAST_SGA = (uint32_t) &(tcds[1]);

    // Transfer atributes, not circular buffer
    tcds[0].ATTR = DMA_ATTR_SSIZE(DMA_TRANSFER_SIZE_8BIT) | DMA_ATTR_DSIZE(DMA_TRANSFER_SIZE_8BIT);

    // Number of bytes per transfer
    tcds[0].NBYTES_MLNO = 0x05;

    // Minor Loop Beggining Value
    tcds[0].BITER_ELINKNO = 0x01;

    // Minor Loop Current Value must be set to the beggining value the first time
    tcds[0].CITER_ELINKNO = 0x01;

    // Enable Major Loop End Interrupts and Scatter-Gather operation
    tcds[0].CSR |= DMA_CSR_INTMAJOR(1) | DMA_CSR_ESG(1);

    // Second buffer
    tcds[1] = tcds[0];

    tcds[1].SADDR = (uint32_t)(sourceBuffers[1]);

    // Link tcds
    tcds[1].DLAST_SGA = &(tcds[0]);

	memcpy(&(DMA0->TCD[0]), &(tcds[0]), sizeof(eDMA_TCD_t));

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
}

__ISR__ DMA_Error_IRQHandler(void)
{
	uint16_t error = DMA0->ERR;

	if (error & 0x01)
	{
		// Clear flag
		DMA0->ERR = 0x01;
		ledBlinkSome(LED_RED, LED_MS2TICK(100), 5);
	}


}

/*******************************************************************************
 ******************************************************************************/
