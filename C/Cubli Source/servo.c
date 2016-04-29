/*
 * servo.c
 *
 *  Created on: Oct 31, 2011
 *      Author: Cubli
 */

#include "servo.h"


/** @addtogroup Source
* @{
*/

/** @addtogroup Low_Level
* @{
*/

/** @defgroup Servo
* @brief This file provides firmware functions to actuate the HSG-500 Servo Motor.
* @{
*/


/** @defgroup Servo_Private_Functions
* @{
*/
void ServoSetPulseWidth(Servo* servo, uint32_t pulseWidth);
/**
* @}
*/


//There are usually 4 channels available for pwm per timer
void InitTimerChannel(Servo* servo, TIM_OCInitTypeDef* TIM_OCInitStruct){
	switch(servo->Channel)
	{
	case Channel1:
		TIM_OC1Init(servo->Timer, TIM_OCInitStruct);
		break;
	case Channel2:
		TIM_OC2Init(servo->Timer, TIM_OCInitStruct);
		break;
	case Channel3:
		TIM_OC3Init(servo->Timer, TIM_OCInitStruct);
		break;
	case Channel4:
		TIM_OC4Init(servo->Timer, TIM_OCInitStruct);
		break;
	}
}

//There are usually 4 channels available for pwm per timer
void PreloadConfigTimerChannel(Servo* servo){
	switch(servo->Channel)
	{
	case Channel1:
		TIM_OC1PreloadConfig(servo->Timer, TIM_OCPreload_Enable);
		break;
	case Channel2:
		TIM_OC2PreloadConfig(servo->Timer, TIM_OCPreload_Enable);
		break;
	case Channel3:
		TIM_OC3PreloadConfig(servo->Timer, TIM_OCPreload_Enable);
		break;
	case Channel4:
		TIM_OC4PreloadConfig(servo->Timer, TIM_OCPreload_Enable);
		break;
	}
}

void InitPWM(PWMChannelInitData* init)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* TIM3 clock enable */
	RCC_APB1PeriphClockCmd(init->TimerClock, ENABLE);

	/* GPIOC and GPIOB clock enable */
	RCC_AHB1PeriphClockCmd(init->GPIOClock, ENABLE);

	/* GPIOC Configuration: TIM3 CH1 (PC6) and TIM3 CH2 (PC7) */
	GPIO_InitStructure.GPIO_Pin = init->Pins;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
	GPIO_Init(init->GPIO, &GPIO_InitStructure);

	/* Connect TIM pins to AF2 */
	int index = 0;
	uint8_t pinSource = GetPinSource(init->Pins, index);
	while(pinSource != 0xFF)
	{
	  GPIO_PinAFConfig(init->GPIO, pinSource, init->GPIO_AF);
	  pinSource = GetPinSource(init->Pins, ++index);
	}
}

// InitPWMChannel must be called before
// startPos in range 500 ... 1000
void StartPWM(Servo* servo, uint32_t startPos)
{
	uint16_t PrescalerValue = (uint16_t) ((SystemCoreClock /2) / 437500) - 1;;

	/* Time base configuration */
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_TimeBaseStructure.TIM_Period = 1000;
	TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(servo->Timer, &TIM_TimeBaseStructure);

	/* PWM1 Mode configuration: Channel1 */
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	InitTimerChannel(servo, &TIM_OCInitStructure);
	PreloadConfigTimerChannel(servo);
	ServoSetPulseWidth(servo, startPos);

	TIM_Cmd(servo->Timer, ENABLE);
}

/**
  * @brief  Sets the pulse width for the servo PWM pin.
  * @param  Channel1 Pulse Pulse Width for output PWM signal
  * @retval None
  */
void ServoSetPulseWidth(Servo* servo, uint32_t pulseWidth)
{
		switch(servo->Channel)
		{
		case Channel1:
			servo->Timer->CCR1 = pulseWidth;
			break;
		case Channel2:
			servo->Timer->CCR2 = pulseWidth;
			break;
		case Channel3:
			servo->Timer->CCR3 = pulseWidth;
			break;
		case Channel4:
			servo->Timer->CCR4 = pulseWidth;
			break;
		}
}

/**
  * @brief  Servo is commanded to SERVO_CLOSED_VALUE (private constant).
  * @param  None
  * @retval None
  */
/*void ServoActuate(void)
{
	ServoSetPosition(SERVO_CLOSED_VALUE);
}*/
void ServoClose(ServoTwoStates* servo)
{
	ServoSetPulseWidth(&servo->Servo, servo->ClosedValue);
}

/**
  * @brief  Servo is commanded to SERVO_OPEN_VALUE (private constant).
  * @param  None
  * @retval None
  */
void ServoOpen(ServoTwoStates* servo)
{
	ServoSetPulseWidth(&servo->Servo, servo->OpenValue);
}

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */
