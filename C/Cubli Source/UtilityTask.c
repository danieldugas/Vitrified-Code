#include "UtilityTask.h"

//typedefs
extern enum CubeModes;
extern enum BalancingMode;
extern enum JumpMode;

// Global Variables
extern uint8_t _balancingMode;
extern uint8_t _jumpMode;

extern cube_mode _modeNumber;
extern bool _balance;

bool _writeData=false;	// global variable, is true if one jump succeeded.



// Check Battery Voltage - Task
#define VoltageMonitorT 40
void VoltageMonitor(void *pvParameters){

	 	// Get the conversion value
	 	//uint16_t value = ADC_GetConversionValue(ADC1);

	portTickType xLastExecutionTime = xTaskGetTickCount();

	uint16_t value=0;
	float voltage_measured=0;	// in V
	float voltage_l=24;
	float voltage=0;
	bool setIdle=false;

	uint8_t counter=0;

	for(;;){
		DelayUntilTime(&xLastExecutionTime, VoltageMonitorT);

		value = ADC_GetConversionValue(ADC1); 		// Get the conversion value
		voltage_measured=(6.8481*(float)value)/(1000.0);				// Actual voltage around 1V lower
		voltage=voltage_measured*.5+voltage_l*.5;	// Moving average to cut away frequencies above ~3Hz.

		//printf("voltage:%f\n\r",voltage);

		// Emergency shut down!
		if(voltage<17.5){
			if(!setIdle){
				_balancingMode=NotBalancing;
				_jumpMode=NoJump;
				_balance=false;

				SetCurrent(_motor[MOTORX].NodeID, 0);
				SetCurrent(_motor[MOTORY].NodeID, 0);
				SetCurrent(_motor[MOTORZ].NodeID, 0);

				setIdle=true;
			}
			if(counter==12){ counter=0; }
			if(counter==0){ LEDToggle(LED3); }//toggle once a second.
			counter++;
		}else if(voltage>16.0){
			setIdle=false;
		}

		voltage_l=voltage;

	}
}

extern bool _shutDown;

/* Task to handle system shut downs */
#define OnButtonHandlerT 40
void OnButtonHandler(void *pvParameters){

	portTickType xLastExecutionTime = xTaskGetTickCount();
	uint8_t counter=0;

	for(;;){
		DelayUntilTime(&xLastExecutionTime, VoltageMonitorT);


		if(counter==3&&!_shutDown){
			_shutDown=true;
			SysShutDown();
		}else if(counter<=3){
			counter++;
		}
	}

}

extern const EPOSMessage _shutdown;

// Shuts Cubli down!
void SysShutDown(){
	float omegaXMeas[3]={0,0,0};

	EnC();
	_balance=false;
	_balancingMode=NotBalancing;
	_jumpMode=NoJump;
	_modeNumber=Idle;
	ExC();

	uint8_t i=0;
	for(i=0;i<3;i++){ SetCurrent(_motor[i].NodeID, 0); };

	// display communications performance data
	printf("\nCommunications Performance __\r\n");
	printf("# of messages sent     : %i\r\n",_messagesSentCount);
	printf("# of messages collected: %i\r\n",_messagesCollectedCount);
	printf("_______________________________\r\n");

	// write data to flash memory
	if(_writeData)
		LEDOn(LED3);
	WriteJumpUpData();
	LEDOff(LED3);

	// Shutdown notification message
	send_message(COM_MCODE_SHUTDOWN, COM_ID_COMPUTER, NULL, 0);

	/* wait till motor stopped! */
	for (i=0;i<3;i++){omegaXMeas[i] = -_motor[i].Velocity;};
	while(fabs(omegaXMeas[0])>20 || fabs(omegaXMeas[1])>20 || fabs(omegaXMeas[1])>20){
		for (i=0;i<3;i++) {
			omegaXMeas[i] = -_motor[i].Velocity;
			int16_t current=25*omegaXMeas[i];
			SetCurrentWSaturation(_motor[i].NodeID,&current);
		}
	}


	/* shut down motor */
	for(i=0;i<3;i++){CanSendMessage(_motor[i].NodeID,&_shutdown); };


	/* suspend tasks */
	vTaskSuspendAll();

	/* cut power off */
	GPIO_WriteBit(GPIOA,GPIO_Pin_5,Bit_RESET);

}


void FormatFlash(){

	// Clear Flags
	FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_PGAERR|FLASH_FLAG_WRPERR);

	// Use FLASH_Sector_11 and VoltageRange_3 --> write pieces of 32bit (float).
	FLASH_EraseSector(FLASH_Sector_11,VoltageRange_3);

}

void RestoreDefaultValues(){
	FLASH_Unlock();
	FormatFlash();
	FLASH_Lock();
	//FLASH_ProgramWord(0x080E0000, (uint32_t)0); // not needed.
}


