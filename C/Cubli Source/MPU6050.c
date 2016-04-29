#include "MPU6050.h"


//http://www.mikrocontroller.net/topic/256391
void I2CBusInit(I2CBusInitData* init)
{
  /*  GPIO Setup  */
  RCC_AHB1PeriphClockCmd(init->GPIOClock, ENABLE);
  RCC_APB1PeriphClockCmd(init->PeripheralClock, ENABLE);

//  GPIO_PinAFConfig(init->GPIO, init->PinSourceClock, init->GPIO_AF);
//  GPIO_PinAFConfig(init->GPIO, init->PinSourceData, init->GPIO_AF);
	int index = 0;
	uint8_t pinSource = GetPinSource(init->Pins, index);
	while(pinSource != 0xFF)
	{
		GPIO_PinAFConfig(init->GPIO, pinSource, init->GPIO_AF);
		pinSource = GetPinSource(init->Pins, ++index);
	}

  GPIO_InitTypeDef GPIO_I2C;
  GPIO_I2C.GPIO_Pin = init->Pins/*init->PinClock | init->PinData*/;
  GPIO_I2C.GPIO_Mode = GPIO_Mode_AF;
  GPIO_I2C.GPIO_OType = GPIO_OType_OD;
  GPIO_I2C.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_I2C.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(init->GPIO, &GPIO_I2C);

  /*  I2C Setup  */
  I2C_InitTypeDef I2C_init;
  I2C_init.I2C_Mode = I2C_Mode_I2C;
  I2C_init.I2C_ClockSpeed = 400000;//400000;
  I2C_init.I2C_DutyCycle = I2C_DutyCycle_16_9;
  I2C_init.I2C_OwnAddress1 = 0x00;
  I2C_init.I2C_Ack = I2C_Ack_Enable;
  I2C_init.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  I2C_Init(init->I2CBus, &I2C_init);

  I2C_Cmd(init->I2CBus, ENABLE);
}

void I2C3BusInit()
{

//	_i2cBus3.I2CBus = I2C3;
//	_i2cBus3.GPIO = GPIOA | GPIOC;
//	_i2cBus3.GPIOClock = RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOA;
//	_i2cBus3.PeripheralClock = RCC_APB1Periph_I2C3;
//	_i2cBus3.Pins = GPIO_Pin_8 | GPIO_Pin_9;
//	_i2cBus3.GPIO_AF = GPIO_AF_I2C3;

  /*  GPIO Setup  */
  RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOA, ENABLE);
  RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOC, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C3, ENABLE);

  GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_I2C3);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource9, GPIO_AF_I2C3);

  //Clock line
  GPIO_InitTypeDef GPIO_I2C;
  GPIO_I2C.GPIO_Pin = GPIO_Pin_8;
  GPIO_I2C.GPIO_Mode = GPIO_Mode_AF;
  GPIO_I2C.GPIO_OType = GPIO_OType_OD;
  GPIO_I2C.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_I2C.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_I2C);

  //Data line
  GPIO_InitTypeDef GPIO_I2C2;
	GPIO_I2C2.GPIO_Pin = GPIO_Pin_9;
	GPIO_I2C2.GPIO_Mode = GPIO_Mode_AF;
	GPIO_I2C2.GPIO_OType = GPIO_OType_OD;
	GPIO_I2C2.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_I2C2.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOC, &GPIO_I2C2);

  /*  I2C Setup  */
  I2C_InitTypeDef I2C_init;
  I2C_init.I2C_Mode = I2C_Mode_I2C;
  I2C_init.I2C_ClockSpeed = 400000;//400000;
  I2C_init.I2C_DutyCycle = I2C_DutyCycle_16_9;
  I2C_init.I2C_OwnAddress1 = 0x00;
  I2C_init.I2C_Ack = I2C_Ack_Enable;
  I2C_init.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  I2C_Init(I2C3, &I2C_init);

  I2C_Cmd(I2C3, ENABLE);
}

