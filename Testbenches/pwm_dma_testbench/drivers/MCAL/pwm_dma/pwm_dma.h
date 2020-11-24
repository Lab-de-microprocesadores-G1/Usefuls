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

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

// PWM DMA Update Callback
// 
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
void pwmdmaInit(uint8_t prescaler, uint16_t mod);

/**
 * @brief Registers callback for frame update request from the driver
 * @param callback    pwmdma_update_callback_t 
 */ 
void pwmdmaOnFrameUpdate(pwmdma_update_callback_t callback);

/**
 * @brief Starts the PWM 
 * @brief firstFrame
 */ 
void pwmdmaStart(uint16_t* firstFrame, uint16_t* secondFrame, size_t frameSize, size_t totalSize);




/*******************************************************************************
 ******************************************************************************/


#endif /* PWM_DMA_H_ */
