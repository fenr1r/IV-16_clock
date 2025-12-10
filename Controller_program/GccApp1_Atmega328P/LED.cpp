/*
 * LEDs.cpp
 *
 * Created: 2025/11/11 2:31:28
 *  Author: fenrir_
 */
 #include <avr/io.h>
 #include "LED.hpp"

void init_LEDs(){

	// LED1 : PB0
	DDRB	|= (1 << DDB0);
	//PORTB	&= ~(1 << PINB0);
	
	// LED2, 3, 4 : PD7, 6, 5
	DDRD	|= ((1 << DDD7) | (1 << DDD6) | (1 << DDD5));
	//PORTC	&= ~((1 << PINC7) || (1 << PINC6) || (1 << PINC5));
	
	return ;	
}

void clear_LEDs(){
	clear_LED1();
	clear_LED2();
	clear_LED3();
	clear_LED4();
}

void set_LEDs(){
	set_LED1();
	set_LED2();
	set_LED3();
	set_LED4();
}

void set_LEDs(const uint8_t val){

	clear_LED1();
	clear_LED2();
	clear_LED3();
	clear_LED4();

	if (((val >> 3) & 0x01) == 0x01) {
		set_LED4();
	}
	
	if (((val >> 2) & 0x01) == 0x01) {
		set_LED3();
	}
	
	if (((val >> 1) & 0x01) == 0x01) {
		set_LED2();
	}
	
	if (((val >> 0) & 0x01) == 0x01) {
		set_LED1();
	}
	
	return ;
}

void set_LED1(const BOOL is_on){
	if (is_on == TRUE) {
		PORTB |= (1 << PINB0);
	} else {
		PORTB &= ~(1 << PINB0);
	}
}

void set_LED1(){
	set_LED1(TRUE);
}

void clear_LED1(){
	set_LED1(FALSE);
}

void set_LED2(const BOOL is_on){
	if (is_on == TRUE) {
		PORTD |= (1 << PIND7);
		} else {
		PORTD &= ~(1 << PIND7);
	}
}

void set_LED2(){
	set_LED2(TRUE);
}

void clear_LED2(){
	set_LED2(FALSE);
}

BOOL read_LED2(){
	return (PORTD >> PIND7) & 0x01;
}

void toggle_LED2(){
	if (read_LED2() == TRUE){
		clear_LED2();
	} else {
		set_LED2();
	}
}

void set_LED3(const BOOL is_on){
	if (is_on == TRUE) {
		PORTD |= (1 << PIND6);
		} else {
		PORTD &= ~(1 << PIND6);
	}
}

void set_LED3(){
	set_LED3(TRUE);
}

void clear_LED3(){
	set_LED3(FALSE);
}

BOOL read_LED3(){
	return (PORTD >> PIND7) & 0x01;
}

void toggle_LED3(){
	if (read_LED3() == TRUE){
		clear_LED3();
		} else {
		set_LED3();
	}
}

void set_LED4(const BOOL is_on){
	if (is_on == TRUE) {
		PORTD |= (1 << PIND5);
		} else {
		PORTD &= ~(1 << PIND5);
	}
}

void set_LED4(){
	set_LED4(TRUE);
}

void clear_LED4(){
	set_LED4(FALSE);
}
