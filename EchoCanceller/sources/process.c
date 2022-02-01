/*---------------------------------------------------------------------------
 * Copyright (c) 2020-2022 Arm Limited (or its affiliates).
 * All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *---------------------------------------------------------------------------*/
#include "process.h"
#include "speex/speex_echo.h"
#include "speex/speex_preprocess.h"
#include "os_support.h"
#include "dsp/basic_math_functions.h"


// Number of samples in input
#define DMABUFSIZE 128

// TAIL used for echo canceller filter sizing
#define TAIL 1000


#define AEC 
#define ANR 

#define NOISE_REDUCTION_LEVEL 20


SpeexEchoState *st=NULL;
SpeexPreprocessState *den=NULL;

void initSpeex()
{
   int sampleRate = 16000;
   #if defined(ANR) && defined(AEC)
   const int noiseReductionLevel=NOISE_REDUCTION_LEVEL;
   #endif
	

	
   #if defined(AEC)
    st = speex_echo_state_init(DMABUFSIZE, TAIL);
    speex_echo_ctl(st, SPEEX_ECHO_SET_SAMPLING_RATE, &sampleRate);
   #endif

   #if defined(ANR)
    den = speex_preprocess_state_init(DMABUFSIZE, sampleRate);
   #endif 

   #if defined(ANR) && defined(AEC)
    speex_preprocess_ctl(den, SPEEX_PREPROCESS_SET_ECHO_STATE, st);
	 speex_preprocess_ctl(den, SPEEX_PREPROCESS_SET_NOISE_SUPPRESS , (void*)&noiseReductionLevel);
   #endif

}

void uninitSpeex()
{
   #if defined(AEC)
      speex_echo_state_destroy(st);
   #endif 

   #if defined(ANR)
      speex_preprocess_state_destroy(den);
   #endif
}


void aec(q15_t *nearMic,
    q15_t *farMic,
    q15_t *farSpeaker,
    int inputSize)
{

   #if defined(AEC)
      speex_echo_cancellation(st, nearMic, farMic, farSpeaker);
   #else
      memcpy(farSpeaker,nearMic,sizeof(q15_t)*inputSize);
   #endif
}

extern void anr(q15_t *input,
    q15_t *output,
    int inputSize)
{
   memcpy(output,input,sizeof(q15_t)*inputSize);

   #if defined(ANR)
      speex_preprocess_run(den, output); 
   #endif
}


/*

Very simple summation beamforming

*/


void beamFormerInit()
{
}

void beamFormer(q15_t *mic1,
    q15_t *mic2,
    q15_t *output,
    int inputSize)
{
  // prescale to avoid saturation
  arm_scale_q15(mic1,0x4000,0,mic1,inputSize);
  arm_scale_q15(mic2,0x4000,0,mic2,inputSize);
  arm_add_q15(mic1,mic2,output,inputSize);
}