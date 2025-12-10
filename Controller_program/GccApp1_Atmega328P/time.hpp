
/*
 * time.hpp
 *
 * Created: 2025/10/13 20:05:01
 *  Author: fenrir_
 */

 #ifndef TIME_H_
 #define TIME_H_

typedef struct {
	unsigned int year;
	unsigned char month;
	unsigned char day;
	unsigned char hour;
	unsigned char minute;
	unsigned char second;
} time;

time time_init();
time str_to_time(const char*);
void time_to_str(const time, char *, const uint8_t);
unsigned char is_leap_year(const unsigned char);
time time_countup(const time);

bool check_time_format(const time);

#endif