void transmit_column(unsigned char data) {
	int i;
	
	/* PORTC[3] connected to SRCLR
	PORTC[2] connected to SRCLK
	PORTC[1] connected to RCLK
	PORTC[0] connected to SER */

	for (i = 0; i < 8; i++) {
		//sets SRCLR to 1 allowing data to be set
		//also clears CRCLK in preparation of sending data
		PORTC = 0x08;
		
		//set SER = next bit of data to be sent
		PORTC |= ((data >> i) & 0x01);
		
		//set SRCLK = 1. Rising edge shifts next bit of data into shift register
		PORTC |= 0x04;
	}
	
	//set RCLK = 1; Rising edge copies data from shift register to storage register
	PORTC |= 0x02;
	
	//clears all lines in preparation of a new transmission
	PORTC = 0x00;
}

void transmit_row(unsigned char data) {
	int i;
	
	/*PORTA[3] connected to SRCLR
	PORTA[2] connected to SRCLK
	PORTA[1] connected to RCLK
	PORTA[0] connected to SER
	*/
	for (i = 0; i < 8; i++) {
		//sets SRCLR to 1 allowing data to be set
		//also clears CRCLK in preparation of sending data
		PORTA = 0x08;
		
		//set SER = next bit of data to be sent
		PORTA |= ((data >> i) & 0x01);
		
		//set SRCLK = 1. Rising edge shifts next bit of data into shift register
		PORTA |= 0x04;
	}
	
	//set RCLK = 1; Rising edge copies data from shift register to storage register
	PORTA |= 0x02;
	
	//clears all lines in preparation of a new transmission
	PORTA = 0x00;
}
