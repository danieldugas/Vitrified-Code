#ifndef SHORTCUTS_H
#define SHORTCUTS_H

#pragma once

#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "stm32f4_discovery.h"
#include "task.h"

#define PI 3.1415926535898f
#define G 9.81f

#define EnC() taskENTER_CRITICAL()
#define ExC() taskEXIT_CRITICAL()
#define LEDOn(led) STM_EVAL_LEDOn(led);
#define LEDOff(led) STM_EVAL_LEDOff(led);
#define LEDToggle(led) STM_EVAL_LEDToggle(led);
#define Delay(waitTime) vTaskDelay(waitTime / portTICK_RATE_MS);
#define DelayUntilTime(lastExecutionTime, waitTime) vTaskDelayUntil(lastExecutionTime, waitTime / portTICK_RATE_MS);

uint8_t GetPinSource(uint16_t GPIOPins, int index);

#endif
