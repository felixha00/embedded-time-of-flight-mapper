// FELIX HA 400190708 TIME OF FLIGHT SENSOR

#include <stdint.h>
#include "tm4c1294ncpdt.h"
#include "SysTick.h"
#include "PLL.h"
// DEFINING FUNCTIONS 

void startProgram(void);

int motorSteps[] = {0b1100, 0b0110, 0b0011, 0b1001};
int motorStepsIndex;

// FINITE STATE MACHINE DECLARATION
enum events {
		S_DISPLACEMENT,
		S_DISTANCE,
    START_LOOPING,
    PRINT_HELLO,
    STOP_LOOPING,
		TURN_MOTOR,
};

enum states {
		DISPLACEMENT,
		DISTANCE,
    START,
    LOOP,
    END,
} state;

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


void step(int i){
	GPIO_PORTM_DATA_R = motorSteps[i];
	SysTick_Wait10ms(10);
	return;
}

void startMotor(void){
	while (1){
		motorStepsIndex = (motorStepsIndex + 1)%4;
		step(motorStepsIndex);
	}
}


// 
void LEDInit(void){	
	
	// PF0
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R5;		              // activate the clock for Port E
	while((SYSCTL_PRGPIO_R&SYSCTL_PRGPIO_R5) == 0){};	        // allow time for clock to stabilize
  GPIO_PORTE_DEN_R = 0b00000001;                         		// Enabled both as digital outputs
	GPIO_PORTE_DIR_R = 0b00000001;
		
	// PL3 
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R10;		              // activate the clock for Port E
	while((SYSCTL_PRGPIO_R&SYSCTL_PRGPIO_R10) == 0){};	        // allow time for clock to stabilize
  GPIO_PORTE_DEN_R = 0b00001000;                         		// Enabled both as digital outputs
	GPIO_PORTE_DIR_R = 0b00001000;
	return;
	}
	
void PortE_Init(void){	
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R4;		              // activate the clock for Port E
	while((SYSCTL_PRGPIO_R&SYSCTL_PRGPIO_R4) == 0){};	        // allow time for clock to stabilize
  GPIO_PORTE_DEN_R = 0b00001111;                         		// Enabled both as digital outputs
	GPIO_PORTE_DIR_R = 0b00001111;
	return;
	}


void PortM_Init(void){
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R11;                 //activate the clock for Port M
	while((SYSCTL_PRGPIO_R&SYSCTL_PRGPIO_R11) == 0){};        //allow time for clock to stabilize 
	GPIO_PORTM_DIR_R = 0b00000000;       								    // make PM0 an input, PM0 is reading if the button is pressed or not 
  GPIO_PORTM_DEN_R = 0b00001111;
	return;
}


///////////////////////////////
int main(void){
	LEDInit();
	PortE_Init();
	PortM_Init();
	
	step_state(START_LOOPING);
	
	if (((GPIO_PORTM_DATA_R&0b00000001)==1)){ // button is pushed down
		startProgram();
	}
	
}