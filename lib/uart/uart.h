/*
 * uart.h
 *
 *  Created on: 21 Mar 2020
 *      Author: jdrozdz
 */


/**********************************************
 * Example code:
 *
 *
 uart_init(9600,9600);
 while(1){
   char buf;
		if(uart_flag || uart_flag1){
			LCD_GoTo(0,0);
			LCD_WriteText("RXD ISR ");
			if(uart_flag > 0){
				LCD_WriteText("0");
				LCD_GoTo(0,1);
				buf = uart_getc(0);
				uart_putc(buf,1);
				LCD_WriteData(buf);
				uart_flag--;
			}
			if(uart_flag1 > 0){
				LCD_WriteText("1");
				LCD_GoTo(0,1);
				buf = uart_getc(1);
				uart_putc(buf,0);
				LCD_WriteData(buf);
				uart_flag1--;
			}
		}
	}
 *
 * *****************************************
 */

#ifndef LIB_UART_UART_H_
#define LIB_UART_UART_H_
#define UART_RX_INTERRUPT	1
#define UART_RX_BUF_SIZE	32
#define UART_RX_BUF_MASK	(UART_RX_BUF_SIZE - 1)

#define UART_BG	4
#define UART_FG	3

#define UART_BLACK		0
#define UART_RED		1
#define UART_GREEN		2
#define UART_YELLOW		3
#define UART_BLUE		4
#define UART_MAGENTA	5
#define UART_CYAN		6
#define UART_WHITE		7

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdio.h>

void uart_init(uint32_t, uint32_t);
void uart_putc(char, uint8_t);
void uart_puts(char*, uint8_t);
void uart_puts_P(const char*, uint8_t);
void uart_putint(int, int, uint8_t);
char uart_getc(uint8_t);
char* uart_gets(uint8_t);

void uart_goto(uint8_t, uint8_t, uint8_t);
void uart_cls(uint8_t);
void uart_setColor(uint8_t, uint8_t, uint8_t);
void uart_reset(uint8_t);
void UART_RX_STR_EVENT(char*, uint8_t);

void register_uart_str_rx_event_callback(void (*callback)(char * pBuf));
void register_uart_str_rx_event_callback0(uint8_t (*callback)(char * pBuf));

extern volatile uint8_t uart_flag, uart_flag1;
#endif /* LIB_UART_UART_H_ */
