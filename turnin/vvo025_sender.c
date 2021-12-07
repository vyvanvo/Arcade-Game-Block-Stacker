/*	Author: Vy Vo
 *  Partner(s) Name: 
 *	Lab Section: 022
 *	Assignment: Final Project 
 *	Exercise Description: Block Stacker
 *	Display LED Matrix
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *	
 *	Demo Link: https://youtu.be/0Ym5bxLDB28
 *
 */
#include <avr/io.h>
#include "usart.h"
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#include "timer.h"
#include "scheduler.h"
#include "shift_reg.h"
#endif

int main(void) {
    /* Insert DDR and PORT initializations */
	DDRD = 0xFF; PORTD = 0x00; //initialize PORTC -> outputs
	DDRB = 0x00; PORTB = 0xFF; //initialize PORTB: outputs
	//DDRA = 0xFF; PORTA = 0x00; //initialize PORTA -> outputs
	
	initUSART(0);
	TimerSet(500);
	TimerOn();
	
	while (1) {
	
		unsigned char btn_pin = ~PINB & 0x80;
	
		if (btn_pin) {
			//PORTD = 0x80;
			if (USART_IsSendReady()) {
				//send data
				USART_Send('1');
				PORTD = 0x80;
			}
		}
		else {
		
			if (USART_IsSendReady(0)) {
				//send data
				USART_Send('0');
				PORTD = 0x40;
				
			}
		}
		
		while(!TimerFlag);
		TimerFlag = 0;
		
	}
	
	
    return 1;
}