//Si unints
void AccConvert(struct AccI2C* acc, Vector3I16* data, Vector3* v)
{
	v->X = (data->X * acc->ConvToSIUnits);
	v->Y = (data->Y * acc->ConvToSIUnits) ;
	v->Z = (data->Z * acc->ConvToSIUnits);
}

//Si unints, Offset Corrected
void GyroConvert(struct GyroI2C* imu, Vector3I16* data, Vector3* v)
{
	v->X = (data->X * imu->ConvToSIUnits);
	v->Y = (data->Y * imu->ConvToSIUnits);
	v->Z = (data->Z * imu->ConvToSIUnits);
}

//Si unints, Offset Corrected
void GyroConvertOC(struct GyroI2C* imu, Vector3I16* data, Vector3* v)
{
	v->X = ((data->X * imu->ConvToSIUnits) - imu->Offset.X);
	v->Y = ((data->Y * imu->ConvToSIUnits) - imu->Offset.Y);
	v->Z = ((data->Z * imu->ConvToSIUnits) - imu->Offset.Z);
}

/** Power on and prepare for general usage.
 * This will activate the device and take it out of sleep mode (which must be done
 * after start-up). This function also sets both the accelerometer and the gyroscope
 * to their most sensitive settings, namely +/- 2g and +/- 250 degrees/sec, and sets
 * the clock source to use the X Gyro for reference, which is slightly better than
 * the default internal clock source.
 */
void MPU6000Init(struct I2CMember* mpu)
{
	//Accelerometer conversion factors
	//2g, 16384 LSB/mg
	//4g, 8192 LSB/mg
	//8g, 4096 LSB/mg
	//16g, 2048 LSB/mg

	//Gyroscope conversion factors
	//250°/s, 131 LSB/°/s
	//500°/s, 65.5 LSB/°/s
	//1000°/s, 32.8 LSB/°/s
	//2000°/s, 16.4 LSB/°/s
	//LEDOn(LED5);
	SetClockSource(mpu, MPU6050_CLOCK_PLL_XGYRO);
	//LEDOn(LED6);
	SetSleepEnabled(mpu, false);
	//LEDOn(LED6);
}

void MPU6000ConfigAcc(struct AccI2C* acc, enum MPU6050AccSensitivity sensitivity){
	SetFullScaleAccelRange(&acc->I2CInfo, sensitivity);
	switch(sensitivity)
	{
	case Max2g:
		acc->ConvToSIUnits = G/8192.0f/2.0; //2g, 16384 LSB/mg
		break;
	case Max4g:
		acc->ConvToSIUnits = G/4096.0f/2.0;
		break;
	case Max8g:
		acc->ConvToSIUnits = G/2048.0f/2.0;
		break;
	case Max16g:
		acc->ConvToSIUnits = G/1024.0f/2.0;
		break;
	}
}

float DegToRad(float deg){
	return PI*deg/180.0f;
}

float RadToDeg(float rad){
	return rad*180.0f/PI;
}

void MPU6000ConfigGyro(struct GyroI2C* gyro, enum MPU6050GyroSensitivity sensitivity)
{
	SetFullScaleGyroRange(&gyro->I2CInfo, sensitivity);
	switch(sensitivity)
	{
	case Max250DegPerS:
		gyro->ConvToSIUnits = PI/(180.0f*131.0f); //131 LSB/°/s
		break;
	case Max500DegPerS:
		gyro->ConvToSIUnits = PI/(180.0f*65.5f);
		break;
	case Max1000DegPerS:
		gyro->ConvToSIUnits = PI/(180.0f*32.8f);
		break;
	case Max2000DegPerS:
		gyro->ConvToSIUnits = PI/(180.0f*16.4f);
		break;
	}
	gyro->Offset = (Vector3){ 0, 0, 0 };
}


/** Set sleep mode status.
 * @param enabled New sleep mode enabled status
 * @see getSleepEnabled()
 * @see MPU6050_RA_PWR_MGMT_1
 * @see MPU6050_PWR1_SLEEP_BIT
 */
