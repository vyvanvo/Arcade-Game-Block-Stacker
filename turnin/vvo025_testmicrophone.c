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
 *	Demo Link: https://youtu.be/0tT-UEeZVoo
 *
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#include "timer.h"
#include "scheduler.h"
#include "shift_reg.h"
//#include "usart.h"
#endif


int main(void) {
    /* Insert DDR and PORT initializations */
	DDRD = 0xFF; PORTD = 0x00; //initialize PORTC -> outputs
	DDRB = 0x0F; PORTB = 0xF0; //initialize PORTB: outputs
	DDRA = 0xFF; PORTA = 0x00; //initialize PORTA -> outputs
	
	//declare an array of tasks
    while (1) {
    	unsigned char tmpB = PINB & 0x40;
    	
    	if (tmpB == 0x40) {
    		PORTD = 0x40;
    	}
    	else if(tmpB == 0x00) {
    		PORTD = 0x00;
    	}
    }
    return 1;
}
