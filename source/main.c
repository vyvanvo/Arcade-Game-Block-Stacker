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
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#include "timer.h"
#include "scheduler.h"
#include "shift_reg.h"
#include "usart.h"
#endif

//global variables
unsigned char blocks[9] = {0xFF, 0xC7, 0xC7, 0xE7, 0xE7, 0xE7, 0xF7, 0xF7, 0xF7};
unsigned char blocks_speed[9] = {0, 3, 3, 2, 2, 2, 1, 1, 1}; //speed of blocks moving side to side
unsigned char end_pos[9] = {0x00, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80}; //stopping position of block
unsigned char blocks_dropped[9] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; //block tower
unsigned char j = 1; // blocks array pointer -> initially set to 1
//j = 0 to set moving block side to side in top row 0x80 or blocks dropping


unsigned char curr_block; //current block that is moving side to side (col)

unsigned char drop = 0; //boolean: 1 -> block that is moving side to side stops and drops, 0 -> block does not drop and continues to move side to side

//const variable
unsigned char start_pos = 0x80;
unsigned char off = 0xFF;

enum I_States {I_Start, I_Wait, I_Drop, I_Hold} i_state;

int I_Tick(int state) {

	//local variables
	unsigned char drop_btn = PINB & 0x40; //microphone input
	//unsigned char drop_btn = ~PINB & 0x40; //btn input

	switch (state) { //transitions
		case I_Start:
			state = I_Wait;
			break;
		
		case I_Wait:
			if (drop_btn) {
				state = I_Drop;
				drop = 1;
			}
			else {
				state = I_Wait;
			}
			
			break;
			
		case I_Drop:
			if (drop_btn) {
				state = I_Hold;
				drop = 0;
			}
			else {
				state = I_Wait;
			}
			
		case I_Hold:
			if (drop_btn) {
				state = I_Hold;
			}
			else {
				state = I_Wait;
			}
			
		default:
			break;
	}
	
	switch(state) { //state actions
		case I_Wait:
			drop = 0;
			break;
		
		case I_Drop:
			break;
			
		case I_Hold:
			break;
			
		default:
			break;
	}
	
	return state;
}

enum MB_States {MB_Start, MB_Right, MB_Left, MB_Fall, MB_CalculateBlock, MB_DisplayLose, MB_DisplayWin} mb_state;

