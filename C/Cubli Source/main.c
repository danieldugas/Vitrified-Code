// General includes
#include "stdio.h"
#include "math.h"
#include "stdbool.h"

// Cortex M4 includes
#include "stm32f4xx.h"

// Scheduler includes
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

// STM32F4-Discovery evaluation board includes
#include "stm32f4_discovery.h"

// Peripheral includes
#include "uart.h"
#include "ansi.h"
#include "ustime.h"
#include "EPOS.h"
#include "servo.h"
#include "MPU6050.h"
#include "HardwareConfig.h"

// Cubli includes
#include "Vector3.h"
#include "Matrix3x3F.h"
#include "Quaternion.h"

#include "Shortcuts.h"
#include "LUT.h"
#include "SDIO.h"
#include "Encoder.h"
#include "uart.h"



// Task functions includes
/* Includes above are technically not necessary, but make
 * debugging easier through indirectly defined bugs.
 */
#include "Peripherals.h"
#include "StateEstimation.h"
#include "Controllers.h"
#include "UtilityTask.h"

//Value of __errno gets changed if a math function (like log) is called for an
//invalid argument (Example: log(-1));
//We don't need to care about, we just need to define it
int __errno;

// Current/Velocity queues for CAN
xQueueHandle _velocityQueue[3];
xQueueHandle _currentQueue[3];

//Task Handles (to delete tasks or change their priority)
xTaskHandle E2CJumpUpTaskHandle;
xTaskHandle F2CJumpUpTaskHandle;
xTaskHandle F2EJumpUpTaskHandle;
xTaskHandle LQR1DTaskHandle;
xTaskHandle AttitudeTaskHandle;
xTaskHandle MPUTaskHandle;

// flag to shut down system.
bool _shutDown=false;
extern bool _writeData;

