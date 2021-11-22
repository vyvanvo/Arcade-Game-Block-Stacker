/*	Author: Vy Vo
 *  Partner(s) Name: 
 *	Lab Section: 022
 *	Assignment: Final Project 
 *	Exercise Description: Block Stacker
 *	moving blocks side to side	
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *	
 *	Demo Link: https://youtu.be/t2GMf8JOJjw
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

//global variables
unsigned char blocks[8] = {0xC7, 0xC7, 0xE7, 0xE7, 0xE7, 0xF7, 0xF7, 0xF7};
unsigned char speed[8] = {6, 6, 4, 4, 4, 2, 2, 2}; //speed of blocks moving side to side
unsigned char j = 0; // blocks array pointer
unsigned char data;

//constant variables
const unsigned char start_pos = 0x80;


enum MB_States {MB_Start, MB_Right, MB_Left} mb_state;

int MB_Tick(int state) {
	//local variables
	static unsigned char i = 0; //time counter
	static unsigned char max = 0; //speed of a block
	
	switch (state) { //transitions
		case MB_Start:
			state = MB_Right;
			i = 0;
			j = 6;
			
			data = blocks[j];
			max = speed[j];
			
			transmit_column(data);
			transmit_row(start_pos);
			
			break;
			
		case MB_Right:
			if (i < max) {
				state = MB_Right; 
			}
			else {
				state = MB_Left;
				//i = 0;
			}
			
			break;
		
		case MB_Left:
			if (i < max) {
				state = MB_Left;
			}
			else {
				state = MB_Right;
				//i = 0;
			}
			
			break;
		
		default:
			break;
	}
	
	switch (state) { //state actions
		case MB_Right:
			i++;
			if (i >= max) {
				if ( (data | 0xFE) != 0xFE ) {
					data = (data >> 1) | 0x80;
					transmit_column(data);
					i = 0;
				}	
			}
			
			break;
		
		case MB_Left:
			i++;
			if (i >= max) {
				if ( (data | 0x7F) != 0x7F ) {
					data = (data << 1) | 0x01;
					transmit_column(data);
					i = 0;
				}	
			}
			
			break;
		
		default:
			break;
	}
	
	return state;
}


int main(void) {
    /* Insert DDR and PORT initializations */
	DDRC = 0xFF; PORTC = 0x00; //initialize PORTC -> outputs
	DDRA = 0x0F; PORTA = 0xF0; //initialize PORTA -> A7-A4 inputs & A3-A0 outputs
	
	//declare an array of tasks
	static task task1;
	task *tasks[] = {&task1};
	const unsigned short num_tasks = sizeof(tasks)/sizeof(*tasks);
	
	//Moving Blocks SM
	task1.state = MB_Start;
	task1.period = 100;
	task1.elapsedTime = task1.period;
	task1.TickFct = &MB_Tick;
	
	unsigned long GCD = tasks[0]->period;
	static unsigned char j = 0;
	for (j = 1; j < num_tasks; j++) {
		GCD = findGCD(GCD, tasks[j]->period);
	}
	
	TimerSet(GCD);
	TimerOn();
	
	static unsigned char i = 0;
	
    /* Insert your solution below */
    while (1) {
    	for (i = 0; i < num_tasks; i++) {
			if (tasks[i]->elapsedTime == tasks[i]->period) {	//task is ready to tick
				tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);	//set next state
				tasks[i]->elapsedTime = 0;	//reset the elapsed time for next tick
			}
			tasks[i]->elapsedTime += GCD;
		}
		
		while(!TimerFlag);
		TimerFlag = 0;
    }
    return 1;
}
