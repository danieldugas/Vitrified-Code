#ifndef UTILITYTASK_H
#define UTILITYTASK_H


#include "Peripherals.h"
#include "Controllers.h"

void VoltageMonitor(void *pvParameters);
void OnButtonHandler(void *pvParameters);
void SysShutDown();

void FormatFlash();
void GetF2EVelocity(float* F2Evel);
void GetE2CVelocity(float* E2Cvel);
void GetF2CVelocity(float* F2Cvel);
bool HasFlashBeenErased();

void WriteJumpUpDataE(float* F2Evel, float* E2Cvel, float* F2Cvel);
void ReadJumpUpData(float* F2Evel, float* E2Cvel, float* F2Cvel);
void WriteJumpUpData();
void RestoreDefaultValues();

void ReadmK(float* mKx,float* mKy, float* mKz);
void ReadEdgeAngle(float* edgeAngle);

#endif
