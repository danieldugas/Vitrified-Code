#include "Controllers.h"

//variables declared in main
extern xTaskHandle E2CJumpUpTaskHandle;
extern xTaskHandle F2CJumpUpTaskHandle;
extern xTaskHandle F2EJumpUpTaskHandle;
extern xTaskHandle LQR1DTaskHandle;
extern xTaskHandle MPUTaskHandle;
extern xTaskHandle AttitudeTaskHandle;
extern int __errno;

#define CONTROLLER_SILENT
#define CHOREOGRAPHY_SILENT

// Global Variables
uint8_t _balancingMode = NotBalancing;
uint8_t _jumpMode = NoJump;

cube_mode _modeNumber = Idle;
bool _balance = false;

uint8_t _spinsCompleted = 0;


//variable for indexing lookup table
unsigned int _lut_counter=0;

//Variables for F2E Jump
float _Htop_1D;

//Variables for 3D-Jump
float _pwhg_top;
float _pwhm_top;
float _Htop_3D;
float _Hhtop_3D;
float _angle_top;

//Variables for F2E Jump Learning <-> LQR Task
float _minEdgeAngle;
float _angVel;
float _meanF2ECurrent;

//Variables for E2C Jump Learning <-> Corner Balancing Task
float _minCornerAngle=99;
float _deltaE;
float _deltaD;
float _torqueE;
float _torqueD;

//Variables for speeding up reaction wheel
bool _speedup[3]={false,false,false};
bool _speedupLQR;
bool _reached_velocity[3]={false,false,false};
float _ww_des[3]={0,0,0};
bool _ready2Adjust=false;

float _jumpSuccess3D=false;
float _jumpSuccess1D=false;

float _JumpDirection[2]={0,0};

extern bool _writeData;

//Variables for Choreography status
bool _moveSuccess = false;

// Functions used by controllers
cube_mode cycleButtonMode( cube_mode buttonModeNumber)
{
	switch ( buttonModeNumber )
	{
	case Idle		 : return ButtonMode1;
	case ButtonMode1 : return ButtonMode2;
	case ButtonMode2 : return ButtonMode3;
	case ButtonMode3 : return Idle       ;
	default 		 : return buttonModeNumber;
	}
}

int isAButtonMode( cube_mode X )
{
	// Warning: Idle is considered a button mode
	// only button modes get cycled
	if ( cycleButtonMode( X ) != X ) return 1;

	return 0;
}

// Controllers

void DetermineRotAxisParam(uint8_t* axisIndex, uint8_t* axisNumber, float* eqAngle)
{
	float g1,g2;
	float angle;
	float v1,v2;

	//Determine axisIndex ( x/y/z )
	if( fabs(_gEst.X)<fabs(_gEst.Y) && fabs(_gEst.X)<fabs(_gEst.Z) ){
		(*axisIndex) = 0;
        #ifndef CONTROLLER_SILENT
			printf("\n\rAxis: x");
		#endif
		g1 = _gEst.Y;
		g2 = _gEst.Z;
	}
	else if ( fabs(_gEst.Y)<fabs(_gEst.X) && fabs(_gEst.Y)<fabs(_gEst.Z) ){
		(*axisIndex) = 1;
#ifndef CONTROLLER_SILENT
		printf("\n\rAxis: y");
#endif
		g1 = _gEst.Z;
		g2 = _gEst.X;
	}
	else{
		(*axisIndex) = 2;
#ifndef CONTROLLER_SILENT
		printf("\n\rAxis: z");
#endif
		g1 = _gEst.X;
		g2 = _gEst.Y;
	}


	//Determine which of the 4 axis
	if(g1<0 && g2<0){ (*axisNumber) = 0;}
	if(g1>0 && g2<0){ (*axisNumber) = 1;}
	if(g1>0 && g2>0){ (*axisNumber) = 2;}
	if(g1<0 && g2>0){ (*axisNumber) = 3;}

#ifndef CONTROLLER_SILENT
	printf("\n\raxisNumber: %i",*axisNumber);
#endif

	//Determine EqAngle

	if((*axisIndex) == 0) {angle = _edgeAngles.X;}
	else if((*axisIndex) == 1){angle = _edgeAngles.Y;}
	else if((*axisIndex) == 2){angle = _edgeAngles.Z;}

#ifndef CONTROLLER_SILENT
	printf("\n\rAngle: %f",angle);
#endif

	if((*axisIndex) == 0) {
		v1 = _cubliParameters.m_K.Y/_cubliParameters.mass;
		v2 = _cubliParameters.m_K.Z/_cubliParameters.mass;
	}
	if((*axisIndex) == 1) {
		v1 = _cubliParameters.m_K.Z/_cubliParameters.mass;
		v2 = _cubliParameters.m_K.X/_cubliParameters.mass;
	}
	if((*axisIndex) == 2) {
		v1 = _cubliParameters.m_K.X/_cubliParameters.mass;
		v2 = _cubliParameters.m_K.Y/_cubliParameters.mass;
	}


	if((*axisNumber) == 0){
		(*eqAngle) = atan2(v1,v2);
	}
	if((*axisNumber) == 1){
		(*eqAngle) = atan2(v1-0.15,v2);
	}
	if((*axisNumber) == 2){
		(*eqAngle) = atan2(v1-0.15,v2-0.15);
	}
	if((*axisNumber) == 3){
		(*eqAngle) = atan2(v1,v2-0.15);
	}

#ifndef CONTROLLER_SILENT
	printf("\n\reqAngle: %f",*eqAngle);
#endif
}


void DetermineRotAxisAndEqAngle(uint8_t* axisIndex, float* eqAngle)
{
	Vector3 g = _gEst;

	if(g.X < 0)
	{
		g.X = -g.X;
	}
	if(g.Y < 0)
	{
		g.Y = -g.Y;
	}
	if(g.Z < 0)
	{
		g.Z = -g.Z;
	}

	float eqCandidates[4] = {-135, -45, 45, 135};
	uint8_t eqIndex;
	uint8_t i;
	float minDelta = 360;
	float angleDeg;
	float delta;

		//x-Oriented-Axis
		if(g.Y > g.X && g.Z > g.X)
		{
			(*axisIndex) = 0;
			angleDeg = _xyzAngles.X*180.0/PI;

		}
		//y-Oriented-Axis
		if(g.X > g.Y && g.Z > g.Y)
		{
			(*axisIndex) = 1;
			angleDeg = _xyzAngles.Y*180/PI;
		}
		//z-Oriented-Axis
		if(g.X > g.Z && g.Y > g.Z)
		{
			(*axisIndex) = 2;
			angleDeg = _xyzAngles.Z*180/PI;
		}

		//-135,-45,45,135
		for(i=0; i<4; ++i)
		{
			delta = angleDeg - eqCandidates[i];
			if(delta < 0) delta = -delta;
			if(delta < minDelta)
			{
				minDelta = delta;
				eqIndex = i;
			}
		}

		(*eqAngle) = eqCandidates[eqIndex] * PI/180.0;
}


#define NonLinControllerTaskT 20
void NonLinControllerTask(void* parameters)
{
		float angle;
		float prevAngle=4;
		Quaternion P_IK;

		float balancingAngle=10.0/180.0*PI;


		// physical quantities
		Vector3 g_K;
		Vector3 K_w_IK;
		Vector3 K_w_w;
		Vector3 p1;
		Vector3 ps;
		Vector3 pw;
		Vector3 v;

		Vector3 mm_K={0.0,0.0,0.0}; // -m_K
		Vector3 g_I = {0,0,-9.81};

		// auxilary variables
		Vector3 tmp;
		Vector3 a1;
		Vector3 a2;
		Vector3 a3;
		Vector3 a4;
		Vector3 a5;
		Vector3 A_5a5;

		float proj;

		// Physical Matrices
		Vector3 m_K;
		if(HasFlashBeenErased()){
			 m_K = _cubliParameters.m_K;
		}else{
			ReadmK(&m_K.X,&m_K.Y,&m_K.Z);	// read from flash memory
			_cubliParameters.m_K.X=m_K.X;
			_cubliParameters.m_K.Y=m_K.Y;
			_cubliParameters.m_K.Z=m_K.Z;
#ifndef CONTROLLER_SILENT
			printf("NLC: read m_K from flash.\n\r");
#endif
		}
#ifndef CONTROLLER_SILENT
		printf("NLC: m_K: %f, %f, %f\n\r",m_K.X,m_K.Y,m_K.Z);
#endif

		uint8_t m_Kcounter=0;			// counter to save m_K

	    const float alpha = 0.002;

		const float Knc1 = 15;
		const float Knc2 = 18;
		const float Knc3 = 12;//7.3;
		const float Knc4 = 0.00001;

		unsigned int excitation_counter = 0;
		float r_scal = 2; //1.5 for slow spinning, 10 for fast spinning
		uint8_t counter=0;


	    while(!_attitudeReady) Delay(300);

		portTickType xLastExecutionTime = xTaskGetTickCount();

		for (;;)
		{
			DelayUntilTime(&xLastExecutionTime, NonLinControllerTaskT);


			if(_balancingMode == CornerBalancing)
			{
				P_IK.W = _finalEst.W;
				P_IK.X = _finalEst.X;
				P_IK.Y = _finalEst.Y;
				P_IK.Z = _finalEst.Z;

				QMulV(&P_IK,&g_I,&g_K);
				V3Minus(&m_K,&mm_K);
				V3Angle0(&g_K,&mm_K,&angle);


				//switch off controller when Cubli is falling, and reset states.
				if(fabs(angle)>fabs(prevAngle)+0.01 && fabs(angle)>balancingAngle){
#ifndef CONTROLLER_SILENT
					//printf("Cubli is falling---> reset\n\r");
#endif
					_balancingMode = NotBalancing;
					_jumpMode = NoJump;
					_balance = false;
					counter=0;
					prevAngle=-4;
					_moveSuccess = false;

					SetCurrent(_motor[0].NodeID, 0);
					SetCurrent(_motor[1].NodeID, 0);
					SetCurrent(_motor[2].NodeID, 0);
				}else{
					prevAngle = angle;
				}

				// determine wether to balance or not
				if(_modeNumber!=Idle && angle < 1.0/180.0*PI){
					// start balancing if the button was pressed
					if ( _modeNumber != GetDown )
                        _balance = true;
					else
					{
				    // force the cube to fall
					_balance = false;
					float ConstantXTorque =  0.1;
					float ConstantYTorque = -0.1;
					float ConstantZTorque =  0.0;
					SetTorqueWSaturation(0,&ConstantXTorque);
					SetTorqueWSaturation(1,&ConstantYTorque);
					SetTorqueWSaturation(2,&ConstantZTorque);
					}
				}

				if( _balance && fabs(angle) < balancingAngle)
				{
					// Get states
					K_w_IK.X = (float)_omega.X;
					K_w_IK.Y = (float)_omega.Y;
					K_w_IK.Z = (float)_omega.Z;

					K_w_w.X = (float)-_motor[MOTORX].Velocity;
					K_w_w.Y = (float)-_motor[MOTORY].Velocity;
					K_w_w.Z = (float)-_motor[MOTORZ].Velocity;


					if(fabs(angle) < 1.5/180.0*PI){

						// change learning update depending on the success of the jump.
						if(counter>10){
							_jumpSuccess3D=true;
							if ( _modeNumber == Get2C && !_moveSuccess )
							{
#ifndef CHOREOGRAPHY_SILENT
								printf("made it to corner\r\n");
#endif
								_moveSuccess = true;
							}
						}else{
							counter++;
						}

						V3Dot(&m_K, &g_K, &proj);
						m_K.X = (1.0f - alpha) * m_K.X + alpha * proj / ((float)96.2361) * g_K.X;
						m_K.Y = (1.0f - alpha) * m_K.Y + alpha * proj / ((float)96.2361) * g_K.Y;
						m_K.Z = (1.0f - alpha) * m_K.Z + alpha * proj / ((float)96.2361) * g_K.Z; // shift m_K, but preserve length.

						V3Normalize(&m_K,&tmp);
						V3MulS(&_cubliParameters.m_K_length,&tmp,&m_K);


						// save m_K_vector...
						if(fabs(K_w_w.X)<10 && fabs(K_w_w.Y)<10 && fabs(K_w_w.Z)<10 &&
								fabs(K_w_IK.X)<0.05 && fabs(K_w_IK.Y)<0.05 && fabs(K_w_IK.Z)<.05){
							m_Kcounter++;
							if(m_Kcounter>=100){
								_writeData=true;
								_cubliParameters.m_K.X=m_K.X;
								_cubliParameters.m_K.Y=m_K.Y;
								_cubliParameters.m_K.Z=m_K.Z;
								m_Kcounter=0;
							}
						}else{
							m_Kcounter=0;
						}


						//when reaching steady state during balancing, print this out and use this value as _cubliParameters.m_K.
#ifndef CONTROLLER_SILENT
						//printf("\n\r\t %f\t %f\t %f",m_K.X, m_K.Y, m_K.Z);
#endif
					}

					// Calculate generalized momenta
					pw.X = _cubliParameters.thetaW[0] * (K_w_IK.X + K_w_w.X);
					pw.Y = _cubliParameters.thetaW[1] * (K_w_IK.Y + K_w_w.Y);
					pw.Z = _cubliParameters.thetaW[2] * (K_w_IK.Z + K_w_w.Z);

					p1.X = _cubliParameters.thetaH[0] * K_w_IK.X + _cubliParameters.thetaH[1] * K_w_IK.Y + _cubliParameters.thetaH[2] * K_w_IK.Z + pw.X;
					p1.Y = _cubliParameters.thetaH[1] * K_w_IK.X + _cubliParameters.thetaH[3] * K_w_IK.Y + _cubliParameters.thetaH[4] * K_w_IK.Z + pw.Y;
					p1.Z = _cubliParameters.thetaH[2] * K_w_IK.X + _cubliParameters.thetaH[4] * K_w_IK.Y + _cubliParameters.thetaH[5] * K_w_IK.Z + pw.Z;;

					// Take out uncontrollable component
					V3Dot(&g_K,&p1,&proj);
					ps.X = p1.X - g_K.X * (proj / ((float)96.2361));
					ps.Y = p1.Y - g_K.Y * (proj / ((float)96.2361));
					ps.Z = p1.Z - g_K.Z * (proj/ ((float)96.2361));

					//new controller
					V3Cross(&m_K, &g_K, &a1);
					V3Cross(&p1, &K_w_IK, &a2);
					V3Cross(&g_K, &K_w_IK, &tmp);
					V3Cross(&m_K, &tmp, &a3);
					V3Cross(&ps, &K_w_IK, &a4);

					a5.X = Knc1 * a4.X + (Knc1+Knc2*Knc3+Knc4)*a1.X + Knc2*a3.X + Knc1*Knc3 * ps.X;
					a5.Y = Knc1 * a4.Y + (Knc1+Knc2*Knc3+Knc4)*a1.Y + Knc2*a3.Y + Knc1*Knc3 * ps.Y;
					a5.Z = Knc1 * a4.Z + (Knc1+Knc2*Knc3+Knc4)*a1.Z + Knc2*a3.Z + Knc1*Knc3 * ps.Z;

					A_5a5.X = _cubliParameters.thetaH[0] * a5.X + _cubliParameters.thetaH[1] * a5.Y + _cubliParameters.thetaH[2] * a5.Z;
					A_5a5.Y = _cubliParameters.thetaH[1] * a5.X + _cubliParameters.thetaH[3] * a5.Y + _cubliParameters.thetaH[4] * a5.Z;
					A_5a5.Z = _cubliParameters.thetaH[2] * a5.X + _cubliParameters.thetaH[4] * a5.Y + _cubliParameters.thetaH[5] * a5.Z;

					v.X = a1.X + a2.X + Knc3 * (p1.X-pw.X) + A_5a5.X;
					v.Y = a1.Y + a2.Y + Knc3 * (p1.Y-pw.Y) + A_5a5.Y;
					v.Z = a1.Z + a2.Z + Knc3 * (p1.Z-pw.Z) + A_5a5.Z;

					SetTorqueWSaturation(0,&v.X);
					SetTorqueWSaturation(1,&v.Y);
					SetTorqueWSaturation(2,&v.Z);

					// printout data
#ifndef CONTROLLER_SILENT
					//printf("%f \t%f \t%f \t%f \t%f \t%f \t%f \t%f \t%f \t%f\n\r",angle,K_w_IK.X,K_w_IK.Y,K_w_IK.Z,K_w_w.X,K_w_w.Y,K_w_w.Z,v.X,v.Y,v.Z);
#endif
				}
			}
		}
}


#define LQRTask1DT 20
void LQRTask1D(void *pvParameters)
{
	float x1=0, x2=0, x3=0, x4=0;
	float theta=0, thetaDot=0;
	uint8_t F2EmotorIndex = MOTORY;

	float maxCurrent = 2.0;
	uint8_t safetyCounter = 0;
	uint16_t excitationCounter=0;
	uint16_t lutCounter=0;

	float prevAngle = 4;

	float current = 0;
	int16_t currentmA = 0;

	float k_ww;
	float k_t;

	// filter params
	float f = 0.01;

	float filterState = 0;//-0.007;

	// wait for IMUS
	while(!_imusReady)
	{
		Delay(100);
	}

	portTickType xLastExecutionTime = xTaskGetTickCount();


	for(;;)
	{
		DelayUntilTime(&xLastExecutionTime, LQRTask1DT);



		if(_balancingMode == EdgeBalancing) {

			EnC();
			theta = _edgeAngles.Y - _cubliParameters.edgeEqAngle;
			thetaDot = _omega.Y;
			ExC();

			maxCurrent = 4.0;
			// allow current of 4A for only one second (as specified in the data sheet)
			safetyCounter++;
			if(safetyCounter>50){
				safetyCounter = 100;
				maxCurrent = 2.0;
			}

			//switch off controller when Cubli is falling, and reset states.
			if((fabs(theta)>fabs(prevAngle) && fabs(theta)>6.0/180.0*PI) || fabs(theta)>fabs(prevAngle)+0.5){
				_balancingMode = NotBalancing;
				_jumpMode = NoJump;
				_balance = false;
				SetCurrent(_motor[F2EmotorIndex].NodeID, 0);
#ifndef CONTROLLER_SILENT
				printf("Balancing 1D: Shut controller off, since Cubli is falling...\n\r");
#endif
			}

#ifndef CONTROLLER_SILENT
			//printf("edge angle:%f\n\r",theta/PI*180.0);
#endif

			prevAngle = theta;


			//start balancing when the user button has been pressed
			if(_modeNumber != Idle && theta < 0.02){
				_balance = true;
			}

			if(_balance)
			{
#ifndef CONTROLLER_SILENT
				//printf("kww,kt: %f\t%f\n\r",k_ww,k_t);
#endif
				// get measurements [x1;x2;x3] = [theta_b; theta_b_dot; theta_w]
				if(theta < 2*PI/180 && theta > -2*PI/180 && safetyCounter>20) {
					filterState = (1.0 - f) * filterState + f *theta;
				}

				x1 = theta - filterState ;
				x2 = thetaDot;
				EnC(); x3 = -_motor[F2EmotorIndex].Velocity; ExC();

				// record minimal angle (and angular velocity there) for F2EJumpLearning
				if(fabs(x1)<fabs(_minEdgeAngle)){
					_minEdgeAngle = x1;
					_angVel = x2;
				}


				// ignore wheel velocities at the beginning (of a F2Ejump)
				if(k_ww<0.08 && safetyCounter>10){
					k_ww = k_ww + 0.002;
				}

				// reduce gain at the beginning of the jump because of uncertainties
				if(k_t<300){
					k_t = k_t*1.2;
				}

				//when reaching steady state during balancing, print this out and use this value as _cubliParameters.edgeEqAngle.
				//***
				//***


				//
				if(_jumpMode == E2CJump){
					//pretty good: 429.6824*x1+60.1172*x2+0.2997*x3;
					//current=429.6824*x1+60.1172*x2+0.2997*x3;

					// with integrator;
					x4+=x3*.02;
					current=435.3*x1+60.7*x2+.31*x3+.03*x4;
				}else{
					current = (k_t*(x1 + x2/8.4251) + k_ww*x3);
				}
				/*if(excitationCounter>500){
					f=0;
					current+=r[lutCounter]*5;
#ifndef CONTROLLER_SILENT
					printf("%i\t %f\t %f\t %f\t %f\t %f\n\r",lutCounter,x1,x2,x3,current,r[lutCounter]);
#endif
					lutCounter++;
					if(lutCounter==LUT_LENGTH){
						lutCounter=0;
					}
				}else{
					excitationCounter++;
				}*/
				//current=412.7726*x1 + 73.9285*x2+0.4436*x3;

				/*// Static friction correction
				if(x3 == 0)
				{
					if(current > 0)
						current += 0.15;
					else
						current -= 0.15;
				}*/

				if (current > maxCurrent)
						current = maxCurrent;
				else if (current < -maxCurrent)
						current = -maxCurrent;

				//record mean of applied current during the first few intervals
				if(safetyCounter==1){_meanF2ECurrent = current;}
				else if(safetyCounter<=12){_meanF2ECurrent = (_meanF2ECurrent*(safetyCounter-1) + current)/(safetyCounter);}

				//alternatively, record total energy input during the first few intervals
				//if(safetyCounter==1){_meanF2ECurrent = -x2*current*0.0335*0.020;}
				//else if(safetyCounter<=12){_meanF2ECurrent = _meanF2ECurrent-x2*current*0.0335*0.020;}

				currentmA = (int16_t)(current * (-1000.0));

				if (x2 < 0.01 && x2 > -0.01 && x1 < 0.001 && x1 > -0.001 && x3<5 && x3>-5)
				{
					if( _modeNumber == ButtonMode2 ){
						_jumpMode = E2CJump;
						k_ww=.2; //k_ww = 0.25; changed to .2 M.M.
						k_t=500;//k_t = 450; changed to 500. M.M.
						f=0;
					}
				}


				if( _modeNumber != Idle && x3 < 250 && x3 > -250)
					SetCurrent(_motor[F2EmotorIndex].NodeID, (int16_t)currentmA);
				else
				{
					SetCurrent(_motor[F2EmotorIndex].NodeID, -25 * _motor[F2EmotorIndex].Velocity);
				}
			}
		}else if(k_ww!=0 || safetyCounter!=0 || prevAngle != 4){ //reset states
			k_ww = 0;
			k_t =  83.725;
			safetyCounter=0;
			filterState=0;
			prevAngle = 4;
			f=0.01;
		}
	}
}


