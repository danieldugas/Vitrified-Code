#include "Matrix3x3F.h"


void Transpose(Matrix3x3* m, Matrix3x3* res) {
	res->e11 = m->e11; res->e12 = m->e21; res->e13 = m->e31;
	res->e21 = m->e12; res->e22 = m->e22; res->e23 = m->e32;
	res->e31 = m->e13; res->e32 = m->e23; res->e33 = m->e33;
}

void M3x3TransformVector(Vector3* v, Matrix3x3* m, Vector3* res) {
	res->X = v->X * m->e11 + v->Y * m->e21 + v->Z * m->e31;
	res->Y = v->X * m->e12 + v->Y * m->e22 + v->Z * m->e32;
	res->Z = v->X * m->e13 + v->Y * m->e23 + v->Z * m->e33;

}

void M3RotationX(float angle, Matrix3x3* res) {
	float c = cos(angle);
	float s = sin(angle);

	res->e11 = 1; res->e12 = 0; res->e13 = 0;
	res->e21 = 0; res->e22 = c; res->e23 = s;
	res->e31 = 0; res->e32 = -s;res->e33 = c;
}

void M3RotationY(float angle, Matrix3x3* res) {
	float c = cos(angle);
	float s = sin(angle);

	res->e11 = c; res->e12 = 0; res->e13 = -s;
	res->e21 = 0; res->e22 = 1; res->e23 = 0;
	res->e31 = s; res->e32 = 0;	res->e33 = c;
}

void M3RotationZ(float angle, Matrix3x3* res) {
	float c = cos(angle);
	float s = sin(angle);

	res->e11 = c; res->e12 = s; res->e13 = 0;
	res->e21 = -s;res->e22 = c; res->e23 = 0;
	res->e31 = 0; res->e32 = 0;	res->e33 = 1;
}

void M3RotationAxis(Vector3 a, float angle, Matrix3x3* res) {
	Vector3 axis;
	V3Normalize(&a, &axis);

	float c = cos(angle);
	float ic = 1 - c;
	float s = sin(angle);

	res->e11 = c + axis.X * axis.X * ic;
	res->e12 = axis.Y * axis.X * ic + axis.Z * s;
	res->e13 = axis.Z * axis.X * ic - axis.Y * s;
	res->e21 = axis.X * axis.Y * ic - axis.Z * s;
	res->e22 = c + axis.Y * axis.Y	* ic;
	res->e23 = axis.Z * axis.Y * ic + axis.X * s;
	res->e31 = axis.X * axis.Z	* ic + axis.Y * s;
	res->e32 = axis.Y * axis.Z * ic - axis.X * s;
	res->e33 = c + axis.Z * axis.Z * ic;
}

void M3Scaling3(float x, float y, float z, Matrix3x3* res) {
	res->e11 = x; res->e12 = 0; res->e13 = 0;
	res->e21 = 0; res->e22 = y; res->e23 = 0;
	res->e31 = 0; res->e32 = 0;	res->e33 = z;
}

void M3Scaling(float s, Matrix3x3* res) {
	M3Scaling3(s, s, s, res);
}

void M3ScalingV3(Vector3* v, Matrix3x3* res) {
	M3Scaling3(v->X, v->Y, v->Z, res);
}

void M3Add(Matrix3x3* m1, Matrix3x3* m2, Matrix3x3* res) {
	res->e11 = m1->e11 + m2->e11,
	res->e12 = m1->e12 + m2->e12,
	res->e13 = m1->e13 + m2->e13,
	res->e21 = m1->e21 + m2->e21,
	res->e22 = m1->e22 + m2->e22,
	res->e23 = m1->e23 + m2->e23,
	res->e31 = m1->e31 + m2->e31,
	res->e32 = m1->e32 + m2->e32,
	res->e33 = m1->e33 + m2->e33 ;
}

void M3Sub(Matrix3x3* m1, Matrix3x3* m2, Matrix3x3* res) {
	res->e11 = m1->e11 - m2->e11,
	res->e12 = m1->e12 - m2->e12,
	res->e13 = m1->e13 - m2->e13,
	res->e21 = m1->e21 - m2->e21,
	res->e22 = m1->e22 - m2->e22,
	res->e23 = m1->e23 - m2->e23,
	res->e31 = m1->e31 - m2->e31,
	res->e32 = m1->e32 - m2->e32,
	res->e33 = m1->e33 - m2->e33 ;
}

