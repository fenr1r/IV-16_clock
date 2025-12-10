/*
 * TEMPSensor.h
 *
 * Created: 2025/11/11 12:41:56
 *  Author: fenrir_
 */ 


#ifndef TEMPSENSOR_H_
#define TEMPSENSOR_H_

void TEMPSensor_init();
void TEMPSensor_startConvert();
uint16_t TEMPSensor_readValue();
uint16_t TEMPSensor_getLastValue();
// void TEMPSensor_stop();	// –¢ŽÀ‘•
uint8_t conv_to_temp(const uint16_t);


#endif /* TEMPSENSOR_H_ */