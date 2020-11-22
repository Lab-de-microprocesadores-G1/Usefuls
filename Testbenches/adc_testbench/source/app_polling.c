/********************************************************************************
  @file     App.c
  @brief    Application functions
  @author   N. Magliola, G. Davidov, F. Farall, J. Gaytán, L. Kammann, N. Trozzo
 *******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <string.h>
#include "drivers/MCAL/adc/adc.h"
#include "drivers/MCAL/uart/uart.h"

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

static char msg[50];
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
        .usingAverage   = 1,
		.averageSamples = ADC_32_SAMPLES_AVERAGE
    };
    
    adcConfig(ADC_POTENTIOMETER, config);

    // UART driver init
    uart_cfg_t uartConfig = {
        .baudRate = UART_BAUD_RATE_9600,
    };

    uartInit(UART_INSTANCE_0, uartConfig);
}

/* Called repeatedly in an infinite loop */
void appRun (void)
{
    if (adcConversionCompleted(ADC_POTENTIOMETER))
    {
        uint32_t conversion = adcGetConversion(ADC_POTENTIOMETER);
    	sprintf(msg, "ADC Measurement %d\r\n", conversion);
    	if (uartCanTx(UART_INSTANCE_0, strlen(msg)))
    	{
            uartWriteMsg(UART_INSTANCE_0, msg, strlen(msg));
    	}
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
