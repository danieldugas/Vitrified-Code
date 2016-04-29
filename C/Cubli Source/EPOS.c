/**
 ******************************************************************************
 * @file    EPOS.c
 * @author  Michael Merz, Tobias Widmer
 * @version V1.0
 * @date    13/12/2011
 * @brief   This file provides firmware functions to configure and control the EPOS 50/5 Maxon Motor Controller.
 ******************************************************************************
 */

#include "EPOS.h"


/** @addtogroup Source
 * @{
 */

/** @addtogroup Low_Level
 * @{
 */

/** @defgroup EPOS
 * @brief This file provides firmware functions to manage the Maxon EPOS 50/5 Controller.
 * @{
 */

/** @defgroup EPOS_Private_Defines
 * @{
 */

#define WRITE   0x22
#define READ 	0x40

#define MAX_CURRENT 4000
/**
 * @}
 */
/** @defgroup EPOS_Private_Variables
 * @{
 */
const EPOSMessage _canBitrate = { WRITE, 0x2001, 0x00, 0x00000000 };
const EPOSMessage _RS232BaudRate = { WRITE, 0x2002, 0x00, 0x00000003 };
const EPOSMessage _motorType = { WRITE, 0x6402, 0x00, 0x0000000B }; //0x0B = 11 - Hall sensor only
const EPOSMessage _continuousCurrent = { WRITE, 0x6410, 0x01, 2000 };
const EPOSMessage _maxCurrent = { WRITE, 0x6410, 0x02, MAX_CURRENT };// Unit: mA, the output current limit should be choosen twice the maximum allowed continous current
const EPOSMessage _polePairNumber = { WRITE, 0x6410, 0x03, 8 };
const EPOSMessage _maxSpeed = { WRITE, 0x6410, 0x04, 6710 };
const EPOSMessage _thermalConstant = { WRITE, 0x6410, 0x05, /*166*/166 };//166=^16.6s
const EPOSMessage _currentPGain = { WRITE, 0x60F6, 0x01, 586 };
const EPOSMessage _currentIGain = { WRITE, 0x60F6, 0x02, 56 };
const EPOSMessage _velocityMode = { WRITE, 0x6060, 0x00, 0x000000FE };
const EPOSMessage _currentMode = { WRITE, 0x6060, 0x00, 0x000000FD };
const EPOSMessage _maxVelocity = { WRITE, 0x607F, 0x00, 16000 };
const EPOSMessage _maxAcceleration = { WRITE, 0x60C5, 0x00, 0xFFFFFFFF };// Motor specificaiton(4294967295)
const EPOSMessage _requestAvgVelocity = { READ, 0x2028, 0x00, 0x00000000 };
const EPOSMessage _requestAvgCurrent = { READ, 0x2027, 0x00, 0x00000000 };
const EPOSMessage _requestActualVelocity = { READ, 0x606C, 0x00, 0x00000000 };
const EPOSMessage _requestSensorActualVelocity = { READ, 0x6069, 0x00, 0x00000000 };
const EPOSMessage _requestVersion = { READ, 0x2003, 0x01, 0x00000000 };
const EPOSMessage _requestActualCurrent = { READ, 0x6078, 0x00, 0x00000000 };
const EPOSMessage _error = { WRITE, 0x6040, 0x00, 0x00000080 };
const EPOSCommand _velocity = { WRITE, 0x206B, 0x00 };
const EPOSCommand _current = { WRITE, 0x2030, 0x00 };
//const EPOSCommand _maxContinuousCurrent = { WRITE, 0x6410, 0x01 };


//Enable, disble device commands
//EPOS2 Firmware Specification page 17, adresses page 190
const EPOSMessage _shutdown = { WRITE, 0x6040, 0x00, 0b00000110 };
const EPOSMessage _switchOn = { WRITE, 0x6040, 0x00, 0b00000111 };
const EPOSMessage _switchOnAndEnable = { WRITE, 0x6040, 0x00, 0b00001111 };
const EPOSMessage _disableVoltage = { WRITE, 0x6040, 0x00, 0b00000000 };
const EPOSMessage _quickstop = { WRITE, 0x6040, 0x00, 0b00000010 };
const EPOSMessage _disable = { WRITE, 0x6040, 0x00, 0b00000111 };
const EPOSMessage _enable = { WRITE, 0x6040, 0x00, 0b00001111 };
const EPOSMessage _faultReset = { WRITE, 0x6040, 0x00, 0b10000000 };

CanRxMsg *RxMessage;
uint8_t TransmitMailbox = 0;

/**
 * @}
 */

/** @defgroup EPOS_Private_Functions
 * @{
 */

/**
 * @}
 */


