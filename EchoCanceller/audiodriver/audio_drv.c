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

/* Event Callback */
static AudioDrv_Event_t CB_Event = NULL;
         
#define HANDLER(NAME)                       \
void NAME##_Handler (void) {               \
                                            \
  NAME->IRQ.Clear = 0x00000001U;          \
  __DSB();                                  \
  __ISB();                                  \
  if (CB_Event != NULL) {                   \
    CB_Event(AUDIO_DRV_EVENT_##NAME##_DATA);\
  }                                         \
}

#define INITAUDIO(NAME)               \
  NAME->Timer.Control = 0U;         \
  NAME->DMA.Control   = 0U;         \
  NAME->IRQ.Clear     = 0x00000001U;\
  NAME->IRQ.Enable    = 0x00000001U;\
  NAME->CONTROL       = 0U;

#define UNINITAUDIO(NAME)             \
  NAME->Timer.Control = 0U;         \
  NAME->DMA.Control   = 0U;         \
  NAME->IRQ.Clear     = 0x00000001U;\
  NAME->IRQ.Enable    = 0x00000000U;\
  NAME->CONTROL       = 0U;

#define INITAUDIOCASE(NAME)                            \
  case AUDIO_DRV_INTERFACE_##NAME:                     \
      if ((NAME->CONTROL & CONTROL_ENABLE_Msk) != 0U) {\
        return AUDIO_DRV_ERROR;                        \
      }                                                \
      NAME->CHANNELS    = channels;                    \
      NAME->SAMPLE_BITS = sample_bits;                 \
      NAME->SAMPLE_RATE = sample_rate;                 \
      break;

#define AUDIOSETBUFCASE(NAME)                                  \
      case AUDIO_DRV_INTERFACE_##NAME:                         \
      if ((NAME->DMA.Control & ARM_VSI_DMA_Enable_Msk) != 0U) {\
        return AUDIO_DRV_ERROR;                                \
      }                                                        \
      NAME->DMA.Address   = (uint32_t)buf;                     \
      NAME->DMA.BlockNum  = block_num;                         \
      NAME->DMA.BlockSize = block_size;                        \
      break;

