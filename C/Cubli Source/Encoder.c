#include "Encoder.h"

float _encoderValue = 0;

void InitEncoder(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_Init(GPIOE,&GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource9, GPIO_AF_TIM1);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource11, GPIO_AF_TIM1);

	TIM_EncoderInterfaceConfig(TIM1, TIM_EncoderMode_TI2, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
	TIM_Cmd(TIM1, ENABLE);

	Delay(100);
	TIM_SetCounter(TIM1,30000);
	Delay(100);
}

void UpdateEncoderValue()
{
	int32_t raw = TIM_GetCounter(TIM1)-30000/*-30000*//*-30526-6*/;	//Counter Initialized with 30000
	_encoderValue = raw*360.0*2.0/8192.0;
}


//void EncoderReader(void* parameters)
//{
//	Enc_Configuration();
//
//		int32_t raw;
//		uint32_t time = 0;
//
//		Delay(100);
//		TIM_SetCounter(TIM1,30000);
//		Delay(100);
//
//		portTickType xLastExecutionTime = xTaskGetTickCount();
//		for( ;; )
//		{
//			time = time + 20;
//			DelayUntilTime( &xLastExecutionTime, 20 );
//
//			raw = TIM_GetCounter(TIM1)-30000/*-30000*//*-30526-6*/;	//Counter Initialized with 30000
//			_encoderValue = (raw/*-alpha1_corr*/)/**0.0859*//**0.088*/*360.0*2.0/8192.0;
//
//
//			//printf ("\n\r%2.2f,%2.2f",(float)time, (float)alpha1_n);
//			//sprintf (USART1TxBuffer, "\n\r%d,%d,%d,%d,%d,%d,%d",imu_data_raw_2.dAlpha,imu_data_raw_2.dBeta,imu_data_raw_2.dGamma,imu_data_raw_1.dAlpha,imu_data_raw_1.dBeta,imu_data_raw_1.dGamma,0);
//			//UART_Send(USART1TxBuffer,sizeof(USART1TxBuffer) - 1);
//		}
//}