#define LQRTask1DNewT 20
void LQRTask1DNew(void *pvParameters)
{
	float x1=0, x2=0, x3=0, x4=0;	// states
	float theta=0, thetaDot=0;		//
	float T=0;						// torque
	float Tprev=0;
	uint8_t MotorIndex = MOTORY;
	uint8_t constant_velocity_counter=0;
	uint8_t i=0;

	// get edge equilibrium angle from flash...
	Delay(100);					// for nicer printouts.
	float edgeAngle=0;
	if(HasFlashBeenErased()){
		 edgeAngle = _cubliParameters.edgeEqAngle;
	}else{
		ReadEdgeAngle(&edgeAngle);	// read from flash memory
		_cubliParameters.edgeEqAngle=edgeAngle;
#ifndef CONTROLLER_SILENT
		printf("LQR1D: read edgeAngle from flash.\n\r");
#endif
	}
#ifndef CONTROLLER_SILENT
	printf("LQR1D:eAngle: %f\n\r",edgeAngle);
#endif


	bool excite=false;				// for exciting the system, f.ex. for system ID
	uint8_t excitation_counter=0;

	uint8_t counter=0;				// for checking if jump up was successful.
	uint8_t edgeAngleCounter=0;		// for storing edge angle
	uint8_t stable_counter=0;	 	// for checking that cubli is balancing stably

	float prevAngle = -4;

	// Feeback gain
	//const float K_csu[4]={16.207666, 2.195534, 0.013419, 0.010536};  // --> good for speeding up wheels -> agressive
	const float K_cb[4]={9.836228, 1.659694, 0.004929, 0.003734}; //--> not too bad, but problem when speeding up wheels
	//const float K_cb[4]={12.314212, 1.832904, 0.007622, 0.001106};

	// measure m_I (as a check for learning)
	Quaternion P_IK;
	Quaternion P_KI;
	Vector3 m_K=_cubliParameters.m_K;
	Vector3 m_I;


	// wait for IMUS
	while(!_imusReady)
	{
		Delay(100);
	}

	portTickType xLastExecutionTime = xTaskGetTickCount();


	for(;;)
	{
		DelayUntilTime(&xLastExecutionTime, LQRTask1DNewT);



		if(_balancingMode == EdgeBalancing) {

			EnC();
			theta = _edgeAngles.Y - edgeAngle;
			thetaDot = _omega.Y;
			ExC();

#ifndef CONTROLLER_SILENT
			//printf("theta:%f\n\r",theta);
#endif
			//switch off controller when Cubli is falling, and reset states.
			if((fabs(theta)>fabs(prevAngle) && fabs(theta)>6.0/180.0*PI) || fabs(theta)>fabs(prevAngle)+0.5){
				_balancingMode = NotBalancing;
				_jumpMode = NoJump;
				_balance = false;
				//_jumpSuccess1D = false;
				_moveSuccess = false;

				prevAngle=-4;

				//Reset Speedup.
				EnC();
				for(i=0;i<3;i++){
					_ww_des[i]=0;
					_speedup[i]=false;
					SetCurrent(_motor[i].NodeID,0);
				}
				_speedupLQR=false;
				ExC();
				x4=0;
				counter=0;
				edgeAngleCounter=0;

#ifndef CONTROLLER_SILENT
				printf("Balancing 1D LQR Shut controller off, since Cubli is falling...\n\r");
#endif
			}else if(_modeNumber != Idle && fabs(theta) < 1.5/180.0*PI){
				if ( _modeNumber != GetDown )
				{
					//start balancing when the user button has been pressed
					_balance = true;
				}
				else
				{
					// special case: when we want cubli to fall down
					float ConstantTorque = -0.1;
					SetTorqueWSaturation(MotorIndex,&ConstantTorque);
					_balance = false;
				}
			}
			if(_balance)
			{
				prevAngle = theta;

				// states:
				x1 = theta;
				x2 = thetaDot;
		 EnC(); x3 = -_motor[MotorIndex].Velocity; ExC();



#ifndef CONTROLLER_SILENT
				printf("%f \t%f \t%f \t%f \t%f \t%i\n\r",_edgeAngles.Y,x1,x2,x3,x4,_speedupLQR);
#endif


				if(fabs(x3)<1.0 && fabs(x1)<0.5/180.0*PI && fabs(x2)<0.01 ){
					edgeAngleCounter++;
					if(edgeAngleCounter>100){
						edgeAngleCounter=0;
						_cubliParameters.edgeEqAngle=_edgeAngles.Y;
						_writeData=true;
					}
				}

				// Calculate P_IK, m_I as a check
				/*P_IK.W = _finalEst.W;
				P_IK.X = _finalEst.X;
				P_IK.Y = _finalEst.Y;
				P_IK.Z = _finalEst.Z;

#ifndef CONTROLLER_SILENT
				printf("%f \t%f \t%f \t%f\n\r",P_IK.W,P_IK.X,P_IK.Y,P_IK.Z);
#endif
				/*
				QConjugate(&P_IK,&P_KI);
				QMulV(&P_KI,&m_K,&m_I);

#ifndef CONTROLLER_SILENT
				printf("m_I: %f \t%f \t%f\n\r",m_I.X, m_I.Y, m_I.Z);
#endif
				 */

				// If in mode 2, Jump to the corner
				if( ( _modeNumber == ButtonMode2 || _modeNumber == Get2C ) && fabs(x3)<2.0){
					_jumpMode=E2CJump;
				}



				x4+=x3*.02;
				T=K_cb[0]*x1+K_cb[1]*x2+K_cb[2]*x3+K_cb[3]*x4;

#ifndef CONTROLLER_SILENT
				printf("T:%f, x3:%f, x4:%f\n\r",T,x3,x4);
#endif


				// excitation
				/*if(excite){
					T=0;//Fall...
				}else{
					excitation_counter++;
					if(excitation_counter>200)
						excite=true;
				}*/


				Tprev=T;


				// apply torque
				SetTorqueWSaturation(MotorIndex,&T);
				if(fabs(Tprev)>fabs(T))	//Anti Inegrator Wind-Up!
					x4-=x3*.02;

				if(counter<10)
					counter++;
				else if(counter==10){
					_jumpSuccess1D=true;
					counter=11;
				}


				// check that cubli has been stable for a while
				if( ( fabs(theta)>fabs(prevAngle) && fabs(theta)>6.0/180.0*PI ) || fabs(theta)>fabs(prevAngle)+0.5)
				{
					stable_counter=0;
				}
				else
				{
					stable_counter++;
					if ( stable_counter >= 10 )
					{
						stable_counter = 0;
						if ( _modeNumber == Get2E && !_moveSuccess ) {
						#ifndef CHOREOGRAPHY_SILENT
							printf("Made it to edge\r\n");
						#endif
						_moveSuccess = true;
						}
					}
				}

				if(_speedupLQR){
					if(fabs(x3)<=10.0f){	//10 rad/s threshold
						constant_velocity_counter++;
					}else{
						constant_velocity_counter=0;
					}
					if(constant_velocity_counter>=10){
						_reached_velocity[1]=true;
					}else{
						_reached_velocity[1]=false;
					}

				}

			}
		}
	}
}




#define LQRTask1DNewT 20
void ControlledFall(void *pvParameters)
{
	float x1=0, x2=0, x3=0, x4=0;	// states
	float theta=0, thetaDot=0;		//
	float T=0;						// torque
	float Tprev=0;
	uint8_t MotorIndex = MOTORY;
	uint8_t constant_velocity_counter=0;
	uint8_t i=0;

	// get edge equilibrium angle from flash...
	Delay(100);					// for nicer printouts.
	float edgeAngle=0;
	if(HasFlashBeenErased()){
		 edgeAngle = _cubliParameters.edgeEqAngle;
	}else{
		ReadEdgeAngle(&edgeAngle);	// read from flash memory
		_cubliParameters.edgeEqAngle=edgeAngle;
#ifndef CONTROLLER_SILENT
		printf("LQR1D: read edgeAngle from flash.\n\r");
#endif
	}
#ifndef CONTROLLER_SILENT
	printf("LQR1D:eAngle: %f\n\r",edgeAngle);
#endif


	bool excite=false;				// for exciting the system, f.ex. for system ID
	uint8_t excitation_counter=0;

	uint8_t counter=0;				// for checking if jump up was successful.
	uint8_t edgeAngleCounter=0;		// for storing edge angle

	float prevAngle = -4;

	// Feeback gain
	//const float K_csu[4]={16.207666, 2.195534, 0.013419, 0.010536};  // --> good for speeding up wheels -> agressive
	const float K_cb[4]={9.836228, 1.659694, 0.004929, 0.003734}; //--> not too bad, but problem when speeding up wheels
	//const float K_cb[4]={12.314212, 1.832904, 0.007622, 0.001106};

	// measure m_I (as a check for learning)
	Quaternion P_IK;
	Quaternion P_KI;
	Vector3 m_K=_cubliParameters.m_K;
	Vector3 m_I;

	int counterf=0;


	// wait for IMUS
	while(!_imusReady)
	{
		Delay(100);
	}

	portTickType xLastExecutionTime = xTaskGetTickCount();


	for(;;)
	{
		DelayUntilTime(&xLastExecutionTime, LQRTask1DNewT);



		if(_balancingMode == EdgeBalancing) {

			EnC();
			theta = _edgeAngles.Y - edgeAngle;
			thetaDot = _omega.Y;
			ExC();

#ifndef CONTROLLER_SILENT
			//printf("theta:%f\n\r",theta);
#endif
			//switch off controller when Cubli is falling, and reset states.
			if((fabs(theta)>fabs(prevAngle) && fabs(theta)>6.0/180.0*PI) || fabs(theta)>fabs(prevAngle)+0.5){
				_balancingMode = NotBalancing;
				_jumpMode = NoJump;
				_balance = false;

				prevAngle=-4;

				//Reset Speedup.
				EnC();
				for(i=0;i<3;i++){
					_ww_des[i]=0;
					_speedup[i]=false;
					SetCurrent(_motor[i].NodeID,0);
				}
				_speedupLQR=false;
				ExC();
				x4=0;
				counter=0;
				edgeAngleCounter=0;

#ifndef CONTROLLER_SILENT
				printf("Balancing 1D LQR Shut controller off, since Cubli is falling...\n\r");
#endif
			}else if( _modeNumber != Idle && fabs(theta) < 1.5/180.0*PI){
				//start balancing when the user button has been pressed
				_balance = true;
			}
			if(_balance)
			{
				counterf++;
				if(counterf == 1000){
#ifndef CONTROLLER_SILENT
					printf("controlled fall.\n\r");
#endif
#ifndef CONTROLLER_SILENT
					printf("%f\t%f\t%f\n\r",theta,thetaDot,-_motor[MotorIndex].Velocity);
#endif
				}
				else if(counterf >= 1000){
#ifndef CONTROLLER_SILENT
					printf("%f\t%f\t%f\n\r",theta,thetaDot,-_motor[MotorIndex].Velocity);
#endif
				}else if(counterf >= 2000){
					_balance=false;
					counterf=0;
				}else{
					prevAngle = theta;

					// states:
					x1 = theta;
					x2 = thetaDot;
					EnC(); x3 = -_motor[MotorIndex].Velocity; ExC();

#ifndef CONTROLLER_SILENT
					printf("%f \t%f \t%f \t%f \t%f \t%i\n\r",_edgeAngles.Y,x1,x2,x3,x4,_speedupLQR);
#endif


					if(fabs(x3)<1.0 && fabs(x1)<0.5/180.0*PI && fabs(x2)<0.01 ){
						edgeAngleCounter++;
						if(edgeAngleCounter>100){
							edgeAngleCounter=0;
							_cubliParameters.edgeEqAngle=_edgeAngles.Y;
							_writeData=true;
						}
					}

					/*// Calculate P_IK, m_I as a check
					P_IK.W = _finalEst.W;
					P_IK.X = _finalEst.X;
					P_IK.Y = _finalEst.Y;
					P_IK.Z = _finalEst.Z;

#ifndef CONTROLLER_SILENT
					printf("%f \t%f \t%f \t%f\n\r",P_IK.W,P_IK.X,P_IK.Y,P_IK.Z);
#endif
					/*
					QConjugate(&P_IK,&P_KI);
					QMulV(&P_KI,&m_K,&m_I);

#ifndef CONTROLLER_SILENT
					printf("m_I: %f \t%f \t%f\n\r",m_I.X, m_I.Y, m_I.Z);
#endif
					 */

					if( _modeNumber == ButtonMode2 && fabs(x3)<2.0){
						_jumpMode=E2CJump;
					}


					x4+=x3*.02;
					T=K_cb[0]*x1+K_cb[1]*x2+K_cb[2]*x3+K_cb[3]*x4;


					// excitation
					/*if(excite){
						T=0;//Fall...
					}else{
						excitation_counter++;
						if(excitation_counter>200)
							excite=true;
					}*/


					Tprev=T;

					// apply torque
					SetTorqueWSaturation(MotorIndex,&T);
					if(fabs(Tprev)>fabs(T))	//Anti Inegrator Wind-Up!
						x4-=x3*.02;

					if(counter<10)
						counter++;
					else if(counter==10){
						_jumpSuccess1D=true;
						counter=11;
					}

					if(_speedupLQR){
						if(fabs(x3)<=10.0f){	//10 rad/s threshold
							constant_velocity_counter++;
						}else{
							constant_velocity_counter=0;
						}
						if(constant_velocity_counter>=10){
							_reached_velocity[1]=true;
						}else{
							_reached_velocity[1]=false;
						}

					}
				}
			}
		}
	}
}



void GetDirection3DJump()
{
	Quaternion P_IK;
	Quaternion P_KI;
	Vector3 m_I;
	Vector3 e_z={0,0,1};
	Vector3 dir;
	Vector3 dir_normalized;

	EnC();
	P_IK.W = _finalEst.W;
	P_IK.X = _finalEst.X;
	P_IK.Y = _finalEst.Y;
	P_IK.Z = _finalEst.Z;
	ExC();

	QConjugate(&P_IK,&P_KI);
	QMulV(&P_KI,&_cubliParameters.m_K,&m_I);

	V3Cross(&e_z,&m_I,&dir);
	V3Normalize(&dir,&dir_normalized);


#ifndef CONTROLLER_SILENT
	//printf("dir: %f,%f,%f, m_I:%f,%f,%f\n\r",dir_normalized.X,dir_normalized.Y,dir_normalized.Z,m_I.X,m_I.Y,m_I.Z);
#endif
	_JumpDirection[0]=dir_normalized.X;
	_JumpDirection[1]=dir_normalized.Y;
}


#define ChoreographyManagementT 50
static bool forceReRunModeManagement = false; // rerun modeManagement without delay when triggered
static int choreography_clock = 0; // all clocks in [ms]
static int    primitive_clock = 0;
// task controlling functions:
int run_choreography_management( char arg )
{
    // Checks whether choreography management is supposed to be running

    static int running = 0;

    if ( arg == '?' ) return running;
    if ( arg == 1 ) running = 1;
    if ( arg == 2 ) running = 0;

    return 0;
}
void start_choreography()
{
    run_choreography_management( 1 );
}
void stop_choreography()
{
    // _modeNumber = GetDown;
    run_choreography_management( 2 );
    choreography_clock = 0;
}
bool announce_recovery(char arg)
{
	// automatically makes sure announcements are only sent once
	static bool announce = false;

	if ( arg == '?') { bool to_return = announce;
					   announce = false;
					   return to_return; }
	if ( arg == 1  ) announce = true;

	return false;
}
bool announce_failure(char arg)
{
	// automatically makes sure announcements are only sent once
	static bool announce = false;

	if ( arg == '?') { bool to_return = announce;
					   announce = false;
					   return to_return; }
	if ( arg == 1  ) announce = true;

	return false;
}
bool announce_other_failure(char arg)
{
	// automatically makes sure announcements are only sent once
	static bool announce = false;

	if ( arg == '?') { bool to_return = announce;
					   announce = false;
					   return to_return; }
	if ( arg == 1  ) announce = true;

	return false;
}
bool react_to_other_failure(char arg)
{
	// automatically make sure other cubli failures are reported only once
	static bool react = false;

	if ( arg == '?') { bool to_return = react;
					   react = false;
					   return to_return; }
	if ( arg == '1') { react = true; }

	return false;
}
bool react_to_resume( char arg )
{
	// automatically make sure computer order to resume gets reported once
	static bool react = false;

	if ( arg == '?') { bool to_return = react;
					   react = false;
					   return to_return; }
	if ( arg == '1') { react = true; }

	return false;
}
// Task itself:
void ChoreographyManagement(void *pvParameters)
{

 	//Description

 /* based on the current action to execute as defined by the timeline status,
  * the action manager executes control routines - F2EJump, E2CJump, etc -
  * combines with ModeManagement in the mode controlled Cubli software.
  */

	com_pcode ongoing_primitive  = COM_PCODE_NONE;
	int failure_clock = 0; // all clocks in [ms]
	int   total_clock = 0;
    typedef enum FailureModes
    {
        NOFAILURE ,
        RECOVERING,
        NEEDASSIST,
		// when waiting for computer to give green light
		RECOVERED ,
		OTHERCUBLI
    } fail_mode;
    fail_mode     failure_status = NOFAILURE;

	portTickType xLastExecutionTime = xTaskGetTickCount();

	for(;;){

		DelayUntilTime(&xLastExecutionTime, ChoreographyManagementT );

		if ( run_choreography_management('?') )
		{

			// When the choreography is just started, initialize
			if ( choreography_clock == 0 )
			{
				failure_status = NOFAILURE;
				failure_clock  = 0;
				  total_clock  = 0;
			   primitive_clock = 0;
				_moveSuccess   = true;
			}

			// Check whether this cubli has recovered
			if ( _moveSuccess )
			{
				if ( failure_status == RECOVERING )
					{ announce_recovery(true); failure_status = RECOVERED; }
			}

			// Check whether to resume
			if ( react_to_resume('?') )
			{
				failure_status = NOFAILURE;
				failure_clock  = 0;
			}

			// Check for success 1 timestep before the primitive is going to change
			if ( ongoing_primitive != get_ongoing_primitive( ( choreography_clock / 1000 ) + COM_P_TIMELINE_DT ) )
			{
				if ( !_moveSuccess && failure_status != RECOVERING )
				{
					failure_status = RECOVERING;
					announce_failure(true);
				}
			}

			// Reset variables when primitive changes
		    if ( ongoing_primitive != get_ongoing_primitive( choreography_clock / 1000 ) || choreography_clock == 0)
		    {
		    	primitive_clock = 0;
		    	_moveSuccess = false;
		    	forceReRunModeManagement = true; // act upon new primitive without delay
#ifndef CHOREOGRAPHY_SILENT
		    	printf("%i ms - Now performing: %i\r\n",
		    			choreography_clock,
						get_ongoing_primitive( choreography_clock / 1000 ) );
#endif
		    }
			// update ongoing primitive
			ongoing_primitive = get_ongoing_primitive( choreography_clock / 1000 );

			// automatically succeed empty choreography move
			if ( ongoing_primitive == COM_PCODE_NONE )
			{
#ifndef CHOREOGRAPHY_SILENT
				if ( !_moveSuccess ) { printf("success move zero");}
#endif
				_moveSuccess = true;
			}

			// Check if other cublis failed
			if ( react_to_other_failure('?') && failure_status == NOFAILURE )
			{
				failure_status = OTHERCUBLI;
				announce_other_failure( true );
			}

			// Advance the timeline clock
			if ( failure_status == NOFAILURE )
			{
				choreography_clock += ChoreographyManagementT;
			}
			else
			{
				failure_clock      += ChoreographyManagementT;
			}
			primitive_clock 	   += ChoreographyManagementT;
		        total_clock 	   += ChoreographyManagementT;

			switch ( ongoing_primitive )
			{
			case COM_PCODE_NONE    :                           break;
			case COM_PCODE_JUMP2E  : _modeNumber = Get2E;      break;
			case COM_PCODE_JUMP2C  : _modeNumber = Get2C;      break;
			case COM_PCODE_JUMP2G  : _modeNumber = GetDown;    break;
			case COM_PCODE_1SPIN   : _modeNumber = SingleSpin; break;

			case COM_PCODE_FIN    :
				stop_choreography();
				break;
			default :
				break;
			}

		}

	}



}

