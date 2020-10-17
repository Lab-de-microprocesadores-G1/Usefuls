/*******************************************************************************
  @file     ultrasonic.c
  @brief    Ultrasonic HC-SR04 Module Driver
  @author   G. Davidov, F. Farall, J. Gaytán, L. Kammann, N. Trozzo
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "board.h"

#include "ultrasonic.h"

#include "drivers/MCAL/gpio/gpio.h"
#include "drivers/MCAL/ftm/ftm.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

// General setting parameters of the ultrasonic driver
#define ULTRASONIC_FTM_PRESCALER	5
#define ULTRASONIC_FTM_MODULE		0xFFFF
#define ULTRASONIC_TRIGGER_DURATION	16

// Assign the FlexTimer module instances and channels for each
// of the pins used to control the ultrasonic module
#define	ECHO_FTM_INSTANCE			FTM_INSTANCE_0
#define ECHO_FTM_CHANNEL			FTM_CHANNEL_0
#define ECHO_PIN					PIN_ECHO

#define TRIGGER_FTM_INSTANCE		FTM_INSTANCE_0
#define TRIGGER_FTM_CHANNEL			FTM_CHANNEL_2
#define TRIGGER_PIN					PIN_TRIGGER

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

static void onEchoInputCapture(uint16_t count);

/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static us_callback 	finishedCallback = NULL;		// Callback called when finishes a measurement process
static bool			processActive = false;			// Whether the measurement process was started and is running
static bool			hasDistance	= false;			// Flag indicating if a distance was calculated recently
static double		currentDistance;				// Buffer of the last distance calculated by the driver

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void ultrasonicInit(void)
{
	static bool init = false;
	if (!init)
	{
		init = true;

		// Initialize the Echo FlexTimer Instance
		ftmInit(ECHO_FTM_INSTANCE, ULTRASONIC_FTM_PRESCALER, ULTRASONIC_FTM_MODULE);

		// Configures the Echo channel as input capture and registers the callback
		// where the timing and distance calculation will be done
		ftmInputCaptureInit(ECHO_FTM_INSTANCE, ECHO_FTM_CHANNEL, FTM_IC_BOTH_EDGES);
		ftmChannelSubscribe(ECHO_FTM_INSTANCE, ECHO_FTM_CHANNEL, onEchoInputCapture);

		// Initialize the Trigger FlexTimer Instance
		ftmInit(TRIGGER_FTM_INSTANCE, ULTRASONIC_FTM_PRESCALER, ULTRASONIC_FTM_MODULE);

		// Configures the Trigger channel as output compare
		ftmOutputCompareInit(TRIGGER_FTM_INSTANCE, TRIGGER_FTM_CHANNEL, FTM_OC_CLEAR, true);
		ftmOutputCompareStop(TRIGGER_FTM_INSTANCE, TRIGGER_FTM_CHANNEL);

		// Starts running both FlexTimer instances
		ftmStart(TRIGGER_FTM_INSTANCE);
		ftmStart(ECHO_FTM_INSTANCE);
	}
}

bool ultrasonicStart(void)
{
	bool result = false;
	if (!processActive)
	{
		ftmOutputCompareStart(TRIGGER_FTM_INSTANCE, TRIGGER_FTM_CHANNEL, ULTRASONIC_TRIGGER_DURATION);
		processActive = true;
		hasDistance = false;
		result = true;
	}
	return result;
}

bool ultrasonicBusy(void)
{
	return processActive;
}

bool ultrasonicHasDistance(void)
{
	bool result = hasDistance;
	if (hasDistance)
	{
		hasDistance = false;
	}
	return result;
}

double ultrasonicGetDistance(void)
{
	return currentDistance;
}

void ultrasonicSubscribe(us_callback callback)
{
	finishedCallback = callback;
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

static void onEchoInputCapture(uint16_t count)
{
	static uint16_t countBuffer;
	if (gpioRead(ECHO_PIN) == LOW)
	{
		if (count >= countBuffer)
		{
			currentDistance = count - countBuffer;
		}
		else
		{
			currentDistance = count + (ULTRASONIC_FTM_MODULE - countBuffer + 1);
		}
		currentDistance = (currentDistance * 640) / (100 * 58);
		hasDistance = true;
		processActive = false;
		if (finishedCallback)
		{
			finishedCallback(currentDistance);
		}
	}
	else
	{
		countBuffer = count;
	}
}

/*******************************************************************************
 *******************************************************************************
						            INTERRUPT SERVICE ROUTINES
 *******************************************************************************
 ******************************************************************************/

/******************************************************************************/