void SetSleepEnabled(struct I2CMember* mpu, bool enabled) {
    WriteBit(mpu->I2CBus, mpu->I2CAddress, MPU6050_RA_PWR_MGMT_1, MPU6050_PWR1_SLEEP_BIT, enabled);
}

/** Set full-scale accelerometer range.
 * @param range New full-scale accelerometer range setting
 * @see getFullScaleAccelRange()
 */
void SetFullScaleAccelRange(struct I2CMember* imu, uint8_t range) {
    WriteBits(imu->I2CBus, imu->I2CAddress, MPU6050_RA_ACCEL_CONFIG, MPU6050_ACONFIG_AFS_SEL_BIT, MPU6050_ACONFIG_AFS_SEL_LENGTH, range);
}

/** Set full-scale gyroscope range.
 * @param range New full-scale gyroscope range value
 * @see getFullScaleRange()
 * @see MPU6050_GYRO_FS_250
 * @see MPU6050_RA_GYRO_CONFIG
 * @see MPU6050_GCONFIG_FS_SEL_BIT
 * @see MPU6050_GCONFIG_FS_SEL_LENGTH
 */
void SetFullScaleGyroRange(struct I2CMember* imu, uint8_t range) {
    WriteBits(imu->I2CBus, imu->I2CAddress, MPU6050_RA_GYRO_CONFIG, MPU6050_GCONFIG_FS_SEL_BIT, MPU6050_GCONFIG_FS_SEL_LENGTH, range);
}

//http://www.invensense.com/mems/gyro/documents/RM-MPU-6000A.pdf
//Page 13 bottom
//ACCEL_HPF Filter Mode Cut-off Frequency
//0	Reset	None
//1	On	5Hz
//2	On	2.5Hz
//3	On	1.25Hz
//4	On	0.63Hz
//7	Hold	None
//void SetAccFilter(struct I2CMember* imu, uint8_t index) {
//    WriteBits(imu->I2CBus, imu->I2CAddress, MPU6050_RA_ACCEL_CONFIG, MPU6050_ACONFIG_ACCEL_HPF_BIT, MPU6050_ACONFIG_ACCEL_HPF_LENGTH, index);
//}


//http://www.invensense.com/mems/gyro/documents/RM-MPU-6000A.pdf
//Page 11 bottom
//DLPF_CFG Accelerometer (Fs = 1kHz) Gyroscope
//Bandwidth (Hz) Delay (ms) Bandwidth (Hz) Delay (ms) Fs (kHz)
//0	260		0		256		0.98	8
//1	184		2.0		188		1.9		1
//2	94		3.0		98		2.8		1
//3	44		4.9		42		4.8		1
//4	21		8.5		20		8.3		1
//5	10		13.8	10		13.4	1
//6	5		19.0	5		18.6	1
//7	RESERVED		RESERVED		8
void SetFilter(struct I2CMember* imu, uint8_t range) {
    WriteBits(imu->I2CBus, imu->I2CAddress, 0x1A, MPU6050_CFG_DLPF_CFG_BIT, MPU6050_CFG_DLPF_CFG_LENGTH, range);
	//#define MPU6050_CFG_DLPF_CFG_BIT    2
	//#define MPU6050_CFG_DLPF_CFG_LENGTH 3
}

/** Set clock source setting.
 * An internal 8MHz oscillator, gyroscope based clock, or external sources can
 * be selected as the MPU-60X0 clock source. When the internal 8 MHz oscillator
 * or an external source is chosen as the clock source, the MPU-60X0 can operate
 * in low power modes with the gyroscopes disabled.
 *
 * Upon power up, the MPU-60X0 clock source defaults to the internal oscillator.
 * However, it is highly recommended that the device be configured to use one of
 * the gyroscopes (or an external clock source) as the clock reference for
 * improved stability. The clock source can be selected according to the following table:
 *
 * <pre>
 * CLK_SEL | Clock Source
 * --------+--------------------------------------
 * 0       | Internal oscillator
 * 1       | PLL with X Gyro reference
 * 2       | PLL with Y Gyro reference
 * 3       | PLL with Z Gyro reference
 * 4       | PLL with external 32.768kHz reference
 * 5       | PLL with external 19.2MHz reference
 * 6       | Reserved
 * 7       | Stops the clock and keeps the timing generator in reset
 * </pre>
 *
 * @param source New clock source setting
 * @see getClockSource()
 * @see MPU6050_RA_PWR_MGMT_1
 * @see MPU6050_PWR1_CLKSEL_BIT
 * @see MPU6050_PWR1_CLKSEL_LENGTH
 */
