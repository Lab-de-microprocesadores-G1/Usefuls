/*******************************************************************************
  @file     pwm_dma.c
  @brief    PWM DMA
  @author   G. Davidov, F. Farall, J. Gaytán, L. Kammann, N. Trozzo
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <string.h>

#include "pwm_dma.h"
#include "MK64F12.h"
#include "hardware.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define DMA_CHANNEL   0

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

// PWM-DMA Context data structure
typedef struct {

  /* Status and control fields of the context */
  bool                      alreadyInitialized;

  /* Data Buffer */
  uint16_t*                 buffer;
  size_t                    bufferSize;
  bool                      loop;  

  /* FTM peripheral instance and channel */
  ftm_instance_t            ftmInstance;
  ftm_channel_t             ftmChannel;

} pwmdma_context_t;


/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

static uint8_t pwmdmaFtm2DmaChannel(ftm_instance_t ftmInstance, ftm_channel_t ftmChannel);

/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static pwmdma_context_t context;

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void pwmdmaInit(uint8_t prescaler, uint16_t mod, ftm_instance_t ftmInstance, ftm_channel_t ftmChannel)
{
  FTM_Type * ftmInstances[] = FTM_BASE_PTRS;
  if (!context.alreadyInitialized)
  {
    // Update the already initialized flag
    context.alreadyInitialized = true;
    
    // Save context variables.
    context.ftmInstance = ftmInstance;
    context.ftmChannel = ftmChannel;

    // Initialize the ftm driver
    ftmInit(ftmInstance, prescaler, 0xFFFF);
    
    // Configure FTM for PWM
    ftmPwmInit(ftmInstance, ftmChannel, FTM_PWM_HIGH_PULSES, FTM_PWM_EDGE_ALIGNED, 1, mod);
    
    // Enable FTM to trigger DMA requests
	ftmInstances[context.ftmInstance]->CONTROLS[context.ftmChannel].CnSC |= FTM_CnSC_DMA(1) | FTM_CnSC_CHIE(1);
  
	// Legacy mode!!
	ftmInstances[context.ftmInstance]->MODE = (ftmInstances[context.ftmInstance]->MODE & ~FTM_MODE_FTMEN_MASK) | FTM_MODE_FTMEN(0);

    // Clock Gating for eDMA and DMAMux
    SIM->SCGC7 |= SIM_SCGC7_DMA_MASK;
	SIM->SCGC6 |= SIM_SCGC6_DMAMUX_MASK;

    // Enable NVIC for DMA channel 0
    NVIC_EnableIRQ(DMA0_IRQn);
  
    // Enable DMAMUX for DMA_CHANNEL and select source
    DMAMUX->CHCFG[DMA_CHANNEL] = DMAMUX_CHCFG_ENBL(1) | DMAMUX_CHCFG_TRIG(0) | DMAMUX_CHCFG_SOURCE(pwmdmaFtm2DmaChannel(context.ftmInstance, context.ftmChannel));

    //ftmStart(context.ftmInstance);
  }
}

void pwmdmaStart(uint16_t* buffer, size_t bufferSize, bool loop)
{
  FTM_Type * ftmInstances[] = FTM_BASE_PTRS;

  // Save the configuration of the transfers
  context.buffer = buffer;
  context.bufferSize = bufferSize;
  context.loop = loop;
  
  // DMA TCD configuration
  // Destination address: FTM CnV for duty change
  DMA0->TCD[DMA_CHANNEL].DADDR = (uint32_t)(&(ftmInstances[context.ftmInstance]->CONTROLS[context.ftmChannel].CnV));

  // Source and destination offsets
  DMA0->TCD[DMA_CHANNEL].SOFF = sizeof(uint16_t);
  DMA0->TCD[DMA_CHANNEL].DOFF = 0;
  
  // Source last address adjustment
  DMA0->TCD[DMA_CHANNEL].SLAST = - sizeof(uint16_t) * bufferSize;
  
  // Set transfer size to 16bits (CnV size)
  DMA0->TCD[DMA_CHANNEL].ATTR = DMA_ATTR_SSIZE(1) | DMA_ATTR_DSIZE(1);
  DMA0->TCD[DMA_CHANNEL].NBYTES_MLNO = (0x01) * (0x02);
  
  // Enable Interrupt on major loop
  DMA0->TCD[DMA_CHANNEL].CSR = DMA_CSR_INTMAJOR(1);

  // Minor Loop Beginning Value
  DMA0->TCD[DMA_CHANNEL].BITER_ELINKNO = bufferSize;
  // Minor Loop Current Value must be set to the beginning value the first time
  DMA0->TCD[DMA_CHANNEL].CITER_ELINKNO = bufferSize;

  // Set source address
  DMA0->TCD[DMA_CHANNEL].SADDR = (uint32_t)(buffer);

  // Set Destination offset at the end of major loop
  DMA0->TCD[DMA_CHANNEL].DLAST_SGA = - sizeof(uint16_t) * bufferSize;
  
  // Enable the DMA channel for requests
  DMA0->ERQ |= (0x0001 << DMA_CHANNEL);
  
  // Starts the ftm driver
  ftmStart(context.ftmInstance);
  //ftmPwmSetEnable(context.ftmInstance, context.ftmChannel, true);
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
__ISR__ DMA0_IRQHandler(void)
{
  uint16_t status = DMA0->INT;

  if (status & (DMA_INT_INT0_MASK << DMA_CHANNEL))
  {
    // Clear flag
    DMA0->INT = (DMA_INT_INT0_MASK << DMA_CHANNEL);

    /* Completed major loop */
    context.currentFrame = !context.currentFrame;   // Ping pong buffer switch  

    if (context.loop)
    {
    	// Continue DMA transfers
    }
    else
    {
    	// Stop DMA transfers
    	ftmStop(context.ftmInstance);
    }
}

uint8_t pwmdmaFtm2DmaChannel(ftm_instance_t ftmInstance, ftm_channel_t ftmChannel)
{
  uint8_t ret = 20;
  switch (ftmInstance)
  {
    case FTM_INSTANCE_0:  ret += ftmChannel; break;
    case FTM_INSTANCE_1:  ret += 8 + ftmChannel; break;
    case FTM_INSTANCE_2:  ret += 10 + ftmChannel; break;
    case FTM_INSTANCE_3:  ret += 12 + ftmChannel; break;
    default: break; 
  }
  
  return ret;
}

/*******************************************************************************
 *******************************************************************************
						            INTERRUPT SERVICE ROUTINES
 *******************************************************************************
 ******************************************************************************/



/******************************************************************************/
