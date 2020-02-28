#include <stdint.h>
#include "tm4c1294ncpdt.h"
#include "Systick.h"
#include "PLL.h"
void start(void);
int dataStore;
int steps[] = { 0b1100, 0b0110, 0b0011, 0b1001};


void PortE0E1_Init(void){	
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R4;		              // activate the clock for Port E
	while((SYSCTL_PRGPIO_R&SYSCTL_PRGPIO_R4) == 0){};	        // allow time for clock to stabilize
  GPIO_PORTE_DEN_R = 0b00001111;                         		// Enabled both as digital outputs
	GPIO_PORTE_DIR_R = 0b00001111;
	return;
	}


void PortM0M1_Init(void){
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R11;                 //activate the clock for Port M
	while((SYSCTL_PRGPIO_R&SYSCTL_PRGPIO_R11) == 0){};        //allow time for clock to stabilize 
	GPIO_PORTM_DIR_R = 0b00000000;       								    // make PM0 an input, PM0 is reading if the button is pressed or not 
  GPIO_PORTM_DEN_R = 0b00001111;
	return;
}



//Turns on D2, D1
void PortN0N1_Init(void){
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R12;                 //activate the clock for Port N
	while((SYSCTL_PRGPIO_R&SYSCTL_PRGPIO_R12) == 0){};	// LED2 - PN0, LED1 - PN1
	GPIO_PORTN_DIR_R=0b00000011;
	GPIO_PORTN_DEN_R=0b00000011;
	return;
}

//Turns on D3, D4
void PortF0F4_Init(void){
  SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R5;                 //activate the clock for Port F
	while((SYSCTL_PRGPIO_R&SYSCTL_PRGPIO_R5) == 0){};
	GPIO_PORTF_DIR_R=0b00010001;
	GPIO_PORTF_DEN_R=0b00010001;
	return;
}

void PortD_Init(void){
  SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R3;                 //activate the clock for Port F
	while((SYSCTL_PRGPIO_R&SYSCTL_PRGPIO_R3) == 0){};
	GPIO_PORTD_DIR_R=0b00001111;
	GPIO_PORTD_DEN_R=0b00001111;
	return;
}


int lookup (int data) {
	if (data == -1){return -1;};
	if (data == 0b11101110){return 0b0001;}; // 1
	if (data == 0b11101101){return 0b0010;}; // 2
	if (data == 0b11101011){return 0b0011;}; // 3
	if (data == 0b11100111){return 0b1010;}; // A
	
	if (data == 0b11011110){return 0b0100;}; // 4
	if (data == 0b11011101){return 0b0101;}; // 5
	if (data == 0b11011011){return 0b0110;}; // 6
	if (data == 0b11010111){return 0b1011;}; // B
	
	if (data == 0b10111110){return 0b0111;}; // 7
	if (data == 0b10111101){return 0b1000;}; // 8
	if (data == 0b10111011){return 0b1001;}; // 9
	if (data == 0b10110111){return 0b1100;}; // C
	
	if (data == 0b01111110){return 0b1110;}; // *
	if (data == 0b01111101){return 0b0000;}; // 0
	if (data == 0b01111011){return 0b1111;}; // #
	if (data == 0b01110111){return 0b1101;}; // D
	
	return 0;
}

void displayOnboard (int data){ // decodes 4 bit data
	
	int d1;
	int d2;
	int d3;
	int d4;
	
	d4 = data&0b1000;
	d3 = data&0b0100;
	d2 = data&0b0010;
	d1 = data&0b0001;
	
	GPIO_PORTN_DATA_R |= d1 << 1; // bit 1 
	GPIO_PORTN_DATA_R |= d2 << 0; // bit 1 
	GPIO_PORTF_DATA_R |= d3 << 4; // bit 3 of PORTF
	GPIO_PORTF_DATA_R |= d4 << 0; // bit 1 of PORTF
	
}





