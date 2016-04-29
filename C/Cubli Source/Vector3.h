#ifndef VECTOR3_H
#define VECTOR3_H

#pragma once

#include "stm32f4xx.h"
#include "stdbool.h"
#include "math.h"
#include "stdint.h"
#include "Shortcuts.h"

typedef struct Vector3 {
	float X;
	float Y;
	float Z;
} Vector3;

typedef struct
{
	int16_t X;
	int16_t Y;
	int16_t Z;
} Vector3I16;

typedef struct
{
	int32_t X;
	int32_t Y;
	int32_t Z;
} Vector3I32;

void V3Length(Vector3* v, float* res);
void V3LengthPow2(Vector3* v, float* res);
void V3Normalize(Vector3* v, Vector3* res);
bool V3Equals(Vector3* v1, Vector3* v2, float epsilon);
void V3Dot(Vector3* v1, Vector3* v2, float* res);
void V3Angle(Vector3* v1, Vector3* v2, float* res);
void V3Angle0(Vector3* v1, Vector3* v2, float* res);
void V3Cross(Vector3* v1, Vector3* v2, Vector3* res);
void V3Add(Vector3* v1, Vector3* v2, Vector3* res);
void V3Sub(Vector3* v1, Vector3* v2, Vector3* res);
void V3Minus(Vector3* v1, Vector3* res);
void V3MulS(float* f, Vector3* v, Vector3* res);
void V3DegToRad(Vector3* deg, Vector3* res);
void V3RadToDeg(Vector3* rad, Vector3* res);
void V3I16Add(Vector3I16* v1, Vector3I16* v2, Vector3I16* res);
void V3I16Div(Vector3I16* v, uint16_t* f, Vector3I16* res);

#endif
