/***************************************************************************//**
  @file     gpio.c
  @brief    Simple GPIO Pin services, similar to Arduino
  @author   Micro Issant
 ******************************************************************************/

#include "gpio.h"

static void clockGateEnable(pin_t pin)
{
	// Checks if port is enabled and if not, enables it.
	switch (PIN2PORT(pin))
	{
		case PA:
		{
			if (!(SIM->SCGC5 & SIM_SCGC5_PORTA_MASK))
			{
				SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;
			}
			break;
		}
		case PB:
		{
			if (!(SIM->SCGC5 & SIM_SCGC5_PORTB_MASK))
			{
				SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;
			}
			break;

		}
		case PC:
		{
			if (!(SIM->SCGC5 & SIM_SCGC5_PORTC_MASK))
			{
				SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;
			}
			break;
		}
		case PD:
		{
			if (!(SIM->SCGC5 & SIM_SCGC5_PORTD_MASK))
			{
				SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;
			}
			break;
		}
		case PE:
		{
			if (!(SIM->SCGC5 & SIM_SCGC5_PORTE_MASK))
			{
				SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;
			}
			break;
		}
		default: break;
	}
}


void gpioMode (pin_t pin, uint32_t mode)
{
	PORT_Type* ports[] = PORT_BASE_PTRS;
	GPIO_Type* gpios[] = GPIO_BASE_PTRS;

	// Enabling the clock gate for the selected port
	clockGateEnable(pin);

	// Setting MUX to GPIO alternative and other PCR options
	ports[PIN2PORT(pin)]->PCR[PIN2NUM(pin)] = ( GPIO_MASK | ( mode & ~TYPE_MASK ) );

	// Setting GPIOs to read/write. If mode is OUTPUT, mode=1, otherwise it's an input.
	gpios[PIN2PORT(pin)]->PDDR |= ( (mode & TYPE_MASK) == OUTPUT) << PIN2NUM(pin);
}

void gpioToggle (pin_t pin)
{
	GPIO_Type* gpios[] = GPIO_BASE_PTRS;

	// Toggling pin
	gpios[PIN2PORT(pin)]->PTOR |= 1 << PIN2NUM(pin);
}

void gpioWrite (pin_t pin, bool value)
{
	GPIO_Type* gpios[] = GPIO_BASE_PTRS;
	uint32_t mask = 0x1 << PIN2NUM(pin);

	// Writing the desired status (HIGH or LOW) in the pin
	if (value == HIGH)
	{
		gpios[PIN2PORT(pin)]->PDOR |= mask;
	}
	else
	{
		gpios[PIN2PORT(pin)]->PDOR &= (~mask);
	}
}

bool gpioRead (pin_t pin)
{
	GPIO_Type* gpios[] = GPIO_BASE_PTRS;

	// Reading the status of the pin
	return (gpios[PIN2PORT(pin)]->PDIR & (1 << PIN2NUM(pin)) ) != LOW;
}





