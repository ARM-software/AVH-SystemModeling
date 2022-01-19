#ifndef _WAVE_H_
#define _WAVE_H_

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#ifdef   __cplusplus
extern "C"
{
#endif



/*

Only working with int16 data and num channel 1

*/
void *openWaveForReading(char *path);
void *openWaveForWriting(char *path,uint32_t sampleRate);


/* For interfacing with Modelica */
int readWavSample(void *wave);
void writeWavSample(void *wave,int value);

double readWavSampleF32(void *wave);
void writeWavSampleF32(void *wave,double value);

void closeWave(void *wave);

#ifdef   __cplusplus
}
#endif

#endif
