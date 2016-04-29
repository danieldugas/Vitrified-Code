/*
 * EPOS.h
 *
 *  Created on: Jun 7, 2011
 *      Author: Michael Merz, Tobias Widmer
 */
#ifndef EPOS_H_
#define EPOS_H_

#pragma once

#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "Shortcuts.h"


/** @addtogroup Source
* @{
*/

/** @addtogroup Low_Level
* @{
*/

/** @addtogroup EPOS
* @{
*/


/** @defgroup EPOS_Exported_TypesDefinitions
* @{
*/
typedef struct MotorData
{
  uint16_t NodeID;
  float Velocity;
} MotorData;

typedef struct
{
	CAN_TypeDef* CanLine; //Example: CAN1
	GPIO_TypeDef* GPIO; // Example: GPIOD
	uint8_t GPIO_AF; //Example: GPIO_AF_CAN1
	uint16_t Pins; //PinRx | PinTx
	uint32_t GPIOClock; // Example: RCC_AHB1Periph_GPIOD
	uint32_t CANClock; // Example: RCC_APB1Periph_CAN1
} CanInitData;

typedef struct
{
	uint8_t AccessType;
	uint16_t Index;
	uint8_t SubIndex;
} EPOSCommand;

typedef struct
{
	uint8_t AccessType;
	uint16_t Index;
	uint8_t SubIndex;
	int32_t Value;
} EPOSMessage;

/**
* @}
*/

/** @defgroup EPOS_Exported_Functions
* @{
*/
void EPOSInit(CanInitData* init);
void CanSendMessage(uint32_t nodeID, const EPOSMessage* message);
void CanSendCommand(uint32_t nodeID, const EPOSCommand* command, int32_t value);
void VelocityMode(uint32_t nodeID);
void SetVelocity(uint32_t nodeID, int32_t target_velocity);
void CurrentMode(uint32_t nodeID);
void EPOSEnable(uint32_t nodeID);
void SetCurrent(uint32_t nodeID, s16 target_current);
void GetAvgVelocity(uint32_t nodeID);
void GetActualVelocity(uint32_t nodeID);
void GetAvgCurrent(uint32_t nodeID);
void GetActualCurrent(uint32_t nodeID);
void InitEPOSNode(uint32_t nodeID);
void ResetNode(uint32_t nodeID);
/**
* @}
*/

/**
* @}
*/

/**
* @}
*/

/**
* @}
*/

#endif /* EPOS_H_ */

