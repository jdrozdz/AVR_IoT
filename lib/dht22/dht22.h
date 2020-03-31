/*
 * dht22.h
 *
 *  Created on: 24 Mar 2020
 *      Author: jdrozdz
 */

#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>
#include "dht_config.h"
#include "../conf/macros.h"

#ifndef LIB_DHT22_DHT22_H_
#define LIB_DHT22_DHT22_H_

#ifndef F_CPU
#define F_CPU 8000000
#endif

#define __DHT_DELAY_SETUP    2000

enum DHT_Status_t {
    DHT_OK,
    DHT_ERROR_HUMIDITY,
    DHT_ERROR_TEMPERATURE,
    DHT_ERROR_CRC,
    DHT_ERROR_TIMEOUT
};

void DHT_Setup();

enum DHT_Status_t DHT_GetStatus();

enum DHT_Status_t DHT_ReadRaw(uint8_t data[4]);

enum DHT_Status_t DHT_GetTemperature(double *temperature);

enum DHT_Status_t DHT_GetHumidity(double *humidity);

enum DHT_Status_t DHT_Read(double *temperature, double *humidity);

#endif /* LIB_DHT22_DHT22_H_ */