void SetClockSource(struct I2CMember* mpu, uint8_t source) {
    WriteBits(mpu->I2CBus, mpu->I2CAddress, MPU6050_RA_PWR_MGMT_1, MPU6050_PWR1_CLKSEL_BIT, MPU6050_PWR1_CLKSEL_LENGTH, source);
}


// ACCEL_*OUT_* registers

/** Get raw 6-axis motion sensor readings (accel/gyro).
 * Retrieves all currently available motion sensor values.
 * @param ax 16-bit signed integer container for accelerometer X-axis value
 * @param ay 16-bit signed integer container for accelerometer Y-axis value
 * @param az 16-bit signed integer container for accelerometer Z-axis value
 * @param gx 16-bit signed integer container for gyroscope X-axis value
 * @param gy 16-bit signed integer container for gyroscope Y-axis value
 * @param gz 16-bit signed integer container for gyroscope Z-axis value
 * @see getAcceleration()
 * @see getRotation()
 * @see MPU6050_RA_ACCEL_XOUT_H
 */
/*void GetMotion6(I2C_TypeDef* I2CBus, uint8_t I2CAddress, int16_t* ax, int16_t* ay, int16_t* az, int16_t* gx, int16_t* gy, int16_t* gz) {
	uint8_t buffer[14];
	//ReadBytes(devAddr, MPU6050_RA_ACCEL_XOUT_H, 14, buffer);
	MPU6000ReadBytes(I2CBus, I2CAddress, MPU6050_RA_ACCEL_XOUT_H, 14, buffer);

	*ax = (((int16_t)buffer[0]) << 8) | buffer[1];
    *ay = (((int16_t)buffer[2]) << 8) | buffer[3];
    *az = (((int16_t)buffer[4]) << 8) | buffer[5];
    // *temp = (((int16_t)buffer[6]) << 8) | buffer[7]; //These two bytes belong to the temperature measurement
    *gx = (((int16_t)buffer[8]) << 8) | buffer[9];
    *gy = (((int16_t)buffer[10]) << 8) | buffer[11];
    *gz = (((int16_t)buffer[12]) << 8) | buffer[13];
}*/


/** Get 3-axis accelerometer readings.
 * These registers store the most recent accelerometer measurements.
 * Accelerometer measurements are written to these registers at the Sample Rate
 * as defined in Register 25.
 *
 * The accelerometer measurement registers, along with the temperature
 * measurement registers, gyroscope measurement registers, and external sensor
 * data registers, are composed of two sets of registers: an internal register
 * set and a user-facing read register set.
 *
 * The data within the accelerometer sensors' internal register set is always
 * updated at the Sample Rate. Meanwhile, the user-facing read register set
 * duplicates the internal register set's data values whenever the serial
 * interface is idle. This guarantees that a burst read of sensor registers will
 * read measurements from the same sampling instant. Note that if burst reads
 * are not used, the user is responsible for ensuring a set of single byte reads
 * correspond to a single sampling instant by checking the Data Ready interrupt.
 *
 * Each 16-bit accelerometer measurement has a full scale defined in ACCEL_FS
 * (Register 28). For each full scale setting, the accelerometers' sensitivity
 * per LSB in ACCEL_xOUT is shown in the table below:
 *
 * <pre>
 * AFS_SEL | Full Scale Range | LSB Sensitivity
 * --------+------------------+----------------
 * 0       | +/- 2g           | 8192 LSB/mg			PROBABLY ALL WRONG, see datasheet
 * 1       | +/- 4g           | 4096 LSB/mg
 * 2       | +/- 8g           | 2048 LSB/mg
 * 3       | +/- 16g          | 1024 LSB/mg
 * </pre>
 *
 * @param x 16-bit signed integer container for X-axis acceleration
 * @param y 16-bit signed integer container for Y-axis acceleration
 * @param z 16-bit signed integer container for Z-axis acceleration
 * @see MPU6050_RA_GYRO_XOUT_H
 */
