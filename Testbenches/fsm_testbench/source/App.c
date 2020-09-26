/********************************************************************************
  @file     App.c
  @brief    Application functions
  @author   N. Magliola, G. Davidov, F. Farall, J. Gaytán, L. Kammann, N. Trozzo
 *******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "app/io.h"

#include "lib/fsm/fsm.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

// Here I'm defining the action routine of each transition done by the
// state machine declared later...
void turnRed(void* event);
void turnGreen(void* event);
void turnBlue(void* event);
void nothing(void* event);

/*******************************************************************************
 * PRIVATE VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static state_t currentState;

/*******************************************************************************
 * VARIABLES TYPES DEFINITIONS
 ******************************************************************************/

// 1 OF 4 USING FSM: Create the event type
//
// The micro-framework used to handle finite state machines does not
// care whether you define your events one way or another, but,
// you need to define how this will work for the following steps...

// ¡Events were defined by the hardware layer in io.h!

// 2 OF 4 USING FSM: Create the states
//
// Assuming you already used your head to figure out how the state machine
// works, you came here to see how to implement it...
extern edge_t STATE_RED[];
extern edge_t STATE_GREEN[];
extern edge_t STATE_BLUE[];

edge_t STATE_RED[] = {
		//	EVENT						NEXT STATE		ACTION
		{	IO_EVENT_LEFT_KEY_PRESSED, 	STATE_GREEN, 	turnGreen},
		DEFAULT_EDGE(STATE_RED, nothing)
};

edge_t STATE_GREEN[] = {
		//	EVENT						NEXT STATE		ACTION
		{	IO_EVENT_LEFT_KEY_PRESSED, 	STATE_BLUE, 	turnBlue},
		{	IO_EVENT_RIGHT_KEY_PRESSED, STATE_RED, 		turnRed},
		DEFAULT_EDGE(STATE_GREEN, nothing)
};

edge_t STATE_BLUE[] = {
		//	EVENT						NEXT STATE		ACTION
		{	IO_EVENT_RIGHT_KEY_PRESSED, STATE_GREEN, 	turnGreen},
		DEFAULT_EDGE(STATE_BLUE, nothing)
};

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/* Called once at the beginning of the program */
void App_Init (void)
{
	// Initialize io device
	ioInit();

	// 3 OF 4 STEPS USING FSM: Create instance of state and initialize it
	currentState = STATE_RED;
}

/* Called repeatedly in an infinite loop */
void App_Run (void)
{
	// Get next event
	io_event_t event = ioGetNextEvent();

	if (event != IO_EVENT_NONE)
	{
		// 4 OF 4 STEPS USING FSM: Cycle the state machine
		//
		// Cycle the state machine using events got from somewhere...
		fsmCycle(&currentState, &event);
	}
}


/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void turnRed(void* event)
{
	ioLed(IO_LED_RED, true);
	ioLed(IO_LED_GREEN, false);
	ioLed(IO_LED_BLUE, false);
}

void turnGreen(void* event)
{
	ioLed(IO_LED_RED, false);
	ioLed(IO_LED_GREEN, true);
	ioLed(IO_LED_BLUE, false);
}

void turnBlue(void* event)
{
	ioLed(IO_LED_RED, false);
	ioLed(IO_LED_GREEN, false);
	ioLed(IO_LED_BLUE, true);
}

void nothing(void* event)
{

}

/*******************************************************************************
 ******************************************************************************/
