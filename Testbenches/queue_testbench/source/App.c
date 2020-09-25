/********************************************************************************
  @file     App.c
  @brief    Application functions
  @author   N. Magliola, G. Davidov, F. Farall, J. Gaytán, L. Kammann, N. Trozzo
 *******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "board/board.h"

#include "drivers/MCAL/gpio/gpio.h"
#include "drivers/HAL/led/led.h"

#include "lib/queue.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define MAX_COUNTER 	10

#define MAX_QUEUE_SIZE	15

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

static void pusher(void);
static void puller(void);

/*******************************************************************************
 * VARIABLES TYPES DEFINITIONS
 ******************************************************************************/

// 1 OF 3 STEPS USING THE QUEUE LIBRARY: Elements of the Queue
//
// Declare and define the elements that ONLY you know that will be used
// in the queue. Obviously,
typedef struct {
	uint8_t magicNumber;
} element_t;


/*******************************************************************************
 * PRIVATE VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/
static uint8_t counter = 0;

// 2 OF 3 STEPS USING THE QUEUE LIBRARY: Create the Queue buffer
//
// You must create an array of the size of elements wanted for the queue,
// this is because the micro-framework created only handles a queue and does not
// manages memory resources.
static element_t buffer[MAX_QUEUE_SIZE];

// 3A OF 3 STEPS USING THE QUEUE LIBRARY: Create the Queue
//
// Here, this will be done as two steps, but this is only because of the context
// in which I'm testing the micro-framework. Other cases would combine both parts
// of the 3rd step.
static queue_t queue;

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/* Called once at the beginning of the program */
void App_Init (void)
{
	// Default board initialization
	boardInit();

	// Initialization of led driver
	ledInit();

	// GPIO switches initialization
	gpioMode(PIN_SW2, INPUT | PULLUP);
	gpioMode(PIN_SW3, INPUT | PULLUP);

	// Registering interruption callbacks
	gpioIRQ(PIN_SW2, GPIO_IRQ_MODE_INTERRUPT_FALLING_EDGE, pusher);
	gpioIRQ(PIN_SW3, GPIO_IRQ_MODE_INTERRUPT_FALLING_EDGE, puller);

	// 3B OF 3 STEPS USING THE QUEUE LIBRARY: Create the Queue
	queue = createQueue(&buffer, MAX_QUEUE_SIZE, sizeof(element_t));
}

/* Called repeatedly in an infinite loop */
void App_Run (void)
{
    // Things to do in an infinite loop.
	ledTurn(LED_RED, isEmpty(&queue));
}


/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

static void pusher(void)
{
	// Generate next number, dummy
	counter = (counter + 1) % MAX_COUNTER;

	// Create the new element to be pushed
	element_t element = { .magicNumber = counter };

	// Pushing a new element
	size_t currentSize;
	bool success = push(&queue, &element);
	if (success)
	{
		// Could push the element!
		currentSize = size(&queue);
	}
	else
	{
		// Oops... couldn't
		currentSize = size(&queue);
		ledTurn(LED_BLUE, ON);
	}
}

static void puller(void)
{
	// I'm verifying there are enough elements to pop, so I can
	// directly pop the next element
	element_t nextElement;
	if (size(&queue))
	{
		// Pop the next element
		nextElement = *(element_t*)pop(&queue);
	}

	// Some notification
	ledTurn(LED_BLUE, OFF);
}


/*******************************************************************************
 ******************************************************************************/
