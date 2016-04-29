#ifndef ENCODER_H
#define ENCODER_H

#pragma once

#include "stm32f4xx.h"
#include "Shortcuts.h"

void InitEncoder(void);
void UpdateEncoderValue();
//void EncoderReader(void* parameters);

#endif
