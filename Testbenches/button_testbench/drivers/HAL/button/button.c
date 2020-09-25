/***************************************************************************//**
  @file     button.c
  @brief    Buttons driver
  @author   G. Davidov, F. Farall, J. Gaytán, L. Kammann, N. Trozzo
 ******************************************************************************/


/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "../../MCAL/systick/systick.h"
#include "../../MCAL/gpio/gpio.h"
#include "../../../board/board.h"
#include "../../../lib/fsm/fsm.h"
#include "../../../lib/event_queue/event_queue.h"
#include "button.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define	SYSTICK_MS				( 1.0 / SYSTICK_ISR_FREQUENCY_HZ * 1000.0 )
#define MS2TICKS(ms)  			( (ms) / SYSTICK_MS )

#define	LKP_MS					3000
#define TYPEMATIC_MS			500 // 500ms for debug

#define	BUTTON_DEVELOPMENT_MODE 1

#define EVENT_QUEUE_SIZE		10

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

// FSM Event IDs
enum {
	NO_EVENT,
	PRESS,
	RELEASE,
	TIMEOUT
};

typedef struct button button_t;

// FSM Event Structure
typedef struct {
	event_id_t 		id;			// Event ID
	button_t*		button;		// Button instance
} button_fsm_event_t;

// Button structure
struct button {
	// Button static properties
	const pin_t 	pinNumber;			// Board pin assigned to button
	const bool 		activeState;		// Whether the button is active high or low
	const bool 		pullEnable;			// Enable/Disable pullup/down
	const uint32_t 	debounceMillis;		// Milliseconds to wait for debounce

	// External event emitters, call this functions on each event
	void 			(*evCallbacks[BUTTON_EV_COUNT]) (void);

	// FSM and event handling
	state_t 		fsmState;
	event_queue_t*  eventQueue;

	// Internal variables
	uint32_t 		debounceTicks;
	uint32_t 		timeoutCount;
};

/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

static void buttonCycle(void);

// Event Generators declaration
event_generator_t buttonReader;
event_generator_t buttonTimer;

// FSM Action Routines declaration
static action_t buttonPressed;
static action_t typematicInit;
static action_t buttonReleased;
static action_t typematicTimeout;
static action_t doNothing;

/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/


/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/


static button_t buttons[BUTTON_COUNT] = {
	//	  Pin	  	ActiveState PE		MsBounce
		{ PIN_SW2,	SW2_ACTIVE,	true,	100 	},
		{ PIN_SW3,	SW3_ACTIVE, true,	100 	}
};

// Current button index
static uint8_t i;			// Used by event generators to know which button is being handled

// FSM States
// Global because static variables can only be initialized with const values
// https://stackoverflow.com/questions/3025050/error-initializer-element-is-not-constant-when-trying-to-initialize-variable-w
extern state_t released;
extern state_t pressed;
extern state_t typematic;


state_t released = {
		{ PRESS, pressed, buttonPressed },
		  DEFAULT_EDGE(released, doNothing)
};

state_t pressed = {
		{ TIMEOUT, typematic, typematicInit },
		{ RELEASE, released, buttonReleased },
		  DEFAULT_EDGE(pressed, doNothing)
};

state_t typematic = {
		{ TIMEOUT, typematic, typematicTimeout },
		{ RELEASE, released, buttonReleased },
		  DEFAULT_EDGE(typematic, doNothing)
};


/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void buttonInit(void)
{
	// Configure periodic interrupts
	SysTick_Init(buttonCycle);

	// Reserve memory for each eventQueue
	static button_fsm_event_t queueBuffers[BUTTON_COUNT][EVENT_QUEUE_SIZE + 1];

	// Construct each button instance
	uint8_t i;
	uint32_t pull = 0;
	for( i = 0 ; i < BUTTON_COUNT ; i++)
	{
		// Initial FSM state
		buttons[i].fsmState = released;

		// Create event queue
		buttons[i].eventQueue = createEventQueue(
				(void *)(queueBuffers[i]),
				EVENT_QUEUE_SIZE + 1,
				sizeof(button_fsm_event_t)
			);

		// Register event generators
		registerEventGenerator(&(buttons[i].eventQueue), buttonReader);
		registerEventGenerator(&(buttons[i].eventQueue), buttonTimer);

		// Check if pullup/down was requested and configure pins for GPIO input
		if (buttons[i].pullEnable)
		{
			pull = buttons[i].activeState ? PULLDOWN : PULLUP;
		}
		gpioMode(buttons[i].pinNumber, INPUT | pull );
		pull = 0;
	}
}

