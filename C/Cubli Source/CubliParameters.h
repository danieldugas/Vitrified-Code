#ifndef CUBLIPARAMETERS_H
#define CUBLIPARAMETERS_H


// Define Cubli Parameters here

#include "com_protocol.h"
//extern enum communicationIDs;
//typedef enum communicationIDs com_id; // in com_protocol.h

#include "Servo.h"
#include "Matrix3x3F.h"
#include "Peripherals.h"

typedef struct CubliParameters {
	//servo
	ServoTwoStates _servo[3];
	ServoTwoStates _servoF2C[3];

	//IMUs
	Matrix3x3 _gyrCorr[6];
	Matrix3x3 _accCorr[6];
	Matrix3x3 _scaleCorr[6];
	Vector3 _accBias[6];
	float _weights[6];

	//physical
	float edgeEqAngle; // equilibrium angle in 1D
	Vector3 m_K;		// vector to the centre of mass * mass of cubli (in K-frame)
	float m_K_length;	// length of m vector
	float mass;		// mass
	float thetaH[6];	// theta_ht_0=theta_0-theta_w, total inertia referred to pivot point - wheel inertia
	float thetaW[3];	// wheel inertia
	float K_M[3];		// motor constant

	//jump
	float omegaF2E;

	float omegaE2C[2];
	int8_t brakeDelayE2C;

	float omegaF2C[3];
	uint8_t brakeDelay[2];	// break delays --> always positive
	uint8_t brakeQueue[3];	// break order


	float dV_invE2C[4];		// Inverse of gradient. E2C jump and F2C jump.
	Matrix3x3 dV_invF2C;

	float lambdaE2C;		// learning parameters
	float lambdaF2C;
	float lambdaF2E;
    
    com_id this_cube_id;      // ID for communications purposes
} CubliParameters;