bool GetAcceleration(struct AccI2C* acc, Vector3I16* data) {
	uint8_t buffer[6];
	if(MPU6000ReadBytes(acc->I2CInfo.I2CBus, acc->I2CInfo.I2CAddress, MPU6050_RA_ACCEL_XOUT_H, 6, buffer)){
		data->X = (((int16_t)buffer[0]) << 8) | buffer[1];
		data->Y = (((int16_t)buffer[2]) << 8) | buffer[3];
		data->Z = (((int16_t)buffer[4]) << 8) | buffer[5];
		return true;
	}else{
#ifndef IMU_SILENT
		printf("missed IMU measurement.\n\r");
#endif
		return false;
	}
}
/** Get X-axis accelerometer reading.
 * @return X-axis acceleration measurement in 16-bit 2's complement format
 * @see getMotion6()
 * @see MPU6050_RA_ACCEL_XOUT_H
 */
int16_t GetAccelerationX(struct AccI2C* acc) {
	uint8_t buffer[2];
	MPU6000ReadBytes(acc->I2CInfo.I2CBus, acc->I2CInfo.I2CAddress, MPU6050_RA_ACCEL_XOUT_H, 2, buffer);
    return (((int16_t)buffer[0]) << 8) | buffer[1];
}
/** Get Y-axis accelerometer reading.
 * @return Y-axis acceleration measurement in 16-bit 2's complement format
 * @see getMotion6()
 * @see MPU6050_RA_ACCEL_YOUT_H
 */
int16_t GetAccelerationY(struct AccI2C* acc) {
	uint8_t buffer[2];
	MPU6000ReadBytes(acc->I2CInfo.I2CBus, acc->I2CInfo.I2CAddress, MPU6050_RA_ACCEL_YOUT_H, 2, buffer);
    return (((int16_t)buffer[0]) << 8) | buffer[1];
}
/** Get Z-axis accelerometer reading.
 * @return Z-axis acceleration measurement in 16-bit 2's complement format
 * @see getMotion6()
 * @see MPU6050_RA_ACCEL_ZOUT_H
 */
int16_t GetAccelerationZ(struct AccI2C* acc) {
	uint8_t buffer[2];
	MPU6000ReadBytes(acc->I2CInfo.I2CBus, acc->I2CInfo.I2CAddress, MPU6050_RA_ACCEL_ZOUT_H, 2, buffer);
    return (((int16_t)buffer[0]) << 8) | buffer[1];
}

// TEMP_OUT_* registers

/** Get current internal temperature.
 * @return Temperature reading in 16-bit 2's complement format
 * @see MPU6050_RA_TEMP_OUT_H
 */
int16_t GetTemperature(I2C_TypeDef* I2CBus, uint8_t I2CAddress) {
	uint8_t buffer[2];
	MPU6000ReadBytes(I2CBus, I2CAddress, MPU6050_RA_TEMP_OUT_H, 2, buffer);
    return (((int16_t)buffer[0]) << 8) | buffer[1];
}

// GYRO_*OUT_* registers

