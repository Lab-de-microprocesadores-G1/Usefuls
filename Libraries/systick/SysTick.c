#include "SysTick.h"
#include "MK64F12.h"
#include "core_cm4.h"
#include "hardware.h"

__ISR__ SysTick_Handler(void);

void (*handler)(void);

bool SysTick_Init (void (*funcallback)(void))
{
	uint32_t ticks = CPU_FREQUENCY_HZ / SYSTICK_ISR_FREQUENCY_HZ;
	handler = funcallback;
	SysTick->CTRL = 0x00;
	NVIC_EnableIRQ(SysTick_IRQn);
	return (bool)SysTick_Config(ticks);
}

__ISR__ SysTick_Handler(void)
{
	handler();
}