#define ModeManagementT 20
void ModeManagement(void *pvParameters)
{

	//Description:

/*	The different modes are:
	- Cubli lying on a face:
		press user button once: F2EJump
		press user button twice: F2EJump + E2CJump
		press user button 3 times: F2CJump

	- Cubli close to the edge:
		press user button once: balancing on the edge as soon as Cubli is close enough to eq position
		press user button twice: balancing on the edge as soon as Cubli is close enough to eq position + E2CJump

	- Cubli close to the corner:
		press user button once: balancing on the corner as soon as Cubli is close enough to eq position
		press user button 4 times: balancing on the edge as soon as Cubli is close enough to eq position + spinning after some time


	Everytime the user button is pressed, the orange LED toggles (on->off / off->on). Pressing the user button twice during 200ms counts as pressing once (to avoid unintended double pressing)

	Apart from setting _jumpMode and _balancingMode after pressing the user button, this task does also reset the _jumpMode when it has been put back on the XY face after balancing.

	*/


	cube_mode oldModeNumber = Idle;
	int waitForButtonPressT = 1000;

	float avg;
	float min;
	float max;

	bool reRunModeManagement = false;


	Vector3 g;
	uint16_t t;
	uint8_t oldBalancingMode = _balancingMode;

	portTickType xLastExecutionTime = xTaskGetTickCount();

	for(;;){

		DelayUntilTime(&xLastExecutionTime, ModeManagementT);

		// Wait for Button press to occur, or specific order to run it (through reRunModeManagement)
		if(oldModeNumber!=_modeNumber || reRunModeManagement || forceReRunModeManagement ){

			if(reRunModeManagement){
				//wait 5 seconds before re-running Mode management
				// to give time for cubli to be stable on a face
				if ( isAButtonMode( _modeNumber ) )
				{
					DelayUntilTime(&xLastExecutionTime, 5000);
				}
				else
				{
					int i;
					for(i = 5000; i > 0; i = i - 100)
					{
						DelayUntilTime(&xLastExecutionTime, 100);
						if ( forceReRunModeManagement ) break;
					}
				}
#ifndef CHOREOGRAPHY_SILENT
				printf("rerun MM.");
#endif
			}
			reRunModeManagement = false;
			forceReRunModeManagement = false;

			// Wait for any following Button presses to occur during the next second
			while(oldModeNumber!=_modeNumber){
				LEDToggle(LED2);

				//Ignore fast double press
				DelayUntilTime(&xLastExecutionTime, 200);
				if ( isAButtonMode( _modeNumber ) && isAButtonMode( oldModeNumber ) )
				{
					if( _modeNumber != cycleButtonMode( oldModeNumber ) && _modeNumber != oldModeNumber )
					{
						_modeNumber = cycleButtonMode( oldModeNumber );
					}
				}

#ifndef CHOREOGRAPHY_SILENT
				printf("oldModeNumber:%i _modeNumber:%i\r\n",
						oldModeNumber,   _modeNumber);
#endif
				oldModeNumber=_modeNumber;

				t=0;
				//Wait for 1 second for another button press to occur
				while(t<waitForButtonPressT && oldModeNumber==_modeNumber){
					DelayUntilTime(&xLastExecutionTime, ModeManagementT);
					t+=ModeManagementT;
				}

			}

			QMulV(&_finalEst,&_gRef,&g);

			if(g.X < 0)
			{
					g.X = -g.X;
			}
			if(g.Y < 0)
			{
					g.Y = -g.Y;
			}
			if(g.Z < 0)
			{
					g.Z = -g.Z;
			}


			//Check whether all elements of g have more or less the same vale -> this means that we're going to balance on the corner
			avg = (g.X + g.Y + g.Z)/3.0;
			min = 0.75 * avg;
			max = 1.25 * avg;
			//Corner
			if(min<g.X && max>g.X && min<g.Y && max>g.Y && min<g.Z && max>g.Z)
			{
				//_balancingMode= Spin;

				switch(_modeNumber){
				case ButtonMode1:
					_balancingMode = CornerBalancing;
					break;
				case ButtonMode2:
					_balancingMode = Spin;
					break;
				case ButtonMode3:
					break;

				case Get2E:
					_balance = false;
					break;
				case Get2C:
					break;
				case GetDown:
					// the fall is handled in the controller
					break;
				case SingleSpin :
					_balancingMode = Spin;
					_spinsCompleted = 0;
					break;


				default:
					_balance = false;
					_modeNumber = Idle;
				break;
				}
			}

			//Check whether one element of g is much larger than the other ones -> this means that the Cubli is lying on a face.
			else if(g.X>2*avg || g.Y>2*avg || g.Z>2*avg)
			{
				_balancingMode = NotBalancing;


				switch(_modeNumber){
				case ButtonMode1:
					_jumpMode = F2EJump; //after that, it will stay on the edge
					break;
				case ButtonMode2:
					_jumpMode = F2EJump; //after that, it will jump to the corner
					break;
				case ButtonMode3:
					_jumpMode = F2CJump;
					break;

				case Get2E:
					_jumpMode = F2EJump;
					break;
				case Get2C:
					_jumpMode = F2CJump;
					break;
				case GetDown:
					_moveSuccess = true;
#ifndef CHOREOGRAPHY_SILENT
					printf("success to floor\r\n");
#endif
					_jumpMode = NoJump;
					_balance = false;
					break;

				default:
					_jumpMode = NoJump;
					_balance = false;
					_modeNumber = Idle;
					break;
				}


			}
			else{
				_balancingMode = EdgeBalancing;

				// Edge
				switch ( _modeNumber )
				{
				case Idle :
					break;
				case ButtonMode1 :
					break;
				case ButtonMode2 :
					break;
				case Get2E :
					break;
				case Get2C :
					break;
				case GetDown :
					// the fall is handled in the controller
					break;
				default :
					_balance = false;
					_modeNumber = Idle;
					break;
				}
			}
		}


		//When the Cubli has been put back on its face, re-run above part of Mode management to set jump modes according to the _modeNumber.
		if(oldBalancingMode!=_balancingMode && _balancingMode==NotBalancing){

			//wait until Cubli has been put on XY-face
			while(fabs(_xyzAngles.X)>0.5 || fabs(_xyzAngles.Y)>0.5 || fabs(_xyzAngles.Z)>0.5){
				DelayUntilTime(&xLastExecutionTime, ModeManagementT);
			}

			reRunModeManagement = true;

		}
		oldBalancingMode = _balancingMode;
	}

}

#define ComManagementT 100
void ComManagement(void *pvParameters)
{

    // Description
    //
    // Catches and interprets data sent through the UART serial port,
    // Responds and updates the relevant tasks.


    static com_state comState = COM_STATE_CONNECTING;

    // state-specific timers and counters
    static int countdown;
    static int droppedCount = 0;
    static int waitingForRecoveryAcknowledgement = 0;
    static int waitingForFailureAcknowledgement = 0;
	static int waitingForOtherFailureAcknowledgement = 0;
    static int resumeIn;

    //TODO: this is just a useful mark

	portTickType xLastExecutionTime = xTaskGetTickCount();

	for(;;){

		DelayUntilTime(&xLastExecutionTime, ComManagementT);


        char buf[RX_SIZE] = "";
        size_t chars_read = 0;
        com_id sender;
        com_mcode message_code = COM_MCODE_NOTHING;
        char extra_content[COM_P_MAX_EXTRA_CONTENT_SIZE] = {0};
        size_t extra_content_len = 0;
        
        // if there is space read onto buffer
        EnC();
		uart_dirtyread(buf, RX_SIZE);
        ExC();

        // find if any message is on there
        if( buf[0] )
        {
            chars_read = decrypt_message(buf, RX_SIZE,
                                         &message_code, &sender,
                                         extra_content, &extra_content_len );
        }

        if(!chars_read) { }
        else
        {
#ifndef CHOREOGRAPHY_SILENT
        printf("%i-state:%i|read:%i|sender:%i|code:%i|content:%i:%s\r\n",
               _cubliParameters.this_cube_id,comState,chars_read,sender,
               message_code,extra_content_len,extra_content);
#endif
        }
        // if ( message_code == COM_MCODE_NOMESSAGE ) { LEDOn(LED3); }

        // Communication state machine
        switch( comState )
        {

        case COM_STATE_CONNECTING :
        // A.K.A cube is just started
            switch(message_code)
            {

            case COM_MCODE_NOTHING :
                droppedCount += 5;
                send_message(COM_MCODE_CONNECT, COM_ID_COMPUTER,
                             NULL, 0 );
                break;
            case COM_MCODE_CONNECT :
                send_message(COM_MCODE_RECEIVED, COM_ID_COMPUTER,
                             NULL, 0 );
                comState = COM_STATE_IDLE;
                break;

            // garbled message or recipient is not me: stay quiet
            case COM_MCODE_NOMESSAGE:
                // i.e. stop after 10 message met with silence,
                //      or 100 met with undecypherable answers
                droppedCount++;
                break;
            default :
                break;
            }
            // End of state actions
            if ( droppedCount >= 100 )
            {
                comState = COM_STATE_IDLE;
            }

            // reset variables when leaving state
            if( comState != COM_STATE_CONNECTING )
            {
                droppedCount = 0;
            }
            break; // COM_STATE_CONNECTING

        case COM_STATE_IDLE :
            switch(message_code)
            {

            case COM_MCODE_CHECK   :
                comState = COM_STATE_CONNECTING;
                break;

            case COM_MCODE_CONNECT :
                // In case the computer did not receive the response
                send_message(COM_MCODE_RECEIVED, COM_ID_COMPUTER,
                             NULL, 0 );
                break;

            case COM_MCODE_BGN_SND_TMLN :
                if ( extra_content_len != 1 ) {
                    printf("error:wrongsize"); break; }
                if ( !initialize_timeline( extra_content[0] ) ) {
                    printf("error:notinitialized"); break; }
                send_message(COM_MCODE_RECEIVED, COM_ID_COMPUTER,
                             NULL, 0 );
                comState = COM_STATE_TRANSFER_TIMELINE;
                break;

            case COM_MCODE_END_SND_TMLN :
            case COM_MCODE_ABORT_SND_TMLN :
                // in case the computer didn't get the last one
                send_message(COM_MCODE_RECEIVED, COM_ID_COMPUTER,
                             NULL, 0 );
                break;

            case COM_MCODE_COUNTDOWN :
                countdown =  COM_P_COUNTDOWN_DELAY * extra_content[0];
                send_message(COM_MCODE_GOT_COUNT, COM_ID_COMPUTER,
                             NULL, 0 );
                comState = COM_STATE_COUNTDOWN;
                break;

            case COM_MCODE_STOP :
                send_message( COM_MCODE_STOP, COM_ID_COMPUTER, NULL, 0);
                break;

            case COM_MCODE_NOMESSAGE :
            case COM_MCODE_NOTHING :
            default :
                break;
            }
            break; // COM_STATE_IDLE

        case COM_STATE_TRANSFER_TIMELINE :
            switch(message_code)
            {

            case COM_MCODE_NOTHING :
            case COM_MCODE_NOMESSAGE :
                send_message(COM_MCODE_WAITING, COM_ID_COMPUTER,
                             NULL, 0 );
                break;

            case COM_MCODE_BGN_SND_TMLN :
                // In case the computer did not receive the response
                // WARNING: this code is duplaicated in the IDLE state
                if ( extra_content_len != 1 ) {
                    printf("error:wrongsize"); break; }
                if ( !initialize_timeline( extra_content[0] ) ) {
                    printf("error:notinitialized"); break; }
                send_message(COM_MCODE_RECEIVED, COM_ID_COMPUTER,
                             NULL, 0 );
                break;

            case COM_MCODE_SND_TMLN_PART :
                if ( extra_content_len != COM_P_TIMELINE_PART_SIZE ) { 
                    printf("error:wrongsize"); break; }
                if ( !set_timeline_part( extra_content ) ) {
                    printf("error:wrongpart"); break; }
                send_message(COM_MCODE_RECEIVED_PART, COM_ID_COMPUTER,
                             &(extra_content[0]), 1 );
                break;

            case COM_MCODE_END_SND_TMLN :
                if ( !apply_timeline() ) {
                    printf("error:notapplied"); break; }
                send_message(COM_MCODE_RECEIVED, COM_ID_COMPUTER,
                             NULL, 0 );
                comState = COM_STATE_IDLE;
                break;

            case COM_MCODE_ABORT_SND_TMLN :
                send_message(COM_MCODE_RECEIVED, COM_ID_COMPUTER,
                             NULL, 0 );
                comState = COM_STATE_IDLE;
                break;

            default :
                break;

            }
            break; // COM_STATE_TRANSFER_TIMELINE

        case COM_STATE_COUNTDOWN :
            countdown -= ComManagementT;
            if ( countdown <= 0 )
            {
                //send_message( COM_MCODE_STARTED, COM_ID_COMPUTER, NULL, 0);
                countdown = 0;
                start_choreography();
                comState = COM_STATE_PERFORMING;
            }
            switch ( message_code )
            {

            case COM_MCODE_STOP :
            	send_message( COM_MCODE_STOP, COM_ID_ALL, NULL, 0);
            	comState = COM_STATE_IDLE;
            	break;

            case COM_MCODE_COUNTDOWN :
            	send_message( COM_MCODE_GOT_COUNT, COM_ID_COMPUTER, NULL, 0);
            	break;

            default :
            	break;
            }
            break; // COM_STATE_COUNTDOWN

        case COM_STATE_PERFORMING :
            switch ( message_code )
			{

            case COM_MCODE_COUNTDOWN :
            case COM_MCODE_CHECK :
            case COM_MCODE_STOP  :
                stop_choreography();
            	send_message( COM_MCODE_STOP, COM_ID_ALL, NULL, 0);
            	comState = COM_STATE_IDLE;
            	break;


            case COM_MCODE_NOMESSAGE :
            	// treat garbled messages as other cubes failing
            case COM_MCODE_FAILED :
            	if( droppedCount > 0 ) { droppedCount--; }
            	// ( only when not waiting to resume )
            	if ( resumeIn == 0 )
            	{
            		react_to_other_failure('1');
            	}
            	break;

            case COM_MCODE_RESUME :
            	// Stop expecting acknowledgements
            	waitingForFailureAcknowledgement = 0;
            	waitingForOtherFailureAcknowledgement = 0;
            	waitingForRecoveryAcknowledgement = 0;
            	droppedCount = 0;
            	if ( resumeIn == 0 )
            	{
            		// ensure that Cubli is paused before resuming
            		react_to_other_failure('1');
            	}
            	send_message( COM_MCODE_RESUME, COM_ID_COMPUTER, NULL, 0);
            	resumeIn = COM_P_COUNTDOWN_DELAY * extra_content[0];
            	break;

            case COM_MCODE_ACKNOWLEDGE :
            	if ( sender != COM_ID_COMPUTER ) { break; }
            	waitingForFailureAcknowledgement = 0;
            	waitingForOtherFailureAcknowledgement = 0;
            	droppedCount = 0;
            	break;

            case COM_MCODE_ACKNOWLEDGE_REC :
            	if ( sender != COM_ID_COMPUTER ) { break; }
            	waitingForRecoveryAcknowledgement = 0;
            	droppedCount = 0;
            	break;

            default :
            	break;

			}
            if ( announce_failure('?') )
            {
            	send_message( COM_MCODE_FAILED, COM_ID_ALL, NULL, 0 );
            	waitingForFailureAcknowledgement = 1000;
            }
            if ( announce_other_failure('?') )
            {
            	send_message( COM_MCODE_ACCUSE, COM_ID_COMPUTER, NULL, 0);
            	waitingForOtherFailureAcknowledgement = 1000;
            }
            if ( announce_recovery('?') )
            {
            	// A recovered cube is no longer failed
            	waitingForFailureAcknowledgement = 0;
            	// Announce recovery
            	send_message( COM_MCODE_RECOVERED, COM_ID_COMPUTER, NULL, 0);
            	waitingForRecoveryAcknowledgement = 1000;
            }

            // Countdown timer for failure acknowledgement
            if ( waitingForFailureAcknowledgement )
            {
            	waitingForFailureAcknowledgement -= ComManagementT;
            	if ( waitingForFailureAcknowledgement <= 0 )
            	{
            		waitingForFailureAcknowledgement = 0;

            		// Resend Failure message
            		announce_failure( 1 );
            		droppedCount+=2 ;
            	}
            }

            // Countdown timer for other cube failure acknowledgement
            if ( waitingForOtherFailureAcknowledgement )
            {
            	waitingForOtherFailureAcknowledgement -= ComManagementT;
            	if ( waitingForOtherFailureAcknowledgement <= 0 )
            	{
            		waitingForOtherFailureAcknowledgement = 0;

            		// Resend Failure message
            		announce_other_failure( 1 );
            		droppedCount+=2 ;
            	}
            }

            // Countdown timer for recovery messages
            if ( waitingForRecoveryAcknowledgement )
            {
            	waitingForRecoveryAcknowledgement -= ComManagementT;
            	if ( waitingForRecoveryAcknowledgement <= 0 )
            	{
            		waitingForRecoveryAcknowledgement = 0;

            		// Resend recovery message
            		announce_recovery( 1 );
            		droppedCount+= 2 ;
            	}
            }

            // Countdown timer for resume sequence
            if ( resumeIn )
            {
            	resumeIn -= ComManagementT;
            	if ( resumeIn <= 0 )
            	{
            		// Resume choreography
            		react_to_resume('1');
            		resumeIn = 0;
#ifndef CHOREOGRAPHY_SILENT
            		printf("Resumed\r\n");
#endif
            	}
            }

            // If the computer has not responded for a while abort the performance
            if ( droppedCount > 20 )
            {
            	stop_choreography();
            	send_message( COM_MCODE_STOP , COM_ID_COMPUTER, NULL, 0 );
            }

            // if the performance has ended go to idle state
            if ( !run_choreography_management('?') )
            {
            	comState = COM_STATE_IDLE;
            }

            // when exiting this state, reset the variables
            if ( comState != COM_STATE_PERFORMING )
            {
            	waitingForRecoveryAcknowledgement = 0;
                waitingForFailureAcknowledgement = 0;
                waitingForOtherFailureAcknowledgement = 0;
                resumeIn = 0;
                droppedCount = 0;
            }

            break; // COM_STATE_PERFORMING

            default :
            	comState = COM_STATE_IDLE;
            break;
        }

        if ( message_code == COM_MCODE_DEBUG )
        {
            // printf/execute all the stuff you want here
        	printf("_modeNumber:%i|_jumpMode:%i|_balancingMode:%i |_moveSuccess:%i\r\n",
        			_modeNumber,   _jumpMode,   _balancingMode,    _moveSuccess        );
        }

        // Erase the message buffer to avoid reading the same one again
        EnC(); memset(buf, 0, RX_SIZE * sizeof(char)); ExC();


	}

}


#define BrakeTestTaskT 20
void BrakeTestTask(void *pvParameters)
{
	/*
	Description: Task to measure breaking profile of a wheel.

	Run:
	-CAN Task @ 500Hz (#define CANTaskT 2)

	*/

	Delay(1000);

	float testVelocity = -500;

	uint8_t motorIndex = MOTORZ;
	uint8_t servoIndex = SERVOZ;
	_jumpMode = F2EJump;

	//Inputs and measurements
	float omegaXMeas,omegaXMeas1=0,omegaXMeas2=0, omegaFiltered, current, accCurrent, staticCurrent;
	float u_k=0;
	float u_km1 = 0;

	float e_k;
	float e_kf = 0;
	float alpha = 0.02;

	portTickType xLastExecutionTime = xTaskGetTickCount();

	if(testVelocity<0){
		accCurrent = -2.0;
		staticCurrent = 1./1000*(testVelocity/2 - 63);
	}
	else{
		accCurrent = 2.0;
		staticCurrent = 1./1000*(testVelocity/2 + 63);
	}

	int16_t currentmA = 0;
	int constantVelocityCounter = 0;

	bool speedUp = true;

	for(;;){
		DelayUntilTime(&xLastExecutionTime, BrakeTestTaskT);

		if(_jumpMode == F2EJump) {

			u_k=0;
			u_km1 = 0;

			e_kf = 0;

			currentmA = 0;
			constantVelocityCounter = 0;

			speedUp = true;
			while(speedUp)
			{
				DelayUntilTime(&xLastExecutionTime, BrakeTestTaskT);
				omegaXMeas = -_motor[motorIndex].Velocity;

				e_k= -omegaXMeas + testVelocity;

				current = accCurrent;
				currentmA = (int16_t)(-1000 * current); // negative is due to legacy code

				//limiting current
				if(currentmA > 2000)
					currentmA = 2000;
				if(currentmA < -2000)
					currentmA = -2000;

				SetCurrent(_motor[motorIndex].NodeID, currentmA);

				if (fabs(e_k)<20){
					accCurrent = 1.5*staticCurrent;
				}

				e_k = -1/3.0*(omegaXMeas+omegaXMeas1+omegaXMeas2) + testVelocity;

				if (fabs(e_k)<1){
					omegaFiltered = testVelocity;
					speedUp = false;
				}

				omegaXMeas2 = omegaXMeas1;
				omegaXMeas1 = omegaXMeas;
			}

			bool runMotor=true;
			while(runMotor)
			{
				DelayUntilTime(&xLastExecutionTime, BrakeTestTaskT);
				omegaXMeas = -_motor[motorIndex].Velocity;

				e_k= -omegaXMeas + testVelocity;

				omegaFiltered = alpha*omegaXMeas + (1-alpha)*omegaFiltered;
				e_kf = -omegaFiltered + testVelocity;

				//integrator
				u_k=(float)0.02*e_k+(float)u_km1;
				u_km1=u_k;

				current=(float)0.05*e_k+(float)0.03*u_k+staticCurrent;

				if(current > 2.0)
						current = 2.0;
					if(current < -2.0)
						current = -2.0;

				currentmA = (int16_t)(-1000 * current); // negative is due to legacy code

				if(e_kf>-0.15f && e_kf<0.15f){
					constantVelocityCounter++;
				}
				else {
					constantVelocityCounter=0;
				}

				if(constantVelocityCounter>50){
					runMotor=false;
					currentmA = 0;
					ServoSetPulseWidth(&(_cubliParameters._servo[servoIndex].Servo), ( uint32_t)_cubliParameters._servo[servoIndex].ClosedValue);
				}

				//limiting current
				if(currentmA > 2000)
					currentmA = 2000;
				if(currentmA < -2000)
					currentmA = -2000;

				SetCurrent(_motor[motorIndex].NodeID, currentmA);
			}

			while(fabs(omegaXMeas)>350){
				omegaXMeas = -_motor[motorIndex].Velocity;
#ifndef CONTROLLER_SILENT
				printf("\n\r%f",(float)omegaXMeas);
#endif
				DelayUntilTime(&xLastExecutionTime, 2);
			}

			//open servo
			ServoSetPulseWidth(&(_cubliParameters._servo[servoIndex].Servo), ( uint32_t)_cubliParameters._servo[servoIndex].OpenValue);

			while(fabs(omegaXMeas)>0.5){
				omegaXMeas = -_motor[motorIndex].Velocity;
#ifndef CONTROLLER_SILENT
				printf("\n\r%f",(float)omegaXMeas);
#endif
				DelayUntilTime(&xLastExecutionTime, 2);
			}

			_jumpMode = NoJump;
		}
	}
}


