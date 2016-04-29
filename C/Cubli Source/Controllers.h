#ifndef CONTROLLERS_H
#define CONTROLLERS_H


#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include "stdbool.h"

#include "uart.h"

#include "Vector3.h"
#include "Matrix3x3F.h"
#include "Quaternion.h"

#include "Shortcuts.h"
#include "LUT.h"

#include "StateEstimation.h"
#include "Peripherals.h"
#include "UtilityTask.h"

#include "com_protocol.h"
#include "timeline_status.h"
#include "CubliParameters.h"
//extern enum communicationIDs;
//typedef enum communicationIDs com_id; // in CubliParameters.h

//typedefs
enum BalancingMode {NotBalancing = 0, EdgeBalancing = 1, CornerBalancing = 2, Track1D = 3, Track3D = 4, Spin = 5};
typedef enum CubeModes
{
    // Non-choreography modes
    Idle         ,
    ButtonMode1  ,
    ButtonMode2  ,
    ButtonMode3  ,

    // Choreography modes
    Get2E        ,
	Get2C        ,
    GetDown      ,
	SingleSpin

} cube_mode;
enum JumpMode
{
	NoJump = 0  ,
	F2EJump = 1 ,
	E2CJump = 2 ,
	F2CJump = 3 ,
	WalkJump = 4
};

cube_mode cycleButtonMode( cube_mode buttonModeNumber );
int       isAButtonMode( cube_mode X );

void DetermineRotAxisAndEqAngle(uint8_t* axisIndex, float* eqAngle);
void DetermineRotAxisParam(uint8_t* axisIndex, uint8_t* axisNumber, float* eqAngle);
void NonLinControllerTask(void* parameters);
void FeedbackLinSpin(void *pvParameters);
void FeedbackLinSpinStab(void *pvParameters);
void SpinStab(void *pvParameters);

void LQRTask1D(void *pvParameters);
void LQRTask1DNew(void *pvParameters);
void ComManagement(void *pvParameters);
void ChoreographyManagement(void *pvParameters);
void ModeManagement(void *pvParameters);
void FilterManagement(void *pvParameters);
void WheelStop(void *pvParameters);
void ReturnToXYFace(void *pvParameters);
void SpeedUpWheels(void *pvParameters);
void Track1DJump(void *pvParameters);
void Track3DJump(void *pvParameters);
void SetCurrentWSaturation(uint8_t MotorIndex,int16_t* currentmA);
void SetTorqueWSaturation(uint8_t MotorIndex,float* T);

//void UartTestTask(void *pvParameters);
void F2EJumpUpLearning(void *pvParameters);
void Face2EdgeJumpUpLearning2(void *pvParameters);
void Face2CornerJumpUpLearning2(void *pvParameters);
void E2CJumpUpLearning(void *pvParameters);
void E2CJumpUpLearning3(void *pvParameters);
void F2CJumpUpLearning(void *pvParameters);
void BrakeTestTask(void *pvParameters);
void ServoTestTask(void *pvParameters);
void ServoTestTask3(void *pvParameters);
void HeartBeat(void *pvParameters);

void CatchingControllerTask(void *pvParameters);

void GetDirection3DJump();
void ControlledFall(void *pvParameters);

#endif