void EPOSInit(CanInitData* init)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* CAN GPIOs configuration **************************************************/

	/* Enable GPIO clock */
	RCC_AHB1PeriphClockCmd(init->GPIOClock, ENABLE);

	//RCC_APB2PeriphClockCmd(CAN_GPIO_CLK, ENABLE);

	/* Connect CAN pins to AF9 */
	//GPIO_PinAFConfig(init->Port, init->PinSourceRX, init->GPIO_AF);
	//GPIO_PinAFConfig(init->Port, init->PinSourceTX, init->GPIO_AF);
	int index = 0;
	uint8_t pinSource = GetPinSource(init->Pins, index);
	while(pinSource != 0xFF)
	{
	  GPIO_PinAFConfig(init->GPIO, pinSource, init->GPIO_AF);
	  pinSource = GetPinSource(init->Pins, ++index);
	}

	/* Configure CAN RX and TX pins */
	GPIO_InitStructure.GPIO_Pin = init->Pins;//init->PinRX | init->PinTX;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(init->GPIO, &GPIO_InitStructure);

	/* CAN configuration ********************************************************/
	/* Enable CAN clock */
	//RCC_APB1PeriphClockCmd(init->CANClock, ENABLE);
	RCC_APB1PeriphClockCmd(init->CANClock, ENABLE);

	/* CAN register init */
	CAN_DeInit(init->CanLine);

	//CAN_StructInit(&CAN_InitStructure);

	/* CAN cell init */
	CAN_InitTypeDef CAN_InitStructure;
	CAN_InitStructure.CAN_TTCM = DISABLE;
	CAN_InitStructure.CAN_ABOM = DISABLE;
	CAN_InitStructure.CAN_AWUM = DISABLE;
	CAN_InitStructure.CAN_NART = DISABLE;
	CAN_InitStructure.CAN_RFLM = ENABLE;
	CAN_InitStructure.CAN_TXFP = DISABLE;
	CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;
	CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;

	/* CAN Baudrate = 1 MBps (CAN clocked at 42 MHz) */
	CAN_InitStructure.CAN_BS1 = CAN_BS1_14tq;
	CAN_InitStructure.CAN_BS2 = CAN_BS2_6tq;
	CAN_InitStructure.CAN_Prescaler = 2;
	CAN_Init(init->CanLine, &CAN_InitStructure);

	/* CAN filter init */
	CAN_FilterInitTypeDef CAN_FilterInitStructure;
	if(init->CanLine == CAN1)
		CAN_FilterInitStructure.CAN_FilterNumber = 0;
	else if(init->CanLine == CAN2)
		CAN_FilterInitStructure.CAN_FilterNumber = 14;
	else
		CAN_FilterInitStructure.CAN_FilterNumber = 0;
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
	CAN_FilterInitStructure.CAN_FilterIdHigh = 0x0000;
	CAN_FilterInitStructure.CAN_FilterIdLow = 0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = 0;
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);

	/* Enable FIFO 0 message pending Interrupt */
	CAN_ITConfig(init->CanLine, CAN_IT_FMP0, ENABLE);
}

void CanSendMessage(uint32_t nodeID, const EPOSMessage* message)
{
	CanTxMsg txMessage;

	txMessage.StdId = nodeID;
	txMessage.RTR = CAN_RTR_DATA;
	txMessage.IDE = CAN_ID_STD;
	txMessage.DLC = 8;
	txMessage.Data[0] = message->AccessType;
	txMessage.Data[1] = (message->Index) & 0xff;
	txMessage.Data[2] = ((message->Index) >> 8) & 0xff;
	txMessage.Data[3] = message->SubIndex;
	txMessage.Data[4] = (message->Value) & 0xff;
	txMessage.Data[5] = ((message->Value) >> 8) & 0xff;
	txMessage.Data[6] = ((message->Value) >> 16) & 0xff;
	txMessage.Data[7] = ((message->Value) >> 24) & 0xff;


	//uint8_t CanTransmitStatus = 0;
	int i = 0;
	portENTER_CRITICAL();
	{
		uint8_t TransmitMailbox = CAN_Transmit(CAN1, &txMessage);
		while ((CAN_TransmitStatus(CAN1, TransmitMailbox) != CANTXOK) && (i != 0xFF)) {
			i++;
		}
	}
	portEXIT_CRITICAL();
}

