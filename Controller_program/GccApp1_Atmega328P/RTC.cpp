/*
 * RTC.cpp
 *
 * Created: 2025/10/14 1:23:39
 *  Author: fenrir_
 */ 

#include <stdint.h>
#include "RTC.hpp"
#include "time.hpp"

#include "twi_master.hpp"


// デバイス依存（非公開）

static void ERROR_CHECK(ret_code_t error_code)
{
	if (error_code != SUCCESS)
	{
		/* Print error code and loop indefinitely until reset */
		// printf(BR "App error! error_code = 0x%02X\n" RESET, error_code);
		// while (1); // loop indefinitely
	}
}

static ret_code_t _RTC_write(const uint8_t I2C_ADDR_WR, const uint8_t reg_addr, const uint8_t val){
	ret_code_t error_code;
	
	uint8_t data[2];
	
	data[0] = reg_addr;
	data[1] = val;
	
	error_code = tw_master_transmit(RTC_ADDR, data, sizeof(data), false);	// repeated-startしない
	
	ERROR_CHECK(error_code);
	
	return error_code;
}

static uint8_t _RTC_readByte(const uint8_t I2C_ADDR_WR, const uint8_t I2C_ADDR_RD, const uint8_t reg_addr){
	ret_code_t error_code;
	
	// 先に、読みたいレジスタのアドレスを送る。repeated-startで止める
	uint8_t send_data = reg_addr;
			
	error_code = tw_master_transmit(RTC_ADDR, &send_data, sizeof(send_data), true);
	
	ERROR_CHECK(error_code);
	
	// 次に、読み出すByte分だけ、読み出す
	#define RECV_DATA_LEN 1
	
	uint8_t recv_data[RECV_DATA_LEN];
	
	for (int i = 0; i < RECV_DATA_LEN; i++){
		recv_data[i] = 0;
	}
	
	error_code = tw_master_receive(RTC_ADDR, recv_data, sizeof(recv_data));
	
	ERROR_CHECK(error_code);
	
	return recv_data[0];
}

static void _RTC_read16Byte(uint8_t * recv_data){
	ret_code_t error_code;
	
	// 先に、読みたいレジスタのアドレスを送る。repeated-startで止める
	uint8_t send_data = 0x00;
	
	error_code = tw_master_transmit(RTC_ADDR, &send_data, sizeof(send_data), true);
	
	ERROR_CHECK(error_code);
	
	// 次に、読み出すByte分だけ、読み出す
	#define RECV_DATA_LEN 16
	
	// uint8_t recv_data[RECV_DATA_LEN];
	
	//for (int i = 0; i < RECV_DATA_LEN; i++){
	//	recv_data[i] = 0;
	//}
	
	error_code = tw_master_receive(RTC_ADDR, recv_data, sizeof(recv_data));
	
	ERROR_CHECK(error_code);
}

// デバイス非依存（公開）
void RTC_init(){
	// 400kHz、内部プルアップは使用しない
	tw_init(TW_FREQ_400K, false);
	
	// RTC_1HZ_Intピンは、プルアップなしにする
	PORTD &= ~(1 << PORTD2);
}
	
void RTC_set_project_default_setting(){
	ret_code_t error_code;
	
	error_code = _RTC_write(RTC_ADDR_WR, RTC_REG_ADDR_CTRL1,			0x00);
	error_code = _RTC_write(RTC_ADDR_WR, RTC_REG_ADDR_CTRL2,			0x00);
	error_code = _RTC_write(RTC_ADDR_WR, RTC_REG_ADDR_CLKOUT_FREQ,		0b10000011);	// 1Hz output enable
	error_code = _RTC_write(RTC_ADDR_WR, RTC_REG_ADDR_TIMER_CTRL,		0x00);		// Timer off
	
	// Alarm disable
	error_code = _RTC_write(RTC_ADDR_WR, RTC_REG_ADDR_ALARM_MIN,		0x00);	
	error_code = _RTC_write(RTC_ADDR_WR, RTC_REG_ADDR_ALARM_HOUR,		0x00);
	error_code = _RTC_write(RTC_ADDR_WR, RTC_REG_ADDR_ALARM_DAY,		0x00);
	error_code = _RTC_write(RTC_ADDR_WR, RTC_REG_ADDR_ALARM_WEEKDAY,	0x00);
	
	ERROR_CHECK(error_code);
}

