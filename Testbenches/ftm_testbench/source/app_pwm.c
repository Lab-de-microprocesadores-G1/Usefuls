/********************************************************************************
  @file     App.c
  @brief    Application functions
  @author   N. Magliola, G. Davidov, F. Farall, J. Gaytán, L. Kammann, N. Trozzo
 *******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "drivers/MCAL/ftm/ftm.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define PWM_DUTY_UPDATE		31250

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

void changeDuty(uint16_t count);
void changeDutyVoid(void);

/*******************************************************************************
 * VARIABLES TYPES DEFINITIONS
 ******************************************************************************/
// typedef int my_int;


/*******************************************************************************
 * PRIVATE VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static uint16_t duty = 10;
static bool 	upCounting = true;

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/* Called once at the beginning of the program */
void App_PWM_Init (void)
{
	// Init FTM for tick 160ns
	ftmInit(FTM_INSTANCE_0, 3, 0xFFFF);

	// Config PWM for duty 1.6us, period 3.2us
	ftmPwmInit(FTM_INSTANCE_0, FTM_CHANNEL_0, FTM_PWM_EDGE_ALIGNED, FTM_PWM_HIGH_PULSES, duty, 20);

	// Subscribe to channel interrupt
	ftmChannelSubscribe(FTM_INSTANCE_0, FTM_CHANNEL_0, changeDuty);

	// Start FTM
	ftmStart(FTM_INSTANCE_0);

}

/* Called repeatedly in an infinit loop */
void App_PWM_Run (void)
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
			ftmPwmSetDuty(FTM_INSTANCE_0, FTM_CHANNEL_0, duty);
			tickCount = PWM_DUTY_UPDATE;
		}
	}
}

void changeDutyVoid(void)
{
	changeDuty(0);
}
/*******************************************************************************
 ******************************************************************************/