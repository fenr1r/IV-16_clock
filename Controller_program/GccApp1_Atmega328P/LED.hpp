/*
 * LED.h
 *
 * Created: 2025/11/11 2:31:44
 *  Author: fenrir_
 */ 


// MEMO
// 
// LED1 : PB0
// LED2 : PD7
// LED3 : PD6
// LED4 : PD5


#ifndef LED_H_
#define LED_H_

#define BOOL int
#define TRUE 1
#define FALSE 0

void init_LEDs();
void clear_LEDs();
void set_LEDs();
void set_LEDs(const uint8_t);

void set_LED1(const BOOL);
void set_LED2(const BOOL);
void set_LED3(const BOOL);
void set_LED4(const BOOL);

void set_LED1();
void clear_LED1();
void set_LED2();
void clear_LED2();
void toggle_LED2();
void set_LED3();
void clear_LED3();
void toggle_LED3();
void set_LED4();
void clear_LED4();

#endif /* LED_H_ */