void M3Mul(Matrix3x3* m1, Matrix3x3* m2, Matrix3x3* res) {
	res->e11 = m1->e11 * m2->e11 + m1->e12 * m2->e21 + m1->e13 * m2->e31;
	res->e12 = m1->e11 * m2->e12 + m1->e12 * m2->e22 + m1->e13 * m2->e32;
	res->e13 = m1->e11 * m2->e13 + m1->e12 * m2->e23 + m1->e13 * m2->e33;

	res->e21 = m1->e21 * m2->e11 + m1->e22 * m2->e21 + m1->e23 * m2->e31;
	res->e22 = m1->e21 * m2->e12 + m1->e22 * m2->e22 + m1->e23 * m2->e32;
	res->e23 = m1->e21 * m2->e13 + m1->e22 * m2->e23 + m1->e23 * m2->e33;

	res->e31 = m1->e31 * m2->e11 + m1->e32 * m2->e21 + m1->e33 * m2->e31;
	res->e32 = m1->e31 * m2->e12 + m1->e32 * m2->e22 + m1->e33 * m2->e32;
	res->e33 = m1->e31 * m2->e13 + m1->e32 * m2->e23 + m1->e33 * m2->e33;
}

void M3MulVM(Vector3* v, Matrix3x3* m, Vector3* res) {
	res->X = v->X * m->e11 + v->Y * m->e21 + v->Z * m->e31;
	res->Y = v->X * m->e12 + v->Y * m->e22 + v->Z * m->e32;
	res->Z = v->X * m->e13 + v->Y * m->e23 + v->Z * m->e33;
}

void M3MulMV(Matrix3x3* m, Vector3* v, Vector3* res) {
	res->X = v->X * m->e11 + v->Y * m->e12 + v->Z * m->e13;
	res->Y = v->X * m->e21 + v->Y * m->e22 + v->Z * m->e23;
	res->Z = v->X * m->e31 + v->Y * m->e32 + v->Z * m->e33;
}

void M3MulS(float f, Matrix3x3* m, Matrix3x3* res) {
	res->e11 = f * res->e11; res->e12 = f * res->e12; res->e13 = f * res->e13;
	res->e21 = f * res->e21; res->e22 = f * res->e22; res->e23 = f * res->e23;
	res->e31 = f * res->e31; res->e32 = f * res->e32; res->e33 = f * res->e33;
}


void Transform(Vector3* v, Matrix3x3* m, Vector3* res) {
	M3MulVM(v, m, res);
}

void M3Identity(Matrix3x3* res) {
	res->e11 = 1; res->e12 = 0; res->e13 = 0;
	res->e21 = 0; res->e22 = 1; res->e23 = 0;
	res->e31 = 0; res->e32 = 0;	res->e33 = 1;
}

float M3x3Determinant(Matrix3x3* m) {
	return m->e11 * (m->e22 * m->e33 - m->e23 * m->e32) + m->e12 * (m->e23 * m->e31
			- m->e21 * m->e33) + m->e31 * (m->e21 * m->e32 - m->e22 * m->e31);
}

void M3Inverse(Matrix3x3* m, Matrix3x3* res) {
	float f = 1.0 / M3x3Determinant(m);
	res->e11 = f * (m->e22 * m->e33 - m->e32 * m->e23);
	res->e12 = f * (m->e13 * m->e32 - m->e33 * m->e12);
	res->e13 = f * (m->e12 * m->e23 - m->e22 * m->e13);
	res->e21 = f * (m->e23 * m->e31 - m->e33 * m->e21);
	res->e22 = f * (m->e11 * m->e33 - m->e31 * m->e13);
	res->e23 = f * (m->e13 * m->e21 - m->e23 * m->e11);
	res->e31 = f * (m->e21 * m->e32 - m->e31 * m->e22);
	res->e32 = f * (m->e12 * m->e31 - m->e32 * m->e11);
	res->e33 = f * (m->e11 * m->e22 - m->e21 * m->e12);
}

//returning the Tilde Matrix of a vector analog the lecture "Mechanik 3" of Prof. Glockner at the ETH Zuerich
void V3TildeMatrix(Vector3 *v,Matrix3x3 *res){
	res->e11 = 0;
	res->e12 = -v->Z;
	res->e13 = v->Y;

	res->e21 = v->Z;
	res->e22 = 0;
	res->e23 = -v->X;

	res->e31 = -v->Z;
	res->e32 = v->X;
	res->e33 = 0;
}
