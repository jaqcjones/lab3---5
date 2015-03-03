/*
 * systick.c
 */

#include "systick.h"

void systick_init(void){
	uint32_t x = 0;
	uint32_t y = 0x7;
	SYSTICK->CS = x;			/* Step 1 - Disable timer */
	SYSTICK->RV = WAIT_CYCLES;	/* Load # cycles to wait before interrupt */
	SYSTICK->CV = x;			/* Set current value to 0 */
	SYSTICK->CS = y;			/* Load 7 to start interrupts */
}