/** Get 3-axis gyroscope readings.
 * These gyroscope measurement registers, along with the accelerometer
 * measurement registers, temperature measurement registers, and external sensor
 * data registers, are composed of two sets of registers: an internal register
 * set and a user-facing read register set.
 * The data within the gyroscope sensors' internal register set is always
 * updated at the Sample Rate. Meanwhile, the user-facing read register set
 * duplicates the internal register set's data values whenever the serial
 * interface is idle. This guarantees that a burst read of sensor registers will
 * read measurements from the same sampling instant. Note that if burst reads
 * are not used, the user is responsible for ensuring a set of single byte reads
 * correspond to a single sampling instant by checking the Data Ready interrupt.
 *
 * Each 16-bit gyroscope measurement has a full scale defined in FS_SEL
 * (Register 27). For each full scale setting, the gyroscopes' sensitivity per
 * LSB in GYRO_xOUT is shown in the table below:
 *
 * <pre>
 * FS_SEL | Full Scale Range   | LSB Sensitivity
 * -------+--------------------+----------------
 * 0      | +/- 250 degrees/s  | 131 LSB/deg/s
 * 1      | +/- 500 degrees/s  | 65.5 LSB/deg/s
 * 2      | +/- 1000 degrees/s | 32.8 LSB/deg/s
 * 3      | +/- 2000 degrees/s | 16.4 LSB/deg/s
 * </pre>
 *
 * @param x 16-bit signed integer container for X-axis rotation
 * @param y 16-bit signed integer container for Y-axis rotation
 * @param z 16-bit signed integer container for Z-axis rotation
 * @see getMotion6()
 * @see MPU6050_RA_GYRO_XOUT_H
 */
bool GetRotation(struct GyroI2C* gyro, Vector3I16* data) {
	uint8_t buffer[6];
	if(MPU6000ReadBytes(gyro->I2CInfo.I2CBus, gyro->I2CInfo.I2CAddress, MPU6050_RA_GYRO_XOUT_H, 6, buffer)){
		data->X = (((int16_t)buffer[0]) << 8) | buffer[1];
		data->Y = (((int16_t)buffer[2]) << 8) | buffer[3];
		data->Z = (((int16_t)buffer[4]) << 8) | buffer[5];
		return true;
	}else{
#ifndef IMU_SILENT
		printf("Missed IMU measurement.\n\r");
#endif
		return false;
	}
}
/** Get X-axis gyroscope reading.
 * @return X-axis rotation measurement in 16-bit 2's complement format
 * @see getMotion6()
 * @see MPU6050_RA_GYRO_XOUT_H
 */
int16_t GetRotationX(struct GyroI2C* gyro) {
	uint8_t buffer[2];
	MPU6000ReadBytes(gyro->I2CInfo.I2CBus, gyro->I2CInfo.I2CAddress, MPU6050_RA_GYRO_XOUT_H, 2, buffer);
    return (((int16_t)buffer[0]) << 8) | buffer[1];
}
/** Get Y-axis gyroscope reading.
 * @return Y-axis rotation measurement in 16-bit 2's complement format
 * @see getMotion6()
 * @see MPU6050_RA_GYRO_YOUT_H
 */
int16_t GetRotationY(struct GyroI2C* gyro) {
	uint8_t buffer[2];
	MPU6000ReadBytes(gyro->I2CInfo.I2CBus, gyro->I2CInfo.I2CAddress, MPU6050_RA_GYRO_YOUT_H, 2, buffer);
    return (((int16_t)buffer[0]) << 8) | buffer[1];
}
/** Get Z-axis gyroscope reading.
 * @return Z-axis rotation measurement in 16-bit 2's complement format
 * @see getMotion6()
 * @see MPU6050_RA_GYRO_ZOUT_H
 */
int16_t GetRotationZ(struct GyroI2C* gyro) {
	uint8_t buffer[2];
	MPU6000ReadBytes(gyro->I2CInfo.I2CBus, gyro->I2CInfo.I2CAddress, MPU6050_RA_GYRO_ZOUT_H, 2, buffer);
    return (((int16_t)buffer[0]) << 8) | buffer[1];
}



