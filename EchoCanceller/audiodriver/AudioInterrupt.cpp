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
#include <stddef.h>
#include "audio_drv.h"
#include "arm_vsi.h"
#ifdef _RTE_
#include "RTE_Components.h"
#endif
#include CMSIS_device_header

#include "cmsis_os2.h"



#include "RingBuffer.h"

#include "arm_math.h"

#include "SchedEvents.h"
#include "AudioConfig.h"
#include "RingConfig.h"

#include "RingInit.h"

#include "schedconfig.h"

extern osThreadId_t gStreamingThreadID;



// Number of bytes read by DMA
#define AUDIO_BLOCK_SIZE_NEARSPEAKER    RING_BUFSIZE_TX
#define AUDIO_BLOCK_SIZE_NEARMIC1       RING_BUFSIZE_RX
#define AUDIO_BLOCK_SIZE_NEARMIC2       RING_BUFSIZE_RX
#define AUDIO_BLOCK_SIZE_FARSPEAKER     RING_BUFSIZE_TX
#define AUDIO_BLOCK_SIZE_FARMIC         RING_BUFSIZE_RX

#define AUDIO_DRV_NBCHANNELS_NEARSPEAKER    AUDIO_DRV_NBCHANNELS_TX
#define AUDIO_DRV_NBCHANNELS_NEARMIC1       AUDIO_DRV_NBCHANNELS_RX
#define AUDIO_DRV_NBCHANNELS_NEARMIC2       AUDIO_DRV_NBCHANNELS_RX
#define AUDIO_DRV_NBCHANNELS_FARSPEAKER     AUDIO_DRV_NBCHANNELS_TX
#define AUDIO_DRV_NBCHANNELS_FARMIC         AUDIO_DRV_NBCHANNELS_RX

#define AUDIO_DRV_CHANNEL_ENCODING_NEARSPEAKER    AUDIO_DRV_CHANNEL_ENCODING_TX
#define AUDIO_DRV_CHANNEL_ENCODING_NEARMIC1       AUDIO_DRV_CHANNEL_ENCODING_RX
#define AUDIO_DRV_CHANNEL_ENCODING_NEARMIC2       AUDIO_DRV_CHANNEL_ENCODING_RX
#define AUDIO_DRV_CHANNEL_ENCODING_FARSPEAKER     AUDIO_DRV_CHANNEL_ENCODING_TX
#define AUDIO_DRV_CHANNEL_ENCODING_FARMIC         AUDIO_DRV_CHANNEL_ENCODING_RX

#define AUDIO_DRV_SAMPLINGFREQUENCY_NEARSPEAKER    AUDIO_DRV_SAMPLINGFREQUENCY_TX
#define AUDIO_DRV_SAMPLINGFREQUENCY_NEARMIC1       AUDIO_DRV_SAMPLINGFREQUENCY_RX
#define AUDIO_DRV_SAMPLINGFREQUENCY_NEARMIC2       AUDIO_DRV_SAMPLINGFREQUENCY_RX
#define AUDIO_DRV_SAMPLINGFREQUENCY_FARSPEAKER     AUDIO_DRV_SAMPLINGFREQUENCY_TX
#define AUDIO_DRV_SAMPLINGFREQUENCY_FARMIC         AUDIO_DRV_SAMPLINGFREQUENCY_RX

// Number of DMA blocks
#define AUDIO_DMA_NB_BLOCKS RING_NBBUFS

extern sched_config config;

#define AUDIOBUFFER(NAME)                                                                     \
__attribute__((section("dma")))                                                               \
__ALIGNED(16) static uint8_t audio_buffer##NAME[AUDIO_DMA_NB_BLOCKS*AUDIO_BLOCK_SIZE_##NAME]; \
static uint8_t *reservedBuf##NAME=NULL;


AUDIOBUFFER(NEARSPEAKER)
AUDIOBUFFER(NEARMIC1)
AUDIOBUFFER(NEARMIC2)
AUDIOBUFFER(FARSPEAKER)
AUDIOBUFFER(FARMIC)