#define HeartBeatT 500
void HeartBeat(void *pvParameters)
{

	//Description:
	/*
	LED blinking once per second, indicating that the software is running on the Cubli.
	If the LED is not blinking, the program crashed, in most cases during intialization (if an IMU is not connected, for instance)

	*/

	portTickType xLastExecutionTime = xTaskGetTickCount();
	for(;;){
		DelayUntilTime(&xLastExecutionTime, HeartBeatT);
		LEDToggle(LED1);

	}
}


#define ServoTestTaskT 20
void ServoTestTask(void *pvParameters)
{

	//Description:
	/*
	Servo Test Task: Closes gently one servo while printing out the position.

	*/

	uint8_t servo_idx=SERVOY;
	uint32_t pos=_cubliParameters._servo[servo_idx].OpenValue;

	portTickType xLastExecutionTime = xTaskGetTickCount();
	for(;;){
		DelayUntilTime(&xLastExecutionTime, ServoTestTaskT);

		if(pos>_cubliParameters._servo[servo_idx].ClosedValue){
		//if(pos>300){
			ServoSetPulseWidth(&(_cubliParameters._servo[servo_idx].Servo), pos);
#ifndef CONTROLLER_SILENT
			printf("Servo:%i\t Position:%i\n\r",(int)servo_idx,(int)pos);
#endif
			pos-=10;
			Delay(500);
		}else{
			ServoSetPulseWidth(&(_cubliParameters._servo[servo_idx].Servo), _cubliParameters._servo[servo_idx].OpenValue);
			servo_idx++;
			if(servo_idx>2) servo_idx=0;
			Delay(3000);
			pos=_cubliParameters._servo[servo_idx].OpenValue;
		}


	}
}


#define ServoTestTask3T 20
void ServoTestTask3(void *pvParameters)
{

	//Description:
	/*
	Servo Test Task: Closes all servos while printing out the position.

	*/

	uint32_t pos=0;

	uint8_t brakeQueue[3];
	brakeQueue[0]=_cubliParameters.brakeQueue[0];
	brakeQueue[1]=_cubliParameters.brakeQueue[1];
	brakeQueue[2]=_cubliParameters.brakeQueue[2];

	uint8_t brakeDelay[2];
	brakeDelay[0]=_cubliParameters.brakeDelay[0];
	brakeDelay[1]=_cubliParameters.brakeDelay[1];


	portTickType xLastExecutionTime = xTaskGetTickCount();
	for(;;){
		DelayUntilTime(&xLastExecutionTime, ServoTestTask3T);

		uint8_t i=0;

		/*
		for(i=0;i<3;i++){
			ServoSetPulseWidth(&(_cubliParameters._servo[i].Servo), _cubliParameters._servo[i].ClosedValue-pos);
#ifndef CONTROLLER_SILENT
			printf("Servo:%i, Pos:%i\n\r",i,_cubliParameters._servo[i].ClosedValue-pos);
#endif
		}*/

		EnC();
		ServoSetPulseWidth(&(_cubliParameters._servoF2C[brakeQueue[0]].Servo), ( uint32_t)_cubliParameters._servoF2C[brakeQueue[0]].ClosedValue);
		//ExC();
		//if(brakeDelay[0]>0){Delay(brakeDelay[0]);}
		//EnC();
		ServoSetPulseWidth(&(_cubliParameters._servoF2C[brakeQueue[1]].Servo), ( uint32_t)_cubliParameters._servoF2C[brakeQueue[1]].ClosedValue);
		//ExC();
		//if(brakeDelay[1]>0){Delay(brakeDelay[1]);}
		//EnC();
		ServoSetPulseWidth(&(_cubliParameters._servoF2C[brakeQueue[2]].Servo), ( uint32_t)_cubliParameters._servoF2C[brakeQueue[2]].ClosedValue);
		ExC();

		Delay(1000);

		for(i=0;i<3;i++){
			ServoSetPulseWidth(&(_cubliParameters._servo[i].Servo), _cubliParameters._servo[i].OpenValue);
#ifndef CONTROLLER_SILENT
			//printf("Servo:%i, Pos:%i\n\r",i,_cubliParameters._servo[i].ClosedValue);
#endif
		}

		Delay(1000);
		pos=pos+10;

	}
}



#define FilterManagementT 10
void FilterManagement(void *pvParameters)
{

	//Description:

	/*
	Whenever the Cubli is spinning up the wheels for a jump, the accelerometer measures peaks due to vibrations.
	In these cases, the accelerometer measurement weight should be reduced in updating the _final.est.

	When the breakes are opened (_jumpMode -> NoJump), the accelerometers usually keep oscillating for about 100ms.
	Therefore, we wait 100ms before we increase the accelerometer update weight.

	*/

	int8_t oldJumpMode = NoJump;

	portTickType xLastExecutionTime = xTaskGetTickCount();

	for(;;){
		DelayUntilTime(&xLastExecutionTime, ModeManagementT);

		if(oldJumpMode!=_jumpMode){

			oldJumpMode=_jumpMode;

			switch(_jumpMode){
				case NoJump:
					DelayUntilTime(&xLastExecutionTime, 100);
					EnC();_weightAccMeas = 0.02;ExC(); //0.02  //problems to catch for 0.05...
				break;
				default:
					EnC();_weightAccMeas = 0.001;ExC();//0.001
				break;
				};
		}
	}
}



#define WheelStopT 20
void WheelStop(void *pvParameters)
{

	//Description

	/*
	 This task smoothly stops the wheels whenever the Cube is lying on a face.
	 (This task should always be enabled; otherwise, a balancing task may set a current before exiting,
	 and then the motor controller will permanently produce this current).
	 */

	Delay(5000);
	portTickType xLastExecutionTime = xTaskGetTickCount();
	int8_t i;

	for(;;){
		DelayUntilTime(&xLastExecutionTime, WheelStopT);

		if(_balancingMode == NotBalancing && _jumpMode == NoJump){

			for(i = 0; i < NUM_MOTORS; i++){
				if( fabs(_motor[i].Velocity)>10 ){
					SetCurrent(_motor[i].NodeID, (int16_t)(-25*_motor[i].Velocity));
				}
				else if( fabs(_motor[i].Velocity)>1 ){
					SetCurrent(_motor[i].NodeID, 0);
				}
			}
		}
	}

}



#define SpeedUpWheelsT 20
void SpeedUpWheels(void *pvParameters)
{

	// index
	uint8_t i=0;

	const uint8_t motorIndex[3]={MOTORX, MOTORY, MOTORZ};

	// controller parameters
	const float kp=0.0046;//0.0044;
	const float ki=0.0197;

	// omega wheel
	float ww[3]={0,0,0};

	// torque variable
	float T=0;

	// error: ww_des[k]-ww[k]
	float ek[3]={0,0,0};
	// history of error: ww_des[k-1]-ww[k-1]
	float ekm1[3]={0,0,0};
	// history of input
	float ukm1[3]={0,0,0};

	// reached value conter
	uint8_t constant_vel_counter=0;


	// static torque for acceleration phase
	const float static_torque=.08;	// in Nm
	float static_torque_sign[3];

	// temporary variables
	float du,u;

	/*// test speedup...
	_speedup[0]=true;
	_ww_des[0]=250;
	_reached_velocity[0]=false;

	_speedup[1]=true;
	_ww_des[1]=-300;*/

	// wait a bit
	Delay(5000);
	portTickType xLastExecutionTime = xTaskGetTickCount();

	for(;;){
		DelayUntilTime(&xLastExecutionTime, SpeedUpWheelsT);

		// iterate over all motors
		for(i=0;i<NUM_MOTORS;i++){

			// speed up if requested
			if(_speedup[i]){

				EnC();
				ww[i]=-_motor[motorIndex[i]].Velocity;
				ExC();

				// print velocity out.
				/*
#ifndef CONTROLLER_SILENT
				printf("%f\t",ww[i]);
				if(i==2 || _speedup[2]==false)
					printf("\n\r");
#endif
				*/

				// ek[k]=ww_des[k]-ww[k]
				ek[i]=_ww_des[i]-ww[i];

				// activate controller
				if(fabs(ww[i])>fabs(_ww_des[i])-5.0){
#ifndef CONTROLLER_SILENT
					//printf("controller mode.\n\r");
#endif

					// discretized PID control
					du=kp*(ek[i]-ekm1[i])+ki*(SpeedUpWheelsT/1000.0)*ek[i];
					u=ukm1[i]+du;
					T=u+static_torque_sign[i];

#ifndef CONTROLLER_SILENT
					//printf("ek, ekm1, du, T: %f, %f, %f, %f",ek[i],ekm1[i],du,T);
#endif

					// apply torque
					SetTorqueWSaturation(i,&T);

					// in case of saturation apply anti integrator-windup
					if(fabs(T)<fabs(u+static_torque_sign[i]))
						u-=du;

					if(fabs(.5*(ekm1[i]+ek[i]))<3.5){	// 3.5 rad/s accuracy
						constant_vel_counter++;
						if(constant_vel_counter>100)
							_reached_velocity[i]=true;
#ifndef CONTROLLER_SILENT
						//printf("wheel: %i, reached velocity\n\r",i);
#endif
					}else{
						constant_vel_counter=0;
					}

					ukm1[i]=u;
					ekm1[i]=ek[i];
				}else{
					constant_vel_counter=0;
#ifndef CONTROLLER_SILENT
					//printf("acceleration mode.\n\r");
#endif

					// account for different sign
					if(_ww_des[i]>0)
						static_torque_sign[i]=static_torque;
					else
						static_torque_sign[i]=-static_torque;

					// apply torque
					SetTorqueWSaturation(i,&static_torque_sign[i]);
				}
			}
		}
	}
}

#define Track1DJumpT 10
void Track1DJump(void *pvParameters)
{

	// state variables
	float phi=0, phi_dot=0, psi_dot=0;
    float y = 0, y_dot = 0, y_dotdot=0; // states of the Feedback linearized system
    float y_des=0, y_dot_des=0, y_dotdot_des=0, y_dotdotdot_des=0; //states of the Feedback linearized system
    float phi_dot_des=0;	// desired angular velocity
    float w=0; //calculated Control input in y_FdBl system
    float T=0; //calculated Motor torque
    const float g=9.81;

    float H_dot=0;	// time derivative of energy
    float H_dotprev=0;	// time derivative of energy -> trapez integration
    float dH=0;	// total amount of energy added to the system due to input torques.

    // controller gains
    //const float K_c[3] = {656.2800, 381.2331, 48.1890};
    //const float K_c[3]={359.0 , 171.4, 26.1};
    const float K_c[3]={301.9695, 155.7343,  24.5937};
    //const float K_c[3]={714, 272, 36};

    // keep track of angle
    float prevAngle=0;


    // get edge equilibrium angle from flash...
	float edgeAngle=0;
	if(HasFlashBeenErased()){
		 edgeAngle = _cubliParameters.edgeEqAngle;
	}else{
		ReadEdgeAngle(&edgeAngle);	// read from flash memory
#ifndef CONTROLLER_SILENT
		printf("Track1D: read edgeAngle from flash.\n\r");
#endif
	}
#ifndef CONTROLLER_SILENT
	printf("Track1D:eAngle: %f\n\r",edgeAngle);
#endif

    const float maxAngle = 30.0*PI/180.0;

    const uint8_t Axis=1;
    const uint8_t MotorIndex = MOTORY;

    float Theta_ht=_cubliParameters.thetaH[3];
    float Theta_w=_cubliParameters.thetaW[Axis];

    float m=sqrt(pow(_cubliParameters.m_K.X,2)+pow(_cubliParameters.m_K.Z,2));

	portTickType xLastExecutionTime = xTaskGetTickCount();


	for(;;){
		//_balancingMode=Track1D;
		DelayUntilTime(&xLastExecutionTime, Track1DJumpT);

		EnC();
		phi = _edgeAngles.Y - edgeAngle;
		ExC();

		if( (_balancingMode == Track1D) && (fabs(phi)<maxAngle)){
			EnC();
			phi_dot = _omega.Y;
			psi_dot =-_motor[MotorIndex].Velocity;
			ExC();


			//reached highest point
			if(fabs(phi)>(fabs(prevAngle)+1.0/180.0*PI)||fabs(phi_dot)<1.0/180.0*PI||fabs(phi)<1.5/180.0*PI){
#ifndef CONTROLLER_SILENT
				printf("phi_t,phi_dott: %f, %f\n\r",phi,phi_dot);
#endif
				// evaluate energy
				_Htop_1D=.5*Theta_ht*phi_dot*phi_dot+m*g*(cos(phi)-1.0)-dH*.05;	// consider only use only 5% of used control energy.

#ifndef CONTROLLER_SILENT
				printf("\n\rH_top_0:%f, dH:%f,_Htop_1D:%f\n\r",.5*Theta_ht*phi_dot*phi_dot+m*g*(cos(phi)-1.0),dH,_Htop_1D);
#endif

				// reset states
				dH=0;
				H_dot=0;
				H_dotprev=0;
				T=0;

				//can start balancing
				if(fabs(phi)<1.5/180.0*PI){
					_balancingMode=EdgeBalancing;
					_balance=true;
					_speedupLQR=false;
#ifndef CONTROLLER_SILENT
					printf("switch to Edge balancing.\n\r");
#endif
				}else{
					_balancingMode = NotBalancing;
					_jumpMode = NoJump;
					_balance = false;
					SetCurrent(_motor[MotorIndex].NodeID, 0);
#ifndef CONTROLLER_SILENT
					printf("Cubli is falling... reseting states.\n\r");
#endif
				}
			}else{
				y = Theta_w*(phi_dot+psi_dot)+Theta_ht*phi_dot;
				y_dot = m*g*sin(phi) ;
				y_dotdot = m*g*cos(phi)*phi_dot;

				// Desired trajectory: (based on constant energy assumption)
				if(phi>0){
					phi_dot_des=-sqrt(-2.0*m*g/Theta_ht*(cos(phi)-1.0));
				}else{
					phi_dot_des=sqrt(-2.0*m*g/Theta_ht*(cos(phi)-1.0));
				}
				y_des = (Theta_ht+Theta_w)*phi_dot_des+Theta_w*psi_dot;
				y_dot_des=m*g*sin(phi);
				y_dotdot_des=m*g*cos(phi)*phi_dot_des;
				y_dotdotdot_des=m*m*g*g/Theta_ht*sin(phi)*(3.0*cos(phi)-2.0);



				// input to feedback-linearized system
				w=y_dotdotdot_des+(K_c[0]*(y_des-y)+K_c[1]*(y_dot_des-y_dot)+K_c[2]*(y_dotdot_des-y_dotdot));

				H_dot=-T*phi_dot;

				// inverse transformation to get the torque
				T = -(w+sin(phi)*pow(phi_dot,2)*m*g)/(m*g*cos(phi))*Theta_ht+m*g*sin(phi);

				// calculate the required current
				SetTorqueWSaturation(MotorIndex, &T);

#ifndef CONTROLLER_SILENT
				printf("%f\t %f\t %f\t %f\t %f\t %f\t %f\t %f\t %f\t %f \t%f\n\r",phi, phi_dot, y, y_dot, y_dotdot, y_des, y_dot_des, y_dotdot_des,y_dotdotdot_des,T,.5*Theta_ht*phi_dot*phi_dot+m*g*(cos(phi)-1.0));
#endif


				dH+=.5*(H_dot+H_dotprev)*(Track1DJumpT/1000.0f);
				H_dotprev=-T*phi_dot;
			}
		}
		prevAngle=phi;
	}
}

void SetTorqueWSaturation(uint8_t MotorIndex,float* T)
{

	//const float km=0.0251;	// motor constant
	const float km=0.0335;
	const float maxTorque=5*km;
	//const float maxTorque=2*km;
	const float maxContinuousTorque=2*km;

	int16_t currentmA=0;

	static uint8_t max_torque_ctr=0;
	static uint8_t relaxation_ctr=0;

	if(MotorIndex==10){		// specified index to reset
			max_torque_ctr=0;
			relaxation_ctr=0;
			return;
	}

	if(max_torque_ctr<4){
		if(*T>maxTorque){
			*T=maxTorque;
			max_torque_ctr++;
		}else if(*T<-maxTorque){
			*T=-maxTorque;
			max_torque_ctr++;
		}
	}else{
		if(*T>maxContinuousTorque){
			*T=maxContinuousTorque;
		}else if(*T<-maxContinuousTorque){
			*T=-maxContinuousTorque;
		}
		relaxation_ctr++;
	}

	if(max_torque_ctr>=4 && relaxation_ctr > 10){
		relaxation_ctr=0;
		max_torque_ctr=0;
	}

	// calculate current
	currentmA=-(int)(1000.0*(*T)/km);
#ifndef CONTROLLER_SILENT
	//printf("currentmA:%i\n\r",currentmA);
#endif
	SetCurrent(_motor[MotorIndex].NodeID, currentmA);
}


void SetCurrentWSaturation(uint8_t MotorIndex,int16_t* currentmA)
{
	const int16_t maxCurrent=5000;
	const int16_t maxContinuousCurrent=2000;

	static uint8_t max_current_ctr=0;
	static uint8_t relaxation_ctr=0;

	if(MotorIndex==10){		// specified index to reset
			max_current_ctr=0;
			relaxation_ctr=0;
			return;
	}


	if(max_current_ctr<4){
		if(*currentmA>maxCurrent){
			*currentmA=maxCurrent;
			max_current_ctr++;
		}else if(*currentmA<-maxCurrent){
			*currentmA=-maxCurrent;
			max_current_ctr++;
		}
	}else{
		if(*currentmA>maxContinuousCurrent){
			*currentmA=maxContinuousCurrent;
		}else if(*currentmA<-maxContinuousCurrent){
			*currentmA=-maxContinuousCurrent;
		}
		relaxation_ctr++;
	}

	if(max_current_ctr>=4 && relaxation_ctr > 10){
		relaxation_ctr=0;
		max_current_ctr=0;
	}
	SetCurrent(_motor[MotorIndex].NodeID, *currentmA);
}



