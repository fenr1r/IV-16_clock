/*
 * uart.cpp
 *
 * Created: 2025/10/14 2:43:43
 *  Author: fenrir_
 */ 

#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "time.hpp"
#include "TC62D748.hpp"
#include "RTC.hpp"
#include "uart.hpp"

// Device dependent
// https://threesons-technicalmemo.blogspot.com/2014/12/2.html

ISR(USART_RX_vect){
	const char recv_c = UDR0;
	
//	#ifdef _DEBUG_ISR
//	uart_puts("[Debug] UART0 Rx irq occured.\r\n");
//	#endif
	
	uart_recv_process(recv_c);
	
	return ;
}

void uart_init()
{
	//USART 0 制御/状態レジスタ A の設定
	UCSR0A = 0x00;//倍速不使用、複数プロッサ動作不使用

	//USART 0 制御/状態レジスタ C の設定
	//非同期動作、パリティなし 、ストップビット1
	//データ長を8ビット（UCSZ02ビットは、UCSR0Bにある）
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);

	//USARTボーレートレジスタの設定
	// UBBR0 = (freq / (baud * 16) - 1)
	// 0x33 : CPUが8MHz, 9600bps
	// 0x03 : CPU 8MHz, 115200bps (125000bpsになる）
	// データシートのTable 19-11.参照
	UBRR0L = 12;
	UBRR0H = 0x00;

	//送受信 ON, 受信割り込み ON
	UCSR0B = (1 << RXCIE0) | (1 << RXEN0) | (1 << TXEN0); 
}

// 取得されるまで待つタイプ
void uart_putc(const char c){
	while( !(UCSR0A & (1<<UDRE0)) );
	UDR0 = c;
}

// 送信されるまでブロッキング
char uart_getc() {
	while( !(UCSR0A & (1<<RXC0)) );
	return UDR0;
}

// 送信されるまでブロッキング
void uart_puts(const char* str){
	while (*str){
		uart_putc(*str++);
	}
}

// Device non-dependent
char uart_recv_buf[UART_RECV_BUF_LEN] = {'\0'};

uint8_t uart_recv_line_ready_flg = 0;
uint8_t uart_recv_state = 0;
uint8_t uart_recv_buf_cnt = 0;

time uart_recv_time;

extern time master_time;

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

static void uart_buf_clear(){
	uart_recv_buf_cnt = 0;
	
	memset(uart_recv_buf, 0, sizeof(uart_recv_buf));
	uart_recv_buf[0] = '\0';
}

static void uart_buf_add(const char c){
	uart_recv_buf[uart_recv_buf_cnt] = c;
	uart_recv_buf_cnt++;
}

static void uart_buf_init(){
	uart_buf_clear();
}

// 'r'	RTCのレジスタを時間部分のみ読み込み、UARTで送信。
// 'R'	RTCのレジスタを全て読み込み、UARTで送信。
// 'w'	RTCへの時刻セット。入力を促すメッセージをUARTで送信。ステートを変える
// 't'	RAM上の時刻データをUARTで送信。
// 'h'	ヘルプを表示
void uart_recv_process(const char recv_c){
	
	uart_putc(recv_c);
	
	if (uart_recv_state == UART_RECV_STATE_NORMAL){
		
		if (recv_c == 'r') {
			uart_puts("\r\n");
			master_time = RTC_read_time();

			uart_puts("Time recovered from RTC is ");
			
			char buf[30] = {'\0'};
			time_to_str(master_time, buf, 30);

			uart_puts(buf);
			uart_puts("\r\n>");
		} else if (recv_c == 'R') {
			uart_puts("\r\n");
			
			RTC_print_all_regs();
			uart_puts("\r\n>");
		} else if (recv_c == 'h') {
			uart_puts("\r\n");
			uart_puts("HELP\r\n");
			uart_puts("'r' : Read time registers of RTC-8564\r\n");
			uart_puts("'R' : Read all registers of RTC-8564\r\n");
			uart_puts("'w' : Write time registers of RTC-8564\r\n");
			uart_puts("'t' : Read time value on RAM\r\n");
			uart_puts("'h' : help\r\n>");
		} else if (recv_c == 't') {
			uart_puts("\r\n");
			uart_puts("Time value on RAM is ");
			
			char buf[30] = {'\0'};
			time_to_str(master_time, buf, 30);

			uart_puts(buf);
			uart_puts("\r\n>");
		} else if (recv_c == 'w') {
			uart_puts("w\r\n");
			uart_puts("Write time to RTC mode.\r\n");
			uart_puts("Input \"YYYY/MM/DD HH:MM:SS\" and Press Enter\r\n>");
			
			uart_recv_state = UART_RECV_STATE_TIME_SET;
			
			// 念の為のクリア
			uart_buf_clear();
		}
	} else if (uart_recv_state == UART_RECV_STATE_TIME_SET) {
		if ((recv_c == '\n') || (recv_c == '\0')) {
			uart_recv_buf[uart_recv_buf_cnt] = '\0';
			
			uart_puts(uart_recv_buf);
			
			uart_recv_time = str_to_time(uart_recv_buf);
			
			char buf[30] = {'\0'};
			time_to_str(uart_recv_time, buf, 30);
			
			uart_puts("\r\nYour input is :\r\n");
			uart_puts(buf);
			uart_puts("\r\n");
			
			if (check_time_format(uart_recv_time)){
				uart_puts("OK to proceed? (y/n)\r\n>");
			
				uart_recv_state = UART_RECV_STATE_TIME_SET_CHK;
			} else {
				uart_puts("Your input exceed the limit. Write cancelled.\r\n>");
				
				uart_recv_state = UART_RECV_STATE_NORMAL;
			}
			
			uart_buf_clear();
			
		} else {
			uart_buf_add(recv_c);
		}
	} else if (uart_recv_state == UART_RECV_STATE_TIME_SET_CHK) {
			if ((recv_c == 'Y') || (recv_c == 'y')) {
				uart_puts("\r\nBegin writing...\r\n");
				
				RTC_write_time(uart_recv_time);
				
				uart_puts("\r\nEnd\r\n>");
				
				// RAMへも即時反映
				master_time = uart_recv_time;
				
			} else if ((recv_c == 'N') || (recv_c == 'n')) {
				uart_puts("\r\nWrite cancelled.\r\n>");
			} else {
				uart_puts("\r\nBad input. Write cancelled.\r\n>");
			}
			
			uart_recv_state = UART_RECV_STATE_NORMAL;
			
			uart_buf_clear();
	} else {
		// Exception
		uart_recv_state = UART_RECV_STATE_NORMAL;
		
		uart_buf_clear();
	}
	
}