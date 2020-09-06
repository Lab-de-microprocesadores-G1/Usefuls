/***************************************************************************//**
  @file     gpio.h
  @brief    Simple GPIO Pin services, similar to Arduino
  @author   Nicolás Magliola
 ******************************************************************************/

#ifndef _GPIO_H_
#define _GPIO_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include "MK64F12.h"


/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

// Ports
enum { PA, PB, PC, PD, PE };

// Convert port and number into pin ID
// Ex: PTB5  -> PORTNUM2PIN(PB,5)  -> 0x25
//     PTC22 -> PORTNUM2PIN(PC,22) -> 0x56
#define PORTNUM2PIN(p,n)    (((p)<<5) + (n))
#define PIN2PORT(p)         (((p)>>5) & 0x07)
#define PIN2NUM(p)          ((p) & 0x1F)


// Modes
#ifndef INPUT

#define TYPE_MASK			0x0080		// Unused bit on PCR
#define GPIO_MASK			(1 << PORT_PCR_MUX_SHIFT)

#define INPUT               0	// Use TYPE_MASK bit to tell whether we use it as INPUT or OUTPUT
#define OUTPUT              TYPE_MASK
#define PULLDOWN			PORT_PCR_PE_MASK
#define PULLUP				( PORT_PCR_PE_MASK | PORT_PCR_PS_MASK )
#define SLEWRATE 			PORT_PCR_SRE_MASK
#define OPENDRAIN			PORT_PCR_ODE_MASK
#define LOCK				PORT_PCR_LK_MASK
#define FILTER				PORT_PCR_PFE_MASK
#define DRIVESTRENGTH		PORT_PCR_DSE_MASK

// Backwards Compatibility
#define INPUT_PULLUP        ( INPUT | PULLUP )
#define INPUT_PULLDOWN      ( INPUT | PULLDOWN )

#endif // INPUT

// Digital values
#ifndef LOW
#define LOW     0
#define HIGH    1
#endif // LOW


// IRQ modes
enum {
    GPIO_IRQ_MODE_DISABLE,
    GPIO_IRQ_MODE_RISING_EDGE,
    GPIO_IRQ_MODE_FALLING_EDGE,
    GPIO_IRQ_MODE_BOTH_EDGES,

    GPIO_IRQ_CANT_MODES
};


/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef uint8_t pin_t;

typedef void (*pinIrqFun_t)(void);


/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/**
 * @brief Configures the specified pin to behave either as an input or an output
 * @param pin the pin whose mode you wish to set (according PORTNUM2PIN)
 * @param mode INPUT, OUTPUT, INPUT_PULLUP or INPUT_PULLDOWN.
 */
void gpioMode (pin_t pin, uint32_t mode);

/**
 * @brief Configures how the pin reacts when an IRQ event ocurrs
 * @param pin the pin whose IRQ mode you wish to set (according PORTNUM2PIN)
 * @param irqMode disable, risingEdge, fallingEdge or bothEdges
 * @param irqFun function to call on pin event
 * @return Registration succeed
 */
bool gpioIRQ (pin_t pin, uint8_t irqMode, pinIrqFun_t irqFun);

/**
 * @brief Write a HIGH or a LOW value to a digital pin
 * @param pin the pin to write (according PORTNUM2PIN)
 * @param val Desired value (HIGH or LOW)
 */
void gpioWrite (pin_t pin, bool value);

/**
 * @brief Toggle the value of a digital pin (HIGH<->LOW)
 * @param pin the pin to toggle (according PORTNUM2PIN)
 */
void gpioToggle (pin_t pin);

/**
 * @brief Reads the value from a specified digital pin, either HIGH or LOW.
 * @param pin the pin to read (according PORTNUM2PIN)
 * @return HIGH or LOW
 */
bool gpioRead (pin_t pin);


/*******************************************************************************
 ******************************************************************************/

#endif // _GPIO_H_