#define Face2EdgeJumpUpLearning2T 20
void Face2EdgeJumpUpLearning2(void *pvParameters)
{

	Delay(2000);

	const uint8_t MotorIndex=MOTORY;
	const uint8_t ServoIndex=SERVOY;
	const uint8_t Axis=1;

	float ww0;
	if(HasFlashBeenErased()){
		ww0 =_cubliParameters.omegaF2E;
	}else{
		GetF2EVelocity(&ww0);
		_cubliParameters.omegaF2E=ww0;
#ifndef CONTROLLER_SILENT
		printf("F2EJ: read values from flash.\n\r");
#endif
	}
#ifndef CONTROLLER_SILENT
	printf("omega_F2E: %f\n\r",ww0);
#endif

	// parameters
	float Theta_w=_cubliParameters.thetaW[Axis];
	float Theta_ht=_cubliParameters.thetaH[3];

	// gradient
	//float df=Theta_ht*pow(Theta_w/(Theta_ht+Theta_w),2)*_cubliParameters.omegaF2E;
	const float df=-0.003010119;

	// correction factor, lambda \in (0,2), 1-> dead beat
	float lambda=_cubliParameters.lambdaF2E;
	float lambda_success=.6;

	//measured angular velocity
	float ww=0;

	bool adjustOmegaF2E=false;

	portTickType xLastExecutionTime = xTaskGetTickCount();

	for(;;){
			DelayUntilTime(&xLastExecutionTime, Face2EdgeJumpUpLearning2T);

			if(_jumpMode==F2EJump){
				_jumpSuccess1D=false;

				// Set initial angular velocity
				EnC();
				_reached_velocity[Axis]=false;
				_ww_des[Axis]=ww0;
				_speedup[Axis]=true;
				ExC();

				// Speed up wheel
				while( !_reached_velocity[Axis] ){

					DelayUntilTime(&xLastExecutionTime, Face2EdgeJumpUpLearning2T);
				}

				vTaskPrioritySet(F2EJumpUpTaskHandle, 	configMAX_PRIORITIES-1);

				// in choreography mode, sync jumps
				const int syncRevTime = 8000;
				if ( _modeNumber == Get2E  )
				{
#ifndef CHOREOGRAPHY_SILENT
					if ( primitive_clock > syncRevTime ) { printf("F2ETook too long to rev\r\n"); }
#endif
					int stalled_clock = syncRevTime;
					while( primitive_clock < syncRevTime )
					{
				        DelayUntilTime(&xLastExecutionTime, Face2EdgeJumpUpLearning2T);
				        stalled_clock -= Face2EdgeJumpUpLearning2T;
				        if ( stalled_clock <= 0 ) { break; }
				        if ( _modeNumber != Get2E ) { break; }
					}
				}

				// Stop speed up zero current
				EnC();
				_speedup[Axis]=false;
				_reached_velocity[Axis]=false;
				SetCurrent(_motor[MotorIndex].NodeID, 0);
				// Close brake
				ServoSetPulseWidth(&(_cubliParameters._servo[ServoIndex].Servo), ( uint32_t)_cubliParameters._servo[ServoIndex].ClosedValue);

				ww=-_motor[MotorIndex].Velocity;
				ExC();

				while(fabs(ww)>200.0){
					ww = -_motor[MotorIndex].Velocity;
					//does not make too much sense to delay shorter than CanTaskT
					DelayUntilTime(&xLastExecutionTime, 10);
				}

				//Open the brake
				ServoSetPulseWidth(&(_cubliParameters._servo[ServoIndex].Servo), ( uint32_t)_cubliParameters._servo[ServoIndex].OpenValue);

				_jumpSuccess1D=false;
				_jumpMode= NoJump;

				SetTorqueWSaturation(10,0);

				// wait a bit
				Delay(60);

				vTaskPrioritySet(F2EJumpUpTaskHandle, 	tskIDLE_PRIORITY + 2);

#ifndef CONTROLLER_SILENT
				printf("ChangeToTrack1D\n\r");
#endif
				_balancingMode=Track1D;
				adjustOmegaF2E = true;

			}
			if(_balancingMode == NotBalancing && adjustOmegaF2E){
					Delay(300);
#ifndef CONTROLLER_SILENT
					printf("df:%f,_Htop_1D:%f\n\r",df,_Htop_1D);
#endif

					if(!_jumpSuccess1D)
						ww0 = ww0 - lambda/df*_Htop_1D;
					else{
						_cubliParameters.omegaF2E=ww0;
						_writeData=true;	// allow to write data in flash.
#ifndef CONTROLLER_SILENT
						printf("successful F2E jump. vel:%f\n\r",ww0);
#endif


						ww0 = ww0 - lambda_success/df*_Htop_1D;
					}


					adjustOmegaF2E = false;

					//EnC();
#ifndef CONTROLLER_SILENT
					printf("\n\rAdjusted F2Evel to: %f\n\r",ww0);
#endif
					//ExC();
			}
	}

}


#define Track3DJumpT 10
void Track3DJump(void *pvParameters)
{
#ifndef CONTROLLER_SILENT
			//printf("\n\rInitializing Feedback Linearization 3D Tracking task\n\r");
#endif

			//Parameters Feedback Linearisation
			//const float K_c[3][7] = {{324.98,0,417.06,0,44.5,0,0},{0,325.0,0,417.06,0,44.5,0},{0,0,0,0,0,0,12}}; // From linearization of Nonlinear Controller
			//const float K_c[3][7] ={{330.774688,0.000000,424.494201,0.000000,45.077469,-0.000000,-0.000000},{0.000000,330.774688,0.000000,424.494201,0.000000,45.077469,-0.000000},{-0.000000,-0.000000,-0.000000,-0.000000,-0.000000,0.000000,12.000000}}; ---> worked reasonably well
			const float K_c[3][7] ={{248.081016,0.000000,239.811667,0.000000,35.064557,-0.000000,-0.000000},{0.000000,248.081016,0.000000,239.811667,0.000000,35.064557,-0.000000},{-0.000000,-0.000000,0.000000,-0.000000,-0.000000,0.000000,7.500000}};

			//const float K_c[3][7] = {{293.2576,0,177.8962,0,25.9960,0,0},{0,293.2576,0,177.8962,0,25.9960,0},{0,0,0,0,0,0,44}}; //Demo
			//const float K_c[3][7] ={{206.734180,0.000000,275.645592,0.000000,40.577469,-0.000000,-0.000000},{0.000000,206.734180,0.000000,275.645592,0.000000,40.577469,-0.000000},{-0.000000,-0.000000,0.000000,-0.000000,-0.000000,0.000000,7.500000}};


			// physical quantities
			Quaternion P_IK;	// Quaternion describing orientation
			Quaternion P_KI;
			Vector3 wh;		// omega_h
			Vector3 wh_I;
			Vector3 ww;		// omega_w
			Vector3 pwh;
			Vector3 pwh_I;

			const float g0 = 9.81;
			Vector3 m_K;
			if(HasFlashBeenErased()){
				 m_K = _cubliParameters.m_K;
			}else{
				ReadmK(&m_K.X,&m_K.Y,&m_K.Z);
			}
			float m_K_length;
			V3Length(&m_K,&m_K_length);
			Vector3 m_I;
			Vector3 mm_K={0.0,0.0,0.0}; // -m_K
			Vector3 g_I={0,0,-9.81};
			Vector3 g_K;

			Vector3 vlin;
			Vector3 vlin_K;

			Vector3 tmp;	// temporary storage
			Vector3 tmp2;

			const float I1=.0455;	// Eigenvalue (2) of Theta_0_ht

			float angle;			// inclination angle
			const float maxAngle=40.0/180.0*PI;


			float theta_dot_des=0;		// desired derivative of inclination angle

			float T[3];	// torque


			// Feedback Linearization variables
			float y[2]={0,0}, ydot[2]={0,0}, ydotdot[2]={0,0};

			//Feedback Linearization Desired variables
			float y_des[2]={0,0}, ydot_des[2]={0,0}, ydotdot_des[2]={0,0}, ydotdotdot_des[2]={0,0};
			float e[7];

			float top_criterion=0;	// pwh' (m cross g).
			float top_criterionPrev=-4;
			float mdg_top=0;
			float dpwhm=0;
			float dH=0;
			float dHh=0;
			float mdwh_top=0;
			float mdwh=0;
			float Tdwhm=0;


			// mechanical quantities to evaluate affect of control input
			float mdpwh_dot=0;
			float mdpwh_dotprev=0;
			float H_dot=0;
			float H_dotprev=0;
			float Hh_dot=0;
			float Hh_dotprev=0;

			Vector3 proj_wh={0,0,0};

			float tmp_scalar=0;

			Delay(1000);

			// store the right jump-direction
			GetDirection3DJump();
#ifndef CONTROLLER_SILENT
			printf("dir: %f %f \n\r",_JumpDirection[0],_JumpDirection[1]);
#endif

			portTickType xLastExecutionTime = xTaskGetTickCount();


			for (;;)
			{
				DelayUntilTime(&xLastExecutionTime, Track3DJumpT);
#ifndef CONTROLLER_SILENT
				//printf("running 3D track\n\r");
#endif

				if(_balancingMode == Track3D)
				{
					//
					//Read out current state of cubli and calculate needed variables
					//EnC();
					P_IK.W = _finalEst.W;
					P_IK.X = _finalEst.X;
					P_IK.Y = _finalEst.Y;
					P_IK.Z = _finalEst.Z;

					wh.X = _omega.X;
					wh.Y = _omega.Y;
					wh.Z = _omega.Z;

					ww.X = -_motor[MOTORX].Velocity;
					ww.Y = -_motor[MOTORY].Velocity;
					ww.Z = -_motor[MOTORZ].Velocity;
					//ExC();

					//derived variables
					pwh.X = _cubliParameters.thetaH[0] * wh.X + _cubliParameters.thetaH[1] * wh.Y + _cubliParameters.thetaH[2] * wh.Z + _cubliParameters.thetaW[0] * (wh.X + ww.X);
					pwh.Y = _cubliParameters.thetaH[1] * wh.X + _cubliParameters.thetaH[3] * wh.Y + _cubliParameters.thetaH[4] * wh.Z + _cubliParameters.thetaW[1] * (wh.Y + ww.Y);
					pwh.Z = _cubliParameters.thetaH[2] * wh.X + _cubliParameters.thetaH[4] * wh.Y + _cubliParameters.thetaH[5] * wh.Z + _cubliParameters.thetaW[2] * (wh.Z + ww.Z);


					// Transfrom variables to inertial frame
					QConjugate(&P_IK,&P_KI);
					QMulV(&P_KI,&wh,&wh_I);
					QMulV(&P_KI,&pwh,&pwh_I);
					QMulV(&P_KI,&m_K,&m_I);


					// get inclination angle
					QMulV(&P_IK,&g_I,&g_K);
					V3Minus(&m_K,&mm_K);
					V3Angle0(&g_K,&mm_K,&angle);

					// calculate the top criterion
					V3Cross(&m_K,&g_K,&tmp);
					V3Dot(&tmp,&pwh,&top_criterion);


					//Is Cubli at the top point? Or already lying on its face?
					if(fabs(angle)< 1.5/180.0*PI || fabs(top_criterion)>fabs(top_criterionPrev)){
						_ready2Adjust=false;

						// was it a good jump --> immediately start balancing Task.
						if(fabs(angle)>5.0/180.0*PI){
#ifndef CONTROLLER_SILENT
							//printf("Cubli is falling, reset\n\r");
#endif
							_balancingMode = NotBalancing;
							_jumpMode = NoJump;
							_balance = false;
							SetCurrent(_motor[0].NodeID, 0);
							SetCurrent(_motor[1].NodeID, 0);
							SetCurrent(_motor[2].NodeID, 0);
						}else{
#ifndef CONTROLLER_SILENT
							//printf("balance!\n\r");
#endif
							_balancingMode = CornerBalancing;
							_jumpMode = NoJump;
							_balance = true;
						}

#ifndef CONTROLLER_SILENT
						//printf("rt\n\r");
#endif
						Delay(10);

						// calculate learning criteria
						V3Dot(&g_K,&pwh,&_pwhg_top);
						_pwhg_top=_pwhg_top/9.81;			// pwh * g_norm
						V3Dot(&m_K,&pwh,&_pwhm_top);
						_pwhm_top=_pwhm_top/m_K_length;		// pwhm * m_norm


						// correct for applied torque
						_pwhm_top-=dpwhm;

						V3Dot(&m_K,&wh,&mdwh_top);			// projection of wh, (wh- m m*wh/||m||^2)
						mdwh_top=mdwh_top/m_K_length/m_K_length;
						V3MulS(&mdwh_top,&m_K,&proj_wh);
						V3Sub(&wh,&proj_wh,&proj_wh);

						// calculate energy
						V3Dot(&m_K,&g_K,&mdg_top);
						_Htop_3D=0.5*wh.X*(_cubliParameters.thetaH[0] * wh.X + _cubliParameters.thetaH[1] * wh.Y + _cubliParameters.thetaH[2] * wh.Z)+
								0.5*wh.Y*(_cubliParameters.thetaH[1] * wh.X + _cubliParameters.thetaH[3] * wh.Y + _cubliParameters.thetaH[4] * wh.Z)+
								0.5*wh.Z*(_cubliParameters.thetaH[2] * wh.X + _cubliParameters.thetaH[4] * wh.Y + _cubliParameters.thetaH[5] * wh.Z)-
								mdg_top-m_K_length*9.81;

						// projected energy measure
						_Hhtop_3D=0.5*proj_wh.X*(_cubliParameters.thetaH[0] * proj_wh.X + _cubliParameters.thetaH[1] * proj_wh.Y + _cubliParameters.thetaH[2] * proj_wh.Z)+
								0.5*proj_wh.Y*(_cubliParameters.thetaH[1] * proj_wh.X + _cubliParameters.thetaH[3] * proj_wh.Y + _cubliParameters.thetaH[4] * proj_wh.Z)+
								0.5*proj_wh.Z*(_cubliParameters.thetaH[2] * proj_wh.X + _cubliParameters.thetaH[4] * proj_wh.Y + _cubliParameters.thetaH[5] * proj_wh.Z)
								-mdg_top-m_K_length*9.81;

						// correct for applied torque
						_Htop_3D-=dH*.5;
						_Hhtop_3D-=dHh*.5;


						// print states at top
#ifndef CONTROLLER_SILENT
						/*printf("pwhg_top: %f\n\r",_pwhg_top);
						printf("pot: %f\n\r",mdg_top-m_K_length*9.81);
						printf("H_top: %f, dH:%f\n\r",_Htop_3D,dH);
						printf("Hh_top: %f\n\r",_Hhtop_3D);*/
#endif

						// reset!
						dH=0;
						dHh=0;
						dpwhm=0;
						T[0]=0;
						T[1]=0;
						T[2]=0;

						mdpwh_dot=0;
						mdpwh_dotprev=0;
						H_dot=0;
						H_dotprev=0;
						Hh_dot=0;
						Hh_dotprev=0;

						top_criterionPrev=-4;

						_ready2Adjust=true;
					}
					else if(fabs(angle) < maxAngle){
						//calculate Fdbl states
						y[0] = pwh_I.X;
						y[1] = pwh_I.Y;


						ydot[0] = -g0*m_I.Y;
						ydot[1] =  g0*m_I.X;


						V3Cross(&wh_I,&m_I,&tmp);

						ydotdot[0] = -g0*tmp.Y;
						ydotdot[1] = g0*tmp.X;


						theta_dot_des=-sqrt(-2*m_K_length/I1*g0*(cos(angle)-1));

						//_JumpDirection[0]=dir_normalized.X;
						//_JumpDirection[1]=dir_normalized.Y;

						y_des[0]=_JumpDirection[0]*theta_dot_des*I1;
						y_des[1]=_JumpDirection[1]*theta_dot_des*I1;

						ydot_des[0]=_JumpDirection[0]*m_K_length*g0*sin(angle);
						ydot_des[1]=_JumpDirection[1]*m_K_length*g0*sin(angle);

						ydotdot_des[0]=_JumpDirection[0]*m_K_length*g0*cos(angle)*theta_dot_des;
						ydotdot_des[1]=_JumpDirection[1]*m_K_length*g0*cos(angle)*theta_dot_des;

						ydotdotdot_des[0]=_JumpDirection[0]*m_K_length*m_K_length*g0*g0/I1*(3.0*cos(angle)-2.0)*sin(angle);
						ydotdotdot_des[1]=_JumpDirection[1]*m_K_length*m_K_length*g0*g0/I1*(3.0*cos(angle)-2.0)*sin(angle);


						//calculate e_Fdbl
						e[0] = -y_des[0] + y[0];
						e[1] = -y_des[1] + y[1];
						e[2] = -ydot_des[0] + ydot[0];
						e[3] = -ydot_des[1] + ydot[1];
						e[4] = -ydotdot_des[0] + ydotdot[0];
						e[5] = -ydotdot_des[1] + ydotdot[1];
						e[6] =  wh_I.Z;

						//caluculate q_Fdbl
						vlin.X = -K_c[0][0]*e[0] -K_c[0][1]*e[1] -K_c[0][2]*e[2] -K_c[0][3]*e[3] -K_c[0][4]*e[4] -K_c[0][5]*e[5] -K_c[0][6]*e[6] + ydotdotdot_des[0];
						vlin.Y = -K_c[1][0]*e[0] -K_c[1][1]*e[1] -K_c[1][2]*e[2] -K_c[1][3]*e[3] -K_c[1][4]*e[4] -K_c[1][5]*e[5] -K_c[1][6]*e[6] + ydotdotdot_des[1];
						vlin.Z = -K_c[2][0]*e[0] -K_c[2][1]*e[1] -K_c[2][2]*e[2] -K_c[2][3]*e[3] -K_c[2][4]*e[4] -K_c[2][5]*e[5] -K_c[2][6]*e[6];


						V3Cross(&wh_I,&m_I,&tmp);
						V3Cross(&wh_I,&tmp,&tmp2);

						vlin.X = 1.0/(g0*m_I.Z)*(vlin.X+vlin.Z*g0*m_I.X+g0*tmp2.Y);
						vlin.Y = 1.0/(g0*m_I.Z)*(vlin.Y+vlin.Z*g0*m_I.Y-g0*tmp2.X);
						//vlin.Z = vlin.Z;

						//calculate T_Fdbl_K

						V3Cross(&m_K,&g_K,&tmp);
						V3Cross(&wh,&pwh,&tmp2);

						// Transform vlin_I back in K-frame
						QMulV(&P_IK,&vlin,&vlin_K);


						// Characterize the effect of the control input.............................................
						V3Dot(&m_K,&wh,&mdwh);
						mdwh=mdwh/m_K_length/m_K_length;
						Tdwhm=mdwh*(T[0]/.0335*0.0251*m_K.X+T[1]/0.0335*0.0251*m_K.Y+T[2]/0.0335*0.0251*m_K.Z);
						H_dot=-(wh.X*T[0]/.0335*.0251+wh.Y*T[1]/.0335*.0251+wh.Z*T[2]/.0335*.0251);
						Hh_dot=H_dot+Tdwhm;
						// -----------------------------------------------------------------------------


						// Calculate torque
						T[0]=-_cubliParameters.thetaH[0]*vlin_K.X-_cubliParameters.thetaH[1]*vlin_K.Y-_cubliParameters.thetaH[2]*vlin_K.Z+tmp.X-tmp2.X;
						T[1]=-_cubliParameters.thetaH[1]*vlin_K.X-_cubliParameters.thetaH[3]*vlin_K.Y-_cubliParameters.thetaH[4]*vlin_K.Z+tmp.Y-tmp2.Y;
						T[2]=-_cubliParameters.thetaH[2]*vlin_K.X-_cubliParameters.thetaH[4]*vlin_K.Y-_cubliParameters.thetaH[5]*vlin_K.Z+tmp.Z-tmp2.Z;

						// do not apply torque.
						//T[0]=0;
						//T[1]=0;
						//T[2]=0;

						SetTorqueWSaturation(0,&T[0]);
						SetTorqueWSaturation(1,&T[1]);
						SetTorqueWSaturation(2,&T[2]);


						// printout tracking performance
#ifndef CONTROLLER_SILENT
						// printf("%f \t%f \t%f \t%f \t%f \t%f \t%f \t%f \t%f \t%f \t%f \t%f \t%f \t%f \t%f \t%f\n\r",angle,y[0],y[1],y_des[0],y_des[1],ydot[0],ydot[1],ydot_des[0],ydot_des[1],ydotdot[0],ydotdot[1],ydotdot_des[0],ydotdot_des[1],T[0],T[1],T[2]);
#endif


						// dpwhm
						V3Cross(&wh,&pwh,&tmp);
						V3Dot(&m_K,&tmp,&tmp_scalar);
						mdpwh_dot=-tmp_scalar/m_K_length;

						//if(n<20){
							// trapez integration of pwhm
							dpwhm+=.5*(mdpwh_dot+mdpwh_dotprev)*.01;


							// trapez integration of H
							dH+=.5*(H_dotprev+H_dot)*0.01;
							dHh+=.5*(Hh_dotprev+Hh_dot)*0.01;

							//n++;
						//}

						Tdwhm=mdwh*(T[0]/.0335*0.0251*m_K.X+T[1]/0.0335*0.0251*m_K.Y+T[2]/0.0335*0.0251*m_K.Z);

						H_dotprev=-(wh.X*T[0]/.0335*.0251+wh.Y*T[1]/.0335*.0251+wh.Z*T[2]/.0335*.0251);
						Hh_dotprev=H_dotprev+Tdwhm;

						mdpwh_dotprev=mdpwh_dot;

						top_criterionPrev=top_criterion;
					}

				}
			}

}




