/********************************************************************************
  @file     App.c
  @brief    Application functions
  @author   N. Magliola, G. Davidov, F. Farall, J. Gaytán, L. Kammann, N. Trozzo
 *******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "adc.h"
#include "uart.h"
#include "string.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

// #define SOME_CONSTANT    20
// #define MACRO(x)         (x)

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

static char * msg = "Potentiometer value = 000";

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/* Called once at the beginning of the program */
void appInit (void)
{
    // Initialize the ADC driver
    adcInit();

    // ADC configuration
    adc_cfg_t config = {
        .diff           = 0,
        .resolution     = ADC_8_BIT_SINGLE_CONV,
        .usingAverage   = 0 
    };
    
    adcConfig(ADC_INSTANCE_0, config);

    // UART driver init
    uart_cfg_t uartConfig = {
        .baudRate = UART_BAUD_RATE_9600,
    }
    uartInit(UART_INSTANCE_0, uartConfig);
}

/* Called repeatedly in an infinite loop */
void appRun (void)
{
    if (adcConversionCompleted(ADC_POTENTIOMETER))
    {
        uint32_t conversion = adcGetConversion(ADC_POTENTIOMETER);
        sprintf(msg + strlen(msg) - 4,"%i", conversion);
        uartWriteMsg(UART_INSTANCE_0, msg, strlen(msg));
    }

    if (adcAvailable(ADC_POTENTIOMETER))
    {
        adcStartConversion(ADC_POTENTIOMETER);
    }
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/



/*******************************************************************************
 ******************************************************************************/
