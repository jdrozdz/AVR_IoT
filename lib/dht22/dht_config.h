//
// Created by jdrozdz on 24/03/2020.
//

#ifndef AVR_DHT_CONFIG_H
#define AVR_DHT_CONFIG_H

#define DHT_PIN    A, 0
#define DHT22      22
#define DHT11      11
#define DHT_TYPE   DHT22

#if (DHT_TYPE == DHT22)
    #define DHT_MIN_TEMP    -40
    #define DHT_MAX_TEMP    80
    #define DHT_MIN_HUM     0
    #define DHT_MAX_HUM     100
    #define DHT_READ_INTERVAL    20
#elif (DHT_TYPE == DHT11)
    #define DHT_MIN_TEMP    0
    #define DHT_MAX_TEMP    50
    #define DHT_MIN_HUM     20
    #define DHT_MAX_HUM     90
    #define DHT_READ_INTERVAL    50
#endif

#endif //AVR_DHT_CONFIG_H