#define E2CJumpUpLearning3T 20
void E2CJumpUpLearning3(void *pvParameters)
{

	Delay(1000);

	portTickType xLastExecutionTime = xTaskGetTickCount();

	float omega_E2C[2];
	if(HasFlashBeenErased()){
		omega_E2C[0] = _cubliParameters.omegaE2C[0];		//-394
		omega_E2C[1] = _cubliParameters.omegaE2C[1];
	}else{
		GetE2CVelocity(omega_E2C);
		_cubliParameters.omegaE2C[0]=omega_E2C[0];
		_cubliParameters.omegaE2C[1]=omega_E2C[1];
#ifndef CONTROLLER_SILENT
		printf("E2CJ: read values form flash.\n\r");
#endif
	}
#ifndef CONTROLLER_SILENT
	printf("omega_E2C: %f, %f\n\r",omega_E2C[0],omega_E2C[1]);
#endif

	bool adjustOmegaE2C=false;	// variable to enable learning after jump.

	int8_t brakeDelay=_cubliParameters.brakeDelayE2C;


	const float lambda=_cubliParameters.lambdaE2C;	// learning parameter
	const float lambda_success=.2;  // learning parameter in case of success

	float dV_inv[4];
	dV_inv[0]=_cubliParameters.dV_invE2C[0];
	dV_inv[1]=_cubliParameters.dV_invE2C[1];
	dV_inv[2]=_cubliParameters.dV_invE2C[2];
	dV_inv[3]=_cubliParameters.dV_invE2C[3];

	const float I1=0.029754829308132;

	float V[2]={0,0};

	float ww[3]={0,0,0};	//wheel velocity.

	Vector3 dww={0,0,0};	// Change in wheel velocity
	uint8_t i=0;

	for(;;){
		DelayUntilTime(&xLastExecutionTime, E2CJumpUpLearning3T);
		if(_jumpMode == E2CJump){
			Delay(500);
			_jumpSuccess3D=false;
			GetDirection3DJump();

			EnC();
			//wheel 1
			_reached_velocity[0]=false;
			_ww_des[0]=omega_E2C[0];
			_speedup[0]=true;

			//balancing wheel
			_speedupLQR=true;
			_reached_velocity[1]=false;

			//wheel 2
			_reached_velocity[2]=false;
			_ww_des[2]=omega_E2C[1];
			_speedup[2]=true;
			ExC();

#ifndef CONTROLLER_SILENT
			printf("3D-Jump-speeding up reaction wheel.\n\r_ww_des: %f, %f, %f\n\r",_ww_des[0],_ww_des[1],_ww_des[2]);
#endif

			// Speed up wheel --- Take care that still in E2CJump - Mode
			while((!_reached_velocity[0] || !_reached_velocity[1] || !_reached_velocity[2]) && _jumpMode==E2CJump){
				DelayUntilTime(&xLastExecutionTime, E2CJumpUpLearning3T);
			}

			// in choreography mode, sync jumps
			const int syncRevTime = 4000;
			if ( _modeNumber == Get2C )
			{
#ifndef CHOREOGRAPHY_SILENT
				if ( primitive_clock > syncRevTime ) { printf("E2C Took too long to rev\r\n"); }
#endif
				int stalled_clock = syncRevTime;
				while( primitive_clock < syncRevTime )
				{
					DelayUntilTime(&xLastExecutionTime, 10);
					stalled_clock -= 10;
					if ( stalled_clock <= 0 ) { break; }
					if ( _modeNumber != Get2C ) { break; }
				}
			}

			// Check that still in E2CJump - Mode - otherwise leave!
			if(_jumpMode ==E2CJump){

				// Stop speed up zero current
				EnC();
				_speedup[0]=false;
				_reached_velocity[0]=false;
				SetCurrent(_motor[0].NodeID, 0);
				_speedup[2]=false;
				_reached_velocity[2]=false;
				SetCurrent(_motor[2].NodeID, 0);
				_speedupLQR=true;
				_reached_velocity[1]=false;
				ExC();



#ifndef CONTROLLER_SILENT
				printf("Close brakes...\n\r"); // Do not brake balancing wheel (wheel number 1)
#endif
				vTaskPrioritySet(E2CJumpUpTaskHandle, 	configMAX_PRIORITIES-1);


				// TODO: Check if this Code is really working properly:
				// --> maybe add Delays while waiting -> facilitates scheduling. -> not a good idea.
				// --> check that it works with various delays.
				// --> check that it works with Cubli 2
				// --> add code to E2CJump!

				// make sure braking is repeatable.
				// wait 0.1s
				while(_cubliParameters._servo[0].Servo.Timer->CNT >= (uint32_t)10){};

				if(brakeDelay>=0){
					EnC();
					ServoSetPulseWidth(&(_cubliParameters._servo[0].Servo), ( uint32_t)_cubliParameters._servo[0].ClosedValue);
					ExC();
					int timidx=0;
					for(timidx=0;timidx<brakeDelay;timidx++){
						while(_cubliParameters._servo[0].Servo.Timer->CNT <= (uint32_t)10){};
						while(_cubliParameters._servo[0].Servo.Timer->CNT >= (uint32_t)10){};
					}
					EnC();
					ServoSetPulseWidth(&(_cubliParameters._servo[2].Servo), ( uint32_t)_cubliParameters._servo[2].ClosedValue);
					ExC();
				}else{
					EnC();
					ServoSetPulseWidth(&(_cubliParameters._servo[2].Servo), ( uint32_t)_cubliParameters._servo[2].ClosedValue);
					ExC();
					int timidx=0;
					for(timidx=0;timidx<-brakeDelay;timidx++){
						while(_cubliParameters._servo[0].Servo.Timer->CNT <= (uint32_t)10){};
						while(_cubliParameters._servo[0].Servo.Timer->CNT >= (uint32_t)10){};
					}
					EnC();
					ServoSetPulseWidth(&(_cubliParameters._servo[0].Servo), ( uint32_t)_cubliParameters._servo[0].ClosedValue);
					ExC();
				}

				/*if(brakeDelay>=0){
					EnC();
					ServoSetPulseWidth(&(_cubliParameters._servo[0].Servo), ( uint32_t)_cubliParameters._servo[0].ClosedValue);
					ExC();
					Delay(brakeDelay);
					EnC();
					ServoSetPulseWidth(&(_cubliParameters._servo[2].Servo), ( uint32_t)_cubliParameters._servo[2].ClosedValue);
					ExC();
				}else{
					EnC();
					ServoSetPulseWidth(&(_cubliParameters._servo[2].Servo), ( uint32_t)_cubliParameters._servo[2].ClosedValue);
					ExC();
					Delay(-brakeDelay);
					EnC();
					ServoSetPulseWidth(&(_cubliParameters._servo[0].Servo), ( uint32_t)_cubliParameters._servo[0].ClosedValue);
					ExC();
				}*/

				for(i=0;i<3;i++){
					ww[i]=-_motor[i].Velocity;
				}
				while(fabs(ww[0])>100.0 || fabs(ww[1])>100.0 || fabs(ww[2])>100.0){
#ifndef CONTROLLER_SILENT
					printf("%f\t%f\t%f\t\n\r",ww[0],ww[1],ww[2]);
#endif
					for(i=0;i<3;i++)
						ww[i]=-_motor[i].Velocity;
					//does not make too much sense to delay shorter than CanTaskT
					DelayUntilTime(&xLastExecutionTime, 10);
				}



				// Release brakes
				ServoSetPulseWidth(&(_cubliParameters._servo[0].Servo), ( uint32_t)_cubliParameters._servo[0].OpenValue);
				ServoSetPulseWidth(&(_cubliParameters._servo[2].Servo), ( uint32_t)_cubliParameters._servo[2].OpenValue);

#ifndef CONTROLLER_SILENT
				printf("breakes released...\n\r");
#endif
				Delay(60);

				// Reset Motor saturation ... use 0 as dummy pointer argument.
				SetTorqueWSaturation(10,0);

				// Set priority back to usual value.
				vTaskPrioritySet(E2CJumpUpTaskHandle, 	tskIDLE_PRIORITY + 2);

				_balancingMode = Track3D;
				_jumpMode = NoJump;
				_balance = true;
				adjustOmegaE2C = true;

#ifndef CONTROLLER_SILENT
				printf("end of E2C Jump task...\n\r");
#endif
			}else{	// leave E2CJump -> stop speeding up wheels
				for(i=0;i<3;i++){
					_ww_des[i]=0;
					_speedup[i]=false;
					SetCurrent(_motor[i].NodeID,0);
				}
			}
		}
		if(_balancingMode == NotBalancing && adjustOmegaE2C && _ready2Adjust){
			Delay(250);	// to check whether the jump succeeded and for nicer print-outs

			// standard deviation is of pwhg is around 0.05 --> use threshold
			V[0]=1/I1*_pwhg_top;
			V[1]=_Hhtop_3D/I1;

#ifndef CONTROLLER_SILENT
			printf("\n\rV: %f, %f, _jumpSuccess3D:%i\n\r",V[0],V[1],_jumpSuccess3D);
#endif

			dww.X=dV_inv[0]*V[0]+dV_inv[1]*V[1];
			dww.Z=dV_inv[2]*V[0]+dV_inv[3]*V[1];

			if(!_jumpSuccess3D){
				omega_E2C[0]-=lambda*dww.X;
				omega_E2C[1]-=lambda*dww.Z;
			}else{
				_cubliParameters.omegaE2C[0]=omega_E2C[0];
				_cubliParameters.omegaE2C[1]=omega_E2C[1];
				_writeData=true;		// allow to write data in flash memory.
#ifndef CONTROLLER_SILENT
				printf("successful E2C jump. set cu pa: %f, %f\n\r",omega_E2C[0],omega_E2C[1]);
#endif
				omega_E2C[0]-=lambda_success*dww.X;
				omega_E2C[1]-=lambda_success*dww.Z;
			}

#ifndef CONTROLLER_SILENT
			printf("omega_E2C: %f\t %f\n\r",omega_E2C[0], omega_E2C[1]);
#endif

			adjustOmegaE2C = false;
		}
	}
}




#define Face2CornerJumpUpLearning2T 20
void Face2CornerJumpUpLearning2(void *pvParameters)
{

		Delay(2000);

		portTickType xLastExecutionTime = xTaskGetTickCount();

		float omega_F2C[3];
		if(HasFlashBeenErased()){
			omega_F2C[0] = _cubliParameters.omegaF2C[0];		//-394
			omega_F2C[1] = _cubliParameters.omegaF2C[1];		//403
			omega_F2C[2] = _cubliParameters.omegaF2C[2];
		}else{
			GetF2CVelocity(omega_F2C);
			_cubliParameters.omegaF2C[0]=omega_F2C[0];
			_cubliParameters.omegaF2C[1]=omega_F2C[1];
			_cubliParameters.omegaF2C[2]=omega_F2C[2];
#ifndef CONTROLLER_SILENT
			printf("F2CJ: read values form flash.\n\r");
#endif
		}

#ifndef CONTROLLER_SILENT
		printf("omega_F2C: %f\t%f\t%f\n\r",omega_F2C[0],omega_F2C[1],omega_F2C[2]);
#endif

		bool adjustOmegaF2C=false;	// variable to enable learning after jump.

		uint8_t brakeDelay[2];
		brakeDelay[0]=_cubliParameters.brakeDelay[0];
		brakeDelay[1]=_cubliParameters.brakeDelay[1];
		uint8_t brakeQueue[3];
		brakeQueue[0]=_cubliParameters.brakeQueue[0];
		brakeQueue[1]=_cubliParameters.brakeQueue[1];
		brakeQueue[2]=_cubliParameters.brakeQueue[2];

		const float lambda=_cubliParameters.lambdaF2C;	// learning parameter 0.6
		const float lambda_success=.1;

		const float I3=0.005881306870547;

		Vector3 V={0,0,0};

		float ww[3]={0,0,0};	//wheel velocity.

		Vector3 dww={0,0,0};	// Change in wheel velocity
		uint8_t i=0;

		for(;;){
			DelayUntilTime(&xLastExecutionTime, Face2CornerJumpUpLearning2T);

			if(_jumpMode == F2CJump){
				Delay(100);
				_jumpSuccess3D=false;
				GetDirection3DJump();

				for(i=0;i<3;i++)
					_reached_velocity[i]=false;

				EnC();
				//wheel 1
				_ww_des[0]=omega_F2C[0];
				_speedup[0]=true;

				//wheel 2
				_ww_des[1]=omega_F2C[1];
				_speedup[1]=true;

				//wheel 2
				_ww_des[2]=omega_F2C[2];
				_speedup[2]=true;
				ExC();

#ifndef CONTROLLER_SILENT
				//printf("3D-Jump-speeding up reaction wheel.\n\r_ww_des: %f, %f, %f\n\r",_ww_des[0],_ww_des[1],_ww_des[2]);
				printf("%f \t %f \t %f\n\r",_ww_des[0],_ww_des[1],_ww_des[2]);
#endif

				// Speed up wheel --- Take care that still in E2CJump - Mode

				while(!_reached_velocity[0] || !_reached_velocity[1] || !_reached_velocity[2]){
					DelayUntilTime(&xLastExecutionTime, Face2CornerJumpUpLearning2T);
				}

				// in choreography mode, sync jumps
				const int syncRevTime = 8000;
				if ( _modeNumber == Get2C )
				{
#ifndef CHOREOGRAPHY_SILENT
					if ( primitive_clock > syncRevTime ) { printf("F2C Took too long to rev\r\n"); }
#endif
					int stalled_clock = syncRevTime;
					while( primitive_clock < syncRevTime )
					{
						DelayUntilTime(&xLastExecutionTime, 10);
						stalled_clock -= 10;
						if ( stalled_clock <= 0 ) { break; }
						if ( _modeNumber != Get2C ) { break; }
					}
				}

#ifndef CONTROLLER_SILENT
				printf("Close brakes...\n\r");
#endif
				vTaskPrioritySet(F2CJumpUpTaskHandle, 	configMAX_PRIORITIES-1);

				// Stop speed up zero current
				for(i=0;i<3;i++){
					EnC();
					_speedup[i]=false;
					_reached_velocity[i]=false;
					SetCurrent(_motor[i].NodeID, 0);
					ExC();
				}

				// wait 0.1s
				while(_cubliParameters._servo[0].Servo.Timer->CNT >= (uint32_t)10){};

				// new code --- brake according to delay
				EnC();
				ServoSetPulseWidth(&(_cubliParameters._servo[_cubliParameters.brakeQueue[0]].Servo), ( uint32_t)_cubliParameters._servo[_cubliParameters.brakeQueue[0]].ClosedValue);
				ExC();

				int timidx=0;
				for(timidx=0;timidx<_cubliParameters.brakeDelay[0];timidx++){
					while(_cubliParameters._servo[0].Servo.Timer->CNT <= (uint32_t)10){};
					while(_cubliParameters._servo[0].Servo.Timer->CNT >= (uint32_t)10){};
				}
				EnC();
				ServoSetPulseWidth(&(_cubliParameters._servo[_cubliParameters.brakeQueue[1]].Servo), ( uint32_t)_cubliParameters._servo[_cubliParameters.brakeQueue[1]].ClosedValue);
				ExC();


				for(timidx=0;timidx<_cubliParameters.brakeDelay[1];timidx++){
					while(_cubliParameters._servo[0].Servo.Timer->CNT <= (uint32_t)10){};
					while(_cubliParameters._servo[0].Servo.Timer->CNT >= (uint32_t)10){};
				}
				EnC();
				ServoSetPulseWidth(&(_cubliParameters._servo[_cubliParameters.brakeQueue[2]].Servo), ( uint32_t)_cubliParameters._servo[_cubliParameters.brakeQueue[2]].ClosedValue);
				ExC();



				// Old code - break at the same time.
				/*EnC();
				ServoSetPulseWidth(&(_cubliParameters._servoF2C[brakeQueue[0]].Servo), ( uint32_t)_cubliParameters._servoF2C[brakeQueue[0]].ClosedValue);
				ServoSetPulseWidth(&(_cubliParameters._servoF2C[brakeQueue[1]].Servo), ( uint32_t)_cubliParameters._servoF2C[brakeQueue[1]].ClosedValue);
				ServoSetPulseWidth(&(_cubliParameters._servoF2C[brakeQueue[2]].Servo), ( uint32_t)_cubliParameters._servoF2C[brakeQueue[2]].ClosedValue);
				ExC();*/


				for(i=0;i<3;i++)
					ww[i]=-_motor[i].Velocity;

				while(fabs(ww[0])>150.0 || fabs(ww[1])>150.0 || fabs(ww[2])>150.0){
#ifndef CONTROLLER_SILENT
					printf("%f\t%f\t%f\t\n\r",ww[0],ww[1],ww[2]);
#endif
					for(i=0;i<3;i++)
						ww[i]=-_motor[i].Velocity;
					//does not make too much sense to delay shorter than CanTaskT
					DelayUntilTime(&xLastExecutionTime, 10);
				}



				// wait 0.1s
				while(_cubliParameters._servo[0].Servo.Timer->CNT >= (uint32_t)100000){};

				// Release brakes
				for(i=0;i<3;i++){
					ServoSetPulseWidth(&(_cubliParameters._servoF2C[i].Servo), ( uint32_t)_cubliParameters._servoF2C[i].OpenValue);
				}

				Delay(60);			// Delay of 60ms turns out to be really good.
				// Reset Motor saturation ... use 0 as dummy pointer argument.
				SetTorqueWSaturation(10,0);
				SetCurrentWSaturation(10,0);

				// Set priority back to usual value.
				vTaskPrioritySet(F2CJumpUpTaskHandle, 	tskIDLE_PRIORITY + 2);

				_balancingMode = Track3D;
				_jumpMode = NoJump;
				_balance=true;

				// adjust omega F2C
				adjustOmegaF2C=true;

#ifndef CONTROLLER_SILENT
				//printf("end of E2C Jump task...\n\r");
#endif
			}
			if(_balancingMode == NotBalancing && adjustOmegaF2C && _ready2Adjust){
				Delay(300);

				V.X=_pwhg_top/I3;
				V.Y=_pwhm_top/I3;
				V.Z=_Htop_3D/I3;

#ifndef CONTROLLER_SILENT
				printf("%f \t %f \t %f\n\r",V.X,V.Y,V.Z);
#endif

				M3MulMV(&_cubliParameters.dV_invF2C,&V,&dww);

				if(!_jumpSuccess3D){
						omega_F2C[0]-=lambda*dww.X;
						omega_F2C[1]-=lambda*dww.Y;
						omega_F2C[2]-=lambda*dww.Z;
				}else{
					_cubliParameters.omegaF2C[0]=omega_F2C[0];
					_cubliParameters.omegaF2C[1]=omega_F2C[1];
					_cubliParameters.omegaF2C[2]=omega_F2C[2];
					_writeData=true;			// allow to write data in flash memory
#ifndef CONTROLLER_SILENT
					printf("successful F2C jump. set %f, %f, %f\n\r",omega_F2C[0],omega_F2C[1],omega_F2C[2]);
#endif
						omega_F2C[0]-=lambda_success*dww.X;
						omega_F2C[1]-=lambda_success*dww.Y;
						omega_F2C[2]-=lambda_success*dww.Z;
				}

				adjustOmegaF2C = false;
			}
		}

}



/* Function for F2C Jump only using two reaction wheels.
#define Face2CornerJumpUpLearning3T 20
void Face2CornerJumpUpLearning3(void *pvParameters)
{

		Delay(2000);

		portTickType xLastExecutionTime = xTaskGetTickCount();

		float omega_F2C[3];
		omega_F2C[0] = _cubliParameters.omegaF2C[0];		//-394
		omega_F2C[1] = _cubliParameters.omegaF2C[1];		//403
		omega_F2C[2] = _cubliParameters.omegaF2C[2];


		bool adjustOmegaF2C=false;	// variable to enable learning after jump.

		uint8_t brakeDelay[2];
		brakeDelay[0]=_cubliParameters.brakeDelay[0];
		brakeDelay[1]=_cubliParameters.brakeDelay[1];
		uint8_t brakeQueue[3];
		brakeQueue[0]=_cubliParameters.brakeQueue[0];
		brakeQueue[1]=_cubliParameters.brakeQueue[1];
		brakeQueue[2]=_cubliParameters.brakeQueue[2];

		float lambda=.8;	// learning parameter 0.8
		float lambda_success=.4;

		const float dV_inv[4]={15.920557, 192.698317, 4.430080, -204.055416};

		const float I3=0.005881306870547;

		float V[2]={0,0};
		//Vector3 V_prev={0,0,0};

		float ww[2]={0,0};	//wheel velocity.

		Vector3 dww={0,0,0};	// Change in wheel velocity
		uint8_t i=0;

		for(;;){
			DelayUntilTime(&xLastExecutionTime, Face2CornerJumpUpLearning3T);
			if(_jumpMode == F2CJump){
				Delay(1000);
				GetDirection3DJump();

				for(i=0;i<2;i++)
					_reached_velocity[i]=false;

				EnC();
				//wheel 1
				_ww_des[0]=omega_F2C[0];
				_speedup[0]=true;

				//wheel 2
				_ww_des[1]=omega_F2C[1];
				_speedup[1]=true;

				ExC();

#ifndef CONTROLLER_SILENT
				//printf("3D-Jump-speeding up reaction wheel.\n\r_ww_des: %f, %f, %f\n\r",_ww_des[0],_ww_des[1],_ww_des[2]);
				printf("%f, %f, %f\n\r",_ww_des[0],_ww_des[1],_ww_des[2]);
#endif

				// Speed up wheel --- Take care that still in E2CJump - Mode

				while(!_reached_velocity[0] || !_reached_velocity[1]){
					DelayUntilTime(&xLastExecutionTime, Face2CornerJumpUpLearning3T);
				}

				// Stop speed up zero current
				for(i=0;i<2;i++){
					EnC();
					_speedup[i]=false;
					_reached_velocity[i]=false;
					SetCurrent(_motor[i].NodeID, 0);
					ExC();
				}

#ifndef CONTROLLER_SILENT
				//printf("Close brakes...\n\r");
#endif
				vTaskPrioritySet(F2CJumpUpTaskHandle, 	configMAX_PRIORITIES-1);
				EnC();
				if(brakeQueue[0]!=2){ ServoSetPulseWidth(&(_cubliParameters._servo[brakeQueue[0]].Servo), ( uint32_t)_cubliParameters._servo[brakeQueue[0]].ClosedValue); }
				ExC();
				if(brakeDelay[0]>0){Delay(brakeDelay[0]);}
				EnC();
				if(brakeQueue[1]!=2){ ServoSetPulseWidth(&(_cubliParameters._servo[brakeQueue[1]].Servo), ( uint32_t)_cubliParameters._servo[brakeQueue[1]].ClosedValue); }
				ExC();
				if(brakeDelay[1]>0){Delay(brakeDelay[1]);}
				EnC();
				if(brakeQueue[2]!=2){ ServoSetPulseWidth(&(_cubliParameters._servo[brakeQueue[2]].Servo), ( uint32_t)_cubliParameters._servo[brakeQueue[2]].ClosedValue); }
				ExC();


				for(i=0;i<2;i++)				// when removing this - Servo Closes and opens again -> missing IMU measurement
					ww[i]=-_motor[i].Velocity;

				while(fabs(ww[0])>200.0 || fabs(ww[1])>200.0){
#ifndef CONTROLLER_SILENT
					printf("%f\t%f\n\r",ww[0],ww[1]);
#endif
					for(i=0;i<2;i++)
						ww[i]=-_motor[i].Velocity;
					//does not make too much sense to delay shorter than CanTaskT
					DelayUntilTime(&xLastExecutionTime, 10);
				}

				// Release brakes
				for(i=0;i<3;i++){
					ServoSetPulseWidth(&(_cubliParameters._servo[i].Servo), ( uint32_t)_cubliParameters._servo[i].OpenValue);
				}

#ifndef CONTROLLER_SILENT
				//printf("breakes released...\n\r");
#endif
				Delay(40);

				// Set priority back to usual value.
				vTaskPrioritySet(F2CJumpUpTaskHandle, 	tskIDLE_PRIORITY + 2);

				_balancingMode = Track3D;
				_jumpMode = NoJump;
				//y_balance = true;

				// adjust omega F2C
				adjustOmegaF2C=true;

#ifndef CONTROLLER_SILENT
				//printf("end of E2C Jump task...\n\r");
#endif
			}
			if(_balancingMode == NotBalancing && adjustOmegaF2C && _ready2Adjust){
				V[0]=_pwhm_top/I3;
				V[1]=_Htop_3D;
				Delay(10);	// for nicer printouts.

#ifndef CONTROLLER_SILENT
				printf("%f, %f\n\r",V[0],V[1]);
#endif

				dww.X=dV_inv[0]*V[0]+dV_inv[1]*V[1];
				dww.Y=dV_inv[2]*V[0]+dV_inv[3]*V[1];

				if(!_jumpSuccess){
						omega_F2C[0]-=lambda*dww.X;
						omega_F2C[1]-=lambda*dww.Y;
						//omega_F2C[2]-=lambda*dww.Z;
				}else{
						omega_F2C[0]-=lambda_success*dww.X;
						omega_F2C[1]-=lambda_success*dww.Y;
						//omega_F2C[2]-=lambda_success*dww.Z;
				}

#ifndef CONTROLLER_SILENT
					printf("\n\rAdjusted omegaF2C to: %f\t %f\t %f\n\r",omega_F2C[0],omega_F2C[1],omega_F2C[2]);
#endif
				adjustOmegaF2C = false;
			}
		}

}*/




