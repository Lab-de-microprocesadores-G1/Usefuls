/********************************************************************************
  @file     App.c
  @brief    Application functions
  @author   N. Magliola, G. Davidov, F. Farall, J. Gaytán, L. Kammann, N. Trozzo
 *******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "drivers/HAL/timer/timer.h"
#include "drivers/HAL/HD44780/HD44780.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define UPDATE_PERIOD		300

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

static void displayCounter(void);

/*******************************************************************************
 * VARIABLES TYPES DEFINITIONS
 ******************************************************************************/

/*******************************************************************************
 * PRIVATE VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static tim_id_t idTimer;
static bool showingMessage = false;

static const char * message = "Boca la concha de tu madre";

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/* Called once at the beginning of the program */
void appInit (void)
{
	// LCD configurations
	hd44780_cfg_t config  = {
			.inc = HD44780_INC_CURSOR,
			.bothLines = HD44780_BOTH_LINES,
			.shift = HD44780_NO_SHIFT,
			.blink = HD44780_DO_BLINK,
			.cursor = HD44780_SHOW_CURSOR
	};
	HD44780Init(config);

	// Initialization of the timer driver
	timerInit();
	idTimer = timerGetId();
}

/* Called repeatedly in an infinite loop */
void appRun (void)
{
	if (!showingMessage && HD44780InitReady())
	{
		timerStart(idTimer, TIMER_MS2TICKS(UPDATE_PERIOD), TIM_MODE_PERIODIC, displayCounter);
		showingMessage = true;
	}
}

void displayCounter(void)
{
	static uint8_t index = 0;
	static bool changed = false;
	if (index < strlen(message))
	{
		if ((index == 15) && !changed)
		{
			// Start shifting
			HD44780WriteInstruction(HD44780_ENTRY_MODE_SET(1, 1));
			changed = true;
		}
		else
		{
			HD44780WriteData(message[index++]);
		}
	}

}
/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/


/*******************************************************************************
 ******************************************************************************/
