#include "Quaternion.h"


//void QEulerToQuat(Vector3* headingAttitudeBank, Quaternion* res) {
//    // Assuming the angles are in radians.
//    float c1 = cos(headingAttitudeBank->X*0.5);
//    float s1 = sin(headingAttitudeBank->X*0.5);
//    float c2 = cos(headingAttitudeBank->Y*0.5);
//    float s2 = sin(headingAttitudeBank->Y*0.5);
//    float c3 = cos(headingAttitudeBank->Z*0.5);
//    float s3 = sin(headingAttitudeBank->Z*0.5);
//    float c1c2 = c1*c2;
//    float s1s2 = s1*s2;
//    res->W =c1c2*c3 - s1s2*s3;
//    res->X =c1c2*s3 + s1s2*c3;
//    res->Y =s1*c2*c3 + c1*s2*s3;
//    res->Z =c1*s2*c3 - s1*c2*s3;
//  }
//
//
void QToEuler(Quaternion* q, Vector3* euler) {
/*	float tmp;
	float sqw = q->W*q->W;
	float sqx = q->X*q->X;
	float sqy = q->Y*q->Y;
	float sqz = q->Z*q->Z;
	float unit = sqx + sqy + sqz + sqw; // if normalised is one, otherwise is correction factor
	float test = q->X*q->Y + q->Z*q->W;
	if (test > 0.499*unit) { // singularity at north pole
		euler->X = 2 * atan2(q->X,q->W);
		euler->Y = 3.1415926535898/2;
		euler->Z = 0;
		return;
	}
	if (test < -0.499*unit) { // singularity at south pole
		euler->X = -2 * atan2(q->X,q->W);
		euler->Y = -3.1415926535898/2;
		euler->Z = 0;
		return;
	}
//    euler->X = atan2(2*q->Y*q->W-2*q->X*q->Z , sqx - sqy - sqz + sqw);
//    euler->Y = asin(2*test/unit);
//    euler->Z = atan2(2*q->X*q->W-2*q->Y*q->Z , -sqx + sqy - sqz + sqw);

	euler->X = atan2(2*q->X*q->W-2*q->Y*q->Z , -sqx + sqy - sqz + sqw);
    euler->Y = atan2(2*q->Y*q->W-2*q->X*q->Z , sqx - sqy - sqz + sqw);

    if(unit >= 0.0001f){
    	tmp=2.0f*test/unit;
    	if(tmp< 0.99999f)
    		euler->Z = asin(tmp);
    	else
    		euler->Z = M_PI/2.0;
    }else
    	euler->Z = 0;*/


	float tmp;
	float sqw = q->W*q->W;
	float sqx = q->X*q->X;
	float sqy = q->Y*q->Y;
	float sqz = q->Z*q->Z;
	float unit = sqx + sqy + sqz + sqw; // if normalised is one, otherwise is correction factor
	float test = q->W*q->Y - q->X*q->Z;
	if (test > 0.499*unit) { // singularity at north pole
		euler->X = 2*atan2(q->X,q->W);
		euler->Y = 3.1415926535898/2;
		euler->Z = 0;
		return;
	}
	if (test < -0.499*unit) { // singularity at south pole
		euler->X = 2*atan2(q->X,q->W);
		euler->Y = -3.1415926535898/2;
		euler->Z = 0;
		return;
	}
	//euler->X = atan2(2*(q->W*q->X - q->Y*q->Z) , -sqx - sqy + sqz + sqw);
    //euler->Z = atan2(2*(q->W*q->Z - q->X*q->Y) , sqx - sqy - sqz + sqw);

	euler->X = atan2(2*(q->W*q->X + q->Y*q->Z) , -sqx - sqy + sqz + sqw);
    euler->Z = atan2(2*(q->W*q->Z + q->X*q->Y) , sqx - sqy - sqz + sqw);

    if(unit >= 0.0001f){
    	tmp=2.0f*test/unit;
    	if(tmp< 0.99999f)
    		euler->Y = asin(tmp);
    	else
    		euler->Y = M_PI/2.0;
    }else
    	euler->Y = 0;
}







