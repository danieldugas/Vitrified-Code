#ifndef US_TIME_H
#define US_TIME_H

#pragma once

#include <FreeRTOS.h>
#include "stm32f4xx.h"
#include <stdint.h>

void     delay_us(unsigned long us);
void     delay_ms(unsigned long ms);
uint64_t get_us_time(void);
void     init_us_timer(void);

#endif
