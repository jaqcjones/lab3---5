/*
 * ADC.c
 * Deals with potentiometer stuff
 * Deals with user button stuff
 */

#include "ADC.h"
#include "stm32f4xx.h"
#include "USART2.h"
#include "mutex.h"

void ADC_init(void) {
	/* Enable GPIOA port peripheral clock */
	RCC->AHB1ENR = RCC->AHB1ENR | RCC_AHB1ENR_GPIOAEN;

	/* Set Pin 1 as analog input (THIS WAS THE PROBLEM - DO OR's ALWAYS!!) */
	GPIOA->MODER = GPIOA->MODER | GPIOx_MODER_PIN1_ANALOGIN;

	/* Enable ADC peripheral clock */
	RCC->APB2ENR = RCC->APB2ENR | RCC_APB2ENR_ADC1EN;

	/* Configure ADC1_SQR1 for sequence length of 1 */
	ADC1->SQR1 = ADC1->SQR1 & ADC1_SQR1_LEN1;

	/* Map 1st reg conversion to ADC channel for potentiometer*/
	ADC1->SQR3 = ADC1->SQR3 | ADC1_SQR3_CONV1_ADCIN1;

	/* Set sample time on SMP1 (ADC_IN1) to 480 cycles */
	ADC1->SMPR2 = ADC1->SMPR2 | ADC1_SMPR2_SMP1;

	/* Enable conversions */
	ADC1->CR2 = ADC1->CR2 | ADC1_CR2_SET_ADON;

}
/*
 * Reads values from potentiometer
 * Returns an array of last 4 values
 */
void read_ADC(long *array, int *index) {
	/* Initiate conversion */
	ADC1->CR2 = ADC1->CR2 | ADC1_CR2_SET_SWSTART;

	/* Read converted value */
	/* Check for end of conversion */
	while ((ADC1->SR & ADC1_SR_EOC_MASK) != ADC1_SR_EOC_MASK) {
		/* Wait */
	}
	/* Read conversion value */
	long conv_val = ADC1->DR;
	/* Convert value to hex and print to terminal */
	//bits_to_hex(conv_val);

	array[*index] = conv_val;
	*index = *index + 1;
	*index = *index % 4;

	return;

}

void bits_to_hex(uint32_t val){
	char ans[10];
	int mod_val = 0;
	int i;
	USART2_send('0');
	USART2_send('x');
	for (i=0;val!=0; i++){
		mod_val = val % 16;
		if (mod_val < 10) {
			mod_val = mod_val + 48;
		}
		else {
			mod_val = mod_val + 55;
		}
		ans[i] = mod_val;
		val = val / 16;
	}
	while (i != 0) {
		USART2_send(ans[i-1]);
		i--;
	}
	return;
}

uint32_t userbutton_recv(void) {
	long samples = 1000;
	uint32_t button_status = GPIOA->IDR & GPIOA_USERB_MASK;
	uint32_t IDR_stat;
	/* Debouncer */
	while ((samples != 0) && (button_status != 0)) {
		button_status = GPIOA->IDR & GPIOA_USERB_MASK;
		samples--;
	}
	return button_status;
}

void print_pot_history(long *arr, int *index) {
	int i;
	int j = *index;
	for (i=0;i<4;i++){
		long send_val = arr[i];
		bits_to_hex(send_val);
		j--;
		j = j+4;
		j = j % 4;
	}
	return;
}

void print_avg_history(long *arr, int *index) {
	int i;
	int j = *index;
	long total_val = 0;
	for (i=0;i<4;i++) {
		long val = arr[i];
		total_val = total_val + val;
		j--;
		j = j+4;
		j = j % 4;
	}
	long avg_val = total_val / 4;
	bits_to_hex(avg_val);
	return;
}

void clear_history(long *arr) {
	int i;
	for (i=0;i<4;i++) {
		arr[i] = 0;
	}
	return;
}

//void lock_mutex(uint32_t *mutex) {
//	while (*mutex != 0) {
//
//	}
//	*mutex = 1;
//}

//void unlock_mutex(uint32_t *mutex) {
//	*mutex = 0;
//}
