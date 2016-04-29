#include "StateEstimation.h"

I2CBusInitData _i2cBus1;
I2CBusInitData _i2cBus2;
//I2CBusInitData _i2cBus3;

#define NUM_I2C_DEVICES 6
I2CMember _imu[NUM_I2C_DEVICES];
AccI2C _acc[NUM_I2C_DEVICES];
GyroI2C _gyr[NUM_I2C_DEVICES];

Vector3 _gyrMeasFiltered[NUM_I2C_DEVICES];
Vector3 _accMeasFiltered[NUM_I2C_DEVICES];

Vector3 _attitude = { 0, 0, 0 };

Vector3 _gRef = {0, 0, 0};
Vector3 _gEst = { 0, 0, 0 };

Quaternion _finalEst = {0,0,0,1};

Vector3 _omega = { 0, 0, 0 };

Vector3 _xyzAngles = {0, 0, 0}; 	// first value is angle gamma around Z, then angle beta around Y, third angle alpha around Z [rad]
Vector3 _edgeAngles = {0, 0, 0};	// angles to balance on edges (unaffected by non-level surfaces)
bool _imusReady = false;
bool _attitudeReady = false;


float _weightAccMeas = 0.02;				// value for Complimentary Filter used for Sensor Fusion

#define AttitudeEstimatorT 10
void AttitudeEstimator(void *parameters)
{
	float weightGyroMeas = 1-_weightAccMeas;

	float factor = 1.0 / NUM_I2C_DEVICES;	// weight of each gyro measurement

	// Axis angle Representations of the Measurements
	Vector3 accRotAxis = {0, 0, 0};
	float accAngle = 0;
	Vector3 gyroRotAxis = {0, 0, 0};
	float gyroAngle = 0;
	Vector3 prevOmega = {0, 0, 0};


	// Quaternion representations of the Estimates
	Quaternion accEst;
	Quaternion gyroEst;
	Quaternion gyroDelta;	// difference between two timesteps
	Quaternion combinedEst;

	float dt = AttitudeEstimatorT * 0.001;	// timestep between two estimates

	float omegaTreshold = 0.00001;

	float tmpf;

	uint8_t i = 0;

	InitEncoder();

	while(!_imusReady) // Estimation can not start before the IMUs are initalized
		Delay(100);

	portTickType lastExecutionTime = xTaskGetTickCount();

	_gRef.X=0;_gRef.Y=0;_gRef.Z=-9.81;

	Vector3 g = _gRef;
	Vector3 g_K;


	V3Normalize(&g, &g);


//	printf("\n\roffset: %2.2f,%2.2f,%2.2f",
//				(float) offset.X, (float) offset.Y, (float) offset.Z);

	_attitudeReady=true;
	//printf("\n\rAttitude Estimator: ready");

	for (;;) {
		DelayUntilTime( &lastExecutionTime, AttitudeEstimatorT);

		//Take average of all gyro measurements
		_omega.X = 0; _omega.Y = 0; _omega.Z = 0;
		for(i = 0; i < NUM_I2C_DEVICES; ++i)
		{
			EnC();
			_omega.X = _omega.X + factor * _gyrMeasFiltered[i].X;
			_omega.Y = _omega.Y + factor * _gyrMeasFiltered[i].Y;
			_omega.Z = _omega.Z + factor * _gyrMeasFiltered[i].Z;
			ExC();
		}
		if(_omega.X < omegaTreshold && _omega.X > -omegaTreshold)
			_omega.X = 0;
		if(_omega.Y < omegaTreshold && _omega.Y > -omegaTreshold)
			_omega.Y = 0;
		if(_omega.Z < omegaTreshold && _omega.Z > -omegaTreshold)
			_omega.Z = 0;

		if(_omega.X != 0 || _omega.Y !=0 || _omega.Z!=0)
		{
			//old Integration: phi[k] = phi[k-1] + dt*omega[k]
//			gyroRotAxis = _omega;
//			V3Normalize(&gyroRotAxis, &gyroRotAxis);
//			gyroAngle = sqrt(_omega.X * _omega.X + _omega.Y * _omega.Y + _omega.Z * _omega.Z) *dt;
//			QAxisAngle(&gyroRotAxis, &gyroAngle, &gyroDelta);


			//new Integration: phi[k] = phi[k-1] + dt*(omega[k]+omega[k-1])/2
			V3Add(&_omega,&prevOmega,&gyroRotAxis);

			gyroAngle = sqrt(gyroRotAxis.X/2 * gyroRotAxis.X/2 + gyroRotAxis.Y/2 * gyroRotAxis.Y/2 + gyroRotAxis.Z/2 * gyroRotAxis.Z/2) *dt;

			V3Normalize(&gyroRotAxis, &gyroRotAxis);
			QAxisAngle(&gyroRotAxis, &gyroAngle, &gyroDelta);

			//Compute orientation estimate of gyro
			QMul(&gyroDelta, &combinedEst, &gyroEst);
		}
		else	// no movement => estimate does not change
		{
			gyroEst = combinedEst;
		}

		prevOmega = _omega;

		//Take weighted average of all accelerometer measurements
		_gEst.X = 0; _gEst.Y = 0; _gEst.Z = 0;
		for(i = 0; i < NUM_I2C_DEVICES; ++i)
		{
			_gEst.X = _gEst.X + _cubliParameters._weights[i] * _accMeasFiltered[i].X;
			_gEst.Y = _gEst.Y + _cubliParameters._weights[i] * _accMeasFiltered[i].Y;
			_gEst.Z = _gEst.Z + _cubliParameters._weights[i] * _accMeasFiltered[i].Z;
		}

		V3Normalize(&_gEst, &_gEst);

		//Compute orientation estimate of accelerometers
		V3Cross(&_gEst, &g, &accRotAxis);
		V3Normalize(&accRotAxis, &accRotAxis);

		tmpf=g.X * _gEst.X + g.Y * _gEst.Y + g.Z * _gEst.Z;
		if(tmpf<0.99999f)
			accAngle = acos(tmpf);
		else
			accAngle = 0;
		// convert to a quaternion

		QAxisAngle(&accRotAxis, &accAngle, &accEst);

		//Fuse the two estimations using a complementary filter and SLERP interpolation
		QInterpolate(&accEst, &gyroEst, &_weightAccMeas, &weightGyroMeas, &combinedEst);

		UpdateEncoderValue();

		EnC();
		QNormalize(&combinedEst, &_finalEst);
		// convert to euler angles.
		QToEuler(&_finalEst,&_xyzAngles);

		//convert to g vector (in the body frame)
		QMulV(&_finalEst,&_gRef,&g_K);
		_edgeAngles.X = atan2(-g_K.Y,-g_K.Z);//atan(g_K.Y/sqrt(g_K.X*g_K.X + g_K.Z*g_K.Z));
		_edgeAngles.Y = atan2(-g_K.Z,-g_K.X);//atan2(g_K.X,-g_K.Z);
		_edgeAngles.Z = atan2(-g_K.X,-g_K.Y);
		ExC();
	}
}


