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
#ifndef _SCHEDCONFIG_H_
#define _SCHEDCONFIG_H_
/*

The AudioSink and AudioSource are requiring definitions from cmsis_os2

*/
#include "cmsis_os2.h"


#ifdef   __cplusplus
extern "C"
{
#endif

typedef struct {
ring_config_t *ringConfigFARMIC;
ring_config_t *ringConfigNEARMIC1;
ring_config_t *ringConfigNEARMIC2;
ring_config_t *ringConfigFARSPEAKER;
ring_config_t *ringConfigNEARSPEAKER;
int nbSink;
float gain;
} sched_config;

#ifdef   __cplusplus
}
#endif

#endif 


