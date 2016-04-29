#ifndef STATEESTIMATION_H_
#define STATEESTIMATION_H_

#pragma once

#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include "stdbool.h"
extern int __errno;

#include "Vector3.h"
#include "Quaternion.h"
#include "Matrix3x3F.h"

#include "Peripherals.h"
#include "HardwareConfig.h"

#include "MPU6050.h"
#include "Encoder.h"

// Global Variables from StateEstimation.c
extern Vector3 _gRef;
extern Vector3 _attitude;
extern Vector3 _gEst;
extern Quaternion _finalEst;
extern Vector3 _omega;
extern Vector3 _xyzAngles;	// Euler Angles zyx
extern Vector3 _edgeAngles; // Used to parametrize the inclination when balancing on edges
extern bool _imusReady;
extern bool _attitudeReady;
extern float _weightAccMeas;

extern I2CMember _imu[6];
extern AccI2C _acc[6];
extern GyroI2C _gyr[6];
extern I2CBusInitData _i2cBus1;
extern I2CBusInitData _i2cBus2;

void AttitudeEstimator(void* parameters);
void MPU6050TestTask(void* parameters);
void IMUCalibration(void* parameters);

void vApplicationIdleHook( void );
void CheckProcessingCapacity(void* parameters);

#endif //STATEESTIMATION_H_