//Rotation order from Craig Page 46
//Rot around X with angle gamma, rot around Y with angle beta, rot around Z with angle alpha
void RotMatToXYZAngle(Matrix3x3* mat, Vector3* res)
{
	res->Y = atan2(-mat->e31, sqrt(mat->e11*mat->e11 + mat->e21*mat->e21));
	float cb = cos(res->Y);
	if(cb >= 0.0001f || cb <= -0.0001f){
		res->Z = atan2(mat->e21/cb, mat->e11/cb);
		res->X = atan2(mat->e32/cb, mat->e33/cb);
	}else{
		res->Z =0;
		res->X =0;
	}
}

void QToRotMat(Quaternion* q, Matrix3x3* mat)
{
	mat->e11 = 1-2*q->Y*q->Y-2*q->Z*q->Z; mat->e12 = 2*(q->X*q->Y - q->Z*q->W); mat->e13 = 2*(q->X*q->Z+q->Y*q->W);
	mat->e21 = 2*(q->X*q->Y + q->Z*q->W); mat->e22 = 1-2*q->X*q->X-2*q->Z*q->Z; mat->e23 = 2*(q->Y*q->Z-q->X*q->W);
	mat->e31 = 2*(q->X*q->Z-q->Y*q->W); mat->e32 = 2*(q->Y*q->Z+q->X*q->W); mat->e33 = 1-2*q->X*q->X-2*q->Y*q->Y;

//	rm_3_3 = [1-2*q->y^2-2*q->z^2, 2*(q->x*q->y - q->z*q->w), 2*(q->x*q->z+q->y*q->w);
//	        2*(q->x*q->y + q->z*q->w), 1-2*q->x^2-2*q->z^2,  2*(q->y*q->z-q->x*q->w);
//	        2*(q->x*q->z-q->y*q->w), 2*(q->y*q->z+q->x*q->w), 1-2*q->x^2-2*q->y^2];
}


//From Gajan
//function q = qm_scalar( q1, s )
//    if norm(q1(1:3)) < 10^-4
//        q = q1;
//    else
//        theta = 2*asin(norm(q1(1:3)));
//        %theta = 2*acos(q1(4));
//        q = [sin(s*theta/2.0)*q1(1:3)/norm(q1(1:3));cos(s*theta/2.0)];
//    end
//end
void QMulS(float* f, Quaternion* q, Quaternion* res)
{
	float norm, thetaHalf, s, c;
	float normSq = q->X*q->X + q->Y*q->Y + q->Z*q->Z;
	if (normSq < 1.0e-6f)
	{
		res->X = q->X; res->Y = q->Y; res->Z = q->Z; res->W = q->W;
	}
	else
	{
		norm = sqrt(q->X*q->X + q->Y*q->Y + q->Z*q->Z);
		if(norm < 0.99999f)
			thetaHalf = asin(norm);
		else
			thetaHalf = M_PI/2.0f;
		s = sin((*f)*thetaHalf);
		c = cos((*f)*thetaHalf);
		res->X = s*q->X / norm; res->Y = s*q->Y / norm; res->Z = s*q->Z / norm;
		res->W = c*q->W / norm;
	}
}

//Second order approximation for Quaternion interpolation
//Source: http://oplnk.net/~genebrownd/Averaging%20Quaternions.pdf page 3
void QInterpolate(Quaternion* q1, Quaternion* q2, float* a, float* b, Quaternion* res)
{
	float chi = (*a)*(1-(*a)*(*a))/6;
	float delta = (*b)*(1-(*b)*(*b))/6;
	float dotp = q1->X*q2->X + q1->Y*q2->Y + q1->Z*q2->Z + q1->W*q2->W;
	float gamma = 1 - dotp * dotp;
	float alpha = (*a) + chi * gamma;
	float beta = (*b) + delta * gamma;

	res->X = alpha * q1->X + beta * q2->X;
	res->Y = alpha * q1->Y + beta * q2->Y;
	res->Z = alpha * q1->Z + beta * q2->Z;
	res->W = alpha * q1->W + beta * q2->W;
}

