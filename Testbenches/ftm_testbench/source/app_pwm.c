/********************************************************************************
  @file     App.c
  @brief    Application functions
  @author   N. Magliola, G. Davidov, F. Farall, J. Gaytán, L. Kammann, N. Trozzo
 *******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "drivers/MCAL/ftm/ftm.h"
#include "drivers/MCAL/gpio/gpio.h"
#include "board/board.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define PWM_DUTY_UPDATE		7812
#define FTM_INSTANCE		FTM_INSTANCE_3
#define FTM_CHANNEL			FTM_CHANNEL_5

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

void changeDuty(uint16_t count);

/*******************************************************************************
 * VARIABLES TYPES DEFINITIONS
 ******************************************************************************/
// typedef int my_int;


/*******************************************************************************
 * PRIVATE VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static uint16_t duty = 10;
static bool 	upCounting = false;

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/* Called once at the beginning of the program */
void appInitPWM (void)
{
	// Init FTM for tick 160ns
	ftmInit(FTM_INSTANCE, 3, 0xFFFF);

	// Config PWM for duty 1.6us, period 3.2us
	ftmPwmInit(FTM_INSTANCE, FTM_CHANNEL, FTM_PWM_EDGE_ALIGNED, FTM_PWM_HIGH_PULSES, duty, 20);

	// Subscribe to channel interrupt
	ftmChannelSubscribe(FTM_INSTANCE, FTM_CHANNEL, changeDuty);

	// Start FTM
	ftmStart(FTM_INSTANCE);

	gpioMode(PIN_PWM_TRIGGER, OUTPUT);
	gpioWrite(PIN_PWM_TRIGGER, LOW);
}

/* Called repeatedly in an infinit loop */
void appRunPWM (void)
{
    // Things to do in an infinit loop.
}


/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void changeDuty(uint16_t count)
{
	static uint32_t tickCount = PWM_DUTY_UPDATE;	// 100ms
	if (tickCount > 0)
	{
		if (--tickCount == 0)
		{
			duty += upCounting ? 1 : -1;
			if (duty == 18)
			{
				upCounting = false;
			}
			else if (duty == 2)
			{
				upCounting = true;
			}
			gpioToggle(PIN_PWM_TRIGGER);
			ftmPwmSetDuty(FTM_INSTANCE, FTM_CHANNEL, duty);
			tickCount = PWM_DUTY_UPDATE;
		}
	}
}

/*******************************************************************************
 ******************************************************************************/