int MB_Tick(int state) {
	//local variables
	static unsigned char i = 0; //time counter
	static unsigned char curr_block; //current block
	static unsigned char speed = 0; //speed of a block
	
	static unsigned char prev_block;
	static unsigned char tmpcurr_block;
	static unsigned char db_row; //row position as it fall in dropping block
	static unsigned char k = 0; //counter to check bit to do bit manipulation to get block_dropped & end_pos
	static unsigned char n = 0; //counter to check bit to do bit manipulation to get block_dropped
	static unsigned char tmp; // tmp to bitmask if (curr_block & 0x01) == (prev_block & 0x01)
	static unsigned char block_dropped; //the block that was dropped after calculating the block to see if the bottom carries it
	static unsigned char blink_on = 1; //display win, 1 -> all lights are on, 0 -> all lights off
	
	//inputs
	//unsigned char drop = ~PINB & 0x40; //drop btn -> replace with mic
	unsigned char reset = ~PINB & 0x80; //restart btn
	
	switch (state) { //transitions
		case MB_Start:
			state = MB_Right;
			i = 0;
			
			curr_block = blocks[j];
			speed = blocks_speed[j];
			
			end_pos[0] = start_pos;
			blocks_dropped[0] = curr_block;
			
			break;
			
		case MB_Right:
			if (drop) {
				state = MB_Fall;
				db_row = start_pos;
			}
			else {
				if (i < speed) {
					state = MB_Right; 
				}
				else {
					state = MB_Left;
				}
			}
			break;
		
		case MB_Left:
			if (drop) {
				state = MB_Fall;
				db_row = start_pos;
			}
			else {
				if (i < speed) {
					state = MB_Left;
				}
				else {
					state = MB_Right;
				}
			}
			
			break;
		
		case MB_Fall:
			if (db_row != end_pos[j]) { 
				db_row = db_row >> 1;
				state = MB_Fall;
			}
			else {
				state = MB_CalculateBlock;
			}
			
			break;
			
		case MB_CalculateBlock:
			if (block_dropped == 0xFF) {
				state = MB_DisplayLose;
				
				if (USART_IsSendReady(0)) {
					//send data
					USART_Send('2', 0);
					PORTD = 0xC0;
				} //send msg to to receiver display "Game Over"
				
			}
			else {
				if (j > 7) {
					state = MB_DisplayWin;
					
					if (USART_IsSendReady(0)) {
						//send data
						USART_Send('1', 0);
						PORTD = 0x40;
					} //send msg to to receiver display "You Win"
					
					//reset for block rain
					blocks_dropped[0] = 0x00;
					end_pos[0] = 0x00;
					k = 0;
					i = 0;
				}
				else {
					state = MB_Right;
					i = 0;
					
					j++;
			
					curr_block = blocks[j];
					speed = blocks_speed[j];
			
					end_pos[0] = start_pos;
					blocks_dropped[0] = curr_block;
				}
			}
			
			break;
			
		case MB_DisplayLose:
			if (reset) {
				state = MB_Right;
				
				if (USART_IsSendReady(0)) {
					//send data
					USART_Send('0', 0);
					PORTD = 0x80;
				} //send msg to receiver to display "Game In Progress"
				
				//reset blocks_dropped array and end_pos array
				end_pos[0] = 0x00;
				blocks_dropped[0] = 0xFF;
				
				for (k = 0; k < 8; k++) {
					blocks_dropped[k+1] = 0xFF;
					end_pos[k+1] = (0x01 << k);
				}
				
				//reset starting postiion
				j = 1;
				i = 0;
				
				//set new game with initial moving block
				curr_block = blocks[j];
				speed = blocks_speed[j];
			
				end_pos[0] = start_pos;
				blocks_dropped[0] = curr_block;
				
			}
			else {
				state = MB_DisplayLose;
			}
			break;
			
		case MB_DisplayWin:
			if (reset) {
				state = MB_Right;
				
				if (USART_IsSendReady(0)) {
					//send data
					USART_Send('0', 0);
					PORTD = 0x80;
				} //send msg to receiver to display "Game In Progress"
				
				//reset blocks_dropped array and end_pos array
				end_pos[0] = 0x00;
				blocks_dropped[0] = 0xFF;
				
				for (k = 0; k < 8; k++) {
					blocks_dropped[k+1] = 0xFF;
					end_pos[k+1] = (0x01 << k);
				}
				
				//reset starting postiion
				j = 1;
				i = 0;
				
				//set new game with initial moving block
				curr_block = blocks[j];
				speed = blocks_speed[j];
			
				end_pos[0] = start_pos;
				blocks_dropped[0] = curr_block;
				
			}
			else {
				state = MB_DisplayWin;
			}
			break;
		
		default:
			break;
	}
	
	switch (state) { //state actions
		case MB_Right:
			i++;
			if (i >= speed) {
				if ( (curr_block | 0xFE) != 0xFE ) { // shift right until on = 0 hits the right edge
					curr_block = (curr_block >> 1) | 0x80; //right shift with 1 msb
					i = 0;
				}	
			}
			
			end_pos[0] = start_pos;
			blocks_dropped[0] = curr_block;
			
			break;
		
		case MB_Left:
			i++;
			if (i >= speed) {
				if ( (curr_block | 0x7F) != 0x7F ) { //shift left until on = 0 hits the left edge
					curr_block = (curr_block << 1) | 0x01;	//left shift with 1 lsb
					i = 0;
				}	
			}
			
			end_pos[0] = start_pos;
			blocks_dropped[0] = curr_block;
			
			break;
		
		case MB_Fall:
			end_pos[0] = db_row;
			break;
			
		case MB_CalculateBlock:
			tmpcurr_block = curr_block;
			if (j > 1) {
				prev_block = blocks_dropped[j-1];
				
				for (k = 0; k < 8; k++) { //on = 0, off = 1
					if ( (tmpcurr_block & 0x01) == (prev_block & 0x01) ) { //check if lsb are equal
						tmp = tmpcurr_block & 0x01; //bitmask to get lsb
						block_dropped |= (tmp << k); //shift lsb of curr_block left to k position
					}
					else { //if curr_block & 0x01 != prev_block & 0x01, turn off that position
						block_dropped |= (0x01 << k); //shift lsb = 1 to k position -> off = 1
					}
					
					tmpcurr_block = tmpcurr_block >> 1;	//shift right to get next lsb
					prev_block = prev_block >> 1;	//shift right to get next lsb
				}
				
				blocks_dropped[j] = block_dropped;	//block dropped gets added to the set of blocks that have already been dropped
			}
			else {
				block_dropped = tmpcurr_block;
				blocks_dropped[j] = block_dropped;	//first block dropped is the current block dropped
			}
					
			break;
			
		case MB_DisplayLose:
			break;
		
		case MB_DisplayWin:
			i++;
			if (i >= 10) {
				if (k < 8) {
					blocks_dropped[k+1] = 0x00;
					end_pos[k+1] = (0x80 >> k);		
					k++;
				}
				else {
					if (blink_on) {//if lights are on, turn off
						for (n = 0; n <= 8; n++) {
							blocks_dropped[n] = 0xFF;
						}
						
						blink_on = 0;
					}
					else { //if light are off, turn on
						for (n = 0; n <= 8; n++) {
							blocks_dropped[n] = 0x00;
						}
						
						blink_on = 1;
					}
				}
				i = 5;
			}
			
			break;
		
		default:
			break;
	}
	
	return state;
}

