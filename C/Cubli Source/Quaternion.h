#ifndef QUATERNION_H
#define QUATERNION_H

#pragma once

#include "Vector3.h"
#include "Matrix3x3F.h"
#include "math.h"
#include "stdlib.h"
#include "math.h"
#include "Vector3.h"
#include "Matrix3x3F.h"

typedef struct {
	float X;
	float Y;
	float Z;
	float W;
} Quaternion;

void QToEuler(Quaternion* q, Vector3* euler);
void RotMatToXYZAngle(Matrix3x3* mat, Vector3* res);
void QToRotMat(Quaternion* q, Matrix3x3* mat);
void QMulS(float* f, Quaternion* q, Quaternion* res);
void QInterpolate(Quaternion* q1, Quaternion* q2, float* a, float* b, Quaternion* res);
void QNormalize(Quaternion* q, Quaternion* res);
void QTrafo(Quaternion* q, Vector3* p1, Vector3* res);
void QConjugate(Quaternion* q, Quaternion* res);
void QMul(Quaternion* q1, Quaternion* q2, Quaternion* res);
void QMulV(Quaternion* q, Vector3* v, Vector3* res);
void QAxisAngle(Vector3* v, float* angle, Quaternion* res);
void QFromEuler(float pitch, float yaw, float roll, Quaternion* res);
void QToEulerAngles(Quaternion* q, Vector3* angles);
void QMatrix(Quaternion* q, Matrix3x3* res);
float QGetAxisAngle(Quaternion* q, Vector3* outAxis);

#endif