// Flash memory alignment
// Start address: 0x080E0000
// Counter:		  0x080E0000	-> if zero flash has been erased otherwise 42.
// F2E Vel		  0x080E0020	(float,32bit)
// E2C Vel		  0x080E0040	(float,32bit)
//				  0x080E0060	(float,32bit)
// F2C Vel		  0x080E0080	(float,32bit)
// 				  0x080E00A0	(float,32bit)
//				  0x080E00C0	(float,32bit)
// m_K			  0x080E00E0	(float,32bit)
//				  0x080E0100	(float,32bit)
// 				  0x080E0120	(float,32bit)
void WriteJumpUpDataE(float* F2Evel, float* E2Cvel, float* F2Cvel){
	FLASH_Unlock();
	FormatFlash();

	uint32_t* tmp;
	tmp=(uint32_t*)F2Evel;

	// Clear Flags
	FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_PGAERR|FLASH_FLAG_WRPERR);

	FLASH_ProgramWord(0x080E0000, (uint32_t)42);

	// Write
	FLASH_ProgramWord(0x080E0020, *tmp);

	tmp=(uint32_t*)E2Cvel;
	FLASH_ProgramWord(0x080E0040, *tmp);
	tmp=(uint32_t*)(E2Cvel+1);
	FLASH_ProgramWord(0x080E0060, *tmp);

	tmp=(uint32_t*)F2Cvel;
	FLASH_ProgramWord(0x080E0080, *tmp);
	tmp=(uint32_t*)(F2Cvel+1);
	FLASH_ProgramWord(0x080E00A0, *tmp);
	tmp=(uint32_t*)(F2Cvel+2);
	FLASH_ProgramWord(0x080E00C0, *tmp);

	FLASH_Lock();
}

// if jump up succeeded data is written to flash memory using this function.
void WriteJumpUpData(){
	printf("\n\nwriting data_____________\n\r");
	printf("omegaF2E: %f\n\r",_cubliParameters.omegaF2E);
	printf("omegaE2C: %f \t%f \t%f\n\r",_cubliParameters.omegaE2C[0],_cubliParameters.omegaE2C[1],_cubliParameters.omegaE2C[2]);
	printf("omegaF2C: %f \t%f \t%f\n\r",_cubliParameters.omegaF2C[0],_cubliParameters.omegaF2C[1],_cubliParameters.omegaF2C[2]);
	printf("m_K: %.9f \t%.9f \t%.9f\n\r",_cubliParameters.m_K.X,_cubliParameters.m_K.Y,_cubliParameters.m_K.Z);
	printf("___________________________\n\r");

	if(_writeData){
		FLASH_Unlock();
		FormatFlash();

		uint32_t* tmp;
		tmp=(uint32_t*)(&_cubliParameters.omegaF2E);

		// Clear Flags
		FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_PGAERR|FLASH_FLAG_WRPERR);

		FLASH_ProgramWord(0x080E0000, (uint32_t)42);

		// Write
		FLASH_ProgramWord(0x080E0020, *tmp);

		tmp=(uint32_t*)_cubliParameters.omegaE2C;
		FLASH_ProgramWord(0x080E0040, *tmp);
		tmp=(uint32_t*)(_cubliParameters.omegaE2C+1);
		FLASH_ProgramWord(0x080E0060, *tmp);

		tmp=(uint32_t*)_cubliParameters.omegaF2C;
		FLASH_ProgramWord(0x080E0080, *tmp);
		tmp=(uint32_t*)(_cubliParameters.omegaF2C+1);
		FLASH_ProgramWord(0x080E00A0, *tmp);
		tmp=(uint32_t*)(_cubliParameters.omegaF2C+2);
		FLASH_ProgramWord(0x080E00C0, *tmp);

		tmp=(uint32_t*)(&_cubliParameters.m_K.X);
		FLASH_ProgramWord(0x080E00E0,*tmp);
		tmp=(uint32_t*)(&_cubliParameters.m_K.Y);
		FLASH_ProgramWord(0x080E0100,*tmp);
		tmp=(uint32_t*)(&_cubliParameters.m_K.Z);
		FLASH_ProgramWord(0x080E0120,*tmp);

		tmp=(uint32_t*)(&_cubliParameters.edgeEqAngle);
		FLASH_ProgramWord(0x080E0140,*tmp);

		FLASH_Lock();
	}
}

void ReadmK(float* mKx,float* mKy, float* mKz){
	*mKx=*((float*)0x080E00E0);
	*mKy=*((float*)0x080E0100);
	*mKz=*((float*)0x080E0120);
}

void ReadEdgeAngle(float* edgeAngle){
	*edgeAngle=*((float*)0x080E0140);
}


void ReadJumpUpData(float* F2Evel, float* E2Cvel, float* F2Cvel){
	*F2Evel=*((float*)0x080E0020);
	E2Cvel[0]=*((float*)0x080E0040);
	E2Cvel[1]=*((float*)0x080E0060);
	F2Cvel[0]=*((float*)0x080E0080);
	F2Cvel[1]=*((float*)0x080E00A0);
	F2Cvel[2]=*((float*)0x080E00C0);
}


void GetF2EVelocity(float* F2Evel){
	(*F2Evel)=*((float*)0x080E0020);
}

void GetE2CVelocity(float* E2Cvel){
	E2Cvel[0]=*((float*)(0x080E0040));
	E2Cvel[1]=*((float*)(0x080E0060));
}

void GetF2CVelocity(float* F2Cvel){
	F2Cvel[0]=*((float*)(0x080E0080));
	F2Cvel[1]=*((float*)(0x080E00A0));
	F2Cvel[2]=*((float*)(0x080E00C0));
}

bool HasFlashBeenErased(){
	uint32_t fl=*((uint32_t*)0x080E0000);
	if(fl==(uint32_t)42){			//42 is the answer to the ultimate question
		return false;
	}else{
		return true;
	}
}
