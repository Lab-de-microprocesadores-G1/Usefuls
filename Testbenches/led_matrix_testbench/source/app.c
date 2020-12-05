/********************************************************************************
  @file     App.c
  @brief    Application functions
  @author   N. Magliola, G. Davidov, F. Farall, J. Gaytán, L. Kammann, N. Trozzo
 *******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "../drivers/HAL/WS2812/WS2812.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define DISPLAY_ROW_SIZE    8
#define DISPLAY_COL_SIZE    8
#define DISPLAY_SIZE        DISPLAY_ROW_SIZE * DISPLAY_COL_SIZE

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

// static void privateFunction(void);

/*******************************************************************************
 * VARIABLES TYPES DEFINITIONS
 ******************************************************************************/

// typedef int  my_int_t;

/*******************************************************************************
 * PRIVATE VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static ws2812_pixel_t display[DISPLAY_SIZE];
static ws2812_pixel_t colors[4] = { WS2812_COLOR_RED, WS2812_COLOR_GREEN, WS2812_COLOR_BLUE, WS2812_COLOR_WHITE };

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/* Called once at the beginning of the program */
void appInit (void)
{
    WS2812Init();

    WS2812SetDisplayBuffer(display, DISPLAY_SIZE);    

    for (uint8_t i = 0 ; i < DISPLAY_ROW_SIZE ; i++)
    {
        for (uint8_t j = 0 ; j < DISPLAY_COL_SIZE ; j++)
        {
        	if (j == i)
        	{
                display[i * DISPLAY_ROW_SIZE + j] = colors[0];
        	}
        	else
        	{
                display[i * DISPLAY_ROW_SIZE + j] = colors[3];
        	}
        }
    }

    WS2812Update();
}

/* Called repeatedly in an infinite loop */
void appRun (void)
{
    // Application iterative tasks, every loop runs this function
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/


/*******************************************************************************
 ******************************************************************************/
