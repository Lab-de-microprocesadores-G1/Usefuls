/*******************************************************************************
  @file     io.c
  @brief    I/O Abstraction Layer for the Application Layer
  @author   G. Davidov, F. Farall, J. Gaytán, L. Kammann, N. Trozzo
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "board/board.h"

#include "drivers/HAL/button/button.h"
#include "drivers/HAL/timer/timer.h"
#include "drivers/HAL/led/led.h"

#include "lib/event_queue/event_queue.h"
#include "lib/queue/queue.h"

#include "io.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define MAX_QUEUE_SIZE	100
#define TIMEOUT_MS		100
#define TIMEOUT_TICKS	TIMER_MS2TICKS(TIMEOUT_MS)

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/

static io_event_t eventBuffer[MAX_QUEUE_SIZE];
static event_queue_t eventQueue;

static io_event_t hardwareBuffer[MAX_QUEUE_SIZE];
static queue_t hardwareQueue;

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

// Hardware event generator
void* hardwareEventGenerator(void);

// Hardware event callback
void buttonLeftPressed(void);
void buttonLeftReleased(void);

void buttonRightPressed(void);
void buttonRightReleased(void);

void timerFired(void);

/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/


/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/


/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void ioLed(io_led_t led, bool status)
{
	ledTurn((led_id_t)led, status ? ON : OFF);
}

io_event_t ioGetNextEvent(void)
{
	io_event_t event = { IO_EVENT_NONE };

	void* nextEvent = getNextEvent(&eventQueue);
	if (nextEvent)
	{
		event = *(io_event_t*)nextEvent;
	}

	return event;
}

void ioInit(void)
{
	// Default initialization of the board
	boardInit();

	// Initialization of drivers
	buttonInit();
	timerInit();
	ledInit();

	// Initialization of internal queue for hardware events
	hardwareQueue = createQueue(&hardwareBuffer, MAX_QUEUE_SIZE, sizeof(io_event_t));

	// Initialization of the event queue
	eventQueue = createEventQueue(&eventBuffer, MAX_QUEUE_SIZE, sizeof(io_event_t));

	// Registering the hardware queue as event generator of the event queue
	registerEventGenerator(&eventQueue, hardwareEventGenerator);

	// Setting up the TIMER and its callback
	// timerStart(timerGetId(), TIMEOUT_TICKS, TIM_MODE_PERIODIC, timerFired);

	// Setting up the BUTTONS and its callback
	buttonSubscribe(BUTTON_LEFT, BUTTON_PRESS, buttonLeftPressed);
	buttonSubscribe(BUTTON_LEFT, BUTTON_RELEASE, buttonLeftReleased);
	buttonSubscribe(BUTTON_RIGHT, BUTTON_PRESS, buttonRightPressed);
	buttonSubscribe(BUTTON_RIGHT, BUTTON_RELEASE, buttonRightReleased);
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void* hardwareEventGenerator(void)
{
	return pop(&hardwareQueue);
}

void buttonLeftPressed(void)
{
	io_event_t event = { IO_EVENT_LEFT_KEY_PRESSED, 10 };
	push(&hardwareQueue, &event);
}

void buttonLeftReleased(void)
{
	io_event_t event = { IO_EVENT_LEFT_KEY_RELEASED, 20 };
	push(&hardwareQueue, &event);
}

void buttonRightPressed(void)
{
	io_event_t event = { IO_EVENT_RIGHT_KEY_PRESSED, 30 };
	push(&hardwareQueue, &event);
}

void buttonRightReleased(void)
{
	io_event_t event = { IO_EVENT_RIGHT_KEY_RELEASED, 40 };
	push(&hardwareQueue, &event);
}

void timerFired(void)
{
	io_event_t event = { IO_EVENT_TIMEOUT };
	push(&hardwareQueue, &event);
}

/******************************************************************************/
