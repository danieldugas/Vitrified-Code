#ifndef HARDWARECONFIG_H
#define HARDWARECONFIG_H


#include "Matrix3x3F.h"
#include "Peripherals.h"

//#include "stm32f4xx.h"
//#include "stm32f4_discovery.h"
//#include "EPOS.h"
//#include "servo.h"
#include "MPU6050.h"
#include "uart.h"
#include "Shortcuts.h"
#include "StateEstimation.h"
#include "CubliParameters.h"
#include "UtilityTask.h"

void InitHardware();
void NVIC_Config();
void EXTILine0_Config();
void OnButtonInit();

void TestIMU(int N);

#endif
