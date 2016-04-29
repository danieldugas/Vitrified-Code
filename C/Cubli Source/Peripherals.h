#ifndef PERIPHERALS_H
#define PERIPHERALS_H

#pragma once

#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include "stdbool.h"
extern int __errno;

#include "EPOS.h"
#include "servo.h"
#include "SDIO.h"
#include <time.h>

#include "HardwareConfig.h"
#include "stm32f4xx.h"


#define MOTORX 0
#define MOTORY 1
#define MOTORZ 2

#define SERVOX 0
#define SERVOY 1
#define SERVOZ 2

#define NUM_MOTORS 3
#define NUM_SERVOS 3 //keep it at 3 even for the 1D case should not matter

// Global Variables from Peripherals.c
extern MotorData _motor[NUM_MOTORS];
extern MotorData _motor1;
extern MotorData _motor2;
extern MotorData _motor3;
extern int16_t _measuredCurrent[NUM_MOTORS];

int getServoClose(int index);
void setServoClose(int index, int flag);

extern CanInitData _canInit;
extern int _servoPos;

// Checks whether motors send back their velocity measurements --- > generates printf output.
void CANVelMeasurementTest();

// Gets new velocity measurements and writes them to the velocity queue (requires CANInitialize() to be run first)
void CANGetNewVelocityMeasurements();

// Gets new current measurements and writes them to the current queue (requires CANInitialize() to be run first)
void CANGetNewCurrentMeasurements();

// Initializes the EasyPositioningSystem.
void CANInitialize();

// Implements the functions above as a periodic Task
void CANTask(void* parameters);

void ServoTask(void *pvParameters);
void SDCardReadWriteTextTask(void* parameters);

#endif
