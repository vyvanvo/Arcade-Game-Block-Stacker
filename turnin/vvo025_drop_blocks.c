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
 *	Demo Link:
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
unsigned char blocks[9] = {0xFF, 0xC7, 0xC7, 0xE7, 0xE7, 0xE7, 0xF7, 0xF7, 0xF7};
unsigned char blocks_speed[9] = {0, 6, 6, 4, 4, 4, 2, 2, 2}; //speed of blocks moving side to side
unsigned char end_pos[9] = {0x00, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80}; //stopping position of block
unsigned char blocks_dropped[9] = {0xFF, 0xC7, 0xC7, 0xE7, 0xE7, 0xFF, 0xFF, 0xFF, 0xFF}; //block tower
unsigned char j = 5; // blocks array pointer -> initially set to 1
//j = 0 to set moving block side to side in top row 0x80 & 


unsigned char curr_block; //current block that is moving side to side (col)
unsigned char next_block; //boolean: 1 -> allow next block to start moving side to side, 0 -> block still dropping / being calculated / displaying end game results 
//const variable
unsigned char start_pos = 0x80;

unsigned char inputs[9] = {0xFF, 0xC7, 0xE3, 0xE7, 0xE7, 0xFB, 0xFB, 0xFB, 0xFB};

enum MB_States {MB_Start, MB_Right, MB_Left} mb_state;

int MB_Tick(int state) {
	//local variables
	static unsigned char i = 0; //time counter
	static unsigned char curr_block; //current block
	static unsigned char speed = 0; //speed of a block
	
	switch (state) { //transitions
		case MB_Start:
			state = MB_Right;
			i = 0;
			
			curr_block = blocks[j];
			speed = blocks_speed[j];
			
			break;
			
		case MB_Right:
			if (i < speed) {
				state = MB_Right; 
			}
			else {
				state = MB_Left;
			}
			
			break;
		
		case MB_Left:
			if (i < speed) {
				state = MB_Left;
			}
			else {
				state = MB_Right;
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
			
			break;
		
		case MB_Left:
			i++;
			if (i >= speed) {
				if ( (curr_block | 0x7F) != 0x7F ) { //shift left until on = 0 hits the left edge
					curr_block = (curr_block << 1) | 0x01;	//left shift with 1 lsb
					i = 0;
				}	
			}
			
			break;
		
		default:
			break;
	}
	
	end_pos[0] = start_pos;
	blocks_dropped[0] = curr_block;
	return state;
}

enum DB_States {DB_Start, DB_Fall, DB_CalculateBlock, DB_DisplayLose} db_state;

int DB_Tick(int state) {
	//local variables
	static unsigned char prev_block;
	static unsigned char tmpcurr_block;
	static unsigned char db_row; //row position as it fall in dropping block
	static unsigned char k = 0; //counter to check bit to do bit manipulation to get block_dropped
	static unsigned char tmp; // tmp to bitmask if (curr_block & 0x01) == (prev_block & 0x01)
	static unsigned char block_dropped; //the block that was dropped after calculating the block to see if the bottom carries it
	
	switch (state) { //transitions
		case DB_Start:
			state = DB_Fall;
			db_row = start_pos;
			curr_block = inputs[1];
			blocks_dropped[0] = curr_block;
			break;
			
		case DB_Fall:
			if (db_row != end_pos[j]) { 
				db_row = db_row >> 1;
				end_pos[0] = db_row;
				state = DB_Fall;
			}
			else {
				state = DB_CalculateBlock;
			}
			
			break;
		
		case DB_CalculateBlock:
			if (block_dropped == 0xFF) {
				state = DB_DisplayLose;
			}
			else {
				state = DB_Fall;
				curr_block = inputs[j];
				blocks_dropped[0] = curr_block;
				db_row = start_pos;
			}
			
			break;
			
		case DB_DisplayLose:
			state = DB_DisplayLose;
			
			break;
		
		default:
			break;
	}
	
	switch (state) { //state actions
		case DB_Fall:
			break;
			
		case DB_CalculateBlock:
		
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
					
					tmpcurr_block = tmpcurr_block >> 1;
					prev_block = prev_block >> 1;
				}
				
				blocks_dropped[j] = block_dropped;
			}
			else {
				blocks_dropped[j] = tmpcurr_block;
			}
			
			if (j < 8) {
				j++;
			}
			
			break;
			
		case DB_DisplayLose:
		
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
		case LM_Start:
		
			break;
		
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
	DDRA = 0x0F; PORTA = 0xF0; //initialize PORTA -> A7-A4 inputs & A3-A0 outputs
	
	//declare an array of tasks
	static task task1, task3;
	task *tasks[] = {&task1, &task3};
	const unsigned short num_tasks = sizeof(tasks)/sizeof(*tasks);
	
	//Moving Blocks SM
	task1.state = MB_Start;
	task1.period = 100;
	task1.elapsedTime = task1.period;
	task1.TickFct = &MB_Tick;
	
	/*task2.state = DB_Start;
	task2.period = 100;
	task2.elapsedTime = task2.period;
	task2.TickFct = &DB_Tick;
	*/
	
	//LED Matrix SM
	task3.state = LM_Start;
	task3.period = 2;
	task3.elapsedTime = task3.period;
	task3.TickFct = &LM_Tick;
	
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
