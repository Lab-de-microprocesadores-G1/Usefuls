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

static void callback(uint32_t sample);

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
    //Initialize the ADC driver
    adcInit();

    //ADC configuration
    adc_cfg_t config = {
        .diff         = 1,
        .resolution   = ADC_9_BIT_DIFF_CONV,
        .usingAverage = ADC_32_SAMPLES_AVERAGE
    };
    
    adcOnConversion(ADC_POTENTIOMETER, callback);

    adcConfig(ADC_POTENTIOMETER, config);

    // UART driver init
    uart_cfg_t uartConfig = {
        .baudRate = UART_BAUD_RATE_9600,
    };

    uartInit(UART_INSTANCE_0, uartConfig);

    adcStartConversion(ADC_POTENTIOMETER);
    
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
void callback(uint32_t sample)
{
    uint32_t conversion = adcGetConversion(ADC_POTENTIOMETER);
	sprintf(msg, "ADC Measurement %d\r\n", conversion);
	if (uartCanTx(UART_INSTANCE_0, strlen(msg)))
	{
        uartWriteMsg(UART_INSTANCE_0, msg, strlen(msg));
	}

    if (adcAvailable(ADC_POTENTIOMETER))
    {
        adcStartConversion(ADC_POTENTIOMETER);
    }
}


/*******************************************************************************
 ******************************************************************************/