//Own implementations of WriteBit and WriteBits, Information form here:
//http://www.i2cdevlib.com/docs/html/class_i2_cdev.html#a913371251b6a41520c080115650e1b59
void WriteBits(I2C_TypeDef* I2C_Bus, uint8_t devAddr, uint8_t	regAddr, uint8_t bitStart, uint8_t length, uint8_t data )
{
	//Example
	//#define MPU6050_GCONFIG_FS_SEL_BIT      4
	//#define MPU6050_GCONFIG_FS_SEL_LENGTH   2
	//WriteBits(devAddr, MPU6050_RA_GYRO_CONFIG, MPU6050_GCONFIG_FS_SEL_BIT, MPU6050_GCONFIG_FS_SEL_LENGTH, range);

	//Bit register form Datasheet page 12
	//Bit-Index:   7      6      5     4    3   2 1 0
	//           XG_ST  YG_ST  ZG_ST   FS_SEL   - - -
	uint8_t bits = data << (bitStart-length+1);

	MPU6000WriteBytes(I2C_Bus, devAddr, regAddr, bits);
}

void WriteBit(I2C_TypeDef* I2C_Bus, uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint8_t data )
{
	uint8_t bits = data << bitNum;
	MPU6000WriteBytes(I2C_Bus, devAddr, regAddr, bits);
}