// Catching Controller --- use Nonlinear controller

#define CatchingControllerTaskT 10
void CatchingControllerTask(void* parameters)
{
		float angle;
		Quaternion P_IK;
		Quaternion P_KI;

		const float Ts=(float)CatchingControllerTaskT/(float)1000.0; // Sampling Time

		float top_criterion=0;
		float top_criterionPrev=-4;

		float balancingAngle=60.0/180.0*PI;


		// physical quantities
		Vector3 g_K;
		Vector3 K_w_IK;
		Vector3 K_w_w;
		Vector3 wh_I;
		Vector3 m_I;
		Vector3 pwh_I;
		Vector3 p1;
		Vector3 ps;
		Vector3 pw;
		Vector3 proj_wh;
		Vector3 tmp;

		Vector3 mm_K={0.0,0.0,0.0}; // -m_K
		Vector3 g_I = {0,0,-9.81};

		// auxilary variables
		Vector3 a1;
		Vector3 a2;
		Vector3 a3;
		Vector3 a4;
		Vector3 a5;
		Vector3 A_5a5;

		float proj;
		float mdwh_top=0;
		float mdg_top=0;
		float dpwhm=0;
		float dH=0;
		float tmp_scalar=0;
		float T[3]={0,0,0};

		// Physical Matrices
		Vector3 m_K;
		if(HasFlashBeenErased()){
			 m_K = _cubliParameters.m_K;
		}else{
			ReadmK(&m_K.X,&m_K.Y,&m_K.Z);
#ifndef CONTROLLER_SILENT
			printf("CC: Read m_K from flash.\n\r");
#endif
		}
#ifndef CONTROLLER_SILENT
		printf("CC: m_K: %f, %f, %f\n\r",m_K.X,m_K.Y,m_K.Z);
#endif
		float m_K_length=1;
		V3Length(&m_K,&m_K_length);

		float mdpwh_dotprev=0;

		float dHh=0;
		float H_dot=0;
		float H_dotprev=0;
		float Hh_dot=0;
		float mdwh=0;
		float mdpwh_dot=0;
		float Tdwhm=0;
		float Hh_dotprev=0;

		float Knc1 = 15;
		float Knc2 = 18;
		float Knc3 = 7.3;
		float Knc4 = 0.00001;


	    while(!_attitudeReady) Delay(300);

		portTickType xLastExecutionTime = xTaskGetTickCount();

		for (;;)
		{
			DelayUntilTime(&xLastExecutionTime, CatchingControllerTaskT);

			if(_balancingMode == Track3D)
			{
				P_IK.W = _finalEst.W;
				P_IK.X = _finalEst.X;
				P_IK.Y = _finalEst.Y;
				P_IK.Z = _finalEst.Z;

				K_w_IK.X = (float)_omega.X;
				K_w_IK.Y = (float)_omega.Y;
				K_w_IK.Z = (float)_omega.Z;

				K_w_w.X = (float)-_motor[MOTORX].Velocity;
				K_w_w.Y = (float)-_motor[MOTORY].Velocity;
				K_w_w.Z = (float)-_motor[MOTORZ].Velocity;


				pw.X = _cubliParameters.thetaW[0] * (K_w_IK.X + K_w_w.X);
				pw.Y = _cubliParameters.thetaW[1] * (K_w_IK.Y + K_w_w.Y);
				pw.Z = _cubliParameters.thetaW[2] * (K_w_IK.Z + K_w_w.Z);

				p1.X = _cubliParameters.thetaH[0] * K_w_IK.X + _cubliParameters.thetaH[1] * K_w_IK.Y + _cubliParameters.thetaH[2] * K_w_IK.Z + pw.X;
				p1.Y = _cubliParameters.thetaH[1] * K_w_IK.X + _cubliParameters.thetaH[3] * K_w_IK.Y + _cubliParameters.thetaH[4] * K_w_IK.Z + pw.Y;
				p1.Z = _cubliParameters.thetaH[2] * K_w_IK.X + _cubliParameters.thetaH[4] * K_w_IK.Y + _cubliParameters.thetaH[5] * K_w_IK.Z + pw.Z;;


				QMulV(&P_IK,&g_I,&g_K);
				V3Minus(&m_K,&mm_K);
				V3Angle0(&g_K,&mm_K,&angle);


				QConjugate(&P_IK,&P_KI);
				QMulV(&P_KI,&K_w_IK,&wh_I);
				QMulV(&P_KI,&p1,&pwh_I);
				QMulV(&P_KI,&m_K,&m_I);


				QMulV(&P_IK,&g_I,&g_K);
				V3Minus(&m_K,&mm_K);
				V3Angle0(&g_K,&mm_K,&angle);

				V3Cross(&m_K,&g_K,&tmp);
				V3Dot(&tmp,&p1,&top_criterion);

#ifndef CONTROLLER_SILENT
				printf("a:%f,tc:%f\n\r",angle,top_criterion);
#endif

				if(fabs(angle)< 1.5/180.0*PI || fabs(top_criterion)>fabs(top_criterionPrev) ){
					_ready2Adjust=false;
					// is Cubli able to balance?
					if(angle>5.0/180.0*PI){
						_balancingMode = NotBalancing;
						_jumpMode = NoJump;
						_balance = false;
						SetCurrent(_motor[0].NodeID, 0);
						SetCurrent(_motor[1].NodeID, 0);
						SetCurrent(_motor[2].NodeID, 0);
					}else{
#ifndef CONTROLLER_SILENT
						printf("balance!\n\r");
#endif
						_balancingMode = CornerBalancing;
						_jumpMode = NoJump;
						_balance = true;
					}

					// calculate learning criteria
					V3Dot(&g_K,&p1,&_pwhg_top);
					_pwhg_top=_pwhg_top/9.81;			// pwh * g_norm
					V3Dot(&m_K,&p1,&_pwhm_top);
					_pwhm_top=_pwhm_top/m_K_length;		// pwhm * m_norm


					// correct for applied torque
					_pwhm_top-=dpwhm;

					V3Dot(&m_K,&K_w_IK,&mdwh_top);			// projection of wh, (wh- m m*wh/||m||^2)
					mdwh_top=mdwh_top/m_K_length/m_K_length;
					V3MulS(&mdwh_top,&m_K,&proj_wh);
					V3Sub(&K_w_IK,&proj_wh,&proj_wh);

					// calculate energy
					V3Dot(&m_K,&g_K,&mdg_top);
					_Htop_3D=0.5*K_w_IK.X*(_cubliParameters.thetaH[0] * K_w_IK.X + _cubliParameters.thetaH[1] * K_w_IK.Y + _cubliParameters.thetaH[2] * K_w_IK.Z)+
							0.5*K_w_IK.Y*(_cubliParameters.thetaH[1] * K_w_IK.X + _cubliParameters.thetaH[3] * K_w_IK.Y + _cubliParameters.thetaH[4] * K_w_IK.Z)+
							0.5*K_w_IK.Z*(_cubliParameters.thetaH[2] * K_w_IK.X + _cubliParameters.thetaH[4] * K_w_IK.Y + _cubliParameters.thetaH[5] * K_w_IK.Z)-
							mdg_top-m_K_length*9.81;

					// projected energy measure
					_Hhtop_3D=0.5*proj_wh.X*(_cubliParameters.thetaH[0] * proj_wh.X + _cubliParameters.thetaH[1] * proj_wh.Y + _cubliParameters.thetaH[2] * proj_wh.Z)+
							0.5*proj_wh.Y*(_cubliParameters.thetaH[1] * proj_wh.X + _cubliParameters.thetaH[3] * proj_wh.Y + _cubliParameters.thetaH[4] * proj_wh.Z)+
							0.5*proj_wh.Z*(_cubliParameters.thetaH[2] * proj_wh.X + _cubliParameters.thetaH[4] * proj_wh.Y + _cubliParameters.thetaH[5] * proj_wh.Z)
							-mdg_top-m_K_length*9.81;

					// correct for applied torque
					_Htop_3D-=dH*.1;	// Only 10%.
					_Hhtop_3D-=dHh*.1;	// Only 10%

					// reset!
					dH=0;
					dHh=0;
					dpwhm=0;
					T[0]=0;
					T[1]=0;
					T[2]=0;

					mdpwh_dot=0;
					mdpwh_dotprev=0;
					H_dot=0;
					H_dotprev=0;
					Hh_dot=0;
					Hh_dotprev=0;

					_ready2Adjust=true;

					top_criterionPrev=-4;

				}else if(fabs(angle) < balancingAngle){

#ifndef CONTROLLER_SILENT
					//printf("\n\r edge to corner jump:%f\t%f\n\r",_cubliParameters.omegaE2C[0],_cubliParameters.omegaE2C[1]);
#endif


					// Take out uncontrollable component
					V3Dot(&g_K,&p1,&proj);
					ps.X = p1.X - g_K.X * (proj / ((float)96.2361));
					ps.Y = p1.Y - g_K.Y * (proj / ((float)96.2361));
					ps.Z = p1.Z - g_K.Z * (proj/ ((float)96.2361));

					//new controller
					V3Cross(&m_K, &g_K, &a1);
					V3Cross(&p1, &K_w_IK, &a2);
					V3Cross(&g_K, &K_w_IK, &tmp);
					V3Cross(&m_K, &tmp, &a3);
					V3Cross(&ps, &K_w_IK, &a4);

					a5.X = Knc1 * a4.X + (Knc1+Knc2*Knc3+Knc4)*a1.X + Knc2*a3.X + Knc1*Knc3 * ps.X;
					a5.Y = Knc1 * a4.Y + (Knc1+Knc2*Knc3+Knc4)*a1.Y + Knc2*a3.Y + Knc1*Knc3 * ps.Y;
					a5.Z = Knc1 * a4.Z + (Knc1+Knc2*Knc3+Knc4)*a1.Z + Knc2*a3.Z + Knc1*Knc3 * ps.Z;

					A_5a5.X = _cubliParameters.thetaH[0] * a5.X + _cubliParameters.thetaH[1] * a5.Y + _cubliParameters.thetaH[2] * a5.Z;
					A_5a5.Y = _cubliParameters.thetaH[1] * a5.X + _cubliParameters.thetaH[3] * a5.Y + _cubliParameters.thetaH[4] * a5.Z;
					A_5a5.Z = _cubliParameters.thetaH[2] * a5.X + _cubliParameters.thetaH[4] * a5.Y + _cubliParameters.thetaH[5] * a5.Z;



					// Characterize the effect of the control input.............................................
					V3Dot(&m_K,&K_w_IK,&mdwh);
					mdwh=mdwh/m_K_length/m_K_length;
					Tdwhm=mdwh*(T[0]/.0335*0.0251*m_K.X+T[1]/0.0335*0.0251*m_K.Y+T[2]/0.0335*0.0251*m_K.Z);
					H_dot=-(K_w_IK.X*T[0]/.0335*.0251+K_w_IK.Y*T[1]/.0335*.0251+K_w_IK.Z*T[2]/.0335*.0251);
					Hh_dot=H_dot+Tdwhm;
					// -----------------------------------------------------------------------------


					T[0] = a1.X + a2.X + Knc3 * (p1.X-pw.X) + A_5a5.X;
					T[1] = a1.Y + a2.Y + Knc3 * (p1.Y-pw.Y) + A_5a5.Y;
					T[2] = a1.Z + a2.Z + Knc3 * (p1.Z-pw.Z) + A_5a5.Z;

					/*
					// Christof's torqueE, torqueD -------------------------------------------------------------------
					//save projected inputs for Learning
					if(safetyCounter==0){
						_torqueE = 0;
						_torqueD = 0;
					}
					if(safetyCounter>0 && safetyCounter<=12){
						V3Cross(&m_K,&g_K,&a1);		//a1: m x g
						V3Normalize(&a1,&a1);

						sc1 = u[0]*a1.X + u[1]*a1.Y + u[2]*a1.Z;

						V3Cross(&m_K,&a1,&a2);		//a2: m x (m x g)
						V3Normalize(&a2,&a2);

						sc2 = u[0]*a2.X + u[1]*a2.Y + u[2]*a2.Z;

						if(safetyCounter==1){
							_torqueE = -sc1;
							_torqueD = -sc2;
						}
						else{
							_torqueE = (_torqueE*(safetyCounter-1) -sc1)/(safetyCounter);
							_torqueD = (_torqueD*(safetyCounter-1) -sc2)/(safetyCounter);
						}
					}
					// ------------------------------------------------------------------------------------------------
					*/

					// Apply torque
					SetTorqueWSaturation(0,&T[0]);
					SetTorqueWSaturation(1,&T[1]);
					SetTorqueWSaturation(2,&T[2]);

					// dpwhm
					V3Cross(&K_w_IK,&p1,&tmp);
					V3Dot(&m_K,&tmp,&tmp_scalar);
					mdpwh_dot=-tmp_scalar/m_K_length;

					dpwhm+=.5*(mdpwh_dot+mdpwh_dotprev)*Ts;


					// trapez integration of H
					dH+=.5*(H_dotprev+H_dot)*Ts;
					dHh+=.5*(Hh_dotprev+Hh_dot)*Ts;

					Tdwhm=mdwh*(T[0]/.0335*0.0251*m_K.X+T[1]/0.0335*0.0251*m_K.Y+T[2]/0.0335*0.0251*m_K.Z);
					H_dotprev=-(K_w_IK.X*T[0]/.0335*.0251+K_w_IK.Y*T[1]/.0335*.0251+K_w_IK.Z*T[2]/.0335*.0251);
					Hh_dotprev=H_dotprev+Tdwhm;

					mdpwh_dotprev=mdpwh_dot;
					top_criterionPrev=top_criterion;

			}
		}
	}
}




