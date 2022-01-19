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
#ifndef _MFCCNODE_H_
#define _MFCCNODE_H_

#include "microfrontend/lib/fft.h"
#include "microfrontend/lib/fft_util.h"
#include "microfrontend/lib/filterbank.h"
#include "microfrontend/lib/filterbank_util.h"
#include "microfrontend/lib/log_scale.h"
#include "microfrontend/lib/log_scale_util.h"
#include "microfrontend/lib/bits.h"
#include "microfrontend/lib/noise_reduction.h"
#include "microfrontend/lib/noise_reduction_util.h"
#include "microfrontend/lib/pcan_gain_control.h"
#include "microfrontend/lib/pcan_gain_control_util.h"


template<typename IN1, int input1Size,typename IN2, int input2Size,
  typename OUT,int outputSize> class MFCC;

template<int inputSize,int outputSize>
class MFCC<q15_t,inputSize,short,1,q15_t,outputSize>: 
public GenericNode21<q15_t,inputSize,short,1,q15_t,outputSize>
{
public:
    MFCC(FIFOBase<q15_t> &src1,FIFOBase<short> &src2,FIFOBase<q15_t> &dst,int sample_rate,int fftlen):
    GenericNode21<q15_t,inputSize,short,1,q15_t,outputSize>(src1,src2,dst),mFFTLen(fftlen)
    {

       ffttempout = (struct complex_int16_t *)malloc((fftlen+2)*sizeof(struct complex_int16_t));
     
       fftconfig = FftPopulateState(mFFTLen);

       FilterbankFillConfigWithDefaults(&filterBankconfig);
       filterBankconfig.num_channels = outputSize;
       filterBankconfig.lower_band_limit = 125.0;
       filterBankconfig.upper_band_limit = 7500.0;

       FilterbankPopulateState(&filterBankconfig, &filterBankState,
                               sample_rate, mFFTLen / 2 + 1);

       NoiseReductionFillConfigWithDefaults(&noiseReductionConfig);
       noiseReductionConfig.smoothing_bits = 10;
       noiseReductionConfig.even_smoothing = 0.025;
       noiseReductionConfig.odd_smoothing = 0.06;
       noiseReductionConfig.min_signal_remaining = 0.05;
       NoiseReductionPopulateState(&noiseReductionConfig,
                                   &noiseReductionState,
                                   filterBankState.num_channels);

       
       int input_correction_bits =
         MostSignificantBit32(mFFTLen) - 1 - (kFilterbankBits / 2);

       PcanGainControlFillConfigWithDefaults(&pcanGainControlConfig);
       pcanGainControlConfig.enable_pcan = 1;
       pcanGainControlConfig.strength = 0.95;
       pcanGainControlConfig.offset = 80.0;
       pcanGainControlConfig.gain_bits = 21;
       PcanGainControlPopulateState(
          &pcanGainControlConfig, &pcanGainControlState,
          noiseReductionState.estimate, filterBankState.num_channels,
          noiseReductionState.smoothing_bits, input_correction_bits);

       LogScaleFillConfigWithDefaults(&logScaleConfig);
       logScaleConfig.enable_log = 1;
       logScaleConfig.scale_shift = 6;

       LogScalePopulateState(&logScaleConfig,&logScaleState);

    };

    int run(){
        q15_t *in=this->getReadBuffer1();
        short *max_abs_output_value=this->getReadBuffer2();
        q15_t *out=this->getWriteBuffer();


        // Apply the FFT to the window's output (and scale it so that the fixed point
        // FFT can have as much resolution as possible).
        int input_shift = 15 - MostSignificantBit32((uint32_t)max_abs_output_value[0]);

        FftCompute(fftconfig,in,inputSize, mFFTLen,ffttempout,input_shift);

        // We can re-use the fft's output buffer to hold the energy.
        int32_t* energy = (int32_t*)ffttempout;

        FilterbankConvertFftComplexToEnergy(&filterBankState, ffttempout,energy);


        FilterbankAccumulateChannels(&filterBankState, energy);
        uint32_t* scaled_filterbank = FilterbankSqrt(&filterBankState, input_shift);



        // Apply noise reduction.
        NoiseReductionApply(&noiseReductionState, scaled_filterbank);

        if (pcanGainControlState.enable_pcan) {
            PcanGainControlApply(&pcanGainControlState, scaled_filterbank);
        }

 
        // Apply the log and scale.
        int correction_bits =
           MostSignificantBit32(mFFTLen) - 1 - (kFilterbankBits / 2);
        uint16_t* logged_filterbank =
            LogScaleApply(&logScaleState, scaled_filterbank,
                    filterBankState.num_channels, correction_bits);

 

        for(int i=0;i<outputSize;i++)
        {
            out[i] = logged_filterbank[i];
        }

        
        return(0);
    };


    ~MFCC()
    {
        free(ffttempout);

        FftFreeStateContents(fftconfig);
        FilterbankFreeStateContents(&filterBankState);
        NoiseReductionFreeStateContents(&noiseReductionState);
        PcanGainControlFreeStateContents(&pcanGainControlState);

    }


protected:
    
    int mFFTLen;
    void *fftconfig;
    struct complex_int16_t *ffttempout;
    q15_t *ffttempin;

    struct FilterbankConfig filterBankconfig;
    struct FilterbankState filterBankState;

    struct LogScaleConfig logScaleConfig;
    struct LogScaleState logScaleState;

    struct NoiseReductionConfig noiseReductionConfig;
    struct NoiseReductionState noiseReductionState;

    struct PcanGainControlConfig pcanGainControlConfig;
    struct PcanGainControlState pcanGainControlState;

    
};

#endif