int main(void)
{
	STM_EVAL_LEDInit(LED3);
	STM_EVAL_LEDInit(LED2);
	STM_EVAL_LEDInit(LED1);

	LEDOn(LED1);


	//Initialize queues
	uint8_t i;
	for(i = 0; i < NUM_MOTORS; ++i)
	{
		_velocityQueue[i] = xQueueCreate(1, sizeof(CanRxMsg));
		_currentQueue[i] = xQueueCreate(1, sizeof(CanRxMsg));
	}

	InitHardware();
#ifndef CONTROLLER_SILENT
	printf("\nBasic Hardware Initialization Done. Start Tasks.\r\n");
#endif


	xTaskCreate( MPU6050TestTask,"MPU6000", configMINIMAL_STACK_SIZE + 450, NULL, tskIDLE_PRIORITY + 4, &MPUTaskHandle );
	xTaskCreate( AttitudeEstimator,"Attitude", configMINIMAL_STACK_SIZE + 400, NULL, tskIDLE_PRIORITY + 3, &AttitudeTaskHandle);
	xTaskCreate( CANTask, "CAN", configMINIMAL_STACK_SIZE + 400, NULL,	tskIDLE_PRIORITY + 3, NULL);


	// Balancing Controllers
	xTaskCreate( LQRTask1DNew, "LQR_1D", configMINIMAL_STACK_SIZE + 350, NULL,	tskIDLE_PRIORITY + 2, &LQR1DTaskHandle);
	xTaskCreate( NonLinControllerTask, "NonLin", configMINIMAL_STACK_SIZE + 550, NULL,	tskIDLE_PRIORITY + 2, NULL);
	xTaskCreate( FeedbackLinSpin, "Spin", configMINIMAL_STACK_SIZE + 550, NULL, tskIDLE_PRIORITY + 2, NULL);
	//xTaskCreate( Track3DJump, "Track3D", configMINIMAL_STACK_SIZE+850, NULL, tskIDLE_PRIORITY+2, NULL);
	xTaskCreate( CatchingControllerTask, "Catch3D", configMINIMAL_STACK_SIZE+650, NULL, tskIDLE_PRIORITY+2, NULL);
	//xTaskCreate( SpinStab, "Spin", configMINIMAL_STACK_SIZE+550, NULL, tskIDLE_PRIORITY+2, NULL);


	// Jumping Tasks
	xTaskCreate( Face2CornerJumpUpLearning2, "F2EJumpUpLearning", configMINIMAL_STACK_SIZE + 350, NULL,	tskIDLE_PRIORITY + 2, &F2CJumpUpTaskHandle);
	xTaskCreate( E2CJumpUpLearning3, "E2CJumpUpLearning", configMINIMAL_STACK_SIZE + 550, NULL,	tskIDLE_PRIORITY + 2, &E2CJumpUpTaskHandle);
	xTaskCreate( SpeedUpWheels, "SpeedUpWheels", configMINIMAL_STACK_SIZE + 350, NULL, tskIDLE_PRIORITY+2, NULL);
	xTaskCreate( Track1DJump, "Track1D", configMINIMAL_STACK_SIZE+450, NULL, tskIDLE_PRIORITY+2, NULL );
	xTaskCreate( Face2EdgeJumpUpLearning2, "F2EJumpUpLearning", configMINIMAL_STACK_SIZE + 450, NULL,	tskIDLE_PRIORITY + 2, NULL);


	// Usability/Performance Enhancement Tasks
	xTaskCreate( ModeManagement, "ModeManagement", configMINIMAL_STACK_SIZE + 250, NULL,	tskIDLE_PRIORITY + 2, NULL);
	xTaskCreate( ChoreographyManagement, "ChoreographyManagement", configMINIMAL_STACK_SIZE + 250, NULL,	tskIDLE_PRIORITY + 2, NULL);

	xTaskCreate( FilterManagement, "FilterManagement", configMINIMAL_STACK_SIZE + 250, NULL,	tskIDLE_PRIORITY + 3, NULL);
	xTaskCreate( WheelStop, "WheelVelocityMangagement", configMINIMAL_STACK_SIZE + 350, NULL,	tskIDLE_PRIORITY + 2, NULL);
    xTaskCreate( HeartBeat, "HeartBeat", configMINIMAL_STACK_SIZE + 250, NULL,	tskIDLE_PRIORITY + 5, NULL);

    xTaskCreate( ComManagement, "ComManagement", configMINIMAL_STACK_SIZE + 250, NULL, tskIDLE_PRIORITY + 3, NULL); //TODO: define priority and size

    xTaskCreate( VoltageMonitor, "VoltageMonitor", configMINIMAL_STACK_SIZE + 250, NULL,	tskIDLE_PRIORITY + 2, NULL);


    // Utility Tasks
    //xTaskCreate( BrakeTestTask, "BreakTestTask", configMINIMAL_STACK_SIZE + 350, NULL,	tskIDLE_PRIORITY + 2, NULL);
	//xTaskCreate( IMUCalibration,"Calib", configMINIMAL_STACK_SIZE + 500, NULL, tskIDLE_PRIORITY + 3, NULL );
	//xTaskCreate( ServoTestTask, "ServoTestTask", configMINIMAL_STACK_SIZE+350,NULL, tskIDLE_PRIORITY+2, NULL);
    //xTaskCreate( ServoTestTask3, "ServoTestTask", configMINIMAL_STACK_SIZE+350,NULL, tskIDLE_PRIORITY+2, NULL);

    //xTaskCreate( ControlledFall, "LQR_1D", configMINIMAL_STACK_SIZE + 350, NULL,	tskIDLE_PRIORITY + 2, &LQR1DTaskHandle);


	// Test tasks
	//xTaskCreate( CheckProcessingCapacity, "CPC", configMINIMAL_STACK_SIZE + 350, NULL,	tskIDLE_PRIORITY + 5, NULL);

	// Old tasks
    //xTaskCreate( SDCardReadWriteTextTask,"SDCard", configMINIMAL_STACK_SIZE + 450, NULL, tskIDLE_PRIORITY + 3, NULL );
	//xTaskCreate( UartTestTask, "UartTestTask", configMINIMAL_STACK_SIZE + 350, NULL,	tskIDLE_PRIORITY + 2, NULL);


#ifndef CONTROLLER_SILENT
	printf("About to Start Scheduler\r\n");
#endif

	vTaskStartScheduler();

#ifndef CONTROLLER_SILENT
	printf("\n\rTaskSchedularexited");
#endif
	while (1);

}