// Not a numer (NAN) issues
//void QInterpolate(Quaternion* q1, Quaternion* q2, float* a, float* b, Quaternion* res)
//{
//	float halfAngle1 = 0;
//	if(q1->W != 0)
//		halfAngle1 = acos(q1->W);
//	float halfAngle2 = 0;
//	if(q2->W != 0)
//		halfAngle2 = acos(q2->W);
//
//	float s1 = sin(halfAngle1);
//	float s2 = sin(halfAngle2);
//
//	float halfAngle = (*a) * halfAngle1 + (*b) * halfAngle2;
//	res->W = cos(halfAngle);
//	float s = sin(halfAngle);
//	res->X = s*((*a) * q1->X / s1 + (*b) * q2->X / s2);
//	res->Y = s*((*a) * q1->Y / s1 + (*b) * q2->Y / s2);
//	res->Z = s*((*a) * q1->Z / s1 + (*b) * q2->Z / s2);
//}


void QNormalize(Quaternion* q, Quaternion* res) {
	// Don't normalize if we don't have to
	float mag2 = q->W * q->W + q->X * q->X + q->Y * q->Y + q->Z * q->Z;
	//if (fabs(mag2) > TOLERANCE && fabs(mag2 - 1->0f) > TOLERANCE)
	if (mag2 >= 0.0001f) {
		float f = 1.0f / sqrt(mag2);
		res->X = q->X * f; res->Y = q->Y * f; res->Z = q->Z * f; res->W = q->W * f;
	}
	else
	{
		res->X = q->X; res->Y = q->Y; res->Z = q->Z; res->W = q->W;
	}
}


void QTrafo(Quaternion* q, Vector3* p1, Vector3* res)
{
	res->X = q->W*q->W*p1->X + 2*q->Y*q->W*p1->Z - 2*q->Z*q->W*p1->Y + q->X*q->X*p1->X + 2*q->Y*q->X*p1->Y + 2*q->Z*q->X*p1->Z - q->Z*q->Z*p1->X - q->Y*q->Y*p1->X;
	res->Y = 2*q->X*q->Y*p1->X + q->Y*q->Y*p1->Y + 2*q->Z*q->Y*p1->Z + 2*q->W*q->Z*p1->X - q->Z*q->Z*p1->Y + q->W*q->W*p1->Y - 2*q->X*q->W*p1->Z - q->X*q->X*p1->Y;
	res->Z = 2*q->X*q->Z*p1->X + 2*q->Y*q->Z*p1->Y + q->Z*q->Z*p1->Z - 2*q->W*q->Y*p1->X - q->Y*q->Y*p1->Z + 2*q->W*q->X*p1->Y - q->X*q->X*p1->Z + q->W*q->W*p1->Z;
}




void QConjugate(Quaternion* q, Quaternion* res) {
	res->X = -q->X; res->Y = -q->Y; res->Z = -q->Z; res->W = q->W;
}

// Multiplying q1 with q2 applies the rotation q1 to q2
void QMul(Quaternion* q1, Quaternion* q2, Quaternion* res) {
	res->X = q2->W * q1->X + q2->X * q1->W + q2->Y * q1->Z - q2->Z * q1->Y;
	res->Y = q2->W * q1->Y + q2->Y * q1->W + q2->Z * q1->X - q2->X * q1->Z;
	res->Z = q2->W * q1->Z + q2->Z * q1->W + q2->X * q1->Y - q2->Y * q1->X;
	res->W = q2->W * q1->W - q2->X * q1->X - q2->Y * q1->Y - q2->Z * q1->Z;
}

// Multiplying a quaternion q with a vector v applies the q-rotation to v
void QMulV(Quaternion* q, Vector3* v, Vector3* res) {
	//v = QNormalize(v);

	//Quaternion vecQuat = new Quaternion(vn.X, vn.Y, vn.Z, 0);
	Quaternion vq = { v->X, v->Y, v->Z, 0 };

	// lhs (rhs corresponds to q)
	Quaternion q2;	QConjugate(q, &q2);

	Quaternion rq; QMul(q, &vq, &rq);
	Quaternion a; QMul(&rq, &q2, &a);

	res->X = a.X; res->Y = a.Y; res->Z = a.Z;
}

// Convert from Axis Angle
void QAxisAngle(Vector3* v, float* angle, Quaternion* res) {
	float a = (*angle) * 0.5f;
	Vector3 vn; V3Normalize(v, &vn);

	float s = sin(a);

	res->X = v->X * s; res->Y = v->Y * s; res->Z = v->Z * s; res->W = cos(a);
}

