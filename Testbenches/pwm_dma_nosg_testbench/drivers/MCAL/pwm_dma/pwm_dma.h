/*******************************************************************************
  @file     pwm_dma.h
  @brief    FTM PWM driver with DMA controller
  @author   G. Davidov, F. Farall, J. Gaytán, L. Kammann, N. Trozzo
 ******************************************************************************/

#ifndef PWM_DMA_H_
#define PWM_DMA_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include "drivers/MCAL/ftm/ftm.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

// The PWMDMA update callback is used to update the next frame to be used in the
// DMA transfer.
// @param frameToUpdate   Pointer to the frame to be updated
// @param frameCounter    Current frame 
typedef void (*pwmdma_update_callback_t)(uint16_t * frameToUpdate, uint8_t frameCounter);

/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/**
 * @brief Initializes the FTM for PWM with DMA support
 * @param prescaler   Prescaler value for the FTM time base
 * @param mod         Modulo for PWM period ticks count
 */ 
void pwmdmaInit(uint8_t prescaler, uint16_t mod, ftm_instance_t ftmInstance, ftm_channel_t ftmChannel);

/**
 * @brief Starts the PWM 
 * @param buffer    	Pointer to the data
 * @param bufferSize   	Total amount of elements
 * @param loop          Whether the transfer should run in loop or not
 */ 
void pwmdmaStart(uint16_t* buffer, size_t bufferSize, bool loop);


/*******************************************************************************
 ******************************************************************************/


#endif /* PWM_DMA_H_ */