void MPU6000WriteBytes(I2C_TypeDef* I2C_Bus, uint8_t I2C_Addr, uint8_t reg, uint8_t val)
{
	  /* While the bus is busy */
	  //STM_EVAL_LEDOn(LED3);
	  while(I2C_GetFlagStatus(I2C_Bus, I2C_FLAG_BUSY));
	  //STM_EVAL_LEDOff(LED3);

	  /* Send Start Condition and wait for EV5 (I2C_EVENT_MASTER_MODE_SELECT event) */
	  I2C_GenerateSTART(I2C_Bus, ENABLE);
	  while(!I2C_CheckEvent(I2C_Bus, I2C_EVENT_MASTER_MODE_SELECT)); // Test on EV5 and clear it

	  /* Send Slave Address + WRITE and wait for EV6 (I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED event) */
	  I2C_Send7bitAddress(I2C_Bus, I2C_Addr<< 1, I2C_Direction_Transmitter);
	  while(!I2C_CheckEvent(I2C_Bus, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

	  /*Send the internal address to read from: MSB of the address first and wait for EV8 (I2C_EVENT_MASTER_BYTE_TRANSMITTED event) */
	  I2C_SendData( I2C_Bus,(uint8_t)reg );
	  while(!I2C_CheckEvent(I2C_Bus, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

	  /*Send register content from: MSB of the address first and wait for EV8 (I2C_EVENT_MASTER_BYTE_TRANSMITTED event) */
	  I2C_SendData( I2C_Bus, (uint8_t)val );
	  while(!I2C_CheckEvent(I2C_Bus, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

      /* Send STOP Condition */
      I2C_GenerateSTOP(I2C_Bus, ENABLE);
      while(I2C_GetFlagStatus(I2C_Bus, I2C_FLAG_STOPF)); // stop bit flag
}

#define MAX_WAIT_ITER 100	// threshold for longest waiting time in function bool MPU6000ReadBytes
#define MAX_WAIT_ITER2 1000 	// TODO good name

/**
  * @brief  Reads 6 bytes from the I2C ITG3200.
  * @param  ITG3200_Data : pointer to the buffer containing the places data is saved.
  * @param  ReadAddr : ITG3200's internal address to read out.
  * @param  num_bytes : Number of bytes that are requested from ITG3200.
  * @retval None
  */
bool MPU6000ReadBytes(I2C_TypeDef* I2C_Bus, uint8_t I2C_Addr, uint8_t ReadAddr, uint8_t num_bytes, uint8_t* ITG3200_Data)
{


	uint8_t i;

	i=0;

    /* While the bus is busy */
    while(I2C_GetFlagStatus(I2C_Bus, I2C_FLAG_BUSY)){
    	if(i>MAX_WAIT_ITER){ return false;	}
    	i++;
    }

    i=0;
    /* Send Start Condition and wait for EV5 (I2C_EVENT_MASTER_MODE_SELECT event) */
    I2C_GenerateSTART(I2C_Bus, ENABLE);
    while(!I2C_CheckEvent(I2C_Bus, I2C_EVENT_MASTER_MODE_SELECT)){
    	if(i>MAX_WAIT_ITER){ return false; }
		i++;
    }// Test on EV5 and clear it

    i=0;
    /* Send Slave Address + WRITE and wait for EV6 (I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED event) */
    I2C_Send7bitAddress(I2C_Bus, I2C_Addr<< 1, I2C_Direction_Transmitter);
    while(!I2C_CheckEvent(I2C_Bus, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)){
    	if(i>MAX_WAIT_ITER){ return false; }
		i++;
    }

    i=0;
    /*Send the internal address to read from: MSB of the address first and wait for EV8 (I2C_EVENT_MASTER_BYTE_TRANSMITTED event) */
    I2C_SendData( I2C_Bus,ReadAddr );
    while(!I2C_CheckEvent(I2C_Bus, I2C_EVENT_MASTER_BYTE_TRANSMITTED)){
    	if(i>MAX_WAIT_ITER){ return false; }
		i++;
    }

    /* Wait for slave to acknowledge ---> added by michael m does not work!*/
    /* i=0;
    while(!I2C_CheckEvent(I2C_Bus, I2C_EVENT_SLAVE_BYTE_RECEIVED)){
    	if(i>MAX_WAIT_ITER){ return false; }
    	i++;
    }*/

    i=0;
    /* RESTART: Send STRAT condition a second time and wait for EV5 (I2C_EVENT_MASTER_MODE_SELECT event) */
    I2C_GenerateSTART(I2C_Bus, ENABLE);
    while(!I2C_CheckEvent(I2C_Bus, I2C_EVENT_MASTER_MODE_SELECT)){ // Test on EV5 and clear it
    	if(i>MAX_WAIT_ITER){ return false; }
		i++;
    }

    i=0;
    /* Send Slave Address + WRITE and wait for EV6 (I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED event) */
    I2C_Send7bitAddress(I2C_Bus, I2C_Addr<< 1, I2C_Direction_Receiver);
    while(!I2C_CheckEvent(I2C_Bus,I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)){    // Test on EV6 and clear it
    	if(i>MAX_WAIT_ITER){ return false; }
		i++;
    }

	i=0;
	uint16_t i2=0;

    /* While there is data to be read */
    while(num_bytes	&& i2 < MAX_WAIT_ITER2 )
    {
      if(num_bytes == 1)
      {
        /* Disable Acknowledgement */
        I2C_AcknowledgeConfig(I2C_Bus, DISABLE);

        /* Send STOP Condition */
        I2C_GenerateSTOP(I2C_Bus, ENABLE);
      }

      /* Test on EV7 and clear it */
      if(I2C_CheckEvent(I2C_Bus, I2C_EVENT_MASTER_BYTE_RECEIVED))
      {
    	  if (num_bytes == 2)
    	  {
    		  I2C_AcknowledgeConfig (I2C_Bus, DISABLE);		/* Disable Acknowledgement */
    	  }

        /* Read a byte from the HMC6343 */
        *ITG3200_Data = I2C_ReceiveData(I2C_Bus);

        /* Point to the next location where the byte read will be saved */
        ITG3200_Data++;

        /* Decrement the read bytes counter */
        num_bytes--;
      }

      i2++;
    }

    if( i2 >= MAX_WAIT_ITER2)
    {

    	/* Disable Acknowledgement */
		I2C_AcknowledgeConfig(I2C_Bus, DISABLE);

		/* Send STOP Condition */
		I2C_GenerateSTOP(I2C_Bus, ENABLE);


	    /* Enable Acknowledgement to be ready for another reception */
	    I2C_AcknowledgeConfig(I2C_Bus, ENABLE);



    	return false;
    }


    /* Enable Acknowledgement to be ready for another reception */
    I2C_AcknowledgeConfig(I2C_Bus, ENABLE);

    return true;
}