#define CONTROLTX(NAME)                                                            \
  if ((control == AUDIO_DRV_CONTROL_##NAME##_DISABLE) ) {                          \
    NAME->Timer.Control = 0U;                                                      \
    NAME->DMA.Control   = 0U;                                                      \
    NAME->CONTROL       = 0U;                                                      \
  } else if ((control == AUDIO_DRV_CONTROL_##NAME##_ENABLE) ) {                    \
    NAME->CONTROL       = CONTROL_ENABLE_Msk;                                      \
    NAME->DMA.Control   = ARM_VSI_DMA_Direction_M2P |                              \
                            ARM_VSI_DMA_Enable_Msk;                                \
    sample_size = NAME->CHANNELS * ((NAME->SAMPLE_BITS + 7U) / 8U);                \
    sample_rate = NAME->SAMPLE_RATE;                                               \
    if ((sample_size == 0U) || (sample_rate == 0U)) {                              \
      NAME->Timer.Interval = 0xFFFFFFFFU;                                          \
    } else {                                                                       \
      block_size = NAME->DMA.BlockSize;                                            \
      NAME->Timer.Interval = (1000000U * (block_size / sample_size)) / sample_rate;\
    }                                                                              \
    NAME->Timer.Control = ARM_VSI_Timer_Trig_DMA_Msk |                             \
                            ARM_VSI_Timer_Trig_IRQ_Msk |                           \
                            ARM_VSI_Timer_Periodic_Msk |                           \
                            ARM_VSI_Timer_Run_Msk;                                 \
  }

#define CONTROLRX(NAME)                                                            \
  if ((control == AUDIO_DRV_CONTROL_##NAME##_DISABLE) ) {                          \
    NAME->Timer.Control = 0U;                                                      \
    NAME->DMA.Control   = 0U;                                                      \
    NAME->CONTROL       = 0U;                                                      \
  } else if ((control == AUDIO_DRV_CONTROL_##NAME##_ENABLE) ) {                    \
    NAME->CONTROL       = CONTROL_ENABLE_Msk;                                      \
    NAME->DMA.Control   = ARM_VSI_DMA_Direction_P2M |                              \
                            ARM_VSI_DMA_Enable_Msk;                                \
    sample_size = NAME->CHANNELS * ((NAME->SAMPLE_BITS + 7U) / 8U);                \
    sample_rate = NAME->SAMPLE_RATE;                                               \
    if ((sample_size == 0U) || (sample_rate == 0U)) {                              \
      NAME->Timer.Interval = 0xFFFFFFFFU;                                          \
    } else {                                                                       \
      block_size = NAME->DMA.BlockSize;                                            \
      NAME->Timer.Interval = (1000000U * (block_size / sample_size)) / sample_rate;\
    }                                                                              \
    NAME->Timer.Control = ARM_VSI_Timer_Trig_DMA_Msk |                             \
                            ARM_VSI_Timer_Trig_IRQ_Msk |                           \
                            ARM_VSI_Timer_Periodic_Msk |                           \
                            ARM_VSI_Timer_Run_Msk;                                 \
  }


#define NEARSPEAKER ARM_VSI1
#define NEARMIC1 ARM_VSI0
#define NEARMIC2 ARM_VSI4
#define FARSPEAKER ARM_VSI3
#define FARMIC ARM_VSI2
#define TFLITE ARM_VSI5

#define NEARSPEAKER_IRQn ARM_VSI1_IRQn
#define NEARMIC1_IRQn ARM_VSI0_IRQn
#define NEARMIC2_IRQn ARM_VSI4_IRQn
#define FARSPEAKER_IRQn ARM_VSI3_IRQn
#define FARMIC_IRQn ARM_VSI2_IRQn

#define NEARSPEAKER_Handler ARM_VSI1_Handler
#define NEARMIC1_Handler ARM_VSI0_Handler
#define NEARMIC2_Handler ARM_VSI4_Handler
#define FARSPEAKER_Handler ARM_VSI3_Handler
#define FARMIC_Handler ARM_VSI2_Handler

HANDLER(NEARSPEAKER)
HANDLER(NEARMIC1)
HANDLER(NEARMIC2)
HANDLER(FARSPEAKER)
HANDLER(FARMIC)

/* Audio Peripheral registers */
#define CONTROL         Regs[0] /* Control receiver */
#define CHANNELS        Regs[1] /* Number of channels */
#define SAMPLE_BITS     Regs[2] /* Sample number of bits (8..32) */
#define SAMPLE_RATE     Regs[3] /* Sample rate (samples per second) */
#define TFLITEOUTPUT    Regs[5] /* Sample rate (samples per second) */

/* Audio Control register definitions */
#define CONTROL_ENABLE_Pos      0U                              /* CONTROL: ENABLE Position */
#define CONTROL_ENABLE_Msk      (1UL << CONTROL_ENABLE_Pos)     /* CONTROL: ENABLE Mask */

/* Driver State */
static uint8_t Initialized = 0U;

#define ENABLEIRQ(NAME)   NVIC->ISER[(((uint32_t)NAME##_IRQn) >> 5UL)] = (uint32_t)(1UL << (((uint32_t)NAME##_IRQn) & 0x1FUL));
#define DISABLEIRQ(NAME)   NVIC->ICER[(((uint32_t)NAME##_IRQn) >> 5UL)] = (uint32_t)(1UL << (((uint32_t)NAME##_IRQn) & 0x1FUL));

void writeTFLiteOutput(int val)
{
  TFLITE->TFLITEOUTPUT = val;
}

/* Initialize Audio Interface */
int32_t AudioDrv_Initialize (AudioDrv_Event_t cb_event) {

  CB_Event = cb_event;

 INITAUDIO(NEARSPEAKER)
 INITAUDIO(NEARMIC1)
 INITAUDIO(NEARMIC2)

 INITAUDIO(FARSPEAKER)
 INITAUDIO(FARMIC)


 ENABLEIRQ(NEARSPEAKER)
 ENABLEIRQ(NEARMIC1)
 ENABLEIRQ(NEARMIC2)

 ENABLEIRQ(FARSPEAKER)
 ENABLEIRQ(FARMIC)

  __DSB();
  __ISB();

  Initialized = 1U;

  return AUDIO_DRV_OK;
}

/* De-initialize Audio Interface */
int32_t AudioDrv_Uninitialize (void) {

 DISABLEIRQ(NEARSPEAKER)
 DISABLEIRQ(NEARMIC1)
 DISABLEIRQ(NEARMIC2)

 DISABLEIRQ(FARSPEAKER)
 DISABLEIRQ(FARMIC)
  __DSB();
  __ISB();

  UNINITAUDIO(NEARSPEAKER)
  UNINITAUDIO(NEARMIC1)
  UNINITAUDIO(NEARMIC2)

  UNINITAUDIO(FARSPEAKER)
  UNINITAUDIO(FARMIC)

  Initialized = 0U;

  return AUDIO_DRV_OK;
}

/* Configure Audio Interface */
int32_t AudioDrv_Configure (uint32_t interface, uint32_t channels, uint32_t sample_bits, uint32_t sample_rate) {
  uint32_t format;

  if (Initialized == 0U) {
    return AUDIO_DRV_ERROR;
  }

  if ((channels <  1U) ||
      (channels > 32U) ||
      (sample_bits <  8U) ||
      (sample_bits > 32U) ||
      (sample_rate == 0U)) {
    return AUDIO_DRV_ERROR_PARAMETER;
  }

  switch (interface) {
    INITAUDIOCASE(NEARSPEAKER)
    INITAUDIOCASE(NEARMIC1)
    INITAUDIOCASE(NEARMIC2)
    INITAUDIOCASE(FARSPEAKER)
    INITAUDIOCASE(FARMIC)
    default:
      return AUDIO_DRV_ERROR_PARAMETER;
  }

  return AUDIO_DRV_OK;
}

/* Set Audio Interface buffer */
int32_t AudioDrv_SetBuf (uint32_t interface, void *buf, uint32_t block_num, uint32_t block_size) {

  if (Initialized == 0U) {
    return AUDIO_DRV_ERROR;
  }

  switch (interface) {
    AUDIOSETBUFCASE(NEARSPEAKER)
    AUDIOSETBUFCASE(NEARMIC1)
    AUDIOSETBUFCASE(NEARMIC2)
    AUDIOSETBUFCASE(FARSPEAKER)
    AUDIOSETBUFCASE(FARMIC)
    default:
      return AUDIO_DRV_ERROR_PARAMETER;
  }

  return AUDIO_DRV_OK;
}

/* Control Audio Interface */
int32_t AudioDrv_Control (uint32_t control) {
  uint32_t sample_size;
  uint32_t sample_rate;
  uint32_t block_size;

  if (Initialized == 0U) {
    return AUDIO_DRV_ERROR;
  }

  

  CONTROLTX(NEARSPEAKER)
  CONTROLRX(NEARMIC1)
  CONTROLRX(NEARMIC2)

  CONTROLTX(FARSPEAKER)
  CONTROLRX(FARMIC)

  return AUDIO_DRV_OK;
}


