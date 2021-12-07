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
//#include "shift_reg.h"
#endif

//Receiver SM
enum R_States {R_Start, R_Wait, R_GameDisplay, R_DisplayWin, R_DisplayLose} r_state;

int R_Tick(int state) {
	
	static unsigned char received;
	
	switch (state) { //transitions
		case R_Start:
			state = R_Wait;
			break;
			
		case R_Wait:
			if (USART_HasReceived(0) ) {
				received = USART_Receive(0);
			
				state = R_Wait; //temp for testing
			
				if (received == '0') {
					PORTA = 0x01; //go to Game Display
				}
				else if (received == '1') {
					PORTA = 0x02; //go to Display Win
				}
				else if (received == '2') {
					PORTA = 0x04;
				}
				else {
					PORTA = 0x08;
				}
				
			}
			
			break;
			
			default:
				break;
	}
	
	switch (state) { //state actions
		case R_Wait:
			
			break;
			
		default:
		
			break;
	}
	
	return state;
}

int main(void) {
    /* Insert DDR and PORT initializations */
	DDRD = 0xFE; PORTD = 0x01; //initialize PORTC -> outputs
	//DDRB = 0x00; PORTB = 0xFF; //initialize PORTB: outputs
	DDRC = 0xFF; PORTC = 0x00; //initialize PORTC -> outputs
	DDRA = 0xFF; PORTA = 0x00; //initialize PORTA -> outputs
	
	initUSART(0);
	TimerSet(1);
	TimerOn();
	
	LCD_init();
	LCD_DisplayString(1, "Game In Progress");
	
	//declare an array of tasks
	static task task1;
	task *tasks[] = {&task1};
	const unsigned short num_tasks = sizeof(tasks)/sizeof(*tasks);
	
	//Input SM
	task1.state = R_Start;
	task1.period = 1;
	task1.elapsedTime = task1.period;
	task1.TickFct = &R_Tick;
	
	unsigned long GCD = tasks[0]->period;
	static unsigned char j = 0;
	for (j = 1; j < num_tasks; j++) {
		GCD = findGCD(GCD, tasks[j]->period);
	}
	
	TimerSet(GCD);
	TimerOn();
	
	static unsigned char i = 0;
	
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
