//
// Created by jdrozdz on 24/03/2020.
//

#include "dht22.h"

enum DHT_Status_t __DHT_STATUS;

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
    }
}
