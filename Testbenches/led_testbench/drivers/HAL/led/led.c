/*******************************************************************************
  @file     led.c
  @brief    LED Driver
  @author   G. Davidov, F. Farall, J. Gaytán, L. Kammann, N. Trozzo
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "led.h"

#include "drivers/MCAL/gpio/gpio.h"
#include "drivers/MCAL/systick/SysTick.h"

#include "board/board.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define SYSTICK_MS		(1 / SYSTICK_ISR_FREQUENCY_HZ * 1000)
#define M2TICKS(ms)		( (ms) / SYSTICK_MS )

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef struct {
	ttick_t		period;		// Period of the blinking mode
	ttick_t		cnt;		// Counter of the period
} led_blink_t;

typedef struct {
	ttick_t		burstPeriod;		// Period of the burst mode
	ttick_t		blinkPeriod;		// Period of the blink mode
	uint8_t		amount;				// Amount of blinks in the burst period
	ttick_t		burstCounter;		// Counter of the period of the burst
	ttick_t		blinkCounter;		// Counter of the period of each blink
	uint8_t		amountCounter;		// Counter of number of blinks done
} led_burst_t;

typedef union {
	led_blink_t		blink;	// The Blinking mode data in the instance
	led_burst_t		burst;	// The Burst mode data in the instance
} led_mode_data_t;

typedef struct {
	pin_t			pin;			// The pin assigned to the LED
	bool			enabled;		// Disable used to stop interruptions during change of setting
	bool			activeState;	// Logic active state of the LED
	led_mode_t 		currentMode;	// Current mode of operation
	led_mode_data_t settings;		// Settings of the current mode of operation
} led_t;

/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/



/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

static void ledIsr(void);
static void ledDisable(led_id_t id);

/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/



/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static led_t leds[AMOUNT_OF_LEDS] = {
		{PIN_LED_RED, LED_ACTIVE},
		{PIN_LED_BLUE, LED_ACTIVE}
};

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void ledInit(void)
{
	// Initializing periodic ISR...
	SysTick_Init(ledIsr);

	// Initializing leds...
	uint8_t index;
	for(index = 0 ; index < AMOUNT_OF_LEDS ; index++)
	{
		// Setting initial state of each LED
		leds[index].currentMode = STATIC;

		// Configuring the pin and port used by the LED
		gpioWrite(leds[index].pin, !leds[index].activeState);
		gpioMode(leds[index].pin, OUTPUT);
	}
}

void ledToggle(led_id_t id)
{
#ifdef LED_DEVELOPMENT_MODE
	if (id >= 0 && id < AMOUNT_OF_LEDS)
#endif
	{
		led_t *led = &leds[id];
		if (led->currentMode == STATIC)
		{
			gpioToggle(led->pin);
		}
	}
#ifdef LED_DEVELOPMENT_MODE
	else
	{
		// Raise some error!
	}
#endif
}

void ledTurn(led_id_t id, led_status_t status)
{
#ifdef LED_DEVELOPMENT_MODE
	if (id >= 0 && id < AMOUNT_OF_LEDS)
#endif
	{
		// Disable current configurations of the led
		ledDisable(id);

		// Change to STATIC mode of operation
		led_t *led = &leds[id];
		led->enabled = false;
		led->currentMode = STATIC;
		gpioWrite(led->pin, status == ON ? led->activeState : led->activeState);
		led->enabled = true;
	}
#ifdef LED_DEVELOPMENT_MODE
	else
	{
		// Raise some error!
	}
#endif
}

void ledBlink(led_id_t id, ttick_t period)
{
#ifdef LED_DEVELOPMENT_MODE
	if (id >= 0 && id < AMOUNT_OF_LEDS)
#endif
	{
		// Disable current configurations of the led
		ledDisable(id);

		// Change to BLINK mode of operation
		led_t *led = &leds[id];
		led->enabled = false;
		led->currentMode = BLINK;
		led->settings.blink.period = period;
		led->settings.blink.cnt = period / 2;
		led->enabled = true;
	}
#ifdef LED_DEVELOPMENT_MODE
	else
	{
		// Raise some error!
	}
#endif
}

void ledBurst(led_id_t id, ttick_t burstPeriod, ttick_t blinkPeriod, uint8_t burst)
{
#ifdef LED_DEVELOPMENT_MODE
	if (id >= 0 && id < AMOUNT_OF_LEDS)
#endif
	{
		// Disable current configurations of the led
		ledDisable(id);

		// Change to BURST mode of operation
		led_t *led = &leds[id];
		led->enabled = false;
		led->currentMode = BURST;

		led->settings.burst.burstPeriod = burstPeriod;
		led->settings.burst.blinkPeriod = blinkPeriod;
		led->settings.burst.amount = burst;

		led->settings.burst.burstCounter = burstPeriod / 2;
		led->settings.burst.blinkCounter = blinkPeriod / 2;
		led->settings.burst.amountCounter = burst * 2;

		gpioWrite(led->pin, led->activeState);
		led->enabled = true;
	}
#ifdef LED_DEVELOPMENT_MODE
	else
	{
		// Raise some error!
	}
#endif
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

static void ledDisable(led_id_t id)
{

#ifdef LED_DEVELOPMENT_MODE
	if (id >= 0 && id < AMOUNT_OF_LEDS)
#endif
	{
		led_t *led = &leds[id];

		led->enabled = false;

		led->currentMode = STATIC;
		gpioWrite(led->pin, !led->activeState);

		led->enabled = true;
	}
#ifdef LED_DEVELOPMENT_MODE
	else
	{
		// Raise some error!
	}
#endif
}

static void ledIsr(void)
{
	uint8_t index;
	for (index = 0 ; index < AMOUNT_OF_LEDS ; index++)
	{
		led_t *led = &leds[index];
		if (led->enabled)
		{
			switch (led->currentMode)
			{
				case STATIC:
					// This mode is not handled by the periodic ISR
					break;

				case BLINK:
					led->settings.blink.cnt--;
					if (led->settings.blink.cnt == 0)
					{
						led->settings.blink.cnt = led->settings.blink.period / 2;
						gpioToggle(led->pin);
					}
					break;

				case BURST:
					led->settings.burst.burstCounter--;
					if (led->settings.burst.burstCounter == 0)
					{
						led->settings.burst.burstCounter = led->settings.burst.burstPeriod / 2;
						led->settings.burst.blinkCounter = led->settings.burst.blinkPeriod / 2;
						led->settings.burst.amountCounter = led->settings.burst.amount * 2;
					}
					else
					{
						if (led->settings.burst.amountCounter > 0)
						{
							led->settings.burst.blinkCounter--;
							if (led->settings.burst.blinkCounter == 0)
							{
								led->settings.burst.blinkCounter = led->settings.burst.blinkPeriod / 2;
								led->settings.burst.amountCounter--;
								gpioToggle(led->pin);
							}
						}

						if (led->settings.burst.amountCounter == 0)
						{
							gpioWrite(led->pin, !led->activeState);
						}
					}
					break;
			}
		}
	}
}

/******************************************************************************/