void RTC_read_all_registers(uint8_t * buf, const uint8_t buf_len){
	if (buf_len < 16) {
		return;
	}
	
	//	_RTC_read16Byte(buf);
	for (int i = 0; i < 16; i++){
		buf[i] = _RTC_readByte(RTC_ADDR_WR, RTC_ADDR_RD, i);
	}
	
	buf[0]	&= RTC_REG_MASK_0;
	buf[1]	&= RTC_REG_MASK_1;
	buf[2]	&= RTC_REG_MASK_2;
	buf[3]	&= RTC_REG_MASK_3;
	buf[4]	&= RTC_REG_MASK_4;
	buf[5]	&= RTC_REG_MASK_5;
	buf[6]	&= RTC_REG_MASK_6;
	buf[7]	&= RTC_REG_MASK_7;
	buf[8]	&= RTC_REG_MASK_8;
	buf[9]	&= RTC_REG_MASK_9;
	buf[10]	&= RTC_REG_MASK_A;
	buf[11]	&= RTC_REG_MASK_B;
	buf[12]	&= RTC_REG_MASK_C;
	buf[13]	&= RTC_REG_MASK_D;
	buf[14]	&= RTC_REG_MASK_E;
	buf[15]	&= RTC_REG_MASK_F;
}

time RTC_read_time(){

	time t;
	
	const unsigned char seconds = _RTC_readByte(RTC_ADDR_WR, RTC_ADDR_RD, RTC_REG_ADDR_SEC);
	const unsigned char minutes = _RTC_readByte(RTC_ADDR_WR, RTC_ADDR_RD, RTC_REG_ADDR_MIN);
	const unsigned char hours	= _RTC_readByte(RTC_ADDR_WR, RTC_ADDR_RD, RTC_REG_ADDR_HOUR);
	const unsigned char days	= _RTC_readByte(RTC_ADDR_WR, RTC_ADDR_RD, RTC_REG_ADDR_DAY);
	// 0x06はweekdays
	const unsigned char months	= _RTC_readByte(RTC_ADDR_WR, RTC_ADDR_RD, RTC_REG_ADDR_MONTH);
	const unsigned char years	= _RTC_readByte(RTC_ADDR_WR, RTC_ADDR_RD, RTC_REG_ADDR_YEAR);

	// RTC形式（10進数）から、普通のucharに変換する

	t.second	= CONV_RTC_SEC_TO_UCHAR(seconds);
	t.minute	= CONV_RTC_MIN_TO_UCHAR(minutes);
	t.hour		= CONV_RTC_HOUR_TO_UCHAR(hours);
	t.day		= CONV_RTC_DAY_TO_UCHAR(days);
	t.month		= CONV_RTC_MONTH_TO_UCHAR(months);
	t.year		= 2000 + CONV_RTC_YEAR_TO_UCHAR(years);

	return t;
}

void RTC_write_time(const time t){
	_RTC_write(RTC_ADDR_WR, RTC_REG_ADDR_SEC, CONV_UCHAR_TO_RTC_SEC(t.second));
	_RTC_write(RTC_ADDR_WR, RTC_REG_ADDR_MIN, CONV_UCHAR_TO_RTC_MIN(t.minute));
	_RTC_write(RTC_ADDR_WR, RTC_REG_ADDR_HOUR, CONV_UCHAR_TO_RTC_HOUR(t.hour));
	_RTC_write(RTC_ADDR_WR, RTC_REG_ADDR_DAY, CONV_UCHAR_TO_RTC_DAY(t.day));
	
	_RTC_write(RTC_ADDR_WR, RTC_REG_ADDR_MONTH, CONV_UCHAR_TO_RTC_MONTH(t.month));
	
	// 年は下2桁を渡す
	const uint8_t year_for_rtc = (uint8_t)(t.year % 100);
	
	_RTC_write(RTC_ADDR_WR, RTC_REG_ADDR_YEAR, CONV_UCHAR_TO_RTC_YEAR(year_for_rtc));

	return;
}

unsigned char RTC_read_VL(){
	const unsigned char second = _RTC_readByte(RTC_ADDR_WR, RTC_ADDR_RD, RTC_REG_ADDR_SEC);

	if ((second & 0x80) != 0x00) {
		return 1;
		} else {
		return 0;
	}
}

void RTC_clear_VL(){
	const unsigned char second = _RTC_readByte(RTC_ADDR_WR, RTC_ADDR_RD, RTC_REG_ADDR_SEC);
	
	_RTC_write(RTC_ADDR_WR, RTC_REG_ADDR_SEC, 0x7F & CONV_UCHAR_TO_RTC_SEC(second));
}