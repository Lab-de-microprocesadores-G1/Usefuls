/********************************************************************************
  @file     App.c
  @brief    Application functions
  @author   N. Magliola, G. Davidov, F. Farall, J. Gaytán, L. Kammann, N. Trozzo
 *******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "board/board.h"

#include "lib/event_queue/event_queue.h"
#include "lib/queue/queue.h"

#include "drivers/MCAL/gpio/gpio.h"
#include "drivers/HAL/timer/timer.h"
#include "drivers/HAL/led/led.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define MAX_QUEUE_SIZE		10

/*******************************************************************************
 * VARIABLES TYPES DEFINITIONS
 ******************************************************************************/

// 1 OF 6 STEPS USING EVENT QUEUE: Create the Event type
//
// First, you must create your own type of event, for example
// here I only assign an id to each event emitted, but you could add
// additional information.
typedef enum {
	KEY_LEFT,
	KEY_RIGHT,
	TIMEOUT
} event_id_t;

typedef struct {
	event_id_t id;
} event_t;

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

void onSW2Pressed(void);
void onSW3Pressed(void);
void onTimer(void);

// 4 OF 6 STEPS USING EVENT QUEUE: Create your event generators
//
// Somewhere... maybe you have event generators coming from other files.
void* hardwareEventGenerator(void);

/*******************************************************************************
 * PRIVATE VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

// 2 OF 6 STEPS USING EVENT QUEUE: Create the buffer for the queue
//
// The micro-framework provided does not manage memory, it only handles
// the queue. So you must create an array of size desired. Remember,
// this size that will be given to the handler is not the same as the effective
// maximum size used, because one element is used to control the size as a
// circular buffer.
static event_t buffer[MAX_QUEUE_SIZE];

// 3A OF 6 STEPS USING EVENT QUEUE: Create the Event Queue
//
// Here it is done in two steps, could be collapsed into one.
static event_queue_t eventQueue;

// Creating an internal queue to collect hardware events and
// make an interface with the event queue...
static event_t hardwareBuffer[MAX_QUEUE_SIZE];
static queue_t hardwareQueue;

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/* Called once at the beginning of the program */
void App_Init (void)
{
	// Board default initialization
	boardInit();

	// Timer initialization
	timerInit();

	// Led initialization
	ledInit();
	// Turn off all leds
	ledTurn(LED_RED, OFF);
	ledTurn(LED_GREEN, OFF);
	ledTurn(LED_BLUE, OFF);

	// GPIO switches initialization
	gpioMode(PIN_SW2, INPUT | PULLUP);
	gpioMode(PIN_SW3, INPUT | PULLUP);

	// Registering interruption callbacks of Timer
	timerStart(timerGetId(), TIMER_MS2TICKS(5000), TIM_MODE_PERIODIC, onTimer);

	// Registering interruption callbacks of GPIO
	gpioIRQ(PIN_SW2, GPIO_IRQ_MODE_INTERRUPT_FALLING_EDGE, onSW2Pressed);
	gpioIRQ(PIN_SW3, GPIO_IRQ_MODE_INTERRUPT_FALLING_EDGE, onSW3Pressed);

	// Instancing the internal queue for hardware
	hardwareQueue = createQueue(&hardwareBuffer, MAX_QUEUE_SIZE, sizeof(event_t));

	// 3B OF 6 STEPS USING EVENT QUEUE: Create the Event Queue
	//
	// Here it is done in two steps, could be collapsed into one.
	eventQueue = createEventQueue(&buffer, MAX_QUEUE_SIZE, sizeof(event_t));

	// 5 OF 6 STEPS USING EVENT QUEUE: Register the event generators
	//
	// Each event generator must be registered, and if you want to be capable
	// of enabling or disabling each generator, then save the id assigned in the queue.
	generator_id_t id = registerEventGenerator(&eventQueue, hardwareEventGenerator);
}

/* Called repeatedly in an infinite loop */
void App_Run (void)
{
	// Some basic functionality...
	static led_id_t currentLed = LED_RED;
	static led_mode_t currentMode = STATIC;

	// 6 OF 6 STEPS USING EVENT QUEUE: Loop through the events
	//
	// Here you can loop each event, remember always to cast, and verify
	// if there is a new event.
	event_t* event = getNextEvent(&eventQueue);
	if (event)
	{
		// Turn off all leds
		ledTurn(LED_RED, OFF);
		ledTurn(LED_GREEN, OFF);
		ledTurn(LED_BLUE, OFF);

		switch (event->id)
		{
			case KEY_LEFT:
				switch (currentMode)
				{
					case STATIC:
						currentMode = BLINK;
						break;
					case BLINK:
						currentMode = BURST;
						break;
					case BURST:
						currentMode = STATIC;
						break;
					default:
						currentMode = STATIC;
						break;
				}
				break;

			case KEY_RIGHT:
				switch (currentMode)
				{
					case BURST:
						currentMode = BLINK;
						break;
					case STATIC:
						currentMode = BURST;
						break;
					case BLINK:
						currentMode = STATIC;
						break;
					default:
						currentMode = STATIC;
						break;
				}
				break;

			case TIMEOUT:
				// Change the current led
				switch (currentLed)
				{
					case LED_RED:
						currentLed = LED_GREEN;
						break;
					case LED_GREEN:
						currentLed = LED_BLUE;
						break;
					case LED_BLUE:
						currentLed = LED_RED;
						break;
					default:
						currentLed = LED_RED;
						break;
				}
				break;
		}

		// Update state of the led
		switch (currentMode)
		{
			case STATIC:
				ledTurn(currentLed, ON);
				break;

			case BLINK:
				ledBlink(currentLed, 300);
				break;

			case BURST:
				ledBurst(currentLed, 1000, 100, 3);
				break;
		}
	}
}


/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void onSW2Pressed(void)
{
	event_t event = { KEY_RIGHT };

	push(&hardwareQueue, &event);
}

void onSW3Pressed(void)
{
	event_t event = { KEY_LEFT };

	push(&hardwareQueue, &event);
}

void onTimer(void)
{
	event_t event = { TIMEOUT };

	push(&hardwareQueue, &event);
}

void* hardwareEventGenerator(void)
{
	return pop(&hardwareQueue);
}

/*******************************************************************************
 ******************************************************************************/