void buttonSubscribe(button_id_t id, button_events_t ev, void (*callback) (void))
{
#ifdef BUTTON_DEVELOPMENT_MODE
	if ((id < BUTTON_COUNT) && (ev < BUTTON_EV_COUNT))
#endif // BUTTON_DEVELOPMENT_MODE
	{
		buttons[id].evCallbacks[ev] = callback;
	}
}

bool isButtonPressed(button_id_t id)
{
	bool ret = false;
#ifdef BUTTON_DEVELOPMENT_MODE
	if (id < BUTTON_COUNT)
#endif // BUTTON_DEVELOPMENT_MODE
	{
		ret = (gpioRead(buttons[id].pinNumber) == buttons[id].activeState);
	}
	return ret;
}
/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void buttonCycle(void)
{
	button_fsm_event_t* ev;
	button_fsm_event_t  evCopy;

	for( i = 0 ; i < BUTTON_COUNT ; i++)
	{
		ev = getNextEvent(buttons[i].eventQueue);
		if(ev != NO_EVENTS)
		{
			// Copy the event! No persistence guaranteed
			evCopy = *(button_fsm_event_t *)(ev);

			// Cycle State Machine
			fsmCycle( &(buttons[i].fsmState), evCopy);
		}
	}
}


// Event Generators definition

void * buttonReader(void)
{
	// Internal use variables
	static bool active;
	static state_t	currentState;

	static button_fsm_event	 newEvent;
	static button_fsm_event* evPointer;

	// No event by default
	evPointer = NO_EVENTS;

	// If we are not waiting for debounce
	if( buttons[i].debounceTicks == 0 )
	{
		// Check whether the button is being pressed or not
		active = (gpioRead(buttons[i].pinNumber) == buttons[i].activeState);
		currentState = buttons[i].fsmState;

		// If there was a change on the button state, emit respective event
		if ( active &&  (currentState == released) )
		{
			newEvent.id = PRESS;
			newEvent.button = &(buttons[i]);
			evPointer = &newEvent;
		}
		else if ( !active && (currentState != released) )
		{
			newEvent.id = RELEASE;
			newEvent.button = &(buttons[i]);
			evPointer = &newEvent;
		}

		// Configure debounce if button pressed or released
		if (evPointer != NO_EVENTS)
		{
			buttons[i].debounceTicks = MS2TICKS(buttons[i].debounceMilis);
		}
	}
	else
	{
		buttons[i].debounceTicks--;
	}

	return evPointer;
}

void * buttonTimer(void)
{
	// Internal use variables
	static button_fsm_event		newEvent;
	static button_fsm_event*	evPointer;

	evPointer = NO_EVENTS;

	if (buttons[i].timeoutCount > 0)
	{
		if(--buttons[i].timeoutCount == 0)
		{
			newEvent.id = TIMEOUT;
			newEvent.button = &(buttons[i]);
			evPointer = &newEvent;
		}
	}

	return evPointer;
}


// FSM Action Routines definition

static void buttonPressed(void* event)
{
	button_t * button = ((button_fsm_event_t *)event)->button;
	button->timeoutCount = MS2TICKS(LKP_MS);
	(button->evCallbacks[BUTTON_PRESS])();
}

static void typematicInit(void* event)
{
	button_t * button = ((button_fsm_event_t *)event)->button;
	button->timeoutCount = MS2TICKS(TYPEMATIC_MS);
	(button->evCallbacks[BUTTON_LKP])();
}

static void buttonReleased(void* event)
{
	button_t * button = ((button_fsm_event_t *)event)->button;
	button->timeoutCount = 0;
	(button->evCallbacks[BUTTON_RELEASE])();
}

static void typematicTimeout(void* event)
{
	button_t * button = ((button_fsm_event_t *)event)->button;
	button->timeoutCount = MS2TICKS(TYPEMATIC_MS);
	(button->evCallbacks[BUTTON_TYPEMATIC])();

}

static void doNothing(void* event) {}

/******************************************************************************/
