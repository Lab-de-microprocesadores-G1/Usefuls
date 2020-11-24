/********************************************************************************
  @file     App.c
  @brief    Application functions
  @author   N. Magliola, G. Davidov, F. Farall, J. Gaytán, L. Kammann, N. Trozzo
 *******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdio.h>

#include "board.h"
#include "drivers/MCAL/pwm_dma/pwm_dma.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define FRAME_SIZE          24 * 8
#define TOTAL_FRAMES        8
#define BUFFER_SIZE         FRAME_SIZE

#define PRESCALER           1
#define MODULO              30

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

static void updateCallback (uint16_t * frameToUpdate, uint8_t frameCounter);

/*******************************************************************************
 * PRIVATE VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static uint16_t     firstFrame[BUFFER_SIZE];
static uint16_t     secondFrame[BUFFER_SIZE];

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/* Called once at the beginning of the program */
void appInit (void)
{
    // Initialize PWM-DMA driver
    pwmdmaInit(PRESCALER, MODULO, FTM_INSTANCE_0, FTM_CHANNEL_0);

    // Register update callback
    pwmdmaOnFrameUpdate(updateCallback);

    // Start the DMA transfer on the PWM
    pwmdmaStart(firstFrame, secondFrame, FRAME_SIZE, TOTAL_FRAMES, true);
}

/* Called repeatedly in an infinite loop */
void appRun (void)
{

}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

static void updateCallback (uint16_t * frameToUpdate, uint8_t frameCounter)
{
    for (uint8_t i = 0 ; i < FRAME_SIZE ; i++)
    {
        frameToUpdate[i] = i % 2 ? 10 : 20;
    }
}


/*******************************************************************************
 ******************************************************************************/
