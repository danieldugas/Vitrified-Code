/*
 * servo.h
 *
 *  Created on: Sep 2, 2011
 *      Author: Cubli
 */

#ifndef SERVO_H_
#define SERVO_H_

#pragma once

#include "stm32f4xx.h"
//#include "stm32_eval.h"
#include "stm32f4xx_tim.h"
#include "Shortcuts.h"

enum ServoChannel {Channel1 = 1, Channel2 = 2, Channel3 = 3, Channel4 = 4};

//Minimum value is -1000, when viewing from top, arm is on the left side
//Maximum value is 1000, when viewing from top, arm is on the right side
//#define SERVO_CLOSED_VALUE 240
//#define SERVO_OPEN_VALUE 62
//#define SERVO_MID_VALUE 140

typedef struct Servo
{
	TIM_TypeDef* Timer;
	enum ServoChannel Channel; //Four channels per timer are available enumerated with 1,2,3,4 (see enumeration ServoChannel)
} Servo;

typedef struct ServoTwoStates
{
	Servo Servo;
	uint32_t OpenValue;
	uint32_t ClosedValue;

} ServoTwoStates;



typedef struct PWMInitData
{
	uint32_t TimerClock; // Example: RCC_APB1Periph_TIM3
	uint32_t GPIOClock;// Example: RCC_AHB1Periph_GPIOC
	GPIO_TypeDef* GPIO; // Example: GPIOC
	uint16_t Pins; // Example: GPIO_Pin_6
	uint8_t GPIO_AF; // Example: GPIO_AF_TIM3
	//uint8_t PinSource; //Example: GPIO_PinSource6
} PWMChannelInitData;


//typedef struct PWMInitData
//{
//	GPIO_TypeDef* Port; // Example: GPIOE
//	uint16_t Pins; // Example: GPIO_Pin_9|GPIO_Pin_11|GPIO_Pin_13|GPIO_Pin_14
//	uint32_t Clocks; // Example: RCC_APB2Periph_TIM1 | RCC_APB2Periph_GPIOE |RCC_APB2Periph_AFIO
//	uint32_t GPIORemap; // Example: RCC_APB2Periph_TIM1, See GPIO_PinRemapConfig(...)
//	//TIM_TypeDef* Timer; // Example: GPIO_FullRemap_TIM1
//	//enum ServoChannel Channel; //Four channels per timer are available enumerated with 1,2,3,4 (see enumeration ServoChannel)
//	Servo Servo;
//} PWMInitData;


/** @defgroup Servo_Exported_Functions
  * @{
  */
//void InitPWM(PWMInitData* initData);
void InitTimerChannel(Servo* servo, TIM_OCInitTypeDef* TIM_OCInitStruct);
void PreloadConfigTimerChannel(Servo* servo);
void InitPWM(PWMChannelInitData* init);
void StartPWM(Servo* servo, uint32_t startPos);
void ServoSetPulseWidth(Servo* servo, uint32_t pulseWidth);
void ServoClose(ServoTwoStates* servo);
void ServoOpen(ServoTwoStates* servo);
void ServoActuate(void);
void ServoRelax(void);
void Servo_Buttons_Init(void);
/**
  * @}
  */

#endif /* SERVO_H_ */


/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */
