/*
 * main.c
 */

#include "stdint.h"     /* uint32_t, etc... */
#include "stm32f4xx.h"  /* Useful definitions for the MCU */
#include "LED.h"        /* C routines in LED.c */
#include "USART2.h"     /* assembly routines in USART.S */
#include "systick.h"	/* C routines in systick.c */
#include "ADC.h"
#include "mutex.h"

long pot_array[4]; /* Holds history of last 4 potentiometer values*/
int index = 0;
uint32_t mutex_var;

void print_newline(void);

/*
 * The systick Interrupt Service Routine
 */
void __attribute__ ((interrupt)) systick_handler(void)
{
	uint32_t state = GPIOD->IDR;
	state = state & LED_GREEN_ON;
	if (state == LED_GREEN_ON) {
		LED_update(LED_GREEN_OFF);
	}
	else {
		LED_update(LED_GREEN_ON);
	}
}

void __attribute__ ((interrupt)) USART2_handler(void) {
	uint8_t char_recv = USART2_recv();
	USART2_send(char_recv);

	if (char_recv == 'h') {
		print_pot_history(pot_array, &index);
	}
	else if (char_recv == 'a') {
		print_avg_history(pot_array, &index);
	}
	else if (char_recv == 'c') {
		uint8_t result = lock_mutex(mutex_var);
		if (result == 0) {
			clear_history(pot_array);
			unlock_mutex(mutex_var);
		}
	}
	print_newline();
}

void print_newline(void) {
	USART2_send('\n');
	USART2_send('\r');
}

int main()
{
	/*
	 * Demonstrate use of LED.h/LED.c - modifies hardware registers using C
	 */
	LED_init();
	LED_update(LED_BLUE_ON);
	LED_update(LED_BLUE_OFF);
	LED_update(LED_RED_ON | LED_BLUE_ON | LED_ORANGE_ON | LED_GREEN_ON );

	/*
	 * Demonstrate use of in-line assembly - enable interrupts
	 */
	__asm ("  cpsie i \n" );


	/* Initialize the USART for 9600, 8N1, send '!' - calls into USART2.S */
	/* NOTE: must set USART2 interrupt config register to enable TX/RX interrupts */
	USART2_init();
	USART2_send('!');

	uint32_t val = 62;
	bits_to_hex(val);

	/* Initialize systick structure holding control registers */
	systick_init();

	/* Configure ADC and potentiometer */
	ADC_init();

	mutex_var = 0;

//	long pot_array[4];
	index = 0;
	int pressed = 0;
	/* Wait here forever */
	while(1) {
		uint32_t userb = userbutton_recv();
		while (userb != 0) {
			uint8_t result = lock_mutex(mutex_var);
			if (result == 0){
				read_ADC(pot_array, &index);
				unlock_mutex(mutex_var);
			}
			pressed = 1;
			userb = userbutton_recv();
		}
		int i;
		int j = index;
		if (pressed == 1) {
			for (i=0;i<4;i++) {
				long send_val = pot_array[j];
				bits_to_hex(send_val);
				j--;
				j = j+4;
				j = j % 4;
			}
			pressed = 0;
			print_newline();
		}
	}

	/* We'll never reach this line */
	return 0;
}