// Convert from Euler Angles
void QFromEuler(float pitch, float yaw, float roll, Quaternion* res) {
	// Basically we create 3 Quaternions, one for pitch, one for yaw, one for roll
	// and multiply those together.
	// the calculation below does the same, just shorter

	//float p = pitch * PIOVER180 / 2.0f;
	//float y = yaw * PIOVER180 / 2.0f;
	//float r = roll * PIOVER180 / 2.0f;

	//float sinp = (float)Math.Sin(p);
	//float siny = (float)Math.Sin(y);
	//float sinr = (float)Math.Sin(r);
	//float cosp = (float)Math.Cos(p);
	//float cosy = (float)Math.Cos(y);
	//float cosr = (float)Math.Cos(r);

	float sinp = sin(pitch);
	float siny = sin(yaw);
	float sinr = sin(roll);
	float cosp = cos(pitch);
	float cosy = cos(yaw);
	float cosr = cos(roll);

	Quaternion q;
	q.X = sinr * cosp * cosy - cosr * sinp * siny;
	q.Y = cosr * sinp * cosy + sinr * cosp * siny;
	q.Z = cosr * cosp * siny - sinr * sinp * cosy;
	q.W = cosr * cosp * cosy + sinr * sinp * siny;

	QNormalize(&q, res);
}

// Returns pitch, yaw, roll in vector
void QToEulerAngles(Quaternion* q, Vector3* angles)
{
	angles->X = atan2(2 * (q->W * q->X * q->Y * q->Z), 1 - 2 * (q->X * q->X + q->Y * q->Y));
	float tmp = 2 * (q->W * q->Y - q->Z * q->X);
	if(tmp < 0.999999f)
		angles->Y = asin(tmp);
	else
		angles->Y = M_PI/2.0f;
	angles->Z = atan2(2 * (q->W * q->Z + q->X * q->Y), 1 - 2 * (q->Y * q->Y + q->Z * q->Z));
}

// Convert to Matrix
void QMatrix(Quaternion* q, Matrix3x3* res) {
	float x2 = q->X * q->X;
	float y2 = q->Y * q->Y;
	float z2 = q->Z * q->Z;
	float xy = q->X * q->Y;
	float xz = q->X * q->Z;
	float yz = q->Y * q->Z;
	float wx = q->W * q->X;
	float wy = q->W * q->Y;
	float wz = q->W * q->Z;

	// This calculation would be a lot more complicated for non-unit length quaternions
	// Note: The constructor of Matrix4 expects the Matrix in column-major format like expected by
	//   OpenGL
	//return new Matrix4F(1.0f - 2.0f * (y2 + z2), 2.0f * (xy - wz), 2.0f * (xz + wy), 0.0f,
	//        2.0f * (xy + wz), 1.0f - 2.0f * (x2 + z2), 2.0f * (yz - wx), 0.0f,
	//        2.0f * (xz - wy), 2.0f * (yz + wx), 1.0f - 2.0f * (x2 + y2), 0.0f,
	//        0.0f, 0.0f, 0.0f, 1.0f);
	res->e11 = 1 - 2 * (y2 + z2);
	res->e12 = 2 * (xy + wz);
	res->e13 = 2 * (xz - wy);
	res->e21 = 2 * (xy	- wz);
	res->e22 = 1 - 2 * (x2 + z2);
	res->e23 = 2 * (yz + wx);
	res->e31 = 2 * (xz + wy);
	res->e32 = 2 * (yz	- wx);
	res->e33 = 1 - 2 * (x2 + y2);
}

// Convert to Axis/Angles
float QGetAxisAngle(Quaternion* q, Vector3* outAxis) {
	float scale=sqrt(q->X * q->X + q->Y * q->Y + q->Z * q->Z);
	float angle=0;
	if(scale >= 0.0001f){
		scale = 1.0 / sqrt(q->X * q->X + q->Y * q->Y + q->Z * q->Z);
		if(q->W < 0.99999f)
			angle = acos(q->W) * 2.0;
		else
			angle = 0;
		outAxis->X = q->X * scale;
		outAxis->Y = q->Y * scale;
		outAxis->Z = q->Z * scale;
	}
	return angle;
}
