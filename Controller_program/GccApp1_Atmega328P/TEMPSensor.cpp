/*
 * TEMPSensor.cpp
 *
 * Created: 2025/11/11 12:41:45
 *  Author: fenrir_
 */ 

#include <avr/io.h>
#include "TEMPSensor.hpp"

static uint16_t last_read_val;

void TEMPSensor_init(){
	// プルアップ抵抗かならず切ること！（分圧されて2.5Vくらいに固定される）
	DDRC	&= ~(1 << DDC0);	// set input
	PORTC	&= ~(1 << PORTC0);	// without pull-up reg.
		
	// ADEN : 1 AD許可
	// ADSC : 0 AD変換開始しない
	// ADATE : 0 AD変換自動開始しない
	// ADIF : 1 AD完了割り込み要求フラグ
	// ADIE : 1 割り込み許可
	// ADPS2-0 : 110 分周比64（400kHzくらいにあわせる）
	ADCSRA = (1 << ADEN) | (0 << ADSC) | (0 << ADATE) | (0 << ADIF) | (1 << ADIE) | (1 << ADPS2) | (1 << ADPS1) | (0 << ADPS0);
	
	// REFS1-0 : 01 (ref = AVcc)
	// ADLAR : 0 変換データレジスタ右揃え
	// MUX3-0 : 0000 ADC0を選択
	ADMUX = (0 << REFS1) | (1 << REFS0) | (0 << ADLAR) | (0 << MUX3) | (0 << MUX2) | (0 << MUX1) | (0 << MUX0);
	
	DIDR0 |= (1 << ADC0D);
}

void TEMPSensor_startConvert(){
	ADCSRA |= (1 << ADSC);
}

uint16_t TEMPSensor_readValue(){
	uint16_t val;
	
	// ADC結果読み込む（ADCLから読むこと）
	uint8_t lower_val = ADCL;
	uint8_t higher_val = ADCH & 0x03;
	
	val = (higher_val << 8) + lower_val;
	//val = ADC;
	
	last_read_val = val;
	
	return val;
}

uint8_t conv_to_temp(const uint16_t sensor_val){
	return (uint8_t)((sensor_val * 5 * 100) / 1024);
}

uint16_t TEMPSensor_getLastValue(){
	return last_read_val;
}