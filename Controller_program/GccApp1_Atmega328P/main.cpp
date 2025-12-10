/*
 * GccApp1_Atmega328P.cpp
 *
 * Created: 2025/10/13 19:46:52
 * Author : fenrir_
 */ 

#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#define F_CPU 8000000
#include <util/delay.h>

#define _DEBUG 1
// #define _DEBUG_ISR 1

#include "time.hpp"
#include "TC62D748.hpp"
#include "RTC.hpp"
#include "uart.hpp"
#include "LED.hpp"
#include "TEMPSensor.hpp"
#include "Switches.hpp"

time master_time;

// ISR(USART_RX_vect) is defined on uart.cpp


void init_1hz_interrupt(){
	// 入力ピンの設定
	// 最初はプルアップ付き入力でコンフィグしたはずだから、割愛
	
	// INT0外部割り込み許可設定。sei()は外側でやる
	EICRA |= (1 << ISC00) | (1 << ISC01);	// 立ち上がりエッジで割り込み
	EIMSK |= (1 << INT0);					// INT0外部割り込み許可
}

ISR(INT0_vect){
	// LED 1 toggle
	toggle_LED2();
	
	// +1 second
	master_time = time_countup(master_time);
	
	const uint16_t last_sensor_val = TEMPSensor_getLastValue();
	const uint8_t temp = conv_to_temp(last_sensor_val);
	
	// 温度が50℃以下のときのみ、IV-16の表示をONにする
	if (temp <= 55){
		set_LED4();
		TC62D748_EnableDisplay();
		
	} else {
		clear_LED4();
		TC62D748_DisableDisplay();
	}
	
	TC62D748_send(master_time);	// 値はどちらにしろ更新しておく
	
	// 温度センサーの、次の値を更新するために、ADC変換開始
	TEMPSensor_startConvert();
	
#ifdef _DEBUG_ISR
	char buf[50];
	time_to_str(master_time, buf, 50);
	
	uart_puts("[Debug] INT0 occured. Updated time is ");
	uart_puts(buf);
	uart_puts("\r\n");
	
	sprintf(buf, "[Debug] Last Val : %d, Temp : %d\r\n", last_temp, temp);
	
	uart_puts(buf);
#endif
}

ISR(ADC_vect){
	ADCSRA |= (1 << ADIF);   // ADIFフラグをクリア
	
	const uint16_t val = TEMPSensor_readValue();
	
#ifdef _DEBUG_ISR
	char buf[50];
	
	sprintf(buf, "[Debug] ADC_vect called. Val : %d, Temp : %d\r\n", val, conv_to_temp(val));
	
	uart_puts(buf);
#endif
}

void init_port(){
	// 一旦、全ポートをプルアップ付き入力にする（未使用ピンの破壊防止）
	// そのあと、各機能のinit()にて出力設定をする
	DDRB = 0x00;	// 全入力
	PORTB = 0xFF;	// 全プルアップ
	DDRC = 0x00;	// 全入力
	PORTC = 0xFE;	// 全プルアップ
	DDRD = 0x00;	// 全入力
	PORTD = 0xFF;	// 全プルアップ
}

static time time_force_init(){

	time t ;

	t.second = 00;
	t.minute = 38;
	t.hour = 16;
	t.day = 11;
	t.month = 11;
	t.year = 2025;

	return t;
}

static void RTC_print_all_regs(){
	char buf[60];
	uint8_t reg_val[16];
	
	RTC_read_all_registers(reg_val, sizeof(reg_val));
	
	sprintf(buf, "[Debug] RTC regs : Control %02x %02x\r\n", reg_val[0], reg_val[1]);
	uart_puts(buf);
	
	sprintf(buf, "[Debug] Sec - Year : %02x %02x %02x %02x %02x %02x %02x\r\n", reg_val[2], reg_val[3], reg_val[4], reg_val[5], reg_val[6], reg_val[7], reg_val[8]);
	uart_puts(buf);
	
	sprintf(buf, "[Debug] Alarm : %02x %02x %02x %02x\r\n", reg_val[9], reg_val[10], reg_val[11], reg_val[12]);
	uart_puts(buf);
	
	sprintf(buf, "[Debug] CLKOUT & Timer : %02x %02x %02x\r\n", reg_val[13], reg_val[14], reg_val[15]);
	uart_puts(buf);
}


int main(void)
{
	init_port();
	
	// Debug用LED ON
	init_LEDs();
	set_LEDs();
	
	// UART 起動メッセージ
	uart_init();
	uart_puts("ATmega328P Clock booted\r\n");
	
	// SPI 初期化
	TC62D748_init();
	
	// スイッチ入力初期化（まだ割り込み不許可）
	init_switches();
	
	// RTC初期化
	RTC_init();

	// RTCのVLが1の場合、RTCの時刻が飛んでいる可能性。初期化しておく。
	if (RTC_read_VL() == 1) {
		uart_puts("[Info] RTC VL bit == 1, time may be wrong\r\n");
		
		RTC_clear_VL();
		RTC_set_project_default_setting();
	}

	// RTCから時刻取得
	// master_time = time_force_init();
	// RTC_write_time(master_time);

	master_time = RTC_read_time();
		
	// とりあえずIV-16に初期値表示
	TC62D748_send(master_time);
	
	// 温度センサー初期化
	TEMPSensor_init();
	
	_delay_ms(5);
	
	// RTCを読んだ直後から割り込みOKに
	init_1hz_interrupt();
	sei();

	// UARTに初期値送信
	char buf[50];
	time_to_str(master_time, buf, 50);
	
	uart_puts("Time recovered from RTC is ");
	uart_puts(buf);
	uart_puts("\r\n");
	
	// UARTにRTCの全レジスタ出力
	RTC_print_all_regs();
	
	// UART 受信待ち
	uart_puts("\r\n>");
	
    while (1) 
    {
		//const char recv_c = uart_getc();
		
		//uart_recv_process(recv_c);
		//uart_putc(recv_c);
		
		set_LED1();
		_delay_ms(500);
		
		clear_LED1();
		_delay_ms(500);
		
		// DEBUG用。+1sしてIV-16の表示を更新
		// -> 割り込み化したのでもう不要
		// master_time = time_countup(master_time);
		// TC62D748_send(master_time);
    }
	
	return 0;
}

