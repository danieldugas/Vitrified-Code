#include "HardwareConfig.h"


void InitHardware() {

	SetCubliParameters();

	// initialize servos: ....................................

	// Set up PWM
	PWMChannelInitData pwmInit;
	pwmInit.TimerClock = RCC_APB1Periph_TIM3;
	pwmInit.GPIOClock = RCC_AHB1Periph_GPIOC;
	pwmInit.GPIO = GPIOC;
	pwmInit.Pins = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8;
	pwmInit.GPIO_AF = GPIO_AF_TIM3;

	InitPWM(&pwmInit);

	int i;
	for(i = 0; i < NUM_SERVOS; ++i){
		StartPWM(&(_cubliParameters._servo[i].Servo), 0);
	}

	//.......................................................


	// Check if user presses on long enough -> set power on.
	// Initialize Servos First -> otherwise Cubli may not start up
	OnButtonInit();

	EXTILine0_Config();		// configure user button in Interrupt mode


	// initialize UART communication with baud rate
	//UARTInit(460800);
	//115200 for wireless transmitter
	UARTInit(115200);


	_canInit.CANClock = RCC_APB1Periph_CAN1;
	_canInit.CanLine = CAN1;
	_canInit.GPIOClock = RCC_AHB1Periph_GPIOD;
	_canInit.Pins = GPIO_Pin_0 | GPIO_Pin_1; //Rx, Tx
	_canInit.GPIO = GPIOD;
	_canInit.GPIO_AF = GPIO_AF_CAN1;

	//Define properties of I2C1 bus
	_i2cBus1.I2CBus = I2C1;
	_i2cBus1.GPIO = GPIOB;
	_i2cBus1.GPIOClock = RCC_AHB1Periph_GPIOB;
	_i2cBus1.PeripheralClock = RCC_APB1Periph_I2C1;
	_i2cBus1.Pins = GPIO_Pin_8 | GPIO_Pin_9; //Clock, Data
	_i2cBus1.GPIO_AF = GPIO_AF_I2C1;

	// Define properties of I2C2 bus
	_i2cBus2.I2CBus = I2C2;
	_i2cBus2.GPIO = GPIOB;
	_i2cBus2.GPIOClock = RCC_AHB1Periph_GPIOB;
	_i2cBus2.PeripheralClock = RCC_APB1Periph_I2C2;
	_i2cBus2.Pins = GPIO_Pin_10 | GPIO_Pin_11; //Clock, Data
	_i2cBus2.GPIO_AF = GPIO_AF_I2C2;

	//NOT POSSIBLE TO INITIALIZE I2C3 IN THIS WAY
	//because the pins are connected to different GPIO Ports (A and C)
	//-> I2C3 has its own initialization method with no arguments
	// Define properties of I2C3 bus
	//_i2cBus3.I2CBus = I2C3;
	//_i2cBus3.GPIO = GPIOA | GPIOC;
	//_i2cBus3.GPIOClock = RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOA;
	//_i2cBus3.PeripheralClock = RCC_APB1Periph_I2C3;
	//_i2cBus3.Pins = GPIO_Pin_8 | GPIO_Pin_9;
	//_i2cBus3.GPIO_AF = GPIO_AF_I2C3;

	//I2C1 devices
	_imu[0].I2CBus = I2C1;
	_imu[0].I2CAddress = 0x68;
	_acc[0].I2CInfo = _imu[0];
	_gyr[0].I2CInfo = _imu[0];
	_imu[1].I2CBus = I2C1;
	_imu[1].I2CAddress = 0x69;
	_acc[1].I2CInfo = _imu[1];
	_gyr[1].I2CInfo = _imu[1];

	//I2C2 devices
	_imu[2].I2CBus = I2C2;
	_imu[2].I2CAddress = 0x68;
	_acc[2].I2CInfo = _imu[2];
	_gyr[2].I2CInfo = _imu[2];
	_imu[3].I2CBus = I2C2;
	_imu[3].I2CAddress = 0x69;
	_acc[3].I2CInfo = _imu[3];
	_gyr[3].I2CInfo = _imu[3];

	//I2C3 devices
	_imu[4].I2CBus = I2C3;
	_imu[4].I2CAddress = 0x68;
	_acc[4].I2CInfo = _imu[4];
	_gyr[4].I2CInfo = _imu[4];
	_imu[5].I2CBus = I2C3;
	_imu[5].I2CAddress = 0x69;
	_acc[5].I2CInfo = _imu[5];
	_gyr[5].I2CInfo = _imu[5];

	//Motor configuration
	_motor[0].NodeID = 1537;
	_motor[0].Velocity = 0;
	_motor[1].NodeID = 1538;
	_motor[1].Velocity = 0;
	_motor[2].NodeID = 1539;
	_motor[2].Velocity = 0;

	//TEST
	//Enc_Configuration();

	 USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);



	 // Enable ADC
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

	 // Set ADC-Settings
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//Reset ADC
	ADC_DeInit();

	ADC_InitTypeDef  ADC_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;

	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_CommonInitStructure.ADC_Prescaler=ADC_Prescaler_Div8;
	ADC_CommonInitStructure.ADC_DMAAccessMode=ADC_DMAAccessMode_Disabled;
	ADC_CommonInitStructure.ADC_TwoSamplingDelay=ADC_TwoSamplingDelay_5Cycles;
	ADC_CommonInit(&ADC_CommonInitStructure);

	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;

	// Initialize adc
	ADC_Init(ADC1, &ADC_InitStructure);

	ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 1, ADC_SampleTime_3Cycles);

	// Run adc
	ADC_Cmd(ADC1, ENABLE);

	// Start the conversion
	ADC_SoftwareStartConv(ADC1);
	// Wait until conversion completion
	while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);

	// Move Servos to open values --------------------------------------------

	for(i = 0; i < NUM_SERVOS; ++i){
			ServoSetPulseWidth(&(_cubliParameters._servo[i].Servo), _cubliParameters._servo[i].OpenValue);

			// Wait for some time...
			// Reset counter, Counter initialized in OnButtonInit();
			TIM_SetCounter(TIM2,0);
			while(TIM_GetCounter(TIM2)<100){}; // wait

	}

}


