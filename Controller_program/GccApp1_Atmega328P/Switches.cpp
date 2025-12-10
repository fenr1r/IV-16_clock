/*
 * Switches.cpp
 *
 * Created: 2025/11/11 17:43:31
 *  Author: fenrir_
 */ 

#include <avr/interrupt.h>
#include <avr/io.h>

#include "Switches.hpp"
#include "LED.hpp"

// PC1, 2, 3

static void readSW(){
	
}

void init_switches(){
	// 最初にプルアップあり入力で設定しちゃうので、スルー（12/24は分圧されるかもしれない。5Vと2.5Vの中間に）
	
	// 内部でTimer0を使い、50msごとにポーリングするようにする
	TCCR0A = 0x00;	// Normal mode, no pin output
	TCCR0B = (1 << CS02) | (0 << CS01) | (1 << CS00);	// clk / 1024 (30ms)
	TCNT0 = 0x00;
	
	TIMSK0 = (1 << TOIE0);	// Enable OVF irq
}

ISR(TIMER0_OVF_vect){
	readSW();
	toggle_LED3();
}
