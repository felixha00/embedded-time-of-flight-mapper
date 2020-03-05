// FELIX HA 400190708 TIME OF FLIGHT SENSOR

#include <stdint.h>
#include "tm4c1294ncpdt.h"
#include "SysTick.h"
#include "PLL.h"
// DEFINING FUNCTIONS 
#define fullRot 2048


int motorSteps[] = {0b1100, 0b0110, 0b0011, 0b1001};
int motorStepsIndex;
uint32_t stepCount;

// FINITE STATE MACHINE DECLARATION
enum events {
		START_ROTATION,
		S_DISTANCE,
    START_LOOPING,
    PRINT_HELLO,
    STOP_LOOPING,
		TURN_MOTOR,
};

enum states {
		ROTATING,
    START,
    LOOP,
    END,
} state;
/*
void step_state(enum events event){
	switch (state){
		case START:
			switch (event){
				case S_DISPLACEMENT:
					state = DISPLACEMENT;
						
            break;
				case S_DISTANCE:
					state = DISTANCE;
        default:
            break;
				
			}
			break;
		}
	}
*/

void toggleDisplacementLED(){
	GPIO_PORTL_DATA_R ^= 0b00001000;
	return;
}
void blinkDistance(){
	GPIO_PORTF_DATA_R ^= 0b00000001;
	SysTick_Wait10ms(3);
	GPIO_PORTF_DATA_R ^= 0b00000001;
	return;
}

void step(int i){

		
	GPIO_PORTM_DATA_R = motorSteps[i];
	SysTick_Wait10ms(1);
	stepCount++;
	
	return;
}

void startMotor(void){
	toggleDisplacementLED();
	for (int i = 0; i < fullRot; i++){
		motorStepsIndex = (motorStepsIndex + 1)%4;
		step(motorStepsIndex);
		
		if (stepCount==256){
			blinkDistance();
			stepCount=0;
		}
		
	}
	toggleDisplacementLED();
	
}


// 
void PF0Init(void){	
	
	// PF0 Distance 
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R5;		              // activate the clock for Port L
	while((SYSCTL_PRGPIO_R&SYSCTL_PRGPIO_R5) == 0){};	        // allow time for clock to stabilize
  GPIO_PORTF_DEN_R = 0b00000001;                         		// Enabled both as digital outputs
	GPIO_PORTF_DIR_R = 0b00000001;
	return;
	}

void PL3Init(void){	
	

	// PL3 Displacement
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R10;		              // activate the clock for Port L
	while((SYSCTL_PRGPIO_R&SYSCTL_PRGPIO_R10) == 0){};	        // allow time for clock to stabilize
  GPIO_PORTL_DEN_R = 0b00001000;                         		// Enabled both as digital outputs
	GPIO_PORTL_DIR_R = 0b00001000;
	return;
	}
	
void PortE_Init(void){	
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R4;		              // activate the clock for Port E
	while((SYSCTL_PRGPIO_R&SYSCTL_PRGPIO_R4) == 0){};	        // allow time for clock to stabilize
  GPIO_PORTE_DEN_R = 0b00001111;                         		// Enabled both as digital outputs
	GPIO_PORTE_DIR_R = 0b00001111;
	GPIO_PORTE_DATA_R = 0b00000000;       
	return;
	}

void PortD_Init(void){	
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R3;		              // activate the clock for Port D
	while((SYSCTL_PRGPIO_R&SYSCTL_PRGPIO_R3) == 0){};	        // allow time for clock to stabilize
  GPIO_PORTD_DIR_R = 0b00000000;       								    // make PM0 an input, PM0 is reading if the button is pressed or not 
  GPIO_PORTD_DEN_R = 0b00001111;
	return;
	}



void PortM_Init(void){
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R11;                 //activate the clock for Port M
	while((SYSCTL_PRGPIO_R&SYSCTL_PRGPIO_R11) == 0){};        //allow time for clock to stabilize 
	GPIO_PORTM_DIR_R = 0b00001111;       								    // make PM0 an input, PM0 is reading if the button is pressed or not 
  GPIO_PORTM_DEN_R = 0b00001111;
	return;
}


void PortN_Init(void){
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R12;                 //activate the clock for Port N
	while((SYSCTL_PRGPIO_R&SYSCTL_PRGPIO_R12) == 0){};	// LED2 - PN0, LED1 - PN1
	GPIO_PORTN_DIR_R=0b00000011;
	GPIO_PORTN_DEN_R=0b00000011;
	return;
}


///////////////////////////////
int main(void){
	PLL_Init();
	SysTick_Init();
	PL3Init();
	PF0Init();
	PortD_Init();
	PortE_Init();
	PortM_Init(); // Motor
	PortN_Init();
	//step_state(START_LOOPING);
	while (1){
	GPIO_PORTL_DATA_R = 0b00001000;
	if((GPIO_PORTD_DATA_R&0b00000001)==0){
		//GPIO_PORTN_DATA_R ^= 0b00000001;

		startMotor();
	}
	}
	}
	
