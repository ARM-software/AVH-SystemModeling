/*---------------------------------------------------------------------------
 * Copyright (c) 2020-2022 Arm Limited (or its affiliates). All rights reserved.
 * Copyright 2018-2022 The TensorFlow Authors. All Rights Reserved.
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
#ifndef _WINDOWNODE_H_
#define _WINDOWNODE_H_

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define kFrontendWindowBits 12

#if defined(ARM_MATH_MVEI) && !defined(ARM_MATH_AUTOVECTORIZE)

static int16_t arm_win_process_samples(const int16_t * pSrc,
                                const int16_t * pCoef, uint32_t blockSize, int16_t * pResult)
{
    int32_t         blkSize = blockSize;
    int16x8_t       curExtremValVec = vdupq_n_s16(0);
    int16_t         maxValue = 0;

    int16x8_t       vecSrc = vld1q(pSrc);
    pSrc += 8;

    do {
        mve_pred16_t    p = vctp16q(blkSize);
        int16x8_t       vecDst, vecCoef;

        vecCoef = vld1q_z(pCoef, p);

        /* long multiply + narrowing */
        vecDst = vuninitializedq_s16();
        vecDst = vqshrnbq_m_n_s32(vecDst, vmullbq_int(vecSrc, vecCoef), kFrontendWindowBits, p);
        vecDst = vqshrntq_m_n_s32(vecDst, vmulltq_int(vecSrc, vecCoef), kFrontendWindowBits, p);

        vecSrc = vld1q_z(pSrc, p);

        vst1q_p(pResult, vecDst, p);

        curExtremValVec = vmaxq_m(vecDst, vabsq(vecDst), curExtremValVec, p);

        blkSize -= 8;
        pSrc += 8;
        pCoef += 8;
        pResult += 8;

    }
    while (blkSize > 0);

    return (vmaxvq(maxValue, curExtremValVec));
}
#else

static int16_t arm_win_process_samples(const int16_t * pSrc,
                                const int16_t * pCoef, uint32_t blockSize, int16_t * pResult)
{
  int max_abs_output_value = 0;
  for (uint32_t i = 0; i < blockSize; ++i) {
    int16_t new_value =
        (((int32_t)*pSrc++) * *pCoef++) >> kFrontendWindowBits;
    *pResult++ = new_value;
    if (new_value < 0) {
      new_value = -new_value;
    }
    if (new_value > max_abs_output_value) {
      max_abs_output_value = new_value;
    }
  }
  return(max_abs_output_value);
}
#endif


template<typename IN, int inputSize,
  typename OUT1,int output1Size,typename OUT2,int output2Size> class Window;

template<int inputSize>
class Window<q15_t,inputSize,q15_t,inputSize,short,1>: public GenericNode12<q15_t,inputSize,q15_t,inputSize,short,1>
{
public:
    Window(FIFOBase<q15_t> &src,FIFOBase<q15_t> &dst1,FIFOBase<short> &dst2):GenericNode12<q15_t,inputSize,q15_t,inputSize,short,1>(src,dst1,dst2)
    {
        const float arg = M_PI * 2.0 / ((float)inputSize);
        int i;
        for (i = 0; i < inputSize; ++i) {
          float float_value = 0.5 - (0.5 * cos(arg * (i + 0.5)));
          // Scale it to fixed point and round it.
          coefficients[i] =
              (q15_t)floor(float_value * (1 << kFrontendWindowBits) + 0.5);
        }
    };

    int run(){
        q15_t *a=this->getReadBuffer();
        q15_t *b=this->getWriteBuffer1();
        short *max_abs_output_value=this->getWriteBuffer2();



        max_abs_output_value[0] = 
           arm_win_process_samples(a, coefficients, inputSize, b);


        
        return(0);
    };

protected:
    q15_t coefficients[inputSize];

};

#endif