#define MPUMPU6050TestTaskT 10
void MPU6050TestTask(void* parameters)
{
	printf("Start I2CBus Initialization\n\r");
	Delay(100);
	EnC();I2CBusInit(&_i2cBus1);ExC();
	Delay(100);
	EnC();I2CBusInit(&_i2cBus2);ExC();
	Delay(100);
	EnC();I2C3BusInit();ExC();
	printf("I2CBus Initialized\n\r");
	printf("Start IMU Initialization\n\r");


	uint8_t numDevices = NUM_I2C_DEVICES;
	uint8_t i;
	for(i = 0; i < numDevices; ++i)
	{
		Delay(20);
		printf("Initialize IMU:%i\n\r",i);
		EnC();MPU6000Init(&_imu[i]);ExC();
		EnC();MPU6000ConfigAcc(&_acc[i], Max16g);ExC();
		EnC();MPU6000ConfigGyro(&_gyr[i], Max1000DegPerS);ExC();
		EnC();SetFilter(&_imu[i], Cutoff21HzDelay8_5);ExC();
	}

	Vector3I16 rawAcc[numDevices];
	Vector3I16 rawGyr[numDevices];
	Vector3 tmp;
	Vector3 tmp2;

	Delay(100);

	//Offset evaluation
	uint16_t numOffsetCalibrationPoints = 100;
	uint16_t numWarmUpSamples = 50;

	float f = 1.0f / numOffsetCalibrationPoints;
	portTickType lastExecutionTime = xTaskGetTickCount();

	//LEDOn(LED4);
	uint8_t j;
	for (i = 0; i < numWarmUpSamples; ++i)
	{
		DelayUntilTime( &lastExecutionTime, MPUMPU6050TestTaskT);
		for(j=0;j<numDevices;++j)
		{
			EnC();GetRotation(&_gyr[j], &rawGyr[j]);ExC();
		}
	}

	for (i = 0; i < numOffsetCalibrationPoints; ++i) {
		DelayUntilTime( &lastExecutionTime, MPUMPU6050TestTaskT);

		for(j=0;j<numDevices;++j)
		{
			EnC();GetRotation(&_gyr[j], &rawGyr[j]);ExC();
			GyroConvert(&_gyr[j], &rawGyr[j], &tmp);
			V3MulS(&f, &tmp, &tmp2);
			V3Add(&tmp2, &_gyr[j].Offset, &_gyr[j].Offset);
		}
	}

	lastExecutionTime = xTaskGetTickCount();
	_imusReady = true;

	LEDOff(LED3);
	printf("IMU's are ready\n\r");

	for (;;)
	{
		DelayUntilTime( &lastExecutionTime, MPUMPU6050TestTaskT);

		for(i = 0; i < numDevices; ++i)
		{

			EnC(); bool test=(GetAcceleration(&_acc[i], &rawAcc[i])&&GetRotation(&_gyr[i], &rawGyr[i]));  ExC();

			if(!test){
#ifndef IMU_SILENT
				printf("IMU:%i\n\r",i);
#endif

				LEDOn(LED3);

				/*
				printf("Reset I2C:%i\n\r",i);
				switch(i){
					case 0:
						EnC();I2CBusInit(&_i2cBus1);ExC();
						break;
					case 1:
						EnC();I2CBusInit(&_i2cBus1);ExC();
						break;
					case 2:
						EnC();I2CBusInit(&_i2cBus2);ExC();
						break;
					case 3:
						EnC();I2CBusInit(&_i2cBus2);ExC();
						break;
					case 4:
						EnC();I2C3BusInit();ExC();
						break;
					case 5:
						EnC();I2C3BusInit();ExC();
						break;
					default:
						break;
				}

				// IMU Restart is causing problems as well...
				printf("RestartIMU\n\r");
				Delay(10);
				EnC();MPU6000Init(&_imu[i]);ExC();
				EnC();MPU6000ConfigAcc(&_acc[i], Max16g);ExC();
				EnC();MPU6000ConfigGyro(&_gyr[i], Max1000DegPerS);ExC();
				EnC();SetFilter(&_imu[i], Cutoff21HzDelay8_5);ExC();
				printf("IMU Restarted\n\r");
				*/
			}
			else
			{
				LEDOff(LED3);
			}
		}

		for(i = 0; i < numDevices; ++i)
		{

			EnC(); AccConvert(&_acc[i], &rawAcc[i], &tmp); ExC();
			V3Add(&tmp, &(_cubliParameters._accBias[i]), &tmp2);
			M3MulMV(&(_cubliParameters._scaleCorr[i]),&tmp2,&tmp);
			M3MulMV(&(_cubliParameters._accCorr[i]),&tmp,&tmp2);
			EnC(); _accMeasFiltered[i] = tmp2; ExC();

			EnC(); GyroConvertOC(&_gyr[i], &rawGyr[i], &tmp); ExC();
			M3MulMV(&(_cubliParameters._gyrCorr[i]),&tmp,&tmp2);
			EnC(); _gyrMeasFiltered[i] = tmp2; ExC();
		}
	}
}