void CanSendCommand(uint32_t nodeID, const EPOSCommand* command, int32_t value)
{
	CanTxMsg txMessage;

	txMessage.StdId = nodeID;
	txMessage.RTR = CAN_RTR_DATA;
	txMessage.IDE = CAN_ID_STD;
	txMessage.DLC = 8;
	txMessage.Data[0] = command->AccessType;
	txMessage.Data[1] = (command->Index) & 0xff;
	txMessage.Data[2] = ((command->Index) >> 8) & 0xff;
	txMessage.Data[3] = command->SubIndex;
	txMessage.Data[4] = value & 0xff;
	txMessage.Data[5] = (value >> 8) & 0xff;
	txMessage.Data[6] = (value >> 16) & 0xff;
	txMessage.Data[7] = (value >> 24) & 0xff;

	//uint8_t CanTransmitStatus = 0;
	int i = 0;
	portENTER_CRITICAL();
	{
		uint8_t TransmitMailbox = CAN_Transmit(CAN1, &txMessage);
		while ((CAN_TransmitStatus(CAN1, TransmitMailbox) != CANTXOK) && (i != 0xFF)) {
			i++;
		}
	}
	portEXIT_CRITICAL();
}

/**
 * @brief  Configures the velocity mode.
 * @param  NODE_NUMBER
 * @retval None
 */
void VelocityMode(uint32_t nodeID) {
	CanSendMessage(nodeID, &_velocityMode);
}

/**
 * @brief  Set the velocity.
 * @param  NODE_NUMBER, Target Velocity
 * @retval None
 */
void SetVelocity(uint32_t nodeID, int32_t target_velocity) {
	CanSendCommand(nodeID, &_velocity, target_velocity);
}

/**
 * @brief  Configures the current mode.
 * @param  NODE_NUMBER
 * @retval None
 */
void CurrentMode(uint32_t nodeID) {
	CanSendMessage(nodeID, &_currentMode);
}

void EPOSEnable(uint32_t nodeID) {
	CanSendMessage(nodeID, &_shutdown);
	Delay(300);
	CanSendMessage(nodeID, &_switchOnAndEnable);
	//After startup, intialization sometimes need to be done twice for some mysterious reasons
//	Delay(300);
//	CanSendMessage(nodeID, &_shutdown);
//	Delay(100);
//	CanSendMessage(nodeID, &_switchOnAndEnable);
}

/**
 * @brief  Set_Current: Sets Current depending on target_current in range +- ouput_current_limit e.g target_current = 0xFFFF =^ maximum current of 5A and 0x0000 -max current of 5A
 * @param  NODE_NUMBER, Target Velocity
 * @retval None
 */
void SetCurrent(uint32_t nodeID, s16 target_current) {
	if (target_current > MAX_CURRENT)
		target_current = MAX_CURRENT;
	if (target_current < -MAX_CURRENT)
		target_current = -MAX_CURRENT;

	CanSendCommand(nodeID, &_current, target_current);
}

/**
 * @brief  Set request for motor speed over CAN.
 * @param  NODE_NUMBER
 * @retval None
 */
void GetAvgVelocity(uint32_t nodeID) {
	CanSendMessage(nodeID, &_requestAvgVelocity);
	//CanSendMessage(nodeID, &_requestVersion);// doesnt work
}

void GetActualVelocity(uint32_t nodeID) {
	CanSendMessage(nodeID, &_requestActualVelocity);
}

void GetAvgCurrent(uint32_t nodeID) {
	CanSendMessage(nodeID, &_requestAvgCurrent);
}

void GetActualCurrent(uint32_t nodeID) {
	CanSendMessage(nodeID, &_requestActualCurrent);
}

//void GetCANData(uint32_t nodeID){
//	CanSendMessage(nodeID, &_requestVersion);
//}


/**
 * @brief  Configures motor type, CAN bitrate, current limits and other parameters.
 * @param  NODE_NUMBER
 * @retval None
 */
void InitEPOSNode(uint32_t nodeID) {
	EPOSEnable(nodeID);
	CanSendMessage(nodeID, &_canBitrate);
	//CanSendMessage(nodeID, &_RS232BaudRate);
	CanSendMessage(nodeID, &_motorType);
	CanSendMessage(nodeID, &_continuousCurrent);
	CanSendMessage(nodeID, &_maxCurrent);
	CanSendMessage(nodeID, &_polePairNumber);
	CanSendMessage(nodeID, &_thermalConstant);
	//CanSendMessage(nodeID, &_currentPGain);
	//CanSendMessage(nodeID, &_currentIGain);

	CanSendMessage(nodeID, &_continuousCurrent);
	CanSendMessage(nodeID, &_maxSpeed);
	CanSendMessage(nodeID, &_maxVelocity);
	CanSendMessage(nodeID, &_maxAcceleration);
}

/**
 * @brief  Resets all Nodes.
 * @param  None
 * @retval None
 */
void ResetNode(uint32_t nodeID) {
	CanSendMessage(nodeID, &_error);
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
