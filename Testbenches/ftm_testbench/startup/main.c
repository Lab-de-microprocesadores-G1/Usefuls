/*******************************************************************************
  @file     main.c
  @brief    Main application
  @author   N. Magliola, G. Davidov, F. Farall, J. Gaytán, L. Kammann, N. Trozzo
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "hardware.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/


/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/


/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

// #define APPLICATION_OVERFLOW
#define APPLICATION_OUTPUT_COMPARE
// #define APPLICATION_INPUT_CAPTURE
// #define APPLICATION_PWM
// #define APPLICATION_OUTPUT_COMPARE_SINGLE_SHOT

void appInitOverflow        (void);
void appRunOverflow         (void);

void appInitOutputCompare   (void);
void appRunOutputCompare    (void);

void appInitInputCapture    (void);
void appRunInputCapture     (void);

void appInitPWM             (void);
void appRunPWM              (void);

void appInitOutputCompareSingleShot   (void);
void appRunOutputCompareSingleShot    (void);

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


/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

int main (void)
{
    hw_Init();
    hw_DisableInterrupts();
#ifdef APPLICATION_OVERFLOW
    appInitOverflow();
#endif
#ifdef APPLICATION_OUTPUT_COMPARE
    appInitOutputCompare();
#endif
#ifdef APPLICATION_INPUT_CAPTURE
    appInitInputCapture();
#endif
#ifdef APPLICATION_PWM
    appInitPWM();
#endif
#ifdef APPLICATION_OUTPUT_COMPARE_SINGLE_SHOT
    appInitOutputCompareSingleShot();
#endif
    hw_EnableInterrupts();

    __FOREVER__
#ifdef APPLICATION_OVERFLOW
        appRunOverflow();
#endif
#ifdef APPLICATION_OUTPUT_COMPARE
        appRunOutputCompare();
#endif
#ifdef APPLICATION_INPUT_CAPTURE
        appRunInputCapture();
#endif
#ifdef APPLICATION_PWM
        appRunPWM();
#endif
#ifdef APPLICATION_OUTPUT_COMPARE_SINGLE_SHOT
    appRunOutputCompareSingleShot();
#endif
}
