/*
 * uart.c
 *
 *  Created on: 21 Mar 2020
 *      Author: jdrozdz
 */

#include "uart.h"

volatile char UART_RxBuf[UART_RX_BUF_SIZE];
volatile uint8_t UART_RxHead;
volatile uint8_t UART_RxTail;
volatile char UART_RxBuf1[UART_RX_BUF_SIZE];
volatile uint8_t UART_RxHead1;
volatile uint8_t UART_RxTail1;

volatile uint8_t uart_flag, uart_flag1;
char buf[20];

void uart_init(uint32_t baud0, uint32_t baud1){
	uint16_t _ubrr = 0;
	if(baud0 >= 2400){
		_ubrr = ((F_CPU+baud0*8UL) / (16UL*baud0)-1);
		UBRR0H |= (unsigned char)(_ubrr >> 8);
		UBRR0L |= (unsigned char)_ubrr;
		UCSR0B |= (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0) | (1 << RXCIE1);
		UCSR0C |= (3 << UCSZ00);
	}

	if(baud1 >= 2400){
		_ubrr = ((F_CPU+baud1*8UL) / (16UL*baud1)-1);
		UBRR1H |= (unsigned char)(_ubrr >> 8);
		UBRR1L |= (unsigned char)_ubrr;
		UCSR1B |= (1 << RXEN1) | (1 << TXEN1) | (1 << RXCIE1);
		UCSR1C |= (3 << UCSZ10);
	}
}

void uart_putc(char data, uint8_t uart){
	if(uart == 0){
		while ( !( UCSR0A & (1<<UDRE0)) );
		UDR0 = data;
	}
	if(uart == 1){
		while ( !( UCSR1A & (1<<UDRE1)) );
		UDR1 = data;
	}
}

void uart_puts(char *data, uint8_t uart){
	register char c;
	while((c = *data++)){
		uart_putc(c, uart);
	}
}

// Send data from FLASH to UART
void uart_puts_P(const char *data, uint8_t uart){
	register char c;
	while((c = pgm_read_byte(data++))){
		uart_putc(c, uart);
	}
}

void uart_putint(int x, int radx, uint8_t uart){
	char buff[20];
	// itoa(x, buff, radx);
	sprintf(buff, "%d", x);
	uart_puts(buff, uart);
}

char uart_getc(uint8_t uart){
	char tmp;
#if UART_RX_INTERRUPT == 0
	if(uart == 0){
		while ( !(UCSR0A & (1<<RXC0)) );
		tmp = UDR0;
	}

	if(uart == 1){
		while ( !(UCSR1A & (1<<RXC1)) );
		tmp = UDR1;
	}

	return tmp;

#else

	if(uart == 0){
		if(UART_RxHead == UART_RxTail)
			return 0;
		UART_RxTail = (UART_RxTail+1) & UART_RX_BUF_MASK;

		tmp = UART_RxBuf[UART_RxTail];
	}

	if(uart == 1){
		if(UART_RxHead1 == UART_RxTail1)
			return 0;
		UART_RxTail1 = (UART_RxTail1+1) & UART_RX_BUF_MASK;
		tmp = UART_RxBuf1[UART_RxTail1];
	}
#endif
	return tmp;
}

void uart_goto(uint8_t x, uint8_t y, uint8_t uart){
	// 0x1b
	sprintf(buf,"\033[%d;%dH",y,x);
	uart_puts(buf, uart);
}

void uart_cls(uint8_t uart){
	uart_puts("\033[2J", uart);
	uart_goto(0,0,uart);
}

void uart_setColor(uint8_t bg, uint8_t fg, uint8_t uart){
	sprintf(buf, "\033[0;4%d;3%dm", bg, fg);
	uart_puts(buf, uart);
}

void uart_reset(uint8_t uart){
	uart_puts("\033[0;m", uart);
}

char *uart_gets(uint8_t uart){
	char c;
	char *data = "";
	while((c = uart_getc(uart)) != 0x0d){
		data += c;
	}
	return data;
}

// wskanik do funkcji callback dla zdarzenia UART_RX_STR_EVENT()
static void (*uart_rx_str_event_callback)(char * pBuf);

// funkcja do rejestracji funkcji zwrotnej w zdarzeniu UART_RX_STR_EVENT()
void register_uart_str_rx_event_callback(void (*callback)(char * pBuf)) {
	uart_rx_str_event_callback = callback;
}

// wskanik do funkcji callback0 dla zdarzenia UART_RX_STR_EVENT()
static uint8_t (*uart_rx_str_event_callback0)(char * pBuf);

// funkcja do rejestracji funkcji zwrotnej w zdarzeniu UART_RX_STR_EVENT()
void register_uart_str_rx_event_callback0(uint8_t (*callback)(char * pBuf)) {
	uart_rx_str_event_callback0 = callback;
}

void UART_RX_STR_EVENT(char *rbuf, uint8_t uart){
	if(uart == 0 && uart_flag){
		rbuf = uart_gets(uart);
		if(rbuf[0]){
			if(uart_rx_str_event_callback0 || uart_rx_str_event_callback){
				uint8_t res = 0;
				if( uart_rx_str_event_callback0 ) {
					res = (*uart_rx_str_event_callback0)( rbuf );
				}
				if( !res && uart_rx_str_event_callback ) {
					(*uart_rx_str_event_callback)( rbuf );
				}
			}
		}
	}
}

ISR(USART0_RX_vect){
	uint8_t tmp_head;
	char data;
	data = UDR0;

	tmp_head = (UART_RxHead+1) & UART_RX_BUF_MASK;

	if(tmp_head == UART_RxTail){
		// obsluga bledu buforu
		UART_RxHead = UART_RxTail;
	}else{
		if(data){
			if(13 == data){
				uart_flag++;
			}
			if(10 != data){
				UART_RxHead = tmp_head;
				UART_RxBuf[tmp_head] = data;
			}
		}
	}
}


ISR(USART1_RX_vect){
	uint8_t tmp_head;
	char data;
	data = UDR1;

	tmp_head = (UART_RxHead1+1) & UART_RX_BUF_MASK;

	if(tmp_head == UART_RxTail1){
		// obsluga bledu buforu
		UART_RxHead1 = UART_RxTail1;
	}else{
		if(data){
			if(13 == data){
				uart_flag1++;
			}
			if(10 != data){
				UART_RxHead1 = tmp_head;
				UART_RxBuf1[tmp_head] = data;
			}
		}
	}
}