// Feedback linearization for spinning
#define FeedbackLinSpinT 20
void FeedbackLinSpin(void *pvParameters)
{

		const float alpha = 0.002; // constant to adjust m_K

		const float K_c[3][7] ={{396.929625,0.000000,363.852175,0.000000,39.564557,-0.000000,-0.000000},{0.000000,396.929625,0.000000,363.852175,0.000000,39.564557,-0.000000},{-0.000000,-0.000000,0.000000,-0.000000,-0.000000,0.000000,12.000000}};


		// physical quantities
		Quaternion P_IK;	// Quaternion describing orientation
		Quaternion P_KI;
		Vector3 wh;		// omega_h
		Vector3 wh_I;
		Vector3 ww;		// omega_w
		Vector3 pwh;
		Vector3 pwh_I;

		const float g0 = 9.81;
		Vector3 m_K;
		if(HasFlashBeenErased()){
			 m_K = _cubliParameters.m_K;
		}else{
			ReadmK(&m_K.X,&m_K.Y,&m_K.Z);
#ifndef CONTROLLER_SILENT
			printf("FLS: Read m_K from flash.\n\r");
#endif
		}
#ifndef CONTROLLER_SILENT
		printf("FLS: m_K: %f, %f, %f",m_K.X,m_K.Y,m_K.Z);
#endif

		float m_K_length;
		V3Length(&m_K,&m_K_length);
		Vector3 m_I;
		Vector3 mm_K={0.0,0.0,0.0}; // storage of -m_K
		Vector3 g_I={0,0,-9.81};
		Vector3 g_K;
		float proj=0;	// temporary storage

		Vector3 vlin;
		Vector3 vlin_K;

		Vector3 tmp;	// temporary storage
		Vector3 tmp2;

		float angle;			// inclination angle
		const float maxAngle=5.0/180.0*PI;
		float anglePrev=-4;

		float wh_des_loc=0;

		float T[3];	// torque


		// Feedback Linearization variables
		float y[2]={0,0}, ydot[2]={0,0}, ydotdot[2]={0,0};
		float e[7];

		uint16_t idx=0;
		uint16_t idle_idx=700;
		bool idle=true;
		bool shutDown=false;	// used for shutting down spinning if something goes wrong


		Delay(1000);

		portTickType xLastExecutionTime = xTaskGetTickCount();


		for (;;)
		{
			DelayUntilTime(&xLastExecutionTime, FeedbackLinSpinT);
#ifndef CONTROLLER_SILENT
			//printf("running 3D track\n\r");
#endif

			if(_balancingMode == Spin)
			{
				//
				//Read out current state of cubli and calculate needed variables
				P_IK.W = _finalEst.W;
				P_IK.X = _finalEst.X;
				P_IK.Y = _finalEst.Y;
				P_IK.Z = _finalEst.Z;


				// get inclination angle
				QMulV(&P_IK,&g_I,&g_K);
				V3Minus(&m_K,&mm_K);
				V3Angle0(&g_K,&mm_K,&angle);

				if( _modeNumber != Idle && angle < 1.0/180.0*PI){
					_balance = true;
				}



				if(fabs(anglePrev)+1.0/180.0*PI<fabs(angle)||fabs(angle)>maxAngle){
					 // Cubli is falling -> reset states
#ifndef CONTROLLER_SILENT
					printf("cubli is falling.\n\r");
#endif
					anglePrev=-4;
					angle=-4;
					_balancingMode=NotBalancing;
					_balance=false;
					_moveSuccess = false;

					idle=true;
					idx=0;
					wh_des_loc=0;

					_balance = false;
					SetCurrent(_motor[0].NodeID, 0);
					SetCurrent(_motor[1].NodeID, 0);
					SetCurrent(_motor[2].NodeID, 0);



				}else{
					anglePrev=angle;
				}


				if(_balance && fabs(angle) < maxAngle){


					// adapt m_K
					if(fabs(angle) < 1.5/180.0*PI){

						V3Dot(&m_K, &g_K, &proj);
						m_K.X = (1.0f - alpha) * m_K.X + alpha * proj / ((float)96.2361) * g_K.X;
						m_K.Y = (1.0f - alpha) * m_K.Y + alpha * proj / ((float)96.2361) * g_K.Y;
						m_K.Z = (1.0f - alpha) * m_K.Z + alpha * proj / ((float)96.2361) * g_K.Z; // shift m_K, but preserve length.

						V3Normalize(&m_K,&tmp);
						V3MulS(&_cubliParameters.m_K_length,&tmp,&m_K);

						//when reaching steady state during balancing, print this out and use this value as _cubliParameters.m_K.
						//***
						//***
#ifndef CONTROLLER_SILENT
						//printf("\n\r\t %f\t %f\t %f",m_K.X, m_K.Y, m_K.Z);
#endif
					}


					wh.X = _omega.X;
					wh.Y = _omega.Y;
					wh.Z = _omega.Z;

					ww.X = -_motor[MOTORX].Velocity;
					ww.Y = -_motor[MOTORY].Velocity;
					ww.Z = -_motor[MOTORZ].Velocity;

					//derived variables
					pwh.X = _cubliParameters.thetaH[0] * wh.X + _cubliParameters.thetaH[1] * wh.Y + _cubliParameters.thetaH[2] * wh.Z + _cubliParameters.thetaW[0] * (wh.X + ww.X);
					pwh.Y = _cubliParameters.thetaH[1] * wh.X + _cubliParameters.thetaH[3] * wh.Y + _cubliParameters.thetaH[4] * wh.Z + _cubliParameters.thetaW[1] * (wh.Y + ww.Y);
					pwh.Z = _cubliParameters.thetaH[2] * wh.X + _cubliParameters.thetaH[4] * wh.Y + _cubliParameters.thetaH[5] * wh.Z + _cubliParameters.thetaW[2] * (wh.Z + ww.Z);


					// Transfrom variables to inertial frame
					QConjugate(&P_IK,&P_KI);
					QMulV(&P_KI,&wh,&wh_I);
					QMulV(&P_KI,&pwh,&pwh_I);
					QMulV(&P_KI,&m_K,&m_I);


					//calculate Fdbl states
					y[0] = pwh_I.X;
					y[1] = pwh_I.Y;


					ydot[0] = -g0*m_I.Y;
					ydot[1] =  g0*m_I.X;


					V3Cross(&wh_I,&m_I,&tmp);

					ydotdot[0] = -g0*tmp.Y;
					ydotdot[1] = g0*tmp.X;


					if(idle){
						// wait till the wheel speeds have decreased.
						if(fabs(ww.X)<50.0 && fabs(ww.Y)<50.0 && fabs(ww.Z)<50.0)
						{
							idx++;
#ifndef CHOREOGRAPHY_SILENT
							if ( !(idx%10) )
							{
								printf("idx: %i\r\n", idx);
							}
#endif
						}
						else
						{
							idx = 0;
						}
						if(idx==idle_idx
					         || ( _modeNumber == SingleSpin && idx >= 100 ) ) // don't wait as long during choreography
						{

							// If cubli has completed 1 spin move and is now idle again
							if ( _spinsCompleted )
							{
								// Declare choreography move as successful
								_moveSuccess = true;
#ifndef CHOREOGRAPHY_SILENT
									printf("Spin move success B\r\n");
#endif
								_balancingMode = CornerBalancing;
								_jumpMode = NoJump;
								_balance = true;
							}
							// otherwise initiate a spin
							else
							{
								idle=false;
								shutDown=false;
								idx=0;
							}
						}
						wh_des_loc=0;
					}

					if(!idle){
						if(idx < TRACK_LENGTH){
#ifndef CONTROLLER_SILENT
							printf("ww:%f,%f,%f\n\r",ww.X,ww.Y,ww.Z);
#endif
							// Upper bound on wheel speed...
							if(fabs(ww.X)<250.0 && fabs(ww.Y)<250.0 && fabs(ww.Z)<250.0 && !shutDown){
								wh_des_loc=wh_des[idx++];
							}else{
								shutDown=true;
								wh_des_loc=wh_I.Z*.99;
								idx++;
							}
						}
						else
						{
							idx=0;
							idle=true;
							_spinsCompleted++ ;
#ifndef CONTROLLER_SILENT
							printf("%i spin\r\n", _spinsCompleted);
#endif
						}
					}

					//calculate e_Fdbl
					e[0] = y[0];
					e[1] = y[1];
					e[2] = ydot[0];
					e[3] = ydot[1];
					e[4] = ydotdot[0];
					e[5] = ydotdot[1];
					e[6] = -wh_des_loc+wh_I.Z;

					//caluculate q_Fdbl
					vlin.X = -K_c[0][0]*e[0] -K_c[0][1]*e[1] -K_c[0][2]*e[2] -K_c[0][3]*e[3] -K_c[0][4]*e[4] -K_c[0][5]*e[5] -K_c[0][6]*e[6];
					vlin.Y = -K_c[1][0]*e[0] -K_c[1][1]*e[1] -K_c[1][2]*e[2] -K_c[1][3]*e[3] -K_c[1][4]*e[4] -K_c[1][5]*e[5] -K_c[1][6]*e[6];
					vlin.Z = -K_c[2][0]*e[0] -K_c[2][1]*e[1] -K_c[2][2]*e[2] -K_c[2][3]*e[3] -K_c[2][4]*e[4] -K_c[2][5]*e[5] -K_c[2][6]*e[6];


					V3Cross(&wh_I,&m_I,&tmp);
					V3Cross(&wh_I,&tmp,&tmp2);

					vlin.X = 1.0/(g0*m_I.Z)*(vlin.X+vlin.Z*g0*m_I.X+g0*tmp2.Y);
					vlin.Y = 1.0/(g0*m_I.Z)*(vlin.Y+vlin.Z*g0*m_I.Y-g0*tmp2.X);

					//calculate T_Fdbl_K
					V3Cross(&m_K,&g_K,&tmp);
					V3Cross(&wh,&pwh,&tmp2);

					// Transform vlin_I back in K-frame
					QMulV(&P_IK,&vlin,&vlin_K);



					// Calculate torque
					T[0]=-_cubliParameters.thetaH[0]*vlin_K.X-_cubliParameters.thetaH[1]*vlin_K.Y-_cubliParameters.thetaH[2]*vlin_K.Z+tmp.X-tmp2.X;
					T[1]=-_cubliParameters.thetaH[1]*vlin_K.X-_cubliParameters.thetaH[3]*vlin_K.Y-_cubliParameters.thetaH[4]*vlin_K.Z+tmp.Y-tmp2.Y;
					T[2]=-_cubliParameters.thetaH[2]*vlin_K.X-_cubliParameters.thetaH[4]*vlin_K.Y-_cubliParameters.thetaH[5]*vlin_K.Z+tmp.Z-tmp2.Z;

					// do not apply torque.
					//T[0]=0;
					//T[1]=0;
					//T[2]=0;

					SetTorqueWSaturation(0,&T[0]);
					SetTorqueWSaturation(1,&T[1]);
					SetTorqueWSaturation(2,&T[2]);

				}

			}
		}

}






// Feedback linearization for spinning -> Stability test
#define FeedbackLinSpinStabT 10
void FeedbackLinSpinStab(void *pvParameters)
{

		const float alpha = 0.002; // constant to adjust m_K

		const float K_c[3][7] ={{396.929625,0.000000,363.852175,0.000000,39.564557,-0.000000,-0.000000},{0.000000,396.929625,0.000000,363.852175,0.000000,39.564557,-0.000000},{-0.000000,-0.000000,0.000000,-0.000000,-0.000000,0.000000,12.000000}};

		//const float K_c[3][7] ={{108.3274,0, 157.0747,         0,   36.4982,   -0.0000,         0},
		 //        {0,  108.3274,         0,  157.0747,         0,   36.4982,         0},
		 //        {0,         0,         0,    0.0000,    0.0000,    0.0000,    4.0000}};


		// physical quantities
		Quaternion P_IK;	// Quaternion describing orientation
		Quaternion P_KI;
		Vector3 wh;		// omega_h
		Vector3 wh_I;
		Vector3 ww;		// omega_w
		Vector3 pwh;
		Vector3 pwh_I;

		const float g0 = 9.81;
		Vector3 m_K;
		if(HasFlashBeenErased()){
			 m_K = _cubliParameters.m_K;
		}else{
			ReadmK(&m_K.X,&m_K.Y,&m_K.Z);
#ifndef CONTROLLER_SILENT
			printf("FLS: Read m_K from flash.\n\r");
#endif
		}
#ifndef CONTROLLER_SILENT
		printf("FLS: m_K: %f, %f, %f",m_K.X,m_K.Y,m_K.Z);
#endif

		float m_K_length;
		V3Length(&m_K,&m_K_length);
		Vector3 m_I;
		Vector3 mm_K={0.0,0.0,0.0}; // storage of -m_K
		Vector3 g_I={0,0,-9.81};
		Vector3 g_K;
		float proj=0;	// temporary storage

		Vector3 vlin;
		Vector3 vlin_K;

		Vector3 tmp;	// temporary storage
		Vector3 tmp2;

		float angle;			// inclination angle
		const float maxAngle=6.0/180.0*PI;
		float anglePrev=-4;

		float wh_des_loc=0;

		float T[3];	// torque


		// Feedback Linearization variables
		float y[2]={0,0}, ydot[2]={0,0}, ydotdot[2]={0,0};
		float e[7];

		uint16_t idx=0;
		uint16_t idle_idx=800;
		bool idle=true;
		bool shutDown=false;	// used for shutting down spinning if something goes wrong

		bool reachedMaxSpeed=false;

		Delay(1000);

		portTickType xLastExecutionTime = xTaskGetTickCount();


		for (;;)
		{
			DelayUntilTime(&xLastExecutionTime, FeedbackLinSpinStabT);
#ifndef CONTROLLER_SILENT
			//printf("running 3D track\n\r");
#endif

			if(_balancingMode == Spin)
			{
				//
				//Read out current state of cubli and calculate needed variables
				P_IK.W = _finalEst.W;
				P_IK.X = _finalEst.X;
				P_IK.Y = _finalEst.Y;
				P_IK.Z = _finalEst.Z;


				// get inclination angle
				QMulV(&P_IK,&g_I,&g_K);
				V3Minus(&m_K,&mm_K);
				V3Angle0(&g_K,&mm_K,&angle);

				if( _modeNumber != 0 && angle < 1.0/180.0*PI){
					_balance = true;
				}

				if(fabs(anglePrev)+1.0/180.0*PI<fabs(angle)||fabs(angle)>maxAngle){
					 // Clubli is falling -> reset states
#ifndef CONTROLLER_SILENT
					printf("cubli is falling.\n\r");
#endif
					anglePrev=-4;
					angle=-4;
					_balancingMode=NotBalancing;
					_balance=false;

					idle=true;
					idx=0;
					wh_des_loc=0;

					_balance = false;
					SetCurrent(_motor[0].NodeID, 0);
					SetCurrent(_motor[1].NodeID, 0);
					SetCurrent(_motor[2].NodeID, 0);
				}else{
					anglePrev=angle;
				}


				if(_balance && fabs(angle) < maxAngle){


					// adapt m_K
					if(fabs(angle) < 1.5/180.0*PI){

						V3Dot(&m_K, &g_K, &proj);
						m_K.X = (1.0f - alpha) * m_K.X + alpha * proj / ((float)96.2361) * g_K.X;
						m_K.Y = (1.0f - alpha) * m_K.Y + alpha * proj / ((float)96.2361) * g_K.Y;
						m_K.Z = (1.0f - alpha) * m_K.Z + alpha * proj / ((float)96.2361) * g_K.Z; // shift m_K, but preserve length.

						V3Normalize(&m_K,&tmp);
						V3MulS(&_cubliParameters.m_K_length,&tmp,&m_K);

						//when reaching steady state during balancing, print this out and use this value as _cubliParameters.m_K.
						//***
						//***
#ifndef CONTROLLER_SILENT
						//printf("\n\r\t %f\t %f\t %f",m_K.X, m_K.Y, m_K.Z);
#endif
					}


					wh.X = _omega.X;
					wh.Y = _omega.Y;
					wh.Z = _omega.Z;

					ww.X = -_motor[MOTORX].Velocity;
					ww.Y = -_motor[MOTORY].Velocity;
					ww.Z = -_motor[MOTORZ].Velocity;

					//derived variables
					pwh.X = _cubliParameters.thetaH[0] * wh.X + _cubliParameters.thetaH[1] * wh.Y + _cubliParameters.thetaH[2] * wh.Z + _cubliParameters.thetaW[0] * (wh.X + ww.X);
					pwh.Y = _cubliParameters.thetaH[1] * wh.X + _cubliParameters.thetaH[3] * wh.Y + _cubliParameters.thetaH[4] * wh.Z + _cubliParameters.thetaW[1] * (wh.Y + ww.Y);
					pwh.Z = _cubliParameters.thetaH[2] * wh.X + _cubliParameters.thetaH[4] * wh.Y + _cubliParameters.thetaH[5] * wh.Z + _cubliParameters.thetaW[2] * (wh.Z + ww.Z);


					// Transfrom variables to inertial frame
					QConjugate(&P_IK,&P_KI);
					QMulV(&P_KI,&wh,&wh_I);
					QMulV(&P_KI,&pwh,&pwh_I);
					QMulV(&P_KI,&m_K,&m_I);


					//calculate Fdbl states
					y[0] = pwh_I.X;
					y[1] = pwh_I.Y;


					ydot[0] = -g0*m_I.Y;
					ydot[1] =  g0*m_I.X;


					V3Cross(&wh_I,&m_I,&tmp);

					ydotdot[0] = -g0*tmp.Y;
					ydotdot[1] = g0*tmp.X;


					if(idle){
						// wait till the wheel speeds have decreased.
						if(fabs(ww.X)<50.0 && fabs(ww.Y)<50.0 && fabs(ww.Z)<50.0) idx++;
						if(idx==idle_idx){
							idle=false;
							shutDown=false;
							idx=0;
							_weightAccMeas=0.001;
						}
						wh_des_loc=0;
					}
					if(!idle){
						wh_des_loc=.5;

#ifndef CONTROLLER_SILENT
						printf("ww:%f,%f,%f\n\r",ww.X,ww.Y,ww.Z);
#endif


					}

					//calculate e_Fdbl
					e[0] = y[0];
					e[1] = y[1];
					e[2] = ydot[0];
					e[3] = ydot[1];
					e[4] = ydotdot[0];
					e[5] = ydotdot[1];
					e[6] = -wh_des_loc+wh_I.Z;

					//caluculate q_Fdbl
					vlin.X = -K_c[0][0]*e[0] -K_c[0][1]*e[1] -K_c[0][2]*e[2] -K_c[0][3]*e[3] -K_c[0][4]*e[4] -K_c[0][5]*e[5] -K_c[0][6]*e[6];
					vlin.Y = -K_c[1][0]*e[0] -K_c[1][1]*e[1] -K_c[1][2]*e[2] -K_c[1][3]*e[3] -K_c[1][4]*e[4] -K_c[1][5]*e[5] -K_c[1][6]*e[6];
					vlin.Z = -K_c[2][0]*e[0] -K_c[2][1]*e[1] -K_c[2][2]*e[2] -K_c[2][3]*e[3] -K_c[2][4]*e[4] -K_c[2][5]*e[5] -K_c[2][6]*e[6];


					V3Cross(&wh_I,&m_I,&tmp);
					V3Cross(&wh_I,&tmp,&tmp2);

					vlin.X = 1.0/(g0*m_I.Z)*(vlin.X+vlin.Z*g0*m_I.X+g0*tmp2.Y);
					vlin.Y = 1.0/(g0*m_I.Z)*(vlin.Y+vlin.Z*g0*m_I.Y-g0*tmp2.X);

					//calculate T_Fdbl_K
					V3Cross(&m_K,&g_K,&tmp);
					V3Cross(&wh,&pwh,&tmp2);

					// Transform vlin_I back in K-frame
					QMulV(&P_IK,&vlin,&vlin_K);



					// Calculate torque
					T[0]=-_cubliParameters.thetaH[0]*vlin_K.X-_cubliParameters.thetaH[1]*vlin_K.Y-_cubliParameters.thetaH[2]*vlin_K.Z+tmp.X-tmp2.X;
					T[1]=-_cubliParameters.thetaH[1]*vlin_K.X-_cubliParameters.thetaH[3]*vlin_K.Y-_cubliParameters.thetaH[4]*vlin_K.Z+tmp.Y-tmp2.Y;
					T[2]=-_cubliParameters.thetaH[2]*vlin_K.X-_cubliParameters.thetaH[4]*vlin_K.Y-_cubliParameters.thetaH[5]*vlin_K.Z+tmp.Z-tmp2.Z;

					// do not apply torque.
					//T[0]=0;
					//T[1]=0;
					//T[2]=0;

					const float threshold=500.0;
					if(fabs(ww.X)>=threshold || fabs(ww.Y)>=threshold || fabs(ww.Z)>=threshold){
#ifndef CONTROLLER_SILENT
						printf("reached max.\n\r");
#endif

						T[0]=0;
						T[1]=0;
						T[2]=0;
						SetTorqueWSaturation(0,&T[0]);
						SetTorqueWSaturation(1,&T[1]);
						SetTorqueWSaturation(2,&T[2]);
						reachedMaxSpeed=true;
					}

					if(!reachedMaxSpeed){
						SetTorqueWSaturation(0,&T[0]);
						SetTorqueWSaturation(1,&T[1]);
						SetTorqueWSaturation(2,&T[2]);
					}
				}

			}
		}

}


// Feedback linearization for spinning -> Stability test
#define SpinStabT 10
void SpinStab(void *pvParameters)
{

		// physical quantities
		Quaternion P_IK;	// Quaternion describing orientation
		Quaternion P_KI;
		Vector3 wh;		// omega_h
		Vector3 ww;		// omega_w
		Vector3 pwh;

		const float g0 = 9.81;
		Vector3 m_K;
		if(HasFlashBeenErased()){
			 m_K = _cubliParameters.m_K;
		}else{
			ReadmK(&m_K.X,&m_K.Y,&m_K.Z);
#ifndef CONTROLLER_SILENT
			printf("FLS: Read m_K from flash.\n\r");
#endif
		}
#ifndef CONTROLLER_SILENT
		printf("FLS: m_K: %f, %f, %f",m_K.X,m_K.Y,m_K.Z);
#endif

		float m_K_length;
		V3Length(&m_K,&m_K_length);
		Vector3 m_I;
		Vector3 mm_K={0.0,0.0,0.0}; // storage of -m_K
		Vector3 g_I={0,0,-9.81};
		Vector3 g_K;

		const float pwhg=9;

		float wwdes[3];
		wwdes[0]=-pwhg/9.81*1.0/5.4*1e4*m_K.X/m_K_length;
		wwdes[1]=-pwhg/9.81*1.0/5.4*1e4*m_K.Y/m_K_length;
		wwdes[2]=-pwhg/9.81*1.0/5.4*1e4*m_K.Z/m_K_length;

		float angle;			// inclination angle
		const float maxAngle=20.0/180.0*PI;

		bool speedup=false;

		Delay(1000);

		portTickType xLastExecutionTime = xTaskGetTickCount();


		for (;;)
		{
			DelayUntilTime(&xLastExecutionTime, SpinStabT);
#ifndef CONTROLLER_SILENT
			//printf("running 3D track\n\r");
#endif

			if(_balancingMode == Spin)
			{
				//
				//Read out current state of cubli and calculate needed variables
				P_IK.W = _finalEst.W;
				P_IK.X = _finalEst.X;
				P_IK.Y = _finalEst.Y;
				P_IK.Z = _finalEst.Z;


				// get inclination angle
				QMulV(&P_IK,&g_I,&g_K);
				V3Minus(&m_K,&mm_K);
				V3Angle0(&g_K,&mm_K,&angle);

				if( _modeNumber != Idle && angle < 1.0/180.0*PI){
					_balance = true;
				}

				if(_balance ){
					wh.X = _omega.X;
					wh.Y = _omega.Y;
					wh.Z = _omega.Z;

					ww.X = -_motor[MOTORX].Velocity;
					ww.Y = -_motor[MOTORY].Velocity;
					ww.Z = -_motor[MOTORZ].Velocity;

					//derived variables
					pwh.X = _cubliParameters.thetaH[0] * wh.X + _cubliParameters.thetaH[1] * wh.Y + _cubliParameters.thetaH[2] * wh.Z + _cubliParameters.thetaW[0] * (wh.X + ww.X);
					pwh.Y = _cubliParameters.thetaH[1] * wh.X + _cubliParameters.thetaH[3] * wh.Y + _cubliParameters.thetaH[4] * wh.Z + _cubliParameters.thetaW[1] * (wh.Y + ww.Y);
					pwh.Z = _cubliParameters.thetaH[2] * wh.X + _cubliParameters.thetaH[4] * wh.Y + _cubliParameters.thetaH[5] * wh.Z + _cubliParameters.thetaW[2] * (wh.Z + ww.Z);


					if(!speedup){
#ifndef CONTROLLER_SILENT
						printf("speed up wheels.\n\r");
						printf("ww des: %f, %f, %f\n\r",wwdes[0],wwdes[1],wwdes[2]);
#endif
						_ww_des[0]=wwdes[0];
						_ww_des[1]=wwdes[1];
						_ww_des[2]=wwdes[2];
						_speedup[0]=true;
						_speedup[1]=true;
						_speedup[2]=true;
						speedup=true;
					}else{
#ifndef CONTROLLER_SILENT
						printf("a, ww: %f, %f, %f, %f\n\r",angle,ww.X,ww.Y,ww.Z);
#endif
						if(_reached_velocity[0]==true&&_reached_velocity[1]==true&&_reached_velocity[2]==true){
#ifndef CONTROLLER_SILENT
							printf("reached vel\n\r");
#endif
							/*_speedup[0]=false;
							_speedup[1]=false;
							_speedup[2]=false;
							SetCurrent(_motor[0].NodeID, 0);
							SetCurrent(_motor[1].NodeID, 0);
							SetCurrent(_motor[2].NodeID, 0);*/
						}
					}
				}

				if(fabs(angle) > maxAngle){
#ifndef CONTROLLER_SILENT
					printf("cubli falling\n\r");
#endif
					_speedup[0]=false;
					_speedup[1]=false;
					_speedup[2]=false;
					SetCurrent(_motor[0].NodeID, 0);
					SetCurrent(_motor[1].NodeID, 0);
					SetCurrent(_motor[2].NodeID, 0);
					_balance=false;
				}

			}
		}
}
