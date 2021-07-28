#include "stm32l476xx.h"
#include "SysTimer.h"
#include "LCD.h"
#include "ADC.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

//PE10 - PE15, PH0 and PH1 for motors
//PA1, PA2, PA3, PA5 for reflectance sensors
//PD0 for distance sensor

#define F 1
#define R 2
#define O 0
 
volatile double result;
uint32_t count = 0, count1 = 0, count2 = 0, count3 = 0;
int tensPlace;
int onesPlace;
char r0 = 'w', r1 = 'w', r2 = 'w', r3 = 'w';

void delay1(int i) {
	while (i > 0) {
		i--;
	}
}

//----------------------------------------------------------------

typedef struct _Motor
{
	unsigned long totalSteps_LEFTWheel;
	unsigned short stepLeft;
	unsigned long totalSteps_RightWheel;
	unsigned short stepRight;
} Motor;


typedef struct _Sys_Time
{
	uint32_t totalTicks;
	float totalS;
} Sys_Time;



/*----------------------------------------------------------------------------
  Function that outputs high for requested pin
 *----------------------------------------------------------------------------*/
void Pin_High_LeftWheel (uint8_t num)
{
	GPIOE->BSRR = (1<<num);
} 

/*----------------------------------------------------------------------------
  Function that outputs low for requested pin
 *----------------------------------------------------------------------------*/
void Pin_Low_LeftWheel (uint8_t num)
{
	GPIOE->BRR = (1<<num);
} 

/*----------------------------------------------------------------------------
  Function that outputs high for requested pin
 *----------------------------------------------------------------------------*/
void Pin_High_RightWheel (uint8_t num)
{
	GPIOE->BSRR = (1<<num);
} 

/*----------------------------------------------------------------------------
  Function that outputs low for requested pin
 *----------------------------------------------------------------------------*/
void Pin_Low_RightWheel (uint8_t num)
{
	GPIOE->BRR = (1<<num);
} 


//----------------------------------------------------------------
// Motor States for right wheel
void coil_m1_right (uint8_t dir,uint8_t p1, uint8_t p2){

	if(dir == F) {
		Pin_High_RightWheel(p1);
		Pin_Low_RightWheel(p2);
	};
	if(dir == R){
		Pin_Low_RightWheel(p1);
		Pin_High_RightWheel(p2);
	}
	if(dir == O) {
		Pin_Low_RightWheel(p1);
		Pin_Low_RightWheel(p2);
	}
	return;
}
//----------------------------------------------------------------
// Motor States for left wheel
void coil_m1_left (uint8_t dir,uint8_t p1, uint8_t p2){

	if(dir == F) {
		Pin_High_LeftWheel(p1);
		Pin_Low_LeftWheel(p2);
	};
	if(dir == R){
		Pin_Low_LeftWheel(p1);
		Pin_High_LeftWheel(p2);
	}
	if(dir == O) {
		Pin_Low_LeftWheel(p1);
		Pin_Low_LeftWheel(p2);
	}
	return;
}

//----------------------------------------------------------------
// motor cases
//Right Motor
int StepMotor_1 (Motor *motor1, int i){

	//int btnPress =0;
	
			if (((GPIOA->IDR & 0x8) == 0x8)) {
			i--;
	//		btnPress = 1;
			delay(50);
		}
		
		if ((GPIOA->IDR & 0x20) == 0x20) {
			i++;
	//	btnPress = 0;
			delay(50);
		}
		
	if (i >= 9) {
		i = 9;
	}
	if (i <= 0) {
		i = 0;
	}
	
//if (btnPress == 1) {	
	switch (motor1->stepRight){
		case 0:
			// 1000
			coil_m1_right(F,12,13); //10 
			coil_m1_right(O,14,15); //00
			break;
		case 1:
			// 1010
			coil_m1_right(F,12,13); //10
			coil_m1_right(F,14,15); //10
			break;
		case 2:
			// 0010
			coil_m1_right(O,12,13); //00
			coil_m1_right(F,14,15); //10
			break;
		case 3:
			// 0110
			coil_m1_right(R,12,13); //
			coil_m1_right(F,14,15);
			break;
		case 4:
			coil_m1_right(R,12,13);
			coil_m1_right(O,14,15);
			break;
		case 5:
			coil_m1_right(R,12,13);
			coil_m1_right(R,14,15);
			break;
		case 6:
			coil_m1_right(O,12,13);
			coil_m1_right(R,14,15);
			break;
		case 7:
			coil_m1_right(F,12,13);
			coil_m1_right(R,14,15);
			break;
	}

	
////if (btnPress == 0) {
//	switch (motor1->step) {
//		case 0:
//			coil_m1_right(F,12,13);
//			coil_m1_right(O,14,15);
//				break;
//		case 1:
//			coil_m1_right(O,12,13);
//			coil_m1_right(F,14,15);
//			break;
//		case 2:
//			coil_m1_right(R,12,13);
//			coil_m1_right(O,14,15);
//			break;
//		case 3:
//			coil_m1_right(O,12,13);
//			coil_m1_right(R,14,15);
//			break;
//	}

	motor1->totalSteps_RightWheel++;
	motor1->stepRight++;
	if (motor1->stepRight == 8){
		motor1->stepRight = 0;
	}
	
	return i;
}

