/*******************************************************************************
  @file     ftm.c
  @brief    FTM Driver
  @author   G. Davidov, F. Farall, J. Gaytán, L. Kammann, N. Trozzo
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "hardware.h"
#include "ftm.h"
#include "../gpio/gpio.h"
#include "../../../CMSIS/MK64F12.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

// Callback types of the FTM driver
typedef void 	(*ov_callback)	(void);
typedef void	(*ch_callback)	(uint16_t);

// Clock source options for the FTM instance
enum {
	FTM_CLOCK_DISABLED,
	FTM_CLOCK_SYSTEM,
	FTM_CLOCK_INTERNAL,
	FTM_CLOCK_EXTERNAL	
};

/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

// FTM instances interruption service routines
void 	FTM_IRQDispatch(uint8_t instance);
__ISR__ FTM0_IRQHandler(void);
__ISR__ FTM1_IRQHandler(void);
__ISR__ FTM2_IRQHandler(void);
__ISR__ FTM3_IRQHandler(void);

// Configure PORT MUX 
void setFtmChannelMux(uint8_t instance, uint8_t channel);

/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

// Callbacks registered for both overflow and channel of every FTM instance
static ov_callback		ftmOverflowCallbacks[FTM_INSTANCE_COUNT];
static ch_callback		ftmChannelCallbacks[FTM_INSTANCE_COUNT][FTM_CHANNEL_COUNT];

// Pointers to FTM Instances
static FTM_Type*	ftmInstances[] = FTM_BASE_PTRS;

// FTMIRQn for NVIC Enabling
static const uint8_t 	ftmIrqs[] = FTM_IRQS;

// The one and only one look up table by Nico "El Rafa" Trozzo
static const pin_t		ftmChannelPins[FTM_INSTANCE_COUNT][FTM_CHANNEL_COUNT] = {
	//	Channel 0			Channel 1			Channel 2			Channel 3		Channel 4			Channel 5			Channel 6			Channel 7
	{ PORTNUM2PIN(PC,1),  PORTNUM2PIN(PA,4),  PORTNUM2PIN(PA,5), PORTNUM2PIN(PC,4), PORTNUM2PIN(PD,4), PORTNUM2PIN(PD,5), PORTNUM2PIN(PD,6)	, PORTNUM2PIN(PD,7)  },	// FTM0
	{ PORTNUM2PIN(PA,12), PORTNUM2PIN(PA,13), 0				   , 0				  , 0				 , 0			   	, 0				   	, 0				     }, // FTM1
	{ PORTNUM2PIN(PB,18), PORTNUM2PIN(PB,19), 0				   , 0				  , 0				 , 0			   	, 0				   	, 0				     }, // FTM2
	{ PORTNUM2PIN(PD,0),  PORTNUM2PIN(PD,1),  PORTNUM2PIN(PD,2), PORTNUM2PIN(PD,3), PORTNUM2PIN(PC,8), PORTNUM2PIN(PC,9), PORTNUM2PIN(PC,10), PORTNUM2PIN(PC,11) }  // FTM3
};

// FTM Channel Pin MUX Alternatives
static const uint8_t	ftmChannelAlts[FTM_INSTANCE_COUNT][FTM_CHANNEL_COUNT] = {
	// Ch0 Ch1 Ch2 Ch3 Ch4 Ch5 Ch6 Ch7
	{  4,  3,  3,  4,  4,  4,  4,  4  }, // FTM0
	{  3,  3,  0,  0,  0,  0,  0,  0  }, // FTM1
	{  3,  3,  0,  0,  0,  0,  0,  0  }, // FTM2
	{  4,  4,  4,  4,  3,  3,  3,  3  }  // FTM3
};

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void ftmInit(uint8_t instance, uint8_t prescaler, uint16_t module)
{
	// Clock gating enable
	switch(instance)
	{
		case FTM_INSTANCE_0:
			SIM->SCGC6 |= SIM_SCGC6_FTM0_MASK;
			break;
			
		case FTM_INSTANCE_1:
			SIM->SCGC6 |= SIM_SCGC6_FTM1_MASK;
			break;

		case FTM_INSTANCE_2:
			SIM->SCGC3 |= SIM_SCGC3_FTM2_MASK;
			SIM->SCGC6 |= SIM_SCGC6_FTM2_MASK;
			break;

		case FTM_INSTANCE_3:
			SIM->SCGC3 |= SIM_SCGC3_FTM3_MASK;
			break;

		default:
			break;
	}

	// Enable Interrupts on NVIC
	NVIC_EnableIRQ(ftmIrqs[instance]);

	// Enable CNTIN and MOD configuration
	ftmInstances[instance]->PWMLOAD = FTM_PWMLOAD_LDOK(1);

	// Free-running counter and prescaler
	ftmInstances[instance]->SC = FTM_SC_PS(prescaler);
	ftmInstances[instance]->CNTIN = 0;
	ftmInstances[instance]->CNT = 0;
	ftmInstances[instance]->MOD = module - 1;

	// Enable advanced mode
	ftmInstances[instance]->MODE |= FTM_MODE_FTMEN(1);
}

void ftmStart(uint8_t instance)
{
	ftmInstances[instance]->SC |= FTM_SC_CLKS(FTM_CLOCK_SYSTEM);
}

void ftmStop(uint8_t instance)
{
	ftmInstances[instance]->SC &= (~FTM_SC_CLKS_MASK);
}

void ftmOverflowSubscribe(uint8_t instance, void (*callback)(void))
{
	if (callback)
	{
		// Enables the timer overflow interrupt for the FTM instance
		ftmInstances[instance]->SC |= FTM_SC_TOIE(1);

		// Registers the callback to be called when timer overflow
		ftmOverflowCallbacks[instance] = callback;
	}
}

void ftmSetOutputValue(uint8_t instance, uint8_t channel, bool value)
{
	// Configures the initial value for the output channel
	uint32_t outputMask = 0x01 << channel;
	ftmInstances[instance]->OUTINIT = value ? (ftmInstances[instance]->OUTINIT | outputMask) : (ftmInstances[instance]->OUTINIT & (~outputMask));

	// Updates the output state of the selected channel
	ftmInstances[instance]->MODE |= FTM_MODE_INIT(1);
}

void ftmSetChannelCount(uint8_t instance, uint8_t channel, uint16_t count)
{
	ftmInstances[instance]->CONTROLS[channel].CnV = count;
}

uint16_t ftmGetChannelCount(uint8_t instance, uint8_t channel)
{
	return ftmInstances[instance]->CONTROLS[channel].CnV;
}

void ftmChannelSubscribe(uint8_t instance, uint8_t channel, void (*callback)(uint16_t))
{
	if (callback)
	{
		// Enables the channel interrupt
		ftmInstances[instance]->CONTROLS[channel].CnSC |= FTM_CnSC_CHIE(1);

		// Registers the callback to be called when channel match occurs
		ftmChannelCallbacks[instance][channel] = callback;
	}
}

void ftmInputCaptureInit(uint8_t instance, uint8_t channel, ftm_ic_mode_t mode)
{
	// Channel set to input capture on given edge/s
	ftmInstances[instance]->CONTROLS[channel].CnSC = FTM_CnSC_ELSA(mode == FTM_IC_RISING_EDGE ? 0 : 1) | FTM_CnSC_ELSB(mode == FTM_IC_FALLING_EDGE ? 0 : 1);
	
	// Pin MUX alternative
	setFtmChannelMux(instance, channel);
}

void ftmOutputCompareInit(uint8_t instance, uint8_t channel, ftm_oc_mode_t mode, uint16_t count)
{
	// Configuration of the channel as output compare
	ftmInstances[instance]->CONTROLS[channel].CnSC = FTM_CnSC_MSB(0) | FTM_CnSC_MSA(1) | FTM_CnSC_ELSB(mode == FTM_OC_TOGGLE ? 0 : 1) | FTM_CnSC_ELSA(mode == FTM_OC_CLEAR ? 0 : 1);
	ftmInstances[instance]->CONTROLS[channel].CnV = ftmInstances[instance]->CNT + count;

	// Pin MUX alternative
	setFtmChannelMux(instance, channel);

	// Enables the matching process on the selected channel
	ftmInstances[instance]->PWMLOAD |= (0x00000001 << channel);
}

void ftmPwmInit(uint8_t instance, uint8_t channel, ftm_pwm_mode_t mode, ftm_pwm_alignment_t alignment, uint16_t duty, uint16_t period)
{
	// Configure up or up/down counter 
	ftmInstances[instance]->SC |= FTM_SC_CPWMS(alignment == FTM_PWM_CENTER_ALIGNED ? 1 : 0);

	// Configure channel to PWM on the given mode and alignment
	ftmInstances[instance]->CONTROLS[channel].CnSC = FTM_CnSC_MSB(1) | FTM_CnSC_ELSB(1) | FTM_CnSC_ELSA(mode == FTM_PWM_LOW_PULSES ? 1 : 0);
	
	// Enable changes on MOD, CNTIN and CnV
	ftmInstances[instance]->PWMLOAD |= FTM_PWMLOAD_LDOK(1) | (0x00000001 << channel);

	// Configure PWM period and duty
	ftmInstances[instance]->CNTIN = 0;
	ftmInstances[instance]->MOD = period - 1;
	ftmInstances[instance]->CONTROLS[channel].CnV = duty;
	
	// Pin MUX alternative
	setFtmChannelMux(instance, channel);
}

void ftmPwmSetDuty(uint8_t instance, uint8_t channel, uint16_t duty)
{
	ftmInstances[instance]->CONTROLS[channel].CnV = duty - 1;
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
void FTM_IRQDispatch(uint8_t instance)
{
	// Verify if the interruption occurred because of the overflow
	// or because of a matching process in any of the timer channels
	if (ftmInstances[instance]->SC & FTM_SC_TOF_MASK)
	{
		// Clear the interruption flag
		ftmInstances[instance]->SC &= (~FTM_SC_TOF_MASK);
		
		// Calls the callback registered (if any)
		ov_callback overflowCallback = ftmOverflowCallbacks[instance];
		if (overflowCallback)
		{
			overflowCallback();
		}
	}
	else
	{
		for (uint8_t channel = 0; channel < FTM_CHANNEL_COUNT; channel++)
		{
			if (ftmInstances[instance]->CONTROLS[channel].CnSC & FTM_CnSC_CHF_MASK)
			{
				// Clear the interruption flag
				ftmInstances[instance]->CONTROLS[channel].CnSC &= (~FTM_CnSC_CHF_MASK);

				// Calls the callback registered (if any)
				ch_callback channelCallback = ftmChannelCallbacks[instance][channel];
				if (channelCallback)
				{
					channelCallback(ftmInstances[instance]->CONTROLS[channel].CnV);
				}
			}
		}
	}
}

__ISR__ FTM0_IRQHandler(void)
{
	FTM_IRQDispatch(FTM_INSTANCE_0);
}

__ISR__ FTM1_IRQHandler(void)
{
	FTM_IRQDispatch(FTM_INSTANCE_1);
}

__ISR__ FTM2_IRQHandler(void)
{
	FTM_IRQDispatch(FTM_INSTANCE_2);
}

__ISR__ FTM3_IRQHandler(void)
{
	FTM_IRQDispatch(FTM_INSTANCE_3);
}

void setFtmChannelMux(uint8_t instance, uint8_t channel)
{
	PORT_Type* 	ports[] = PORT_BASE_PTRS;
	pin_t 		pin = ftmChannelPins[instance][channel];
	uint8_t 	alt = ftmChannelAlts[instance][channel];
	ports[PIN2PORT(pin)]->PCR[PIN2NUM(pin)] |= PORT_PCR_MUX(alt);
}

/******************************************************************************/
