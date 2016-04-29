#include "Vector3.h"

//Do not return floats, that does not work
void V3Length(Vector3* v, float* res)
{
	(*res) = sqrt(v->X * v->X + v->Y * v->Y + v->Z * v->Z);
}

void V3LengthPow2(Vector3* v, float* res) {
	(*res) = v->X * v->X + v->Y * v->Y + v->Z * v->Z;
}

void V3Normalize(Vector3* v, Vector3* res) {
	float lengthPow2 = v->X * v->X + v->Y * v->Y + v->Z * v->Z;
	if (lengthPow2 >= 0.0001f) {
		float f = 1.0 / sqrt(lengthPow2);
		res->X = f * v->X;
		res->Y = f * v->Y;
		res->Z = f * v->Z;
	}
}

bool V3Equals(Vector3* v1, Vector3* v2, float epsilon)
{
	float dx = v1->X - v2->X;
	dx = (dx < 0 ? -dx : dx);

	float dy = v1->Y - v2->Y;
	dy = (dy < 0 ? -dy : dy);

	float dz = v1->Z - v2->Z;
	dz = (dz < 0 ? -dz : dz);

	return dx < epsilon && dy < epsilon && dz < epsilon;
}

void V3Dot(Vector3* v1, Vector3* v2, float* res)
{
	(*res) = v1->X * v2->X + v1->Y * v2->Y + v1->Z * v2->Z;
}

void V3Angle(Vector3* v1, Vector3* v2, float* res)
{
	float f=sqrt(v1->X * v1->X + v1->Y * v1->Y + v1->Z * v1->Z) * sqrt(v2->X * v2->X + v2->Y * v2->Y + v2->Z * v2->Z);

	if(f >= (float)0.0001){
		float g=(v1->X * v2->X + v1->Y * v2->Y + v1->Z * v2->Z) / f;
		if(g <= 0.9999999f)
			(*res) = acos(g);
		else
			(*res) = 0.0f;
	}else
		(*res)=0.0f;
}


// for accurate angles close to 0, returns norm of the angle.
void V3Angle0(Vector3* v1, Vector3* v2, float* res)
{
	float f=sqrt(v1->X * v1->X + v1->Y * v1->Y + v1->Z * v1->Z) * sqrt(v2->X * v2->X + v2->Y * v2->Y + v2->Z * v2->Z);

	if(f >= (float)0.0001){
		float g= fabs(   sqrt(  (v1->Y * v2->Z - v1->Z * v2->Y)*(v1->Y * v2->Z - v1->Z * v2->Y) + (v1->Z * v2->X - v1->X * v2->Z)*(v1->Z * v2->X - v1->X * v2->Z) + (v1->X * v2->Y - v1->Y * v2->X)*(v1->X * v2->Y - v1->Y * v2->X)  ) / f);
		if(g <= 0.9999999f)
			(*res) = asin(g);
		else
			(*res) = PI/2;
	}else
		(*res)=0.0f;
}

void V3Cross(Vector3* v1, Vector3* v2, Vector3* res) {
	res->X = v1->Y * v2->Z - v1->Z * v2->Y;
	res->Y = v1->Z * v2->X - v1->X * v2->Z;
	res->Z = v1->X * v2->Y - v1->Y * v2->X;
}

void V3Add(Vector3* v1, Vector3* v2, Vector3* res) {
	res->X=v1->X+v2->X;
	res->Y=v1->Y+v2->Y;
	res->Z=v1->Z+v2->Z;
}

void V3Sub(Vector3* v1, Vector3* v2, Vector3* res) {
	res->X = v1->X - v2->X;
	res->Y = v1->Y - v2->Y;
	res->Z = v1->Z - v2->Z;

}

void V3Minus(Vector3* v1, Vector3* res){
	res->X=-v1->X; res->Y=-v1->Y; res->Z=-v1->Z;
}


/*void V3WeightedSum(Vector3* vecArr, float* weights, uint8_t length, Vector3* res){
	uint8_t i = 0;
	res->X = 0; res->Y = 0; res->Z = 0;
	for(;i<length;++i){
		res->X = res->X + weights[i] * vecArr[i].X;
		res->Y = res->Y + weights[i] * vecArr[i].Y;
		res->Z = res->Z + weights[i] * vecArr[i].Z;
	}
}*/

void V3MulS(float* f, Vector3* v, Vector3* res) {
	res->X = (*f) * v->X;
	res->Y = (*f) * v->Y;
	res->Z = (*f) * v->Z;
}

void V3DegToRad(Vector3* deg, Vector3* res){
	//return PI*deg/180.0f;
	res->X = deg->X*PI/180.0f;
	res->Y = deg->Y*PI/180.0f;
	res->Z = deg->Z*PI/180.0f;
}

void V3RadToDeg(Vector3* rad, Vector3* res){
	//return rad*180.0f/PI;
	res->X = rad->X * 180 / PI;
	res->Y = rad->Y * 180 / PI;
	res->Z = rad->Z * 180 / PI;
}

/*-----------------------------------------------------------*/
//Vector3I16
/*-----------------------------------------------------------*/

void V3I16Add(Vector3I16* v1, Vector3I16* v2, Vector3I16* res) {
	res->X=v1->X+v2->X;
	res->Y=v1->Y+v2->Y;
	res->Z=v1->Z+v2->Z;
}

void V3I16Div(Vector3I16* v, uint16_t* f, Vector3I16* res) {
	res->X = v->X / (*f);
	res->Y = v->Y / (*f);
	res->Z = v->Z / (*f);
}
