//
// Created by jdrozdz on 31/03/2020.
//

#ifndef AVR_RELAYS_MOD_H
#define AVR_RELAYS_MOD_H

#include <avr/io.h>

#define RELAY_DIR			DDRC
#define RELAY_PORT			PORTC
#define RELAY1				PC3
#define RELAY2				PC2
#define RELAY3				PC1
#define RELAY4				PC0
#define RELAY_DISABLE_ALL()	(RELAY_PORT |= (1 << RELAY1) | (1 << RELAY2) | (1 << RELAY3) | (1 << RELAY4))
#define RELAY_CONFIG()		(RELAY_DIR |= (1 << RELAY1) | (1 << RELAY2) | (1 << RELAY3) | (1 << RELAY4))
#define RELAY_ENABLE(x) 	(RELAY_PORT &= ~(1 << x))
#define RELAY_DISABLE(x) 	(RELAY_PORT |= (1 << x))
#define RELAY_STATUS(x)		(!(PINC & 1 << x))
#define RELAY_STATUS_STRING(x)		RELAY_STATUS(x) == 1 ? "<input type='checkbox' data-name='switch-button' checked data-toggle='toggle' data-onstyle='success' data-offstyle='danger' />" : "<input type='checkbox' data-name='switch-button' data-toggle='toggle' data-onstyle='success' data-offstyle='danger' />"

#endif //AVR_RELAYS_MOD_H
