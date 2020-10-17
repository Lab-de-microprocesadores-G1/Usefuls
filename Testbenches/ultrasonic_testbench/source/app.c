/********************************************************************************
  @file     App.c
  @brief    Application functions
  @author   N. Magliola, G. Davidov, F. Farall, J. Gaytán, L. Kammann, N. Trozzo
 *******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "drivers/HAL/ultrasonic/ultrasonic.h"
#include "drivers/MCAL/ftm/ftm.h"
#include "board.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

// #define SOME_CONSTANT    20
// #define MACRO(x)         (x)

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

static void onSwitchPressed(void);
static void onMeasurement(double measurement);
static void onOverflow(void);

/*******************************************************************************
 * VARIABLES TYPES DEFINITIONS
 ******************************************************************************/

// typedef int  my_int_t;

/*******************************************************************************
 * PRIVATE VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

// static int myVar;

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/* Called once at the beginning of the program */
void appInit (void)
{
	// Default board initialization
	boardInit();

	// Initialization of the ultrasonic driver
	ultrasonicInit();
	ultrasonicSubscribe(onMeasurement);

	// Initializationg of FTM to be used as PWM
	ftmInit(FTM_INSTANCE_3, 5, 10000);
	ftmPwmInit(FTM_INSTANCE_3, FTM_CHANNEL_1, FTM_PWM_HIGH_PULSES, FTM_PWM_EDGE_ALIGNED, 100, 10000);
	ftmStart(FTM_INSTANCE_3);

	// Another FTM for continuous measurement
	ftmInit(FTM_INSTANCE_2, 7, 25000);
	ftmOverflowSubscribe(FTM_INSTANCE_2, onOverflow);
	ftmStart(FTM_INSTANCE_2);
}

/* Called repeatedly in an infinite loop */
static double 	distance;
static uint16_t duty;

void appRun (void)
{
	if (ultrasonicHasDistance())
	{
		distance = ultrasonicGetDistance();
		duty = distance * 9900 / 2500 + 100;
		ftmPwmSetDuty(FTM_INSTANCE_3, FTM_CHANNEL_1, duty);
	}
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

static void onOverflow(void)
{
	if (!ultrasonicBusy())
	{
		ultrasonicStart();
	}
}

static void onMeasurement(double measurement)
{
}

/*******************************************************************************
 ******************************************************************************/
