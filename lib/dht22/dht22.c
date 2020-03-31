//
// Created by Jerzy Drozdz on 24/03/2020.
//

#include "dht22.h"

#define __DHT_TEMPERATURE_DEC_BIT       2
#define __DHT_TEMPERATURE_FRAC_BIT      3
#define __DHT_HUMIDITY_DEC_BIT          0
#define __DHT_HUMIDITY_FRAC_BIT         1

enum DHT_Status_t __DHT_STATUS;

static double ExtractTemperature(uint8_t t1, uint8_t t2);
static double ExtractHumidity(uint8_t h1, uint8_t h2);

void DHT_Setup() {
    _delay_ms(__DHT_DELAY_SETUP);
    __DHT_STATUS = DHT_OK;
}

enum DHT_Status_t DHT_GetStatus() {
    return (__DHT_STATUS);
}

enum DHT_Status_t DHT_ReadRaw(uint8_t data[4]) {
    uint8_t buffer[5] = { 0, 0, 0, 0 };
    uint8_t retries, i;
    int8_t j;

    __DHT_STATUS = DHT_OK;

    if(__DHT_STATUS == DHT_OK) {
        DigitalWrite(DHT_PIN, Low);
        PinMode(DHT_PIN, Output);
        _delay_ms(DHT_READ_INTERVAL);

        DigitalWrite(DHT_PIN, High);
        PinMode(DHT_PIN, Input);

        retries = 0;

        while(DigitalRead(DHT_PIN)) {
            _delay_us(2);
            retries += 2;
            if(retries > 60) {
                __DHT_STATUS = DHT_ERROR_TIMEOUT;
                break;
            }
        }
    }

    if(__DHT_STATUS == DHT_OK) {
        retries = 0;
        while(!DigitalRead(DHT_PIN)) {
            _delay_us(2);
            retries += 2;
            if(retries > 100) {
                __DHT_STATUS = DHT_ERROR_TIMEOUT;
                break;
            }
        }

        retries = 0;
        while(DigitalRead(DHT_PIN)) {
            _delay_us(2);
            retries += 2;
            if(retries > 100) {
                __DHT_STATUS = DHT_ERROR_TIMEOUT;
                break;
            }
        }

        if(__DHT_STATUS == DHT_OK) {
            for(i = 0; i < 5; i++) {
                for (j = 7; j >= 0; j--) {
                    retries = 0;
                    while (!DigitalRead(DHT_PIN)) {
                        _delay_us(2);
                        retries += 2;
                        if(retries > 70) {
                            __DHT_STATUS = DHT_ERROR_TIMEOUT;
                            j = -1;
                            i = 5;
                            break;
                        }
                    }

                    if(__DHT_STATUS == DHT_OK) {
                        _delay_us(35);
                        if(DigitalRead(DHT_PIN)) {
                            BitSet(buffer[i], j);

                            retries = 0;
                            while(DigitalRead(DHT_PIN)) {
                                _delay_us(2);
                                retries += 2;
                                if(retries > 100) {
                                    __DHT_STATUS = DHT_ERROR_TIMEOUT;
                                    break;
                                }
                            }
                        }
                    }

                    if(__DHT_STATUS == DHT_OK) {
                        if((uint8_t)(buffer[0] + buffer[1] + buffer[2] + buffer[3]) != buffer[4]) {
                            __DHT_STATUS = DHT_ERROR_CRC;
                        } else {
                            for(i = 0;i < 4; i++) {
                                data[i] = buffer[i];
                            }
                        }
                    }
                }
            }
        }
    }

    return DHT_GetStatus();
}

enum DHT_Status_t DHT_GetTemperature(double *temperature) {
    double *waste = 0;
    return DHT_Read(temperature, waste);
}

enum DHT_Status_t DHT_GetHumidity(double *humidity) {
    double *waste = 0;
    return DHT_Read(waste, humidity);
}

enum DHT_Status_t DHT_Read(double *temperature, double *humidity) {
    uint8_t data[4] = { 0, 0, 0, 0 };
    enum DHT_Status_t status = DHT_ReadRaw(data);

    if(status == DHT_OK) {
        *temperature = ExtractTemperature(data[__DHT_TEMPERATURE_DEC_BIT], data[__DHT_TEMPERATURE_FRAC_BIT]);
        *humidity = ExtractHumidity(data[__DHT_HUMIDITY_DEC_BIT], data[__DHT_HUMIDITY_FRAC_BIT]);

        if(*temperature < DHT_MIN_TEMP || *temperature > DHT_MAX_TEMP) {
            __DHT_STATUS = DHT_ERROR_TEMPERATURE;
        } else if (*humidity < DHT_MIN_HUM || *humidity > DHT_MAX_HUM) {
            __DHT_STATUS = DHT_ERROR_HUMIDITY;
        }
    }

    return DHT_GetStatus();
}

double ExtractHumidity(uint8_t h1, uint8_t h2) {
    double hum = 0.0;

#if (DHT_TYPE == DHT11)
    hum = h2;
#elif (DHT_TYPE == DHT22)
    hum = ((h1 << 8) | h2) / 10.0;
#endif

    return hum;
}

double ExtractTemperature(uint8_t t1, uint8_t t2) {
    double t = 0.0;
#if (DHT_TYPE == DHT11)
    t = t = t2;
#elif (DHT_TYPE == DHT22)
    t = (BitCheck(t1, 7)) ? ( (((t1 & 0x7f) << 8) | t2) / -10.0 ) : ( ((t1 << 8) | t2)  / 10);
#endif

    return t;
}