static const CubliParameters OldCubli={
			._servo[0].Servo = (struct Servo) {TIM3, Channel1},
			._servo[0].OpenValue = 950,
			._servo[0].ClosedValue = 450,

			._servo[1].Servo = (struct Servo) {TIM3, Channel2},
			._servo[1].OpenValue = 900,
			._servo[1].ClosedValue = 390,

			._servo[2].Servo = (struct Servo) {TIM3, Channel3},
			._servo[2].OpenValue = 950,
			._servo[2].ClosedValue = 440,

			._gyrCorr[0] = (struct Matrix3x3) {-0.999696, -0.004083, -0.024307, -0.024323, 0.003898, 0.999697, -0.003987, 0.999984, -0.003996},
			._gyrCorr[1] = (struct Matrix3x3) {0.005857, -0.999979, 0.002733, 0.999982, 0.005861, 0.001485, -0.001501, 0.002724, 0.999995},
			._gyrCorr[2] = (struct Matrix3x3) {0.011851, -0.999930, -0.000410, -0.999920, -0.011849, -0.004369, 0.004363, 0.000462, -0.999990},
			._gyrCorr[3] = (struct Matrix3x3) {0.011536, -0.999932, 0.001837, -0.020297, -0.002071, -0.999792, 0.999727, 0.011497, -0.020319},
			._gyrCorr[4] = (struct Matrix3x3) {0.007830, 0.999964, 0.003138, 0.005240, 0.003097, -0.999981, -0.999956, 0.007846, -0.005215},
			._gyrCorr[5] = (struct Matrix3x3) {-0.007414, 0.999967, 0.003417, -0.999972, -0.007418, 0.001205, 0.001231, -0.003408, 0.999993},

			._accCorr[0] = (struct Matrix3x3) {0.999696, 0.004083, 0.024307, 0.024323, -0.003898, -0.999697, 0.003987, -0.999984, 0.003996},
			._accCorr[1] = (struct Matrix3x3) {-0.005857, 0.999979, -0.002733, -0.999982, -0.005861, -0.001485, 0.001501, -0.002724, -0.999995},
			._accCorr[2] = (struct Matrix3x3) {-0.011851, 0.999930, 0.000410, 0.999920, 0.011849, 0.004369, -0.004363, -0.000462, 0.999990},
			._accCorr[3] = (struct Matrix3x3) {-0.011536, 0.999932, -0.001837, 0.020297, 0.002071, 0.999792, -0.999727, -0.011497, 0.020319},
			._accCorr[4] = (struct Matrix3x3) {-0.007830, -0.999964, -0.003138, -0.005240, -0.003097, 0.999981, 0.999956, -0.007846, 0.005215},
			._accCorr[5] = (struct Matrix3x3) {0.007414, -0.999967, -0.003417, 0.999972, 0.007418, -0.001205, -0.001231, 0.003408, -0.999993},

			._scaleCorr[0] = (struct Matrix3x3) {0.989435, 0.000000, 0.000000, 0.000000, 0.998341, 0.000000, 0.000000, 0.000000, 0.985026},
			._scaleCorr[1] = (struct Matrix3x3) {0.995392, 0.000000, 0.000000, 0.000000, 0.995661, 0.000000, 0.000000, 0.000000, 0.979323},
			._scaleCorr[2] = (struct Matrix3x3) {0.992250, 0.000000, 0.000000, 0.000000, 1.001607, 0.000000, 0.000000, 0.000000, 0.989508},
			._scaleCorr[3] = (struct Matrix3x3) {0.999010, 0.000000, 0.000000, 0.000000, 0.998625, 0.000000, 0.000000, 0.000000, 0.987658},
			._scaleCorr[4] = (struct Matrix3x3) {0.997475, 0.000000, 0.000000, 0.000000, 0.999976, 0.000000, 0.000000, 0.000000, 0.995437},
			._scaleCorr[5] = (struct Matrix3x3) {0.990967, 0.000000, 0.000000, 0.000000, 0.996548, 0.000000, 0.000000, 0.000000, 0.987927},

			._accBias[0] = (struct Vector3) {-0.192861, 0.028640, 0.287348},
			._accBias[1] = (struct Vector3) {-0.345477, 0.010665, -0.533294},
			._accBias[2] = (struct Vector3) {-0.218668, 0.012576, 0.497350},
			._accBias[3] = (struct Vector3) {-0.174534, 0.048156, -0.565967},
			._accBias[4] = (struct Vector3) {-0.224028, -0.037380, 0.681696},
			._accBias[5] = (struct Vector3) {-0.373211, -0.072138, -0.203117},


			._weights[0] = 0.2351,
			._weights[1] = 0.6453,
			._weights[2] = -0.0436,
			._weights[3] = 0.3622,
			._weights[4] = -0.4074,
			._weights[5] = 0.2084,


			.edgeEqAngle = 0.7687,

			.m_K  =  (struct Vector3) {0.1125,0.1139,0.1073},

			.mass = 1.544,
			.m_K_length = 0.19,

			.thetaH[0] = 0.0217,
			.thetaH[1] = -0.0083,
			.thetaH[2] = -0.0077,
			.thetaH[3] = 0.0214,
			.thetaH[4] = -0.0079,
			.thetaH[5] = 0.0224,

			.thetaW[0]= 5.36e-04,
			.thetaW[1]= 5.36e-04,
			.thetaW[2]= 5.36e-04,



			.omegaF2E = -605,

			.omegaE2C[0] = 404,
			.omegaE2C[1] = -386,
			//.breakDelayE2C = {-13,0},

			.brakeDelay={13,0},
			.brakeQueue={0,2,1},

			.omegaF2C[0] = -636,
			.omegaF2C[1] = 644,
			.omegaF2C[2] =0,
			//.breakDelayF2C = {-4,0},

			.dV_invE2C={1,1,1,1},

			.lambdaE2C=1.5,
			.lambdaF2C=1,
			.lambdaF2E=1.5,

            .this_cube_id = COM_ID_OLDCUBLI
};

