#include "Peripherals.h"


// Declared in main
extern xQueueHandle _velocityQueue[NUM_MOTORS];
extern xQueueHandle _currentQueue[NUM_MOTORS];

CanInitData _canInit;

MotorData _motor1 = { 1537, 0 }; //Temporarily other id
MotorData _motor2 = { 1538, 0 };
MotorData _motor3 = { 1539, 0 };
MotorData _motor[NUM_MOTORS];

int16_t _measuredCurrent[NUM_MOTORS];


void CANInitialize()
{
	EPOSInit(&_canInit);
	printf("EPOS INIT\n\r");
	Delay(500);

	uint8_t i;
	for(i = 0; i < NUM_MOTORS; ++i)
	{
		printf("EPOS Enable Start:%i\n\r",i);
		EPOSEnable(_motor[i].NodeID);
		printf("EPOS ENABLE:%i\n\r",i);
		Delay(100);
	}

	//Do it twice, not optimal but works better (*25.11.13: happened not to work at all when removed!!!)
	for(i = 0; i < NUM_MOTORS; ++i)
	{
		EPOSEnable(_motor[i].NodeID);
		Delay(100);
	}

	for(i = 0; i < NUM_MOTORS; ++i)
	{
		CurrentMode(_motor[i].NodeID);
		printf("Set Current Mode:%i\n\r",i);
		Delay(200);
	}
}


void CANVelMeasurementTest()
{
	uint8_t i;
	CanRxMsg velocity_rx; //, version_rx;

	printf("motor check ----------\n\r");

	for ( i = 0; i < NUM_MOTORS; ++i )
	{
		printf("motor %i ......:",i);
		GetAvgVelocity(_motor[i].NodeID);
		// GetActualVelocity(_motor[i].NodeID); // Strange Behavior
		// Block on queue until new CanRxMsg struct arrived
		xQueueReceive(_velocityQueue[i], &velocity_rx, portMAX_DELAY);
		printf("ok\n\r");
	}
	printf("\n\r");
}

void CANGetNewVelocityMeasurements()
{
	CanRxMsg velocity_rx; //, version_rx;
	int32_t tmp;
	uint8_t i;

	for(i = 0; i < NUM_MOTORS; ++i)
	{
		GetAvgVelocity(_motor[i].NodeID);
		//GetActualVelocity(_motor[i].NodeID);//Strange
		// Block on queue until new CanRxMsg struct arrived
		xQueueReceive( _velocityQueue[i], &velocity_rx, portMAX_DELAY);

		if(!(velocity_rx.Data[4] == 0xFF && velocity_rx.Data[5] == 0xFF && velocity_rx.Data[6] == 0xFF && velocity_rx.Data[7] == 0xFF))
		{
			tmp = (((int32_t) velocity_rx.Data[7]) << 24
					| ((int32_t) velocity_rx.Data[6]) << 16
					| ((int32_t) velocity_rx.Data[5]) << 8
					| ((int32_t) velocity_rx.Data[4]));

			EnC(); _motor[i].Velocity = ((float)tmp)*2.0*PI/60.0; ExC();
		}
	}
}


void CANGetNewCurrentMeasurements()
{
	CanRxMsg current_rx;
	uint8_t i;

	for(i = 0; i < NUM_MOTORS; ++i)
	{
		GetActualCurrent(_motor[i].NodeID);

		// Block on queue until new CanRxMsg struct arrived				/
		xQueueReceive( _currentQueue[i], &current_rx, portMAX_DELAY);

		if(!(current_rx.Data[4] == 0xFF && current_rx.Data[5] == 0xFF))
		{
			_measuredCurrent[i] = (((int16_t) current_rx.Data[5]) << 8 | ((int16_t) current_rx.Data[4]));
		}
	}
}


#define CANTaskT 10 // usual setting: 10, break test: 2
void CANTask(void *pvParameters)
{
	printf("Start CAN Initialization\n\r");
	CANInitialize();
	printf("CAN Initialized\n\r");

	CANVelMeasurementTest();

	portTickType lastExecutionTime = xTaskGetTickCount();
	for (;;)
	{
		//Use 10 ms! Works bad with 20ms for some mysterious reason
		DelayUntilTime(&lastExecutionTime, CANTaskT);
		CANGetNewVelocityMeasurements();
		//CANGetNewCurrentMeasurements();
	}
}


#define SDCardReadWriteTextTaskT 50
void SDCardReadWriteTextTask(void* parameters)
{
	uint8_t outbuf[512];
	uint8_t inbuf[512];
	int i;

	//LED_LowLevel_Init();
	//UART_LowLevel_Init();
	SD_LowLevel_Init();   //Initialize PINS, vector table and SDIO interface
	//UART_SendLine("Ready...Steady\n");

	printf("SSD\n\r");
	LEDOn(LED2);

	//Initialize the SD
	SD_Init();  //After return from this function sd is in transfer mode.
	//UART_SendLine("GO\n");

	printf("SDI\n\r");
	LEDOff(LED2);

	//Write a single block to the SD
	for (i=0;i<512;i++) {
		outbuf[i]='a';
	}

	//Single Block Test
	SD_WriteSingleBlock(outbuf, 3000);
	SD_ReadSingleBlock(inbuf, 3000);
	//UART_SendLine("First Block\n");
	//UART_Send(inbuf,512);
	printf("\n\r%s", "First Block");
	printf("\n\r%i,%i,%i,%i", (int)inbuf[0], (int)inbuf[1], (int)inbuf[2], (int)inbuf[3]);

	SD_WriteSingleBlock(outbuf, 3001);
	SD_ReadSingleBlock(inbuf, 3001);
	//UART_SendLine("Second Block\n");
	//UART_Send(inbuf,512);
	printf("\n\r%s", "Second Block");
	printf("\n\r%i,%i,%i,%i", (int)inbuf[0], (int)inbuf[1], (int)inbuf[2], (int)inbuf[3]);

	//Multiple block
	SD_StartMultipleBlockWrite(1000);
	SD_WriteData(outbuf,500);
	SD_WriteData(outbuf+50,50);
	SD_WriteData(outbuf,500);
	SD_StopMultipleBlockWrite();


	SD_ReadSingleBlock(inbuf, 1000);
	//UART_SendLine("Mult. Block 1\n");
	//UART_Send(inbuf,512);
	printf("\n\r%s", "Mult. Block 1");
	printf("\n\r%i,%i,%i,%i", (int)inbuf[0], (int)inbuf[1], (int)inbuf[2], (int)inbuf[3]);
	SD_WaitTransmissionEnd();
	SD_ReadSingleBlock(inbuf, 1001);
	//UART_SendLine("Mult. Block 2\n");
	//UART_Send(inbuf,512);
	printf("\n\r%s", "Mult. Block 2");
	printf("\n\r%i,%i,%i,%i", (int)inbuf[0], (int)inbuf[1], (int)inbuf[2], (int)inbuf[3]);

//	LEDOn(LED6);

	portTickType lastExecutionTime = xTaskGetTickCount();
	for (;;)
	{
		DelayUntilTime( &lastExecutionTime, SDCardReadWriteTextTaskT);

	}
}


