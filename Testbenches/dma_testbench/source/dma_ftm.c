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
#include "drivers/MCAL/ftm/ftm.h"

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

#define MAJOR_COUNT 64

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
	DMA_MUX_FTM0_CH0 = 20,
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

static uint16_t sourceBuffers[2][768];
static uint16_t destinationBuffer[768];

static eDMA_TCD_t tcds[2] __attribute__ ((aligned(32)));

static uint32_t	byteCount = 0;

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/* Called once at the beginning of the program */
void appInit (void)
{
	boardInit();

	// Initialize LED driver
	ledInit();

	// Initialize FTM Driver
	ftmInit(FTM_INSTANCE_0, 1, 0xFFFF);

	// Configure PWM
	ftmPwmInit(FTM_INSTANCE_0, FTM_CHANNEL_0, FTM_PWM_HIGH_PULSES, FTM_PWM_EDGE_ALIGNED, 31, 62);

	// Enable FTM to trigger DMA requests
	FTM0->CONTROLS[0].CnSC |= FTM_CnSC_DMA(1) | FTM_CnSC_CHIE(1);

	// Legacy mode!!
	FTM0->MODE = (FTM0->MODE & ~FTM_MODE_FTMEN_MASK) | FTM_MODE_FTMEN(0);

    // Filling sourceBuffer
    for (int i = 0 ; i < 768 ; i++)
    {
        sourceBuffers[0][i] = 40;
        sourceBuffers[1][i] = 20;
    }

    // Clock Gating for eDMA and DMAMux
    SIM->SCGC7 |= SIM_SCGC7_DMA_MASK;
	SIM->SCGC6 |= SIM_SCGC6_DMAMUX_MASK;

    // Enable DMAMUX for DMA0 and select PORTC (SW2 PORT) as DMA source
    DMAMUX->CHCFG[0] = DMAMUX_CHCFG_ENBL(1) | DMAMUX_CHCFG_TRIG(0) | DMAMUX_CHCFG_SOURCE(DMA_MUX_FTM0_CH0);

    // Enable NVIC for DMA channel 0
    NVIC_EnableIRQ(DMA0_IRQn);
    NVIC_EnableIRQ(DMA_Error_IRQn);

	// Enable error interrupts
    DMA0->EEI = 1;

    // Filling TCD...
    // Source and destination addresses
    tcds[0].SADDR = (uint32_t)(sourceBuffers[0]);
    tcds[0].DADDR = (uint32_t)(&(FTM0->CONTROLS[0].CnV));

    // Source and destination offsets
    tcds[0].SOFF = sizeof(uint16_t);
    tcds[0].DOFF = 0;

    // Source and destination last address adjustment
    tcds[0].SLAST = 0;
    tcds[0].DLAST_SGA = (uint32_t) &(tcds[1]);

    // Transfer attributes, not circular buffer
    tcds[0].ATTR = DMA_ATTR_SSIZE(DMA_TRANSFER_SIZE_16BIT) | DMA_ATTR_DSIZE(DMA_TRANSFER_SIZE_16BIT);

    // Number of bytes per transfer
    tcds[0].NBYTES_MLNO = 0x02;

    // Minor Loop Beginning Value
    tcds[0].BITER_ELINKNO = 768;

    // Minor Loop Current Value must be set to the beginning value the first time
    tcds[0].CITER_ELINKNO = 768;

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

    // Start FTM module
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
	uint16_t status = DMA0->INT;

	if (status & 0x01)
	{
		// Clear flag
		DMA0->INT = 0x01;

		ledBlinkSome(LED_GREEN, LED_MS2TICK(100), 5);

		for (uint32_t i = 0 ; i < 65536 ; i++);
		while(2);
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
