#ifndef MATRIX3X3_H
#define MATRIX3X3_H


#include "Vector3.h"
#include "math.h"

typedef struct Matrix3x3 {
	float e11;
	float e12;
	float e13;
	float e21;
	float e22;
	float e23;
	float e31;
	float e32;
	float e33;
} Matrix3x3;

void Transpose(Matrix3x3* m, Matrix3x3* res);
void M3x3TransformVector(Vector3* v, Matrix3x3* m, Vector3* res);
void M3RotationX(float angle, Matrix3x3* res);
void M3RotationY(float angle, Matrix3x3* res);
void M3RotationZ(float angle, Matrix3x3* res);
void M3RotationAxis(Vector3 a, float angle, Matrix3x3* res);
void M3Scaling3(float x, float y, float z, Matrix3x3* res);
void M3Scaling(float s, Matrix3x3* res);
void M3ScalingV3(Vector3* v, Matrix3x3* res);
void M3Add(Matrix3x3* m1, Matrix3x3* m2, Matrix3x3* res);
void M3Sub(Matrix3x3* m1, Matrix3x3* m2, Matrix3x3* res);
void M3Mul(Matrix3x3* m1, Matrix3x3* m2, Matrix3x3* res);
void M3MulVM(Vector3* v, Matrix3x3* m, Vector3* res);
void M3MulMV(Matrix3x3* m, Vector3* v, Vector3* res);
void M3MulS(float f, Matrix3x3* m, Matrix3x3* res);
void Transform(Vector3* v, Matrix3x3* m, Vector3* res);
void M3Identity(Matrix3x3* res);
float M3x3Determinant(Matrix3x3* m);
void M3Inverse(Matrix3x3* m, Matrix3x3* res);
void V3TildeMatrix(Vector3 *v,Matrix3x3 *res);

#endif