void OnButtonInit(){
	/* Enable GPIOA clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	/* Enable SYSCFG clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	EXTI_InitTypeDef EXTI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	/* Configure PA6 pin as input */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	//GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// Set up Timer - To make sure user presses On-Button long enough.
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	TIM_TimeBaseInitTypeDef timerInitStructure;
	timerInitStructure.TIM_Prescaler = 42000;
	timerInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	timerInitStructure.TIM_Period = 4000;//1000;
	timerInitStructure.TIM_ClockDivision = TIM_CKD_DIV4;
	timerInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM2, &timerInitStructure);
	TIM_Cmd(TIM2, ENABLE);



	bool longEnough=false;

	//wait for user to release on-button();
	while(!GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_6)){
		/* User has pressed on long enough -> startup sequence */
		if(TIM_GetCounter(TIM2)>=500){

			STM_EVAL_POWERInit();	// initialize power switch on pin PA5
			STM_EVAL_POWEROn();		// switch power on


//			/* Configures LED 3..5 */
//			STM_EVAL_LEDInit(LED3);
//			STM_EVAL_LEDInit(LED4);
//			STM_EVAL_LEDInit(LED5);

			/* Configure Interupt Routines */
			NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);
			NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
			NVIC_Config();

			longEnough=true;
			LEDOn(LED3);
		}
		if(TIM_GetCounter(TIM2)>=3000){
			LEDOff(LED1);
			RestoreDefaultValues();
		}
	}


	// On button not pressed long enough
	if(!longEnough){
		 /* Configure the GPIO_LED pin */
      GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	  GPIO_Init(GPIOA, &GPIO_InitStructure);

	  GPIO_WriteBit(GPIOA,GPIO_Pin_5,Bit_RESET);
	  while(1){ };
	}

	/* Connect EXTI Line1 to PA6 pin */
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource6);

	/* Configure EXTI Line6 */
	EXTI_InitStructure.EXTI_Line = EXTI_Line6;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	/* Enable and set EXTI Line1 Interrupt to the lowest priority */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}


/**
 * @brief  Configures the NVIC for CAN.
 * @param  None
 * @retval None
 */
void NVIC_Config() {
	NVIC_InitTypeDef NVIC_InitStructure;

//#ifdef  USE_CAN1
	NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;
	//NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQHandler;
//#else  /* USE_CAN2 */
//	NVIC_InitStructure.NVIC_IRQChannel = CAN2_RX0_IRQn;
//#endif /* USE_CAN1 */

	//NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =
			(configMAX_SYSCALL_INTERRUPT_PRIORITY >> 4) + 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}


/**
 * @brief  Configures EXTI Line0 (connected to PA0 pin) in interrupt mode
 * @param  None
 * @retval None
 */
void EXTILine0_Config()
{
	EXTI_InitTypeDef EXTI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	/* Enable GPIOA clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	/* Enable SYSCFG clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	/* Configure PA0 pin as input floating */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Connect EXTI Line0 to PA0 pin */
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);

	/* Configure EXTI Line0 */
	EXTI_InitStructure.EXTI_Line = EXTI_Line0;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	/* Enable and set EXTI Line0 Interrupt to the lowest priority */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}


// Function to test one IMU at the time.
void TestIMU(int N){
	printf("Start to test IMU:%i\n\r",N);
	printf("I2CBus initialization started\n\r");
	Delay(100);
	if(N<2){
		EnC();I2CBusInit(&_i2cBus1);ExC();
	}else if(N<4){
		EnC();I2CBusInit(&_i2cBus2);ExC();
	}else if(N<6){
		EnC();I2C3BusInit();ExC();
	}

	printf("I2CBus Initialized\n\r");
	printf("Start IMU Initialization\n\r");
	Delay(100);
	EnC();MPU6000Init(&_imu[N]);ExC();
	EnC();MPU6000ConfigAcc(&_acc[N], Max16g);ExC();
	EnC();MPU6000ConfigGyro(&_gyr[N], Max1000DegPerS);ExC();
	EnC();SetFilter(&_imu[N], Cutoff21HzDelay8_5);ExC();
	printf("IMU initialized.\n\r");
	Delay(100);
}