static const CubliParameters NewCubli={
		._servo[0].Servo = (struct Servo) {TIM3, Channel1},
		._servo[0].OpenValue = 950,
		._servo[0].ClosedValue = 400,

		._servo[1].Servo = (struct Servo) {TIM3, Channel2},
		._servo[1].OpenValue = 950,
		._servo[1].ClosedValue = 400,

		._servo[2].Servo = (struct Servo) {TIM3, Channel3},
		._servo[2].OpenValue = 950,
		._servo[2].ClosedValue = 400,

		._servoF2C[0].Servo = (struct Servo) {TIM3, Channel1},
		._servoF2C[0].OpenValue = 950,
		._servoF2C[0].ClosedValue = 400,//610,

		._servoF2C[1].Servo = (struct Servo) {TIM3, Channel2},
		._servoF2C[1].OpenValue = 950,
		._servoF2C[1].ClosedValue = 400,

		._servoF2C[2].Servo = (struct Servo) {TIM3, Channel3},
		._servoF2C[2].OpenValue = 950,
		._servoF2C[2].ClosedValue = 400,//650, //680 worked


		._weights[0] = 0.0784,
		._weights[1] = 0.0973,
		._weights[2] = 0.6606,
		._weights[3] = 0.2475,
		._weights[4] = -0.3883,
		._weights[5] = 0.3045,

		._gyrCorr[0] = (struct Matrix3x3) {-0.022772, -0.999733, 0.003885, -0.999741, 0.022770, -0.000441, 0.000353, -0.003894, -0.999992},
		._gyrCorr[1] = (struct Matrix3x3) {-0.001758, -0.005294, -0.999984, 0.013110, 0.999900, -0.005316, 0.999913, -0.013119, -0.001688},
		._gyrCorr[2] = (struct Matrix3x3) {0.001517, -0.999994, -0.002985, 0.999991, 0.001506, 0.003878, -0.003873, -0.002991, 0.999988},
		._gyrCorr[3] = (struct Matrix3x3) {-0.006450, -0.013028, 0.999894, -0.004772, -0.999903, -0.013059, 0.999968, -0.004855, 0.006387},
		._gyrCorr[4] = (struct Matrix3x3) {-0.007800, -0.999937, -0.008086, -0.999949, 0.007851, -0.006314, 0.006377, 0.008036, -0.999947},
		._gyrCorr[5] = (struct Matrix3x3) {0.999990, 0.001354, -0.004153, 0.004153, -0.000636, 0.999991, 0.001351, -0.999999, -0.000642},

		._accCorr[0] = (struct Matrix3x3) {0.022772, 0.999733, -0.003885, 0.999741, -0.022770, 0.000441, -0.000353, 0.003894, 0.999992},
		._accCorr[1] = (struct Matrix3x3) {0.001758, 0.005294, 0.999984, -0.013110, -0.999900, 0.005316, -0.999913, 0.013119, 0.001688},
		._accCorr[2] = (struct Matrix3x3) {-0.001517, 0.999994, 0.002985, -0.999991, -0.001506, -0.003878, 0.003873, 0.002991, -0.999988},
		._accCorr[3] = (struct Matrix3x3) {0.006450, 0.013028, -0.999894, 0.004772, 0.999903, 0.013059, -0.999968, 0.004855, -0.006387},
		._accCorr[4] = (struct Matrix3x3) {0.007800, 0.999937, 0.008086, 0.999949, -0.007851, 0.006314, -0.006377, -0.008036, 0.999947},
		._accCorr[5] = (struct Matrix3x3) {-0.999990, -0.001354, 0.004153, -0.004153, 0.000636, -0.999991, -0.001351, 0.999999, 0.000642},

		._scaleCorr[0] = (struct Matrix3x3) {0.999284, 0.000000, 0.000000, 0.000000, 0.998959, 0.000000, 0.000000, 0.000000, 0.992720},
		._scaleCorr[1] = (struct Matrix3x3) {0.999827, 0.000000, 0.000000, 0.000000, 0.991605, 0.000000, 0.000000, 0.000000, 0.991855},
		._scaleCorr[2] = (struct Matrix3x3) {0.999932, 0.000000, 0.000000, 0.000000, 0.998356, 0.000000, 0.000000, 0.000000, 0.994017},
		._scaleCorr[3] = (struct Matrix3x3) {0.999262, 0.000000, 0.000000, 0.000000, 0.990666, 0.000000, 0.000000, 0.000000, 0.992211},
		._scaleCorr[4] = (struct Matrix3x3) {0.994728, 0.000000, 0.000000, 0.000000, 0.996111, 0.000000, 0.000000, 0.000000, 0.990321},
		._scaleCorr[5] = (struct Matrix3x3) {0.999194, 0.000000, 0.000000, 0.000000, 0.999344, 0.000000, 0.000000, 0.000000, 0.995919},

		._accBias[0] = (struct Vector3) {-0.021265, 0.042841, 0.437935},
		._accBias[1] = (struct Vector3) {0.081849, 0.019679, 0.422015},
		._accBias[2] = (struct Vector3) {0.023893, 0.043421, 0.384597},
		._accBias[3] = (struct Vector3) {0.074417, 0.016545, 0.243389},
		._accBias[4] = (struct Vector3) {0.038325, 0.056835, 0.178190},
		._accBias[5] = (struct Vector3) {-0.098681, -0.020079, -0.951191},


		//__________________________________________________________________________
		//
		//Physical Parameters
		//__________________________________________________________________________

		.edgeEqAngle = 0.793648775067751,


		.m_K  =  (struct Vector3) {0.116291814,        0.109998725,     0.119902030},


		.mass = 1.474,
		.m_K_length = 0.2,//.1873,

		.thetaH[0] = 0.0217,
		.thetaH[1] = -0.0083,
		.thetaH[2] = -0.0077,
		.thetaH[3] = 0.0214,
		.thetaH[4] = -0.0079,
		.thetaH[5] = 0.0224,

		.thetaW[0]= 5.4e-04,
		.thetaW[1]= 5.4e-04,
		.thetaW[2]= 5.4e-04,


		//__________________________________________________________________________
		//
		//Jump Parameters
		//__________________________________________________________________________
		.omegaF2E = -522,

		.omegaE2C[1] = -230,//-311
		.omegaE2C[0] = 280,//297
		//.breakDelayE2C = {23,0},

		.brakeDelay={0,0},//{12,1},
		.brakeQueue={0,1,2},

		.brakeDelayE2C=-5,	// if brakeDelayE2C>0 --> brake x-wheel first then z-wheel
							// if brakeDelayE2C<0 --> brake z-wheel first then x-wheel

		.omegaF2C[0] = 493, //488
		.omegaF2C[1] = -430, //-533
		.omegaF2C[2] = 56,//-12

		.dV_invE2C={-37.197927, 9.011810, -46.902192, -8.897986}, //{-38.428147, 9.173714, -45.717606, -9.135335},
		.dV_invF2C={4.794455, 6.376430, 1.183590, 5.000030, 11.079341, -1.174973, -10.231896, 0.101663, 0.002067},


		.lambdaE2C=1.5,//1.5,	adjusted parameter
		.lambdaF2C=1,
		.lambdaF2E=1.5,

        .this_cube_id = COM_ID_NEWCUBLI
};


