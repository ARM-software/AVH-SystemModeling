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
#ifndef _PROCESS_H_
#define _PROCESS_H_

#include "arm_math_types.h"

#ifdef   __cplusplus
extern "C"
{
#endif

void initSpeex();
void uninitSpeex();

extern void aec(q15_t *nearMic,
    q15_t *farMic,
    q15_t *farSpeaker,
    int inputSize);

extern void anr(q15_t *input,
    q15_t *output,
    int inputSize);

extern void beamFormer(q15_t *mic1,
    q15_t *mic2,
    q15_t *output,
    int inputSize);

extern void beamFormerInit();

#ifdef   __cplusplus
}
#endif

#endif