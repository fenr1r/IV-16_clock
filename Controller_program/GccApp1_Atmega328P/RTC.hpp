/*
 * RTC.h
 *
 * Created: 2025/10/14 1:24:11
 *  Author: fenrir_
 */ 

#include "time.hpp"

#ifndef RTC_H_
#define RTC_H_

#define RTC_ADDR	0b01010001
#define RTC_ADDR_WR 0b10100010	// addr 1010001 + Write (0)
#define RTC_ADDR_RD 0b10100011	// addr 1010001 + Read (1)

#define RTC_REG_ADDR_CTRL1			0x00
#define RTC_REG_ADDR_CTRL2			0x01
#define RTC_REG_ADDR_SEC			0x02
#define RTC_REG_ADDR_MIN			0x03
#define RTC_REG_ADDR_HOUR			0x04
#define RTC_REG_ADDR_DAY			0x05
#define RTC_REG_ADDR_WEEKDAY		0x06
#define RTC_REG_ADDR_MONTH			0x07
#define RTC_REG_ADDR_YEAR			0x08
#define RTC_REG_ADDR_ALARM_MIN		0x09
#define RTC_REG_ADDR_ALARM_HOUR		0x0A
#define RTC_REG_ADDR_ALARM_DAY		0x0B
#define RTC_REG_ADDR_ALARM_WEEKDAY	0x0C
#define RTC_REG_ADDR_CLKOUT_FREQ	0x0D
#define RTC_REG_ADDR_TIMER_CTRL		0x0E
#define RTC_REG_ADDR_TIMER			0x0F

#define RTC_REG_MASK_0	0xFF
#define RTC_REG_MASK_1	0xBF
#define RTC_REG_MASK_2	0xFF
#define RTC_REG_MASK_3	0x7F
#define RTC_REG_MASK_4	0x3F
#define RTC_REG_MASK_5	0x3F
#define RTC_REG_MASK_6	0x08
#define RTC_REG_MASK_7	0x9F
#define RTC_REG_MASK_8	0xFF
#define RTC_REG_MASK_9	0xFF
#define RTC_REG_MASK_A	0xBF
#define RTC_REG_MASK_B	0xBF
#define RTC_REG_MASK_C	0x87
#define RTC_REG_MASK_D	0x83
#define RTC_REG_MASK_E	0x83
#define RTC_REG_MASK_F	0xFF

#define CONV_BCD_TO_UCHAR(u) (((u & 0xF0) >> 4) * 10 + (u & 0x0F))
#define CONV_RTC_SEC_TO_UCHAR(u)	( CONV_BCD_TO_UCHAR(u & 0x7F) )
#define CONV_RTC_MIN_TO_UCHAR(u)	( CONV_BCD_TO_UCHAR(u & 0x7F) )
#define CONV_RTC_HOUR_TO_UCHAR(u)	( CONV_BCD_TO_UCHAR(u & 0x3F) )
#define CONV_RTC_DAY_TO_UCHAR(u)	( CONV_BCD_TO_UCHAR(u & 0x3F) )
#define CONV_RTC_MONTH_TO_UCHAR(u)	( CONV_BCD_TO_UCHAR(u & 0x1F) )
#define CONV_RTC_YEAR_TO_UCHAR(u)	( CONV_BCD_TO_UCHAR(u) )

#define CONV_UCHAR_TO_BCD(u)		( ((((unsigned char)(u / 10)) << 4) & 0xF0) | ((u % 10) & 0x0F) )
#define CONV_UCHAR_TO_RTC_SEC(u) 	( CONV_UCHAR_TO_BCD(u) & RTC_REG_MASK_2 )
#define CONV_UCHAR_TO_RTC_MIN(u) 	( CONV_UCHAR_TO_BCD(u) & RTC_REG_MASK_3 )
#define CONV_UCHAR_TO_RTC_HOUR(u) 	( CONV_UCHAR_TO_BCD(u) & RTC_REG_MASK_4 )
#define CONV_UCHAR_TO_RTC_DAY(u) 	( CONV_UCHAR_TO_BCD(u) & RTC_REG_MASK_5 )
#define CONV_UCHAR_TO_RTC_MONTH(u) 	( CONV_UCHAR_TO_BCD(u) & RTC_REG_MASK_7 )
#define CONV_UCHAR_TO_RTC_YEAR(u) 	( CONV_UCHAR_TO_BCD(u) )

// デバイス非依存（公開）
void RTC_init();
void RTC_set_project_default_setting();
void RTC_read_all_registers(uint8_t*, const uint8_t);
time RTC_read_time();
void RTC_write_time(const time);
unsigned char RTC_read_VL();
void RTC_clear_VL();



#endif /* RTC_H_ */