#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <string.h>
#include "lib/uart/uart.h"
#include "lib/network/network.h"
#include "lib/network/enc28j60.h"
#include "lib/dht22/dht22.h"

#include "lib/conf/relays_mod.h"
#include "lib/conf/html.h"

#define BUFFER_SIZE 900

static uint8_t mymac[6] = {0x62,0x5F,0x70,0x72,0x61,0x79};
static uint8_t myip[4] = {172,254,0,16};
static uint16_t mywwwport = 80;

volatile uint8_t relays_lock[4] = { 0, 0, 0, 0 };
double temperature[1];
double humidity[1];

uint8_t buf[BUFFER_SIZE+1],browser;
uint16_t plen;

void Configure_Timer0();
void Configure_Timer2();
void sendpage(void);
void header(void);
void table_cell(uint8_t relay);
uint16_t http200();
void http301();

volatile uint16_t counter = 0;
volatile uint16_t enabled = 0;
volatile uint8_t counter0 = 0;
volatile uint8_t sec0 = 0;
volatile uint8_t dht_status;

const uint8_t CNT_MAX = 6;


int main(void)
{
	RELAY_CONFIG();
	RELAY_DISABLE_ALL();

    // Configure_Timer0();
	Configure_Timer2();
	uart_init(115200, 115200);

	// Ethernet configuration
	uint16_t dat_p;
	CLKPR = (1<<CLKPCE);
	CLKPR = 0;
	_delay_loop_1(50);
	ENC28J60_Init(mymac);
	ENC28J60_ClkOut(2);
	_delay_loop_1(50);
	ENC28J60_PhyWrite(PHLCON,0x0476);
	_delay_loop_1(50);
	init_network(mymac,myip,mywwwport);

	uart_puts("Smart office", 0);
	uart_putc('\n', 0);
	uart_puts("UART 0", 0);
    uart_puts("Smart office", 1);
    uart_putc('\n', 1);
    uart_puts("UART 1", 1);

	sei();
    while(1) {
    	plen = ENC28J60_PacketReceive(BUFFER_SIZE,buf);
    	        if(plen==0) continue;
    	        if(eth_is_arp(buf,plen)) {
    	            arp_reply(buf);
    	            continue;
    	        }
    	        if(eth_is_ip(buf,plen)==0) continue;
    	        if(buf[IP_PROTO]==IP_ICMP && buf[ICMP_TYPE]==ICMP_REQUEST) {
    	            icmp_reply(buf,plen);
    	            continue;
    	        }

    	        if(buf[IP_PROTO]==IP_TCP && buf[TCP_DST_PORT]==0 && buf[TCP_DST_PORT+1]==mywwwport) {
    	            if(buf[TCP_FLAGS] & TCP_SYN) {
    	                tcp_synack(buf);
    	                continue;
    	            }
    	            if(buf[TCP_FLAGS] & TCP_ACK) {
    	                init_len_info(buf);
    	                dat_p = get_tcp_data_ptr();
    	                if(dat_p==0) {
    	                    if(buf[TCP_FLAGS] & TCP_FIN) tcp_ack(buf);
    	                    continue;
    	                }

    	                if(strstr((char*)&(buf[dat_p]),"User Agent")) browser=0;
    	                else if(strstr((char*)&(buf[dat_p]),"MSIE")) browser=1;
    	                else browser=2;

    	                if(strstr((char*)&(buf[dat_p]),"?id=1")){
    	                	if(RELAY_STATUS(RELAY1) == 1){
								RELAY_DISABLE(RELAY1);
							} else {
								RELAY_ENABLE(RELAY1);
							}
    	                	http301();
    	                	sendpage();
    	                }

    	                if(strstr((char*)&(buf[dat_p]),"?id=2")){
    	                	if(RELAY_STATUS(RELAY2) == 1){
    	                		RELAY_DISABLE(RELAY2);
    	                	} else {
    	                		RELAY_ENABLE(RELAY2);
    	                	}
    	                	http301();
    	                	sendpage();
						}

    	                if(strstr((char*)&(buf[dat_p]),"?id=3")){
    	                	if(RELAY_STATUS(RELAY3) == 1){
								RELAY_DISABLE(RELAY3);
							} else {
								RELAY_ENABLE(RELAY3);
							}
    	                	http301();
    	                	sendpage();
						}

    	                if(strstr((char*)&(buf[dat_p]),"?id=4")){
    	                	if(RELAY_STATUS(RELAY4) == 1){
								RELAY_DISABLE(RELAY4);
							} else {
								RELAY_ENABLE(RELAY4);
							}
    	                	http301();
    	                	sendpage();
						}

    	                if(strncmp("/ ",(char*)&(buf[dat_p+4]),2)==0){
    	                	plen=http200();
    	                	header();
                            plen=make_tcp_data(buf, plen, HTML_TABLE_START);
                            // Display relays status
                            plen=make_tcp_data(buf, plen, HTML_TABLE_ROW_START);
                            table_cell(RELAY1);
                            table_cell(RELAY2);
                            table_cell(RELAY3);
                            table_cell(RELAY4);
                            plen=make_tcp_data(buf, plen, HTML_TABLE_ROW_END);
                            plen=make_tcp_data(buf, plen, HTML_TABLE_END);
                            plen=make_tcp_data(buf, plen, HTML_FOOTER);

    	                	// Display temperature and humidity
                            plen=make_tcp_data(buf, plen, "<div>Status: ");
                            // plen=make_tcp_data(buf, plen, sec0);
                            plen=make_tcp_data(buf, plen, "</div>");
                            plen=make_tcp_data(buf, plen, "<div>Temperatura: ");
                            // plen=make_tcp_data(buf, plen, temperature[0]);
                            plen=make_tcp_data(buf, plen, "</div>");
                            plen=make_tcp_data(buf, plen, "<div>Wilgotnosc: ");
                            // plen=make_tcp_data(buf, plen, humidity[0]);
                            plen=make_tcp_data(buf, plen, "</div>");

    	                    sendpage();
    	                }

    	            }
    	        }

    }

    return 0;
}

