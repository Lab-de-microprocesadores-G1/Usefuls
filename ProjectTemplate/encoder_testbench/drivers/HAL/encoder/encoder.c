/*******************************************************************************
  @file     encoder.c
  @brief    [...]
  @author   G. Davidov, F. Farall, J. Gaytán, L. Kammann, N. Trozzo
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include "encoder.h"
// #include "../../board/board.h"
#include "../../MCAL/gpio/gpio.h"
#include "MK64F12.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
#define ENCODER_ISR_HANDLER(i,s) \
      void encoderISRHandler_##i_##s(void) { \
          FSMCycle(i,s); \
      }
#define ENCODER_ISR_HANDLER_NAME(i,s) encoderISRHandler_##i_##s
#define ENCODER_ISR_HANDLER_PROTOTYPE(i,s) void encoderISRHandler_##i_##s(void);

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/
#define ENCODER1_A_PIN PORTNUM2PIN(PC,3)
#define ENCODER1_B_PIN PORTNUM2PIN(PC,2)


typedef enum uint8_t {
	IDLE,
  RC1,
  RC2,
  RC3,
  RCC1,
  RCC2,
  RCC3
} encoder_state_t;

typedef void (*encoder_isr_t)(void);

typedef struct {
  pin_t pinNumberA;
  pin_t pinNumberB;
  encoder_state_t state;
  bool enabled;
  encoder_callback_t clockwiseCallback;
  encoder_callback_t counterClockwiseCallback;
  encoder_isr_t isrA;
  encoder_isr_t isrB;
} encoder_t;

typedef enum uint8_t {
  A,
  B
} encoder_signal_t;



/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/
static void FSMCycle(encoder_id_t id, encoder_signal_t signal);
static void initSingleEncoder(encoder_id_t id);
ENCODER_ISR_HANDLER_PROTOTYPE(0,A)
ENCODER_ISR_HANDLER_PROTOTYPE(0,B)

/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/
static encoder_t encodersList[ENCODER_COUNT] = {
  {ENCODER1_A_PIN, ENCODER1_B_PIN, IDLE, false, NULL, NULL, ENCODER_ISR_HANDLER_NAME(0,A), ENCODER_ISR_HANDLER_NAME(0,B)}
  //* To add a new encoder:
  //* {ENCODERn_A_PIN, ENCODERn_B_PIN, IDLE, false, NULL, NULL, ENCODER_ISR_HANDLER_NAME(n,A), ENCODER_ISR_HANDLER_NAME(n,B)}
};


/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
void encoderInit(void)
{
  for (uint8_t id = 0; id < ENCODER_COUNT; id++)
  {
    initSingleEncoder(id);
  }
}

void registerCallbacks(encoder_id_t id, encoder_callback_t clockwiseCallback, encoder_callback_t counterClockwiseCallback)
{
  #ifdef TIMER_DEVELOPMENT_MODE
  if(id >= ENCODER_COUNT || id < 0)
    return; // exception (NMI)
  #endif

  registerClockwiseCallback(id, clockwiseCallback);
  registerCounterClockwiseCallback(id, counterClockwiseCallback);
  enableEncoder(id);

}

void registerClockwiseCallback(encoder_id_t id, encoder_callback_t clockwiseCallback)
{
  #ifdef TIMER_DEVELOPMENT_MODE
  if(id >= ENCODER_COUNT || id < 0)
    return; // exception (NMI)
  #endif
  
  encodersList[id].clockwiseCallback = clockwiseCallback;
  enableEncoder(id);

}

void registerCounterClockwiseCallback(encoder_id_t id, encoder_callback_t counterClockwiseCallback)
{
  #ifdef TIMER_DEVELOPMENT_MODE
  if(id >= ENCODER_COUNT || id < 0)
    return; // exception (NMI)
  #endif

  encodersList[id].counterClockwiseCallback = counterClockwiseCallback;
  enableEncoder(id);
}

void disableEncoder(encoder_id_t id)
{
  #ifdef TIMER_DEVELOPMENT_MODE
  if(id >= ENCODER_COUNT || id < 0)
    return; // exception (NMI)
  #endif

  encodersList[id].enabled = false;
}

void enableEncoder(encoder_id_t id)
{
  #ifdef TIMER_DEVELOPMENT_MODE
  if(id >= ENCODER_COUNT || id < 0)
    return; // exception (NMI)
  #endif
  
  encodersList[id].enabled = true;
}




/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
ENCODER_ISR_HANDLER(0,A)
ENCODER_ISR_HANDLER(0,B)

void initSingleEncoder(encoder_id_t id)
{
  gpioMode(encodersList[id].pinNumberA, INPUT);
  gpioMode(encodersList[id].pinNumberB, INPUT);
  gpioIRQ(encodersList[id].pinNumberA, GPIO_IRQ_MODE_INTERRUPT_BOTH_EDGES, encodersList[id].isrA);
  gpioIRQ(encodersList[id].pinNumberB, GPIO_IRQ_MODE_INTERRUPT_BOTH_EDGES, encodersList[id].isrB);
}

static void FSMCycle(encoder_id_t id, encoder_signal_t signal)
{
  
}

/******************************************************************************/