//----------------------------------------------------------------
//Left Motor
int StepMotor_2 (Motor *motor1, int i){

	//int btnPress =0;
	
			if (((GPIOA->IDR & 0x8) == 0x8)) {
			i--;
	//		btnPress = 1;
			delay(50);
		}
		
		if ((GPIOA->IDR & 0x20) == 0x20) {
			i++;
	//	btnPress = 0;
			delay(50);
		}
		
	if (i >= 9) {
		i = 9;
	}
	if (i <= 0) {
		i = 0;
	}
	
//if (btnPress == 1) {	
	switch (motor1->stepLeft){
		case 0:
			// 1000
			coil_m1_left(F,12,13); //10 
			coil_m1_left(O,14,15); //00
			break;
		case 1:
			// 1010
			coil_m1_left(F,12,13); //10
			coil_m1_left(F,14,15); //10
			break;
		case 2:
			// 0010
			coil_m1_left(O,12,13); //00
			coil_m1_left(F,14,15); //10
			break;
		case 3:
			// 0110
			coil_m1_left(R,12,13); //
			coil_m1_left(F,14,15);
			break;
		case 4:
			coil_m1_left(R,12,13);
			coil_m1_left(O,14,15);
			break;
		case 5:
			coil_m1_left(R,12,13);
			coil_m1_left(R,14,15);
			break;
		case 6:
			coil_m1_left(O,12,13);
			coil_m1_left(R,14,15);
			break;
		case 7:
			coil_m1_left(F,12,13);
			coil_m1_left(R,14,15);
			break;
	}

	
////if (btnPress == 0) {
//	switch (motor1->step) {
//		case 0:
//			coil_m1_left(F,12,13);
//			coil_m1_left(O,14,15);
//				break;
//		case 1:
//			coil_m1_left(O,12,13);
//			coil_m1_left(F,14,15);
//			break;
//		case 2:
//			coil_m1_left(R,12,13);
//			coil_m1_left(O,14,15);
//			break;
//		case 3:
//			coil_m1_left(O,12,13);
//			coil_m1_left(R,14,15);
//			break;
//	}

	motor1->totalSteps_LEFTWheel++;
	motor1->stepLeft++;
	if (motor1->stepLeft == 8){
		motor1->stepLeft = 0;
	}
	
	return i;
}
//----------------------------------------------------------------
// Prototypes
void coil_m1_right (uint8_t dir, uint8_t p1, uint8_t p2);
void coil_m1_left (uint8_t dir, uint8_t p1, uint8_t p2);
int StepMotor_1(Motor *, int);
int StepMotor_2(Motor *, int);



void Motor_Init_Right(void) {

	/* Enable GPIOs clock */ 	
	RCC->AHB2ENR |=   RCC_AHB2ENR_GPIOEEN;
	RCC->AHB2ENR |=   RCC_AHB2ENR_GPIOAEN;
	
	//PE12-PE15
	GPIOE->MODER &= ~(0xFFU << (2*12));
	GPIOE->MODER |= (0x55U<< (2*12));
	GPIOE->OSPEEDR |= (0xFFU<<(2*12));
	GPIOE->OTYPER &= ~(0xFU<<12);
	GPIOE->PUPDR &= ~(0xFFU << (2*12));
	
	GPIOA->MODER &= ~(0xFFU);
	GPIOA->MODER |= (0x55U);
	GPIOA->PUPDR &= (0xFFU);
	GPIOA->PUPDR |= (0x880);
	
	
}

void Motor_Init_Left(void) {
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOHEN;
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOEEN;
	
	//PE10, PE11
	GPIOE->MODER &= ~(0xFU << 20);
	GPIOE->MODER |= (0x5U << 20);
	GPIOE->OSPEEDR |= (0xFU << 20);
	GPIOE->OTYPER &= ~(0x3U << 10);
	GPIOE->PUPDR &= ~(0xFU << 20);
	
	//PH0, PH1
	GPIOH->MODER &= ~(0xFU);
	GPIOH->MODER |= (0x5U);
	GPIOH->OSPEEDR |= (0xFU);
	GPIOH->OTYPER &= ~(0x3U);
	GPIOH->PUPDR &= ~(0x3U);
	
}


