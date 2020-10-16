/*******************************************************************************
  @file     ftm.h

  @brief    FTM Driver
			When using the FTM driver you should always follow these general steps,
				(1°) Timer general setup using ftmInit(...)
				(2°) Channel specific setup, for instance ftmOutputCompareInit(...) 
					 and, maybe, register a callback on channel match
				(3°) Never forget starting the ftm at the end with ftmStart(...)

  @author   G. Davidov, F. Farall, J. Gaytán, L. Kammann, N. Trozzo
 ******************************************************************************/

#ifndef MCAL_FTM_FTM_H_
#define MCAL_FTM_FTM_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

// FTM Channels
enum {
	FTM_CHANNEL_0,
	FTM_CHANNEL_1,
	FTM_CHANNEL_2,
	FTM_CHANNEL_3,
	FTM_CHANNEL_4,
	FTM_CHANNEL_5,
	FTM_CHANNEL_6,
	FTM_CHANNEL_7,
	FTM_CHANNEL_COUNT
};

// FTM Instances
enum {
	FTM_INSTANCE_0,
	FTM_INSTANCE_1,
	FTM_INSTANCE_2,
	FTM_INSTANCE_3,
	FTM_INSTANCE_COUNT
};

// Input Capture Modes
typedef enum {
	FTM_IC_RISING_EDGE,
	FTM_IC_FALLING_EDGE,
	FTM_IC_BOTH_EDGES
} ftm_ic_mode_t;

// Output Compare Modes
typedef enum {
	FTM_OC_TOGGLE,
	FTM_OC_CLEAR,
	FTM_OC_SET
} ftm_oc_mode_t;

// PWM Modes
typedef enum {
	FTM_PWM_HIGH_PULSES,
	FTM_PWM_LOW_PULSES
} ftm_pwm_mode_t;

// PWM Alignments
typedef enum {
	FTM_PWM_EDGE_ALIGNED,
	FTM_PWM_CENTER_ALIGNED
} ftm_pwm_alignment_t;

/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/*****************************
*                            *
* FTM TIMER GENERAL SERVICES *
*                            *
*****************************/

/*
 * @brief Initializes FTM
 * @param instance 		FTM Instance
 * @param prescaler		Frequency divider
 * @param module		Value to reset the count
 */
void ftmInit(uint8_t instance, uint8_t prescaler, uint16_t module);

/*
 * @brief Starts FTM
 * @param instance		FTM Instance
 */
void ftmStart(uint8_t instance);

/*
 * @brief Stops FTM
 * @param instance		FTM Instance
 */
void ftmStop(uint8_t instance);

/*
 * @brief Registers action to be done on instance overflow
 * @param instance		FTM Instance
 * @param callback		Callback to be called on overflow event
 */
void ftmOverflowSubscribe(uint8_t instance, void (*callback)(void));

/*****************************
*                            *
*  CHANNEL GENERAL SERVICES  *
*                            *
*****************************/

/**
 * @brief Sets FTM output value
 * @param instance		FTM Instance
 * @param channel		FTM Channel
 * @param value			Value to be written
 */
void ftmSetOutputValue(uint8_t instance, uint8_t channel, bool value);

/*
 * @brief Sets FTM count
 * @param instance		FTM Instance
 * @param channel		FTM Channel
 * @param count			Number of FTM ticks to count for next event
 */
void ftmSetChannelCount(uint8_t instance, uint8_t channel, uint16_t count);

/*
 * @brief Returns current value of FTM Channel count
 */
uint16_t ftmGetChannelCount(uint8_t instance, uint8_t channel);

/*
 * @brief Registers action to be done on channel event
 * @param instance		FTM Instance
 * @param channel		FTM Channel
 * @param callback		Callback to be called on channel event
 */
void ftmChannelSubscribe(uint8_t instance, uint8_t channel, void (*callback)(uint16_t));

/*****************************
*                            *
*   CHANNEL SETUP SERVICES   *
*                            *
*****************************/

/*
 * @brief Configures FTM Input Capture Mode
 * @param instance		FTM Instance
 * @param channel		FTM Channel
 * @param mode			FTM Input Capture Mode
 */
void ftmInputCaptureInit(uint8_t instance, uint8_t channel, ftm_ic_mode_t mode);

/*
 * @brief Configures FTM Output Compare Mode
 * @param instance		FTM Instance
 * @param channel		FTM Channel
 * @param mode			FTM Output Compare Mode
 * @param count			Number of FTM ticks to count for event
 */
void ftmOutputCompareInit(uint8_t instance, uint8_t channel, ftm_oc_mode_t mode, uint16_t count);

/*
 * @brief Configures FTM PWM mode
 * @param instance		FTM Instance
 * @param channel		FTM Channel
 * @param mode			PWM mode
 * @param alignment		PWM Alignment
 * @param duty			Duty cycle ticks count
 * @param period		Period ticks count
 */
void ftmPwmInit(uint8_t instance, uint8_t channel, ftm_pwm_mode_t mode, ftm_pwm_alignment_t alignment, uint16_t duty, uint16_t period);

/*
 * @brief Sets PWM Duty Cycle
 * @param instance		FTM Instance
 * @param channel		FTM Channel
 * @param duty			Duty cycle ticks count
 */
void ftmPwmSetDuty(uint8_t instance, uint8_t channel, uint16_t duty);


/*******************************************************************************
 ******************************************************************************/

#endif