static const CubliParameters Cubli2={
		._servo[0].Servo = (struct Servo) {TIM3, Channel1},
		._servo[0].OpenValue = 950,
		._servo[0].ClosedValue = 500,//630,//610,

		._servo[1].Servo = (struct Servo) {TIM3, Channel2},
		._servo[1].OpenValue = 950,
		._servo[1].ClosedValue = 500,//750

		._servo[2].Servo = (struct Servo) {TIM3, Channel3},
		._servo[2].OpenValue = 950,
		._servo[2].ClosedValue = 500,//670,//650, //680 worked

		._servoF2C[0].Servo = (struct Servo) {TIM3, Channel1},
		._servoF2C[0].OpenValue = 950,
		._servoF2C[0].ClosedValue = 500,//550,//640,//610,

		._servoF2C[1].Servo = (struct Servo) {TIM3, Channel2},
		._servoF2C[1].OpenValue = 950,
		._servoF2C[1].ClosedValue = 500,//750,

		._servoF2C[2].Servo = (struct Servo) {TIM3, Channel3},
		._servoF2C[2].OpenValue = 950,
		._servoF2C[2].ClosedValue = 500,//600,//670,//650, //680 worked

		._weights[0] = 0.0784,
		._weights[1] = 0.0973,
		._weights[2] = 0.6606,
		._weights[3] = 0.2475,
		._weights[4] = -0.3883,
		._weights[5] = 0.3045,

		._gyrCorr[0] = (struct Matrix3x3) {-0.006321, 0.999819, -0.017939, 0.999974, 0.006382, 0.003388, 0.003501, -0.017917, -0.999833},
		._gyrCorr[1] = (struct Matrix3x3) {-0.000742, -0.013988, -0.999902, -0.002285, 0.999900, -0.013986, 0.999997, 0.002274, -0.000774},
		._gyrCorr[2] = (struct Matrix3x3) {-0.039313, 0.999146, -0.012696, -0.999219, -0.039258, 0.004561, 0.004058, 0.012866, 0.999909},
		._gyrCorr[3] = (struct Matrix3x3) {-0.006022, -0.023626, 0.999703, 0.012421, -0.999646, -0.023549, 0.999905, 0.012276, 0.006314},
		._gyrCorr[4] = (struct Matrix3x3) {0.022985, 0.999735, -0.001358, 0.999736, -0.022986, -0.000367, -0.000398, -0.001349, -0.999999},
		._gyrCorr[5] = (struct Matrix3x3) {0.999823, 0.003383, -0.018499, 0.018465, 0.009844, 0.999781, 0.003564, -0.999946, 0.009780},

		._accCorr[0] = (struct Matrix3x3) {0.006321, -0.999819, 0.017939, -0.999974, -0.006382, -0.003388, -0.003501, 0.017917, 0.999833},
		._accCorr[1] = (struct Matrix3x3) {0.000742, 0.013988, 0.999902, 0.002285, -0.999900, 0.013986, -0.999997, -0.002274, 0.000774},
		._accCorr[2] = (struct Matrix3x3) {0.039313, -0.999146, 0.012696, 0.999219, 0.039258, -0.004561, -0.004058, -0.012866, -0.999909},
		._accCorr[3] = (struct Matrix3x3) {0.006022, 0.023626, -0.999703, -0.012421, 0.999646, 0.023549, -0.999905, -0.012276, -0.006314},
		._accCorr[4] = (struct Matrix3x3) {-0.022985, -0.999735, 0.001358, -0.999736, 0.022986, 0.000367, 0.000398, 0.001349, 0.999999},
		._accCorr[5] = (struct Matrix3x3) {-0.999823, -0.003383, 0.018499, -0.018465, -0.009844, -0.999781, -0.003564, 0.999946, -0.009780},

		._scaleCorr[0] = (struct Matrix3x3) {0.998907, 0.000000, 0.000000, 0.000000, 1.003879, 0.000000, 0.000000, 0.000000, 1.006304},
		._scaleCorr[1] = (struct Matrix3x3) {1.002994, 0.000000, 0.000000, 0.000000, 0.995372, 0.000000, 0.000000, 0.000000, 0.993079},
		._scaleCorr[2] = (struct Matrix3x3) {1.004747, 0.000000, 0.000000, 0.000000, 1.001041, 0.000000, 0.000000, 0.000000, 0.996825},
		._scaleCorr[3] = (struct Matrix3x3) {0.999334, 0.000000, 0.000000, 0.000000, 1.007123, 0.000000, 0.000000, 0.000000, 0.987998},
		._scaleCorr[4] = (struct Matrix3x3) {1.007579, 0.000000, 0.000000, 0.000000, 0.995898, 0.000000, 0.000000, 0.000000, 1.000644},
		._scaleCorr[5] = (struct Matrix3x3) {0.996154, 0.000000, 0.000000, 0.000000, 1.005479, 0.000000, 0.000000, 0.000000, 1.002266},

		._accBias[0] = (struct Vector3) {0.058315, 0.064172, -0.039230},
		._accBias[1] = (struct Vector3) {0.093518, 0.071329, 0.036469},
		._accBias[2] = (struct Vector3) {0.114273, 0.032261, 0.089650},
		._accBias[3] = (struct Vector3) {0.070251, 0.081130, -1.000000},
		._accBias[4] = (struct Vector3) {-0.008397, 0.072460, 0.071937},
		._accBias[5] = (struct Vector3) {0.054563, -0.003643, 0.071416},


		//__________________________________________________________________________
		//
		//Physical Parameters
		//__________________________________________________________________________

		.edgeEqAngle = 0.7911,


		.m_K  =  (struct Vector3) { 0.120231323, 0.106255047, 0.119391359},



		.mass = 1.474,
		.m_K_length = .2,

		.thetaH[0] = 0.0217,
		.thetaH[1] = -0.0083,
		.thetaH[2] = -0.0077,
		.thetaH[3] = 0.0214,
		.thetaH[4] = -0.0079,
		.thetaH[5] = 0.0224,

		.thetaW[0]= 5.4e-04,
		.thetaW[1]= 5.4e-04,
		.thetaW[2]= 5.4e-04,


		//__________________________________________________________________________
		//
		//Jump Parameters
		//__________________________________________________________________________
		.omegaF2E = -456,//-505,

		.omegaE2C[0] = 254,//248,
		.omegaE2C[1] = -245,//-267,

		.brakeDelayE2C= 0,	// if brakeDelayE2C>0 --> brake x-wheel first then z-wheel
							// if brakeDelayE2C<0 --> brake z-wheel first then x-wheel

		.brakeQueue={1,0,2},
		.brakeDelay={7,3},

		.omegaF2C[0] =  465,
		.omegaF2C[1] = -312,
		.omegaF2C[2] =  54,

		.dV_invE2C={-38.358347, 8.956661, -45.746695, -8.725655},

		.dV_invF2C={4.794455, 6.376430, 1.183590, 5.000030, 11.079341, -1.174973, -10.231896, 0.101663, 0.002067},


		.lambdaE2C=1.2,
		.lambdaF2C=1,
		.lambdaF2E=1.5,

        .this_cube_id = COM_ID_CUBLI2

};



CubliParameters _cubliParameters;

// Define which cubli is used here
void SetCubliParameters();

#endif