void Configure_Timer0() {
    TCCR0A |= (1 << COM0A0) | (1 << WGM01);
    TCCR0B |= (1 << CS00) | (1 << CS01) | (1 << CS02);
    OCR0A = CNT_MAX; // not always
    TIMSK0 |= (1 << OCIE0A);
}

void Configure_Timer2() {
	TCCR2A |= (1 << WGM21) | (1 << COM2A1); // Enable CTC Mode
	TCCR2B |= (1 << CS20) | (1 << CS21) | (1 << CS22); // Enable 1024 prescaler
	OCR2A = CNT_MAX;
	TIMSK2 |= (1 << OCIE2A); // Enable interrupt
}

uint16_t http200() {
	return make_tcp_data_pos(buf,0,PSTR("HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n"));
}

void http301() {
	plen=http200();
	plen=make_tcp_data_pos(buf,plen,PSTR("<script>history.back(-1);</script>"));
}

void header(void) {
	plen=make_tcp_data(buf, plen, HTML_HEADER);
	plen=make_tcp_data(buf, plen, HTML_JS_CUSTOM1);
	plen=make_tcp_data(buf, plen, HTML_JS_JQUERY);
	plen=make_tcp_data(buf, plen, HTML_BOOTSTRAP);
	plen=make_tcp_data(buf, plen, HTML_BOOTSTRAP_TOGGLEBTN);
	plen=make_tcp_data(buf, plen, HTML_BOOTSTRAP_TOGGLE_BTN_JS);
	plen=make_tcp_data(buf, plen, HTML_CONTENT_START);
}

void table_cell(uint8_t relay) {
    plen=make_tcp_data(buf, plen, "<td>");
    plen=make_tcp_data(buf, plen, RELAY_STATUS_STRING(relay));
	plen=make_tcp_data(buf, plen, "</td>");
}

void sendpage(void) {
    tcp_ack(buf);
    tcp_ack_with_data(buf,plen);
}

// Interrupts
ISR(TIMER0_COMPA_vect) {
    if(counter0 <= 6) {
        counter0++;
    } else {
        counter0 = 0;
        sec0++;
    }

    sec0 = sec0 <= 59 ? sec0++ : 0;
}

ISR(TIMER2_COMPA_vect) {
	if(counter <= 6) {
		counter++;
	} else {
		counter = 0;
		if(relays_lock[0] == 0 || relays_lock[1] == 0 || relays_lock[2] == 0 || relays_lock[3] == 0) {
            enabled++;
        }
	}

	if(enabled == 80 && relays_lock[0] == 0) {
		RELAY_ENABLE(RELAY1);
        relays_lock[0] = 1;
	}

	if(enabled == 480 && relays_lock[1] == 0) {
		RELAY_ENABLE(RELAY2);
        relays_lock[1] = 1;
	}

	if(enabled == 880 && relays_lock[2] == 0) {
		RELAY_ENABLE(RELAY3);
        relays_lock[2] = 1;
	}

	if(enabled == 1200 && relays_lock[3] == 0) {
		RELAY_ENABLE(RELAY4);
        relays_lock[3] = 1;
	}
}
