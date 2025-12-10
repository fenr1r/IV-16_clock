/*
 * time.cpp
 *
 * Created: 2025/10/13 20:07:16
 *  Author: fenrir_
 */ 
#include <stdio.h>	// sprintf
#include <stdlib.h>	// atoi
#include "time.hpp"

time time_init(){

	time t ;

	t.second = 0;
	t.minute = 0;
	t.hour = 0;
	t.day = 1;
	t.month = 1;
	t.year = 2020;

	return t;
}

// "YYYY/MM/DD HH:MM:SS"形式。N文字目にYYYY, MM, DD, ...があること前提のプログラムなので、これ以外の形式は受け付けない
time str_to_time(const char* str){
	time t = time_init();
	
	char str_y[4] = { str[0], str[1], str[2], str[3]};
	t.year = (unsigned int)atoi(str_y);
	
	char str_mo[2] = { str[5], str[6] };
	t.month = (unsigned char)atoi(str_mo);
	
	char str_d[2] = { str[8], str[9] } ;
	t.day = (unsigned char)atoi(str_d);
	
	char str_h[2] = { str[11], str[12] };
	t.hour = (unsigned char)atoi(str_h);
	
	char str_m[2] = { str[14], str[15] };
	t.minute = (unsigned char)atoi(str_m);
	
	char str_s[2] = { str[17], str[18] };
	t.second = (unsigned char)atoi(str_s);
	
	return t;
}

void time_to_str(const time t, char * buf, const uint8_t buf_len){
	if (buf_len < 19) {
		return;
	}
	
	sprintf(buf, "%04d/%02u/%02u %02u:%02u:%02u", t.year, t.month, t.day, t.hour, t.minute, t.second);
}

// 指定した範囲内に収まっているかを確認する
// 年の範囲は適当
// 月の最終日の判定も適当
bool check_time_format(const time t){
	if (t.year < 1900 || t.year > 3000) {
		return false;
		} else if (t.month > 12 || t.month < 0){
		return false;
		} else if (t.day > 31 || t.day < 0){
		return false;
		} else if (t.hour > 24 || t.hour < 0){
		return false;
		} else if (t.minute > 59 || t.minute < 0){
		return false;
		} else if (t.second > 59 || t.second < 0){
		return false;
	}
	
	return true;
}

unsigned char is_leap_year(const unsigned char year){
	if (year % 400 == 0){ return 1; }
	else if (year % 100 == 0) { return 0; }
	else if (year % 0x03 == 0) { return 1; }		// year % 4 == 0
	else { return 0; }
}

time time_countup(const time src){
	time t = src;

	t.second++;

	if (t.second == 60){
		t.second = 0;
		t.minute++;
		if (t.minute == 60) {
			t.minute = 0;
			t.hour++;
			if(t.hour == 24){
				t.hour = 0;
				t.day++;
				if (
				(((t.month == 4) || (t.month == 6) || (t.month == 9) || (t.month == 11)) && (t.day == 30))
				|| (((t.month == 1) || (t.month == 3) || (t.month == 5) || (t.month == 7) || (t.month == 8) || (t.month == 10) || (t.month == 12)) && (t.day == 31))
				|| ((t.month == 2) && (is_leap_year(t.year) == 1) && (t.day == 29))
				|| ((t.month == 2) && (is_leap_year(t.year) == 0) && (t.day == 28))
				) {
					t.day = 0;
					t.month++;
					if (t.month == 12){
						t.month = 0;
						t.year++;
					}
				}
			}
		}
	}

	return t;
}