void System_Clock_Init(void){
	
	RCC->CR |= RCC_CR_MSION; 
	
	// Select MSI as the clock source of System Clock
	RCC->CFGR &= ~RCC_CFGR_SW; 
	
	// Wait until MSI is ready
	while ((RCC->CR & RCC_CR_MSIRDY) == 0); 	
	
	// MSIRANGE can be modified when MSI is OFF (MSION=0) or when MSI is ready (MSIRDY=1). 
	RCC->CR &= ~RCC_CR_MSIRANGE; 
	RCC->CR |= RCC_CR_MSIRANGE_7;  // Select MSI 8 MHz	
 
	// The MSIRGSEL bit in RCC-CR select which MSIRANGE is used. 
	// If MSIRGSEL is 0, the MSIRANGE in RCC_CSR is used to select the MSI clock range.  (This is the default)
	// If MSIRGSEL is 1, the MSIRANGE in RCC_CR is used. 
	RCC->CR |= RCC_CR_MSIRGSEL; 
	
	// Enable MSI and wait until it's ready	
	while ((RCC->CR & RCC_CR_MSIRDY) == 0); 		
}


void refsensor_init(void) {
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
	GPIOA->MODER &= ~(0xCFC);
}	
void refsensor1(void) {
	count = 0;
	GPIOA->MODER &= ~(0xC);
	GPIOA->MODER |= (0x4);
	GPIOA->ODR |= (0x1<<1);
	delay1(40000);
	GPIOA->MODER &= ~(0xC);
	while ((GPIOA->IDR & (0x1 << 1)) > 0) {
		count++;
	}
	if (count > 0x2000) {
		r0 = 'b';
		LCD_WriteChar(&r0, 0, 0, 0);
	}
	else {
		r0 = 'w';
		LCD_WriteChar(&r0, 0, 0, 0);
	}
}

void refsensor2(void) {
	count1 = 0;
	GPIOA->MODER &= ~(0x3 << 6);
	GPIOA->MODER |= (0x1 << 6);
	GPIOA->ODR |= (0x1<<2);
	delay1(40000);
	GPIOA->MODER &= ~(0x3 << 6);
	while ((GPIOA->IDR & (0x1 << 2)) > 0) {
		count1++;
	}
	if (count1 > 0x2000) {
		r1 = 'b';
		LCD_WriteChar(&r1, 0, 0, 1);
	}
	else {
		r1 = 'w';
		LCD_WriteChar(&r1, 0, 0, 1);
	}
}

void refsensor3(void) {
	count2 = 0;
	GPIOA->MODER &= ~(0xC << 4);
	GPIOA->MODER |= (0x4 << 4);
	GPIOA->ODR |= (0x1<<3);
	delay1(40000);
	GPIOA->MODER &= ~(0xC << 4);
	while ((GPIOA->IDR & (0x1 << 3)) > 0) {
		count2++;
	}
	if (count2 > 0x2000) {
		r2 = 'b';
		LCD_WriteChar(&r2, 0, 0, 2);
	}
	else {
		r2 = 'w';
		LCD_WriteChar(&r2, 0, 0, 2);
	}
	
}

void refsensor4(void) {
	count3 = 0;
	GPIOA->MODER &= ~(0xC<<8);
	GPIOA->MODER |= (0x4 << 8);
	GPIOA->ODR |= (0x1<<5);
	delay1(40000);
	GPIOA->MODER &= ~(0xC);
	while ((GPIOA->IDR & (0x1 << 5)) > 0) {
		count3++;
	}
	if (count3 > 0x2000) {
		r3 = 'b';
		LCD_WriteChar(&r3, 0, 0, 3);
	}
	else {
		r3 = 'w';
		LCD_WriteChar(&r3, 0, 0, 3);
	}
	
}

int main(void){

	int i = 0;
	//int array[10] = {1, 5, 10, 15, 20, 25, 30, 35, 40, 45};
	int array1[10] = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100};
	Motor motor1, motor2;
	Sys_Time sys_time;	
	
	System_Clock_Init(); // Set System Clock as 8 MHz
	SysTick_Init();
	Motor_Init_Right();
	Motor_Init_Left();
	
	refsensor_init();
	LCD_Initialization();
	LCD_Clear();
	LCD_bar();
	LCD_DisplayString("LINE");
	
	motor1.stepRight = 0;
	motor2.stepLeft = 0;
	motor1.totalSteps_RightWheel = 0;
	motor2.totalSteps_LEFTWheel = 0;
	sys_time.totalTicks = 0;


	while(1){
			refsensor1();
			refsensor2();
			refsensor3();
			refsensor4(); //checks all four reflectance sensors, dependant on char characters, it goes in the way of the black line
			StepMotor_1(&motor1, i);
			StepMotor_2(&motor2, i);
      delay(array1[i]);
			sys_time.totalTicks++;
	}
}
