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
#ifndef _CLAMP_H_
#define _CLAMP_H_


template<typename IN, int inputSize,
  typename OU1,int outputSize> class Clamp;

template<int inputSize>
class Clamp<q15_t,inputSize,q7_t,inputSize>: public GenericNode<q15_t,inputSize,q7_t,inputSize>
{
public:
    Clamp(FIFOBase<q15_t> &src,FIFOBase<q7_t> &dst):GenericNode<q15_t,inputSize,q7_t,inputSize>(src,dst)
    {
       
    };

    int run(){
        q15_t *a=this->getReadBuffer();
        q7_t *b=this->getWriteBuffer();

        constexpr int32_t value_scale = 256;
        constexpr int32_t value_div = static_cast<int32_t>((25.6f * 26.0f) + 0.5f);
        
        for (size_t i = 0; i < inputSize; ++i) 
        {
           // These scaling values are derived from those used in input_data.py in the
           // training pipeline.
           // The feature pipeline outputs 16-bit signed integers in roughly a 0 to 670
           // range. In training, these are then arbitrarily divided by 25.6 to get
           // float values in the rough range of 0.0 to 26.0. This scaling is performed
           // for historical reasons, to match up with the output of other feature
           // generators.
           // The process is then further complicated when we quantize the model. This
           // means we have to scale the 0.0 to 26.0 real values to the -128 to 127
           // signed integer numbers.
           // All this means that to get matching values from our integer feature
           // output into the tensor input, we have to perform:
           // input = (((feature / 25.6) / 26.0) * 256) - 128
           // To simplify this and perform it in 32-bit integer math, we rearrange to:
           // input = (feature * 256) / (25.6 * 26.0) - 128
    
           int32_t value = ((a[i] * value_scale) + (value_div / 2)) / value_div;
           value -= 128;
           if (value < -128) {
              value = -128;
           }
           if (value > 127) {
             value = 127;
           }
           b[i] = value;
        }


        return(0);
    };


};

#endif