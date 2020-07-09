// FELIX HA 400190708 TIME OF FLIGHT SENSOR

#include <stdint.h>
#include "tm4c1294ncpdt.h"
#include "SysTick.h"
#include "PLL.h"
#include "vl53l1x_api.h"
#include "uart.h"
#include "onboardLEDs.h"

// DEFINING FUNCTIONS 
#define fullRot 2048

int stop_tof = 0; 
int tof_init = 0;
int init = 0;
int motorSteps[] = {0b1100, 0b0110, 0b0011, 0b1001};
int motorStepsIndex;
uint32_t stepCount;


#define isInterrupt 1 /* If isInterrupt = 1 then device working in interrupt mode, else device working in polling mode */

void I2C_Init(void);
void UART_Init(void);
void PortG_Init(void);
void VL53L1X_XSHUT(void);


//capture values from VL53L1X for inspection
uint16_t debugArray[2048];

// TOF INTEGRATION 
uint16_t	dev=0x52;
int status=0;
volatile int IntCount;
//
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
	uint8_t byteData, sensorState=0, myByteArray[10] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF} , i=0;
  uint16_t wordData;
  uint8_t ToFSensor = 1; // 0=Left, 1=Center(default), 2=Right
  uint16_t Distance;
  uint16_t SignalRate;
  uint16_t AmbientRate;
  uint16_t SpadNum; 
  uint8_t RangeStatus;
  uint8_t dataReady;
		
		

	
/* Those basic I2C read functions can be used to check your own I2C functions */
  status = VL53L1_RdByte(dev, 0x010F, &byteData);					// This is the model ID.  Expected returned value is 0xEA
  myByteArray[i++] = byteData;

  status = VL53L1_RdByte(dev, 0x0110, &byteData);					// This is the module type.  Expected returned value is 0xCC
  myByteArray[i++] = byteData;
	
	status = VL53L1_RdWord(dev, 0x010F, &wordData);
	status = VL53L1X_GetSensorId(dev, &wordData);

	//sprintf(printf_buffer,"Model_ID=0x%x , Module_Type=0x%x\r\n",myByteArray[0],myByteArray[1]);
	//UART_printf(printf_buffer);

	// Booting ToF chip
	
	
	while(sensorState==0){
		status = VL53L1X_BootState(dev, &sensorState);
		SysTick_Wait10ms(10);
  }
	
	FlashAllLEDs();
	
	status = VL53L1X_ClearInterrupt(dev); /* clear interrupt has to be called to enable next interrupt*/
  /* This function must to be called to initialize the sensor with the default setting  */
  status = VL53L1X_SensorInit(dev);
	// Status_Check("SensorInit", status);

	toggleDisplacementLED();
  status = VL53L1X_StartRanging(dev);   /* This function has to be called to enable the ranging */
	Status_Check("StartRanging", status);
	
	
	while ((GPIO_PORTD_DATA_R&0b00000001)!=0){ // while button is not pressed
	if (stop_tof == 1){break;}
	for (int i = 0; i < fullRot; i++){
		if((GPIO_PORTD_DATA_R&0b00000001)==0){
		stop_tof = 1;
		break;
		}
		motorStepsIndex = (motorStepsIndex + 1)%4;
		step(motorStepsIndex);
		if (stepCount==32){ // every 5.625 degrees
			blinkDistance();
			
			while (dataReady == 0){
		  status = VL53L1X_CheckForDataReady(dev, &dataReady);
          FlashLED3(1);
          //VL53L1_WaitMs(dev, 5);
	  }

      dataReady = 0;
			status = VL53L1X_GetRangeStatus(dev, &RangeStatus);
			status = VL53L1X_GetDistance(dev, &Distance); // gets distance
      FlashLED4(1);

      debugArray[i] = Distance;
			
			sprintf(printf_buffer,"%d",i+1);
			UART_printf(printf_buffer);
		
			UART_printf("!"); // CHARACTER TO SIGNIFY END OF ANGLE SENT
		
			sprintf(printf_buffer,"%d",Distance);
			UART_printf(printf_buffer);
			
			UART_printf("?"); // CHARACTER TO SIGNIFY END OF DISTANCE SENT 
			FlashLED3(1);
			stepCount=0;
				if((GPIO_PORTD_DATA_R&0b00000001)==0){
				stop_tof = 1;
				break;
		}
}
	
	}
	
}
	toggleDisplacementLED();
	UART_printf("&");
	VL53L1X_StopRanging(dev);
	SysTick_Wait10ms(10);
	return; 
}