int kbLoop(void){

	dataStore = -1;
	GPIO_PORTE_DATA_R =  0b00001110;
	//Checks If Button 3 is pressed - D2 lights up
	//Unique code is:	1010 - In the order of PE1 PE0 PM1 PM0
	while((GPIO_PORTM_DATA_R&0b00000001)==0){
	dataStore = 0b11101110;
		//GPIO_PORTN_DATA_R= lookup(dataStore);
	}
	while((GPIO_PORTM_DATA_R&0b00000010)==0){
	dataStore = 0b11101101;
		//GPIO_PORTN_DATA_R= lookup(dataStore);
		 
	}
	
	while((GPIO_PORTM_DATA_R&0b00000100)==0){
	dataStore = 0b11101011;
		//GPIO_PORTN_DATA_R= lookup(dataStore);
	}
	
	while((GPIO_PORTM_DATA_R&0b00001000)==0){
	  dataStore = 0b11100111;
		//GPIO_PORTN_DATA_R= lookup(dataStore);
	}

	/////////////////////////////////////////////////////////////////////////////
 
	//Row 2
	GPIO_PORTE_DATA_R =  0b00001101;            //Data is still as registers 

	//Checks if Button 1 is pressed - D3 lights up 
	//Unique code is: 0110 - In order of PE1 PE0 PM1 PM0
 	while((GPIO_PORTM_DATA_R&0b00000001)==0){
	dataStore = 0b11011110;
		//GPIO_PORTN_DATA_R= lookup(dataStore);
		 
	}
	
	while((GPIO_PORTM_DATA_R&0b00000010)==0){
	dataStore = 0b11011101;
		//GPIO_PORTN_DATA_R= lookup(dataStore);
		 
	}
	
	while((GPIO_PORTM_DATA_R&0b00000100)==0){
	dataStore = 0b11011011;
		//GPIO_PORTN_DATA_R= lookup(dataStore);
		 
	}
	
	while((GPIO_PORTM_DATA_R&0b00001000)==0){
	  dataStore = 0b11010111;
		//GPIO_PORTN_DATA_R= lookup(dataStore);
		 
	}
	

/////////////////////////////////////////////////////////////////////////////
	// Row 3 
	GPIO_PORTE_DATA_R =  0b00001011;
	
	while((GPIO_PORTM_DATA_R&0b00000001)==0){
	dataStore = 0b10111110;
		//GPIO_PORTN_DATA_R= lookup(dataStore);
		 
	}
	
	while((GPIO_PORTM_DATA_R&0b00000010)==0){
	dataStore = 0b10111101;
		//GPIO_PORTN_DATA_R= lookup(dataStore);
		 
	}
	
	while((GPIO_PORTM_DATA_R&0b00000100)==0){
	dataStore = 0b10111011;
		//GPIO_PORTN_DATA_R= lookup(dataStore);
		 
	}
	
	while((GPIO_PORTM_DATA_R&0b00001000)==0){
	  dataStore = 0b10110111;
		//GPIO_PORTN_DATA_R= lookup(dataStore);
		 
	}
/////////////////////////////////////////////////////////////////////////////
	// Row 4 
	GPIO_PORTE_DATA_R =  0b00000111; //0111
	
	while((GPIO_PORTM_DATA_R&0b00000001)==0){
	dataStore = 0b01111110;
		//GPIO_PORTN_DATA_R= lookup(dataStore);
		 
	}
	
	while((GPIO_PORTM_DATA_R&0b00000010)==0){
	dataStore = 0b01111101;
		//GPIO_PORTN_DATA_R= lookup(dataStore);
		 
	}
	
	while((GPIO_PORTM_DATA_R&0b00000100)==0){
	dataStore = 0b01111011;
		//GPIO_PORTN_DATA_R= lookup(dataStore);
		 
	}
	
	while((GPIO_PORTM_DATA_R&0b00001000)==0){
		
	  dataStore = 0b01110111;
		//GPIO_PORTN_DATA_R= lookup(dataStore);
		
	}	
	
	return lookup(dataStore);
}


void stepFwd (int speed){
		
		for (int i = 3; i >= 0; i--){
			GPIO_PORTD_DATA_R = steps[i];
			SysTick_Wait10ms(speed);
			
		}
		return;
		
}

void stepBk (int speed){
		
		for (int i = 0; i < 4; i++){
			GPIO_PORTD_DATA_R = steps[i];
			SysTick_Wait10ms(speed);
		}
		return;
		
}

void startMotor(int angle, int direction, int speed){
		int count = 0;
		
		if (kbLoop() == 0b0000){
			start();
		}
		
		if (direction == 0){
			while(1){
				stepBk(speed);
				count++;
				if (count == angle){
				GPIO_PORTN_DATA_R ^= 0b00000001;
					SysTick_Wait10ms(10);
					GPIO_PORTN_DATA_R ^= 0b00000001;
					count = 0;
				}
			}
			
		}
		if (direction == 1)  {
			while(1){
				stepFwd(speed);
				count++;
				if (count == angle){
					GPIO_PORTN_DATA_R ^= 0b00000001;
					SysTick_Wait10ms(10);
					GPIO_PORTN_DATA_R ^= 0b00000001;
					count = 0;
				
			}
		}
		
		startMotor(angle, direction, speed);
	}
		
}





void getSpeed (int angle, int direction){
	
	GPIO_PORTN_DATA_R ^= 0b00000001;
	SysTick_Wait10ms(30);
	
	int data = kbLoop();
	int speedsArr[] = {1,2,3,4,5,6,7,8,9};
	
	if (data == 0b0000){
		start();
	}
	if (data == -1){

		getSpeed(angle, direction);
	}
	else {
	for (int i = 0; i < 9; i++){
		if (data == speedsArr[i]){
			startMotor (angle, direction, data);
	}
}
	return;
	
}
	}
	
void getDirection (int angle){
	
	GPIO_PORTN_DATA_R ^= 0b00000001;
	SysTick_Wait10ms(10);
	
	int data = kbLoop();
	
	if (data == -1){
		getDirection(angle);
	}
	if (data == 0b1110){
		getSpeed(angle, 1);
		
	}
	else if (data == 0b1111)  {
		getSpeed(angle, 0);
	}
	else if (data == 0b0000) {
		start();
	}
	
	return;
}



void getAngle (void){

	
	GPIO_PORTN_DATA_R ^= 0b00000001;
	SysTick_Wait10ms(20);
	
	int data = kbLoop();
	switch(data){
		case 0b1010: // A
			getDirection(64);//11.25
		case 0b1011: // B
			getDirection(256); 
		case 0b1100:
			getDirection(512);

		case 0b1101:
			getDirection(2048);
		case 0b0000:
			start();
		default:
			getAngle();
		
}
	return;
	}

void start(void){
		getAngle();
		return;
}
	


int main(void){
	PLL_Init();
	SysTick_Init();
	PortE0E1_Init();
	PortM0M1_Init();
	PortN0N1_Init();
	PortF0F4_Init();
	PortD_Init();
	while(1){
		start();
		
}
}
