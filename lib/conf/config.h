#ifndef CONFIG_H
#define CONFIG_H

#include <avr/io.h>

#define SPI_DDR  DDRB   // Use Data Direction Register of Port B
#define SPI_PORT PORTB  // Use Port B for Serial Peripheral Interface
#define SPI_CS   4      // Use Pin 2 for Chip Select
#define SPI_MOSI 5      // Use Pin 3 for Master Out Slave In
#define SPI_MISO 6      // Use Pin 4 for Master In Slave out
#define SPI_SCK  7      // Use Pin 5 for Serial Clock

#endif