// 
#define I2C_MCS_ACK             0x00000008  // Data Acknowledge Enable
#define I2C_MCS_DATACK          0x00000008  // Acknowledge Data
#define I2C_MCS_ADRACK          0x00000004  // Acknowledge Address
#define I2C_MCS_STOP            0x00000004  // Generate STOP
#define I2C_MCS_START           0x00000002  // Generate START
#define I2C_MCS_ERROR           0x00000002  // Error
#define I2C_MCS_RUN             0x00000001  // I2C Master Enable
#define I2C_MCS_BUSY            0x00000001  // I2C Busy
#define I2C_MCR_MFE             0x00000010  // I2C Master Function Enable

#define MAXRETRIES              5           // number of receive attempts before giving up
void I2C_Init(void){
  SYSCTL_RCGCI2C_R |= SYSCTL_RCGCI2C_R0;           // activate I2C0
  SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R1;          // activate port B
  while((SYSCTL_PRGPIO_R&0x0002) == 0){};// ready?

    GPIO_PORTB_AFSEL_R |= 0x0C;           // 3) enable alt funct on PB2,3       0b00001100
    GPIO_PORTB_ODR_R |= 0x08;             // 4) enable open drain on PB3 only

    GPIO_PORTB_DEN_R |= 0x0C;             // 5) enable digital I/O on PB2,3
//  GPIO_PORTB_AMSEL_R &= ~0x0C;          // 7) disable analog functionality on PB2,3

                                                                                // 6) configure PB2,3 as I2C
//  GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R&0xFFFF00FF)+0x00003300;
  GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R&0xFFFF00FF)+0x00002200;    //TED
    I2C0_MCR_R = I2C_MCR_MFE;                      // 9) master function enable
    I2C0_MTPR_R = 0b0000000000000101000000000111011;                                        // 8) configure for 100 kbps clock (added 8 clocks of glitch suppression ~50ns)
//    I2C0_MTPR_R = 0x3B;                                        // 8) configure for 100 kbps clock
        
  // 20*(TPR+1)*20ns = 10us, with TPR=24
    // TED 100 KHz
    //     CLK_PRD = 8.3ns
    //    TIMER_PRD = 1
    //    SCL_LP = 6
    //    SCL_HP = 4
    //    10us = 2 * (1 + TIMER_PRD) * (SCL_LP + SCL_HP) * CLK_PRD
    //    10us = 2 * (1+TIMER+PRD) * 10 * 8.3ns
    //  TIMER_PRD = 59 (0x3B)
    //
    // TIMER_PRD is a 6-bit value.  This 0-127
    //    @0: 2 * (1+ 0) * 10 * 8.3ns --> .1667us or 6.0MHz
    //  @127: 2 * (1+ 127) * 10 * 8.3ns --> 47kHz
    
    
}

void PortG_Init(void){ // TOF SENSOR CODE 
    //Use PortG0
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R6;                // activate clock for Port N
    while((SYSCTL_PRGPIO_R&SYSCTL_PRGPIO_R6) == 0){};    // allow time for clock to stabilize
    GPIO_PORTG_DIR_R &= 0x00;                                        // make PG0 in (HiZ)
		GPIO_PORTG_AFSEL_R &= ~0x01;                                     // disable alt funct on PG0
		GPIO_PORTG_DEN_R |= 0x01;                                        // enable digital I/O on PG0
                                                                                                    // configure PG0 as GPIO
		//GPIO_PORTN_PCTL_R = (GPIO_PORTN_PCTL_R&0xFFFFFF00)+0x00000000;
		GPIO_PORTG_AMSEL_R &= ~0x01;                                     // disable analog functionality on PN0

    return;
}

void VL53L1X_XSHUT(void){ // PART OF TOF SENSOR
    GPIO_PORTG_DIR_R |= 0x01;                                        // make PG0 out
    GPIO_PORTG_DATA_R &= 0b11111110;                                 //PG0 = 0
    FlashAllLEDs();
    SysTick_Wait10ms(10);
    GPIO_PORTG_DIR_R &= ~0x01;                                            // make PG0 input (HiZ)
}

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
	if (init == 0){
		
	PLL_Init();
	SysTick_Init();
	onboardLEDs_Init();
	PL3Init();
	I2C_Init();
	UART_Init();
	PF0Init();
	PortD_Init();
	PortE_Init();
	PortM_Init(); // Motor
	PortN_Init();
	init = 1; // flag that all ports are already initialized
	}
	
	while (1){
	GPIO_PORTL_DATA_R = 0b00001000;
	
	if((GPIO_PORTD_DATA_R&0b00000001)==0){
		stop_tof = 0; // set tof to not be stopped
		startMotor();
	}
	}
	}
	
