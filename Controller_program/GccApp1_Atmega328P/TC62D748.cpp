/*
 * TC62D748.cpp
 *
 * Created: 2025/10/13 20:00:19
 *  Author: fenrir_
*/ 

#include <avr/io.h>

#include "time.hpp"
#include "TC62D748.hpp"

// nSLAT (OUT) : PB1
// nOE (OUT) : PB2
// MOSI (OUT) : PB3
// MISO (IN) : PB4 (unused)
// SCK (OUT) : PB5

static bool isDisplay;

static void nSLAT_on(){
	PORTB	|= (1 << PORTB1);
}

static void nSLAT_off(){
	PORTB	&= ~(1 << PORTB1);
}

static void nOE_on(){
	PORTB	|= (1 << PORTB2);
}

static void nOE_off(){
	PORTB	&= ~(1 << PORTB2);
}

static unsigned char conv_dec_to_7seg(const unsigned char dec){
	switch (dec){
		// G F E D C B Aの順番で送る。右詰めで定義している（最上位ビットは0）。1の位は1bit左シフトする
		case 0: return 0b00111111;
		case 1: return 0b00000110;
		case 2: return 0b01011011;
		case 3: return 0b01001111;
		case 4: return 0b01100110;
		case 5: return 0b01101101;
		case 6: return 0b01111101;
		case 7: return 0b00100111;
		case 8: return 0b01111111;
		case 9: return 0b01101111;
		default : return 0b00000000;
	}
}

static void SPI_MasterTransmit(char cData)
{
	/* 転送を開始 */
	SPDR = cData;
	/* 転送完了を待つ */
	while(!(SPSR & (1<<SPIF))){}
	
	return;
}

void TC62D748_EnableDisplay(){
	isDisplay = true;
}

void TC62D748_DisableDisplay(){
	isDisplay = false;
}

void TC62D748_init()
{
	// MOSI, SCK, nOE, nSLATを出力に、SCKを入力に設定
	DDRB	|= (1 << DDB1) | (1 << DDB2) | (1 << DDB3) | (1 << DDB5);
	
	// 出力ポートは、とりあえず0にしておく
	PORTB	|= (1 << PORTB5) | (1 << PORTB3) | (1 << PORTB2) | (1 << PORTB1);
	
	// SPIをマスターとして有効に設定、クロック・レートをfck/16に設定
	SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);
	
	// 初期は消灯設定
	isDisplay = false;
}

void TC62D748_send(const time t){
	nOE_on();		// disable output
	nSLAT_off();
	
	//SCK = 0;
	//MOSI = 0;

	// 1の位から先に送る。1の位はシフト必要
	SPI_MasterTransmit((conv_dec_to_7seg((unsigned char)(t.hour % 10)) << 1) & 0xFE);
	SPI_MasterTransmit(conv_dec_to_7seg((unsigned char)(t.hour / 10)));
	
	// 1の位から先に送る。1の位はシフト必要
	SPI_MasterTransmit((conv_dec_to_7seg((unsigned char)(t.minute % 10)) << 1) & 0xFE);
	SPI_MasterTransmit(conv_dec_to_7seg((unsigned char)(t.minute / 10)));
	
	// 1の位から先に送る。1の位はシフト必要
	SPI_MasterTransmit((conv_dec_to_7seg((unsigned char)(t.second % 10)) << 1) & 0xFE);
	SPI_MasterTransmit(conv_dec_to_7seg((unsigned char)(t.second / 10)));

	// nSLAT on-off
	nSLAT_on();
	nSLAT_off();

	if (isDisplay) {
		// nOE enable
		nOE_off();
	}

	return;
}