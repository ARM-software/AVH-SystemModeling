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

#ifndef __AUDIO_DRV_H
#define __AUDIO_DRV_H

#ifdef  __cplusplus
extern "C"
{
#endif

#include <stdint.h>




#define MKAUDIOINTF(ID)        (ID) 

#define MKAUDIOCTRLENABLE(ID)  (2*ID)

#define MKAUDIOCTRLDISABLE(ID)  (2*ID+1)  

#define MKAUDIOEVENT(ID) (1<<ID)  

#define AUDIO_DRV_INTERFACE_NEARSPEAKER MKAUDIOINTF(1)
#define AUDIO_DRV_INTERFACE_NEARMIC1 MKAUDIOINTF(0)
#define AUDIO_DRV_INTERFACE_NEARMIC2 MKAUDIOINTF(4)

#define AUDIO_DRV_INTERFACE_FARSPEAKER MKAUDIOINTF(3)
#define AUDIO_DRV_INTERFACE_FARMIC MKAUDIOINTF(2)

#define AUDIO_DRV_CONTROL_NEARSPEAKER_ENABLE MKAUDIOCTRLENABLE(1)
#define AUDIO_DRV_CONTROL_NEARMIC1_ENABLE MKAUDIOCTRLENABLE(0)
#define AUDIO_DRV_CONTROL_NEARMIC2_ENABLE MKAUDIOCTRLENABLE(4)
#define AUDIO_DRV_CONTROL_FARSPEAKER_ENABLE MKAUDIOCTRLENABLE(3)
#define AUDIO_DRV_CONTROL_FARMIC_ENABLE MKAUDIOCTRLENABLE(2)

#define AUDIO_DRV_CONTROL_NEARSPEAKER_DISABLE MKAUDIOCTRLDISABLE(1)
#define AUDIO_DRV_CONTROL_NEARMIC1_DISABLE MKAUDIOCTRLDISABLE(0)
#define AUDIO_DRV_CONTROL_NEARMIC2_DISABLE MKAUDIOCTRLDISABLE(4)
#define AUDIO_DRV_CONTROL_FARSPEAKER_DISABLE MKAUDIOCTRLDISABLE(3)
#define AUDIO_DRV_CONTROL_FARMIC_DISABLE MKAUDIOCTRLDISABLE(2)

#define AUDIO_DRV_EVENT_NEARSPEAKER_DATA MKAUDIOEVENT(1)
#define AUDIO_DRV_EVENT_NEARMIC1_DATA MKAUDIOEVENT(0)
#define AUDIO_DRV_EVENT_NEARMIC2_DATA MKAUDIOEVENT(4)
#define AUDIO_DRV_EVENT_FARSPEAKER_DATA MKAUDIOEVENT(3)
#define AUDIO_DRV_EVENT_FARMIC_DATA MKAUDIOEVENT(2)


/* Return code */
#define AUDIO_DRV_OK                        (0)  ///< Operation succeeded
#define AUDIO_DRV_ERROR                     (-1) ///< Unspecified error
#define AUDIO_DRV_ERROR_BUSY                (-2) ///< Driver is busy
#define AUDIO_DRV_ERROR_TIMEOUT             (-3) ///< Timeout occurred
#define AUDIO_DRV_ERROR_UNSUPPORTED         (-4) ///< Operation not supported
#define AUDIO_DRV_ERROR_PARAMETER           (-5) ///< Parameter error

/**
\brief Audio Status
*/
typedef struct {
  uint32_t tx_active        :  1;       ///< Transmitter active
  uint32_t rx_active        :  1;       ///< Receiver active
  uint32_t reserved         : 30;
} AudioDrv_Status_t;

/**
  \fn          AudioDrv_Event_t
  \brief       Audio Events callback function type: void (*AudioDrv_Event_t) (uint32_t event
  \param[in]   event events notification mask
  \return      none
*/
typedef void (*AudioDrv_Event_t) (uint32_t event);

#define GETBUFFERHEADER(NAME)     extern uint8_t* Audio##NAME##Buffer()

GETBUFFERHEADER(NEARSPEAKER);
GETBUFFERHEADER(NEARMIC1);
GETBUFFERHEADER(NEARMIC2);
GETBUFFERHEADER(FARSPEAKER);
GETBUFFERHEADER(FARMIC);

extern void writeTFLiteOutput(int val);

extern int32_t AudioDrv_Setup(void);

/**
  \fn          int32_t AudioDrv_Initialize (AudioDrv_Event_t cb_event)
  \brief       Initialize Audio Interface.
  \param[in]   cb_event pointer to \ref AudioDrv_Event_t
  \return      return code
*/
int32_t AudioDrv_Initialize (AudioDrv_Event_t cb_event);



/**
  \fn          int32_t AudioDrv_Uninitialize (void)
  \brief       De-initialize Audio Interface.
  \return      return code
*/
int32_t AudioDrv_Uninitialize (void);

/**
  \fn          int32_t AudioDrv_Configure (uint32_t interface, uint32_t channels, uint32_t sample_bits, uint32_t sample_rate)
  \brief       Configure Audio Interface.
  \param[in]   interface   audio interface
  \param[in]   channels    number of channels
  \param[in]   sample_bits sample number of bits (8..32)
  \param[in]   sample_rate sample rate (samples per second)
  \return      return code
*/
int32_t AudioDrv_Configure (uint32_t interface, uint32_t channels, uint32_t sample_bits, uint32_t sample_rate);

/**
  \fn          int32_t AudioDrv_SetBuf (uint32_t interface, void *buf, uint32_t block_num, uint32_t block_size)
  \brief       Set Audio Interface buffer.
  \param[in]   interface   audio interface
  \param[in]   buf         pointer to buffer for audio data
  \param[in]   block_num   number of blocks in buffer (must be 2^n)
  \param[in]   block_size  block size in number of samples
  \return      return code
*/
int32_t AudioDrv_SetBuf (uint32_t interface, void *buf, uint32_t block_num, uint32_t block_size);

/**
  \fn          int32_t AudioDrv_Control (uint32_t control)
  \brief       Control Audio Interface.
  \param[in]   control operation
  \return      return code
*/
int32_t AudioDrv_Control (uint32_t control);



#ifdef  __cplusplus
}
#endif

#endif /* __AUDIO_DRV_H */