//Use together with MATLAB script TODO...
#define IMUCalibrationT 10
void IMUCalibration(void* parameters)
{
        EnC();I2CBusInit(&_i2cBus1);ExC();
        Delay(100);

        EnC();I2CBusInit(&_i2cBus2);ExC();
        Delay(100);


        EnC();I2C3BusInit();ExC();
        Delay(100);
        LEDOn(LED3);




        uint8_t numDevices = NUM_I2C_DEVICES;
        uint8_t i;
        for(i = 0; i < numDevices; ++i)
        {
                EnC();MPU6000Init(&_imu[i]);ExC();
                EnC();MPU6000ConfigAcc(&_acc[i], Max16g);ExC();
                EnC();MPU6000ConfigGyro(&_gyr[i], Max2000DegPerS);ExC();
        }

        //LEDOn(LED3);

        Vector3I16 rawAcc[numDevices];
        Vector3I16 rawGyr[numDevices];
        Vector3 tmp;
        Vector3 tmp2;

        //Offset evaluation
        uint16_t numOffsetCalibrationPoints = 100;
        uint16_t numWarmUpSamples = 50;

        float f = 1.0f / numOffsetCalibrationPoints;
        portTickType lastExecutionTime = xTaskGetTickCount();

        //LEDOn(LED4);
        uint8_t j;
        for (i = 0; i < numWarmUpSamples; ++i)
        {
                DelayUntilTime( &lastExecutionTime, IMUCalibrationT);
                for(j=0;j<numDevices;++j)
                {
                        EnC();GetRotation(&_gyr[j], &rawGyr[j]);ExC();
                }
        }

        for (i = 0; i < numOffsetCalibrationPoints; ++i) {
                DelayUntilTime( &lastExecutionTime, 10);

                for(j=0;j<numDevices;++j)
                {
                        EnC();GetRotation(&_gyr[j], &rawGyr[j]);ExC();
                        GyroConvert(&_gyr[j], &rawGyr[j], &tmp);
                        V3MulS(&f, &tmp, &tmp2);
                        V3Add(&tmp2, &_gyr[j].Offset, &_gyr[j].Offset);
                }
        }

        uint32_t counter = 0;
        uint32_t outputFreq = 5;

        lastExecutionTime = xTaskGetTickCount();
        _imusReady = true;
        printf("\n\rIMUs: ready");
        for (;;)
        {
                DelayUntilTime( &lastExecutionTime, 10);

                for(i = 0; i < numDevices; ++i)
                {
                        EnC();GetAcceleration(&_acc[i], &rawAcc[i]);ExC();
                        EnC();GetRotation(&_gyr[i], &rawGyr[i]);ExC();
                }

                for(i = 0; i < numDevices; ++i)
                {
                        AccConvert(&_acc[i], &rawAcc[i], &tmp);
                        _accMeasFiltered[i] = tmp;

                        GyroConvertOC(&_gyr[i], &rawGyr[i], &tmp);
                        _gyrMeasFiltered[i] = tmp;

                        if(counter % outputFreq == 0)
                        {
                        	//AccConvert(&_acc[i], &rawAcc[i], &tmp2);M3MulMV(&_cubliParameters._accCorr[i],&tmp2,&tmp); _accMeasFiltered[i] = tmp;
                        	//GyroConvert(&_gyr[i], &rawGyr[i], &tmp2);M3MulMV(&_cubliParameters._gyrCorr[i],&tmp2,&tmp); _gyrMeasFiltered[i] = tmp;
                                printf("\n\r%i\t%2.2f\t%2.2f\t%2.2f",
                                        (int)i, (float)_accMeasFiltered[i].X, (float)_accMeasFiltered[i].Y, (float)_accMeasFiltered[i].Z);
                        }
                }
                ++counter;
        }

}


uint16_t ulIdleCycleCount=0;
void vApplicationIdleHook( void ){
	ulIdleCycleCount++;
}

void CheckProcessingCapacity(void* parameters){
	//Description:
	//The IdleHook function above will count up a counter when the scheduler has no tasks to execute.
	//By looking at this counter, one can get a feeling on how much capacity of the microcontroller is used
	//(e.g. the counter being zero all the time means that there is no capacity left, and low priority tasks may not be executed.
	//Special care is advised when printing out many variables, especially at baudrate 115200.

	portTickType lastExecutionTime = xTaskGetTickCount();
	for(;;){
		DelayUntilTime( &lastExecutionTime, 2);
		printf("\t%i", ulIdleCycleCount);
		ulIdleCycleCount=0;
	}
}
