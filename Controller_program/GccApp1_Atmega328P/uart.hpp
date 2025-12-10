/*
 * uart.hpp
 *
 * Created: 2025/10/14 2:43:34
 *  Author: fenrir_
 */ 


#ifndef UART_H_
#define UART_H_

// Device dependent
void uart_init();
void uart_putc(const char);
char uart_getc();
void uart_puts(const char*);

// Device non-dependent
#define UART_RECV_STATE_NORMAL		0x01
#define UART_RECV_STATE_TIME_SET	0x02
#define UART_RECV_STATE_TIME_SET_CHK	0x04

#define UART_RECV_BUF_LEN 32

void uart_recv_process(const char);

#endif /* UART_H_ */