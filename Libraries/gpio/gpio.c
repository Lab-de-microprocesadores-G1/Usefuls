/***************************************************************************//**
  @file     gpio.c
  @brief    Simple GPIO Pin services, similar to Arduino
  @author   Micro Issant
 ******************************************************************************/

#include "gpio.h"

void PORTA_IRQHandler(void);
void PORTB_IRQHandler(void);
void PORTC_IRQHandler(void);
void PORTD_IRQHandler(void);
void PORTE_IRQHandler(void);

void (*(drivers[5][32]))(void);

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

	clockGateEnable(pin);

	// Setting MUX to GPIO alternative.
	ports[PIN2PORT(pin)]->PCR[PIN2NUM(pin)] = ( GPIO_MASK | ( mode & ~TYPE_MASK ) );

	// Setting GPIOs to read/write. If mode is OUTPUT, mode=1, otherwise it's an input.
	gpios[PIN2PORT(pin)]->PDDR |= ( (mode & TYPE_MASK) == OUTPUT) << PIN2NUM(pin);
}

void gpioToggle (pin_t pin)
{
	GPIO_Type* gpios[] = GPIO_BASE_PTRS;

	// Toggling pin.
	gpios[PIN2PORT(pin)]->PTOR |= 1 << PIN2NUM(pin);
}

void gpioWrite (pin_t pin, bool value)
{
	GPIO_Type* gpios[] = GPIO_BASE_PTRS;
	uint32_t mask = 0x1 << PIN2NUM(pin);

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

	return (gpios[PIN2PORT(pin)]->PDIR & (1 << PIN2NUM(pin)) ) != LOW;
}

bool gpioIRQ (pin_t pin, uint8_t irqMode, pinIrqFun_t irqFun)
{
	PORT_Type* ports[] = PORT_BASE_PTRS;
	IRQn_Type irqs[] = PORT_IRQS;

	ports[PIN2PORT(pin)]->PCR[PIN2NUM(pin)] |= PORT_PCR_IRQC(irqMode);
	drivers[PIN2PORT(pin)][PIN2NUM(pin)] = irqFun;

	NVIC_EnableIRQ(irqs[PIN2PORT(pin)]);
}

void portHandler(uint8_t port)
{
	PORT_Type* ports[] = PORT_BASE_PTRS;
	uint32_t isfr = ports[port]->ISFR;

	ports[port]->ISFR = 0xFFFFFFFF;

	uint8_t i = 0;
	for (i = 0; i < 32 && isfr; i++)
	{
		if (isfr & 1)
		{
			if (drivers[port][i])
			{
				drivers[port][i]();
			}
		}

		isfr = isfr >> 1;
	}
}

void PORTA_IRQHandler(void)
{
	portHandler(PA);
}
void PORTB_IRQHandler(void)
{
	portHandler(PB);
}
void PORTC_IRQHandler(void)
{
	portHandler(PC);
}
void PORTD_IRQHandler(void)
{
	portHandler(PD);
}
void PORTE_IRQHandler(void)
{
	portHandler(PE);
}
