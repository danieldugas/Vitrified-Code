//Source: https://github.com/yigiter/Sample-STM32F4-codes

#ifndef __SDIOLIB_H
#define __SDIOLIB_H

#pragma once

#include "stdint.h"
//#include "EvalBoard.h"
#include "stm32f4xx.h"
//#include "UARTLib.h"
#include "Shortcuts.h"


#ifdef __cplusplus
 extern "C" {
#endif


 void SD_LowLevel_Init(void);
 void SD_Init(void);
 void SD_WriteSingleBlock(uint8_t *buf, uint32_t blk);
 void SD_ReadSingleBlock(uint8_t *buf, uint32_t blk);
 void SD_StartMultipleBlockWrite(uint32_t blk);
 void SD_StopMultipleBlockWrite(void);
 void SD_WriteData(uint8_t *buf, uint32_t cnt);
 void SD_WaitTransmissionEnd(void);


#ifdef __cplusplus
}
#endif
#endif //__SDIOLIB_H