enum LM_States {LM_Start, LM_DisplayMatrix} lm_state;

int LM_Tick(int state) {

	//local variables
	static unsigned char lm_col;
	static unsigned char lm_row;
	static unsigned char m = 0;

	switch (state) { //transitions
		case LM_Start:
			state = LM_DisplayMatrix;
			break;
		
		case LM_DisplayMatrix:
			state = LM_DisplayMatrix;
			break;
			
		default:
			break;
	}
	
	switch(state) { //state actions
		case LM_DisplayMatrix:
			lm_col = blocks_dropped[m];
			lm_row = end_pos[m];
					
			transmit_column(lm_col);
			transmit_row(lm_row);
						
			if (m < 8) {
				m++;
			}
			else {
				m = 0;
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
	DDRB = 0x0F; PORTB = 0xF0; //initialize PORTB: outputs
	DDRA = 0xFF; PORTA = 0x00; //initialize PORTA -> outputs
	
	//initalize USART
	initUSART(0);
	
	//declare an array of tasks
	static task task1, task2, task4;
	task *tasks[] = {&task1, &task2, &task4};
	const unsigned short num_tasks = sizeof(tasks)/sizeof(*tasks);
	
	//Input SM
	task1.state = I_Start;
	task1.period = 100;
	task1.elapsedTime = task1.period;
	task1.TickFct = &I_Tick;
	
	//Moving Blocks SM
	task2.state = MB_Start;
	task2.period = 100;
	task2.elapsedTime = task2.period;
	task2.TickFct = &MB_Tick;
	
	//Dropping Blocks SM
	/*task3.state = DB_Start;
	task3.period = 100;
	task3.elapsedTime = task3.period;
	task3.TickFct = &DB_Tick;
	*/
	//LED Matrix SM
	task4.state = LM_Start;
	task4.period = 2;
	task4.elapsedTime = task4.period;
	task4.TickFct = &LM_Tick;
	
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