#define DEFGETBUFFER(NAME)       \
uint8_t* Audio##NAME##Buffer()\
{                             \
  return(audio_buffer##NAME); \
}

DEFGETBUFFER(NEARSPEAKER)
DEFGETBUFFER(NEARMIC1)
DEFGETBUFFER(NEARMIC2)
DEFGETBUFFER(FARSPEAKER)
DEFGETBUFFER(FARMIC)


#define RXEVENT(RXNAME)                                                           \
if (event & AUDIO_DRV_EVENT_##RXNAME##_DATA)                                               \
  {                                                                                      \
                                                                                         \
    ringInterruptReleaseBuffer(config.ringConfig##RXNAME,(void *)gStreamingThreadID);    \
    int reserved##RXNAME=ringInterruptReserveBuffer(config.ringConfig##RXNAME);          \
    reservedBuf##RXNAME=ringGetBufferAddress(config.ringConfig##RXNAME,reserved##RXNAME);\
  }






#define TXEVENT(TXNAME)                                                     \
  if (event & AUDIO_DRV_EVENT_##TXNAME##_DATA)                                     \
  {                                                                                \
                                                                                   \
    ringInterruptReleaseBuffer(config.ringConfig##TXNAME,(void *)gStreamingThreadID);    \
    int reserved##TXNAME=ringInterruptReserveBuffer(config.ringConfig##TXNAME);          \
    reservedBuf##TXNAME=ringGetBufferAddress(config.ringConfig##TXNAME,reserved##TXNAME);\
  }



#define ENABLEDMA(NAME)                                     \
  ret = AudioDrv_Control(AUDIO_DRV_CONTROL_##NAME##_ENABLE);\
  if (ret != 0) {                                           \
    return ret;                                             \
  }

static void AudioEvent (uint32_t event) {

  RXEVENT(NEARMIC1);
  RXEVENT(NEARMIC2);
  RXEVENT(FARMIC);

  TXEVENT(NEARSPEAKER);
  TXEVENT(FARSPEAKER);

}

#define CONFIGUREAUDIO(NAME)                                                           \
ret = AudioDrv_Configure(AUDIO_DRV_INTERFACE_##NAME,                                   \
                           AUDIO_DRV_NBCHANNELS_##NAME,  /* single channel */          \
                           8U * AUDIO_DRV_CHANNEL_ENCODING_##NAME, /* 16 sample bits */\
                           static_cast<uint32_t>(AUDIO_DRV_SAMPLINGFREQUENCY_##NAME)); \
  if (ret != 0) {                                                                      \
    return ret;                                                                        \
  }



#define CONFIGURERXAUDIOBUFFER(NAME)                                                     \
  int reserved##NAME=ringInterruptReserveBuffer(config.ringConfig##NAME);                \
  reservedBuf##NAME=ringGetBufferAddress(config.ringConfig##NAME,reserved##NAME);        \
                                                                                         \
                                                                                         \
  ret = AudioDrv_SetBuf(AUDIO_DRV_INTERFACE_##NAME,                                      \
                        audio_buffer##NAME, AUDIO_DMA_NB_BLOCKS,AUDIO_BLOCK_SIZE_##NAME);\
  if (ret != 0) {                                                                        \
    return ret;                                                                          \
  }





  #define CONFIGURETXAUDIOBUFFER(NAME)                                     \
  {                                                                        \
  /* dataflow must be 1 packet ahead of the TX interrupt*/                 \
  ringUserReserveBuffer(config.ringConfig##NAME);                                \
  ringUserReleaseBuffer(config.ringConfig##NAME);                                \
                                                                           \
  ringUserReserveBuffer(config.ringConfig##NAME);                                \
  ringUserReleaseBuffer(config.ringConfig##NAME);                                \
                                                                           \
  int reserved##NAME=ringInterruptReserveBuffer(config.ringConfig##NAME);        \
  reservedBuf##NAME=ringGetBufferAddress(config.ringConfig##NAME,reserved##NAME);\
  }                                                                        \
                                                                           \
  ret = AudioDrv_SetBuf(AUDIO_DRV_INTERFACE_##NAME,                        \
                        audio_buffer##NAME, AUDIO_DMA_NB_BLOCKS ,AUDIO_BLOCK_SIZE_##NAME);




int32_t AudioDrv_Setup(void) {
  int32_t ret;

  ret = AudioDrv_Initialize(AudioEvent);
  if (ret != 0) {
    return ret;
  }

/* Microphone configuration */

  CONFIGUREAUDIO(NEARMIC1)
  CONFIGUREAUDIO(NEARMIC2)
  CONFIGUREAUDIO(FARMIC)

  CONFIGURERXAUDIOBUFFER(NEARMIC1)
  CONFIGURERXAUDIOBUFFER(NEARMIC2)
  CONFIGURERXAUDIOBUFFER(FARMIC)

 
  ENABLEDMA(NEARMIC1);
  ENABLEDMA(NEARMIC2);
  ENABLEDMA(FARMIC);


/* Speaker configuration */

  CONFIGUREAUDIO(NEARSPEAKER)
  CONFIGUREAUDIO(FARSPEAKER)


  /* Work because user process not started yet
  */
  CONFIGURETXAUDIOBUFFER(NEARSPEAKER)
  CONFIGURETXAUDIOBUFFER(FARSPEAKER)

 
  ENABLEDMA(NEARSPEAKER)
  ENABLEDMA(FARSPEAKER)

  return 0;
}

