/*---------------------------------------------------------------------------
 * Copyright (c) 2020-2022 Arm Limited (or its affiliates). All rights reserved.
 *
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

#include "main.h"

#include "custom.h"

#include "arm_math.h"

#include "RingInit.h"
#include "RingBuffer.h"
#include "schedconfig.h"
#include "scheduler.h"
#include "AudioConfig.h"
#include "RingConfig.h"
#include <stdio.h>
#include <stdlib.h>
#include "audio_drv.h"
#include "arm_vsi.h"
#include "process.h"

#if defined(MICROSPEECH)
#include "main_functions.h"
#endif


osThreadId_t gStreamingThreadID;

osThreadId_t gMLThreadID;
osMutexId_t ml_mutex_Id=NULL; 
int32_t g_latest_audio_timestamp = 0;


static int error=0;


/* Memory to use for the ring buffer.
ALIGNED in case it may be used directly by the HW
Each subbuffer must also be 4 bytes aligned.

The sizes should be defined in a RingConfig.h header.
*/


sched_config config;


static const osThreadAttr_t audio_thread_attr = {
  .stack_size = 4096U,
	.priority = osPriorityRealtime7 
};

#if defined(MICROSPEECH)
static const osThreadAttr_t ml_thread_attr = {
  .stack_size = 4096U
};


static void ml_task (void *argument) {
  while (1) {
    mlloop();
  }
}
#endif

static void scheduler_task (void *argument) {
  (void)argument;
  uint32_t nb=0;

	if (error == 0)
  {
      nb=scheduler(&error,&config);
  }
  printf("NB SCHEDULE DONE nb=%d, Error=%d\r\n",nb,error);
  printf("STOP SIMU with NbSink %d\r\n",config.nbSink);

	uninitSpeex();
  (void)AudioDrv_Uninitialize ();
	

  free(config.ringConfigFARMIC);
  free(config.ringConfigNEARMIC1);
  free(config.ringConfigNEARMIC2);
  free(config.ringConfigFARSPEAKER);
  free(config.ringConfigNEARSPEAKER);
	
	#if defined(MICROSPEECH)
	osThreadTerminate(gMLThreadID);
  #endif
}


#define TIMEOUT 1000

/*---------------------------------------------------------------------------
 * Application initialization
 *---------------------------------------------------------------------------*/
void app_initialize (void) {
  gStreamingThreadID = osThreadNew(scheduler_task, NULL, &audio_thread_attr);
  
	#if defined(MICROSPEECH)
	gMLThreadID = osThreadNew(ml_task, NULL, &ml_thread_attr);
	mlsetup();

  ml_mutex_Id=osMutexNew(NULL) ;
  if (ml_mutex_Id == NULL)  {
    printf("Error creating mutex\n");
  }
  #endif
	

  uint8_t *audio_bufferNEARMIC1=AudioNEARMIC1Buffer();
  uint8_t *audio_bufferNEARMIC2=AudioNEARMIC2Buffer();
  uint8_t *audio_bufferNEARSPEAKER=AudioNEARSPEAKERBuffer();

  uint8_t *audio_bufferFARMIC=AudioFARMICBuffer();
  uint8_t *audio_bufferFARSPEAKER=AudioFARSPEAKERBuffer();

  config.ringConfigFARMIC=(ring_config_t*)malloc(sizeof(ring_config_t));
  config.ringConfigNEARMIC1=(ring_config_t*)malloc(sizeof(ring_config_t));
  config.ringConfigNEARMIC2=(ring_config_t*)malloc(sizeof(ring_config_t));
  config.ringConfigFARSPEAKER=(ring_config_t*)malloc(sizeof(ring_config_t));
  config.ringConfigNEARSPEAKER=(ring_config_t*)malloc(sizeof(ring_config_t));
  config.gain=2.0;
  config.nbSink=0;
	
  ringInit(config.ringConfigNEARMIC1,
    RING_NBBUFS,
    RING_BUFSIZE_RX,
    audio_bufferNEARMIC1,
    ARM_VSI0_IRQn,TIMEOUT);

  ringInit(config.ringConfigNEARMIC2,
    RING_NBBUFS,
    RING_BUFSIZE_RX,
    audio_bufferNEARMIC2,
    ARM_VSI4_IRQn,TIMEOUT);

  ringInit(config.ringConfigNEARSPEAKER,
    RING_NBBUFS,
    RING_BUFSIZE_TX,
    audio_bufferNEARSPEAKER,
    ARM_VSI1_IRQn,TIMEOUT);

  ringInit(config.ringConfigFARMIC,
    RING_NBBUFS,
    RING_BUFSIZE_RX,
    audio_bufferFARMIC,
    ARM_VSI2_IRQn,TIMEOUT);

  ringInit(config.ringConfigFARSPEAKER,
    RING_NBBUFS,
    RING_BUFSIZE_TX,
    audio_bufferFARSPEAKER,
    ARM_VSI3_IRQn,TIMEOUT);

  

  
	
	error=AudioDrv_Setup();
	
	initSpeex();
  beamFormerInit();
}
