/* Copyright 2018 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/
#include "fft.h"

#include <string.h>

#include <arm_math.h>


void FftCompute(void *config,
                int16_t* input,
                int input_size,
                int fft_size,
                struct complex_int16_t* output,
                int input_scale_shift) {

  int16_t* fft_input = input;
  // First, scale the input by the given shift.
  size_t i;
  for (i = 0; i < input_size; ++i) {
    fft_input[i] = static_cast<int16_t>(static_cast<uint16_t>(input[i])
                                        << input_scale_shift);
  }
  // Zero out whatever else remains in the top part of the input.
  for (; i < fft_size; ++i) {
    fft_input[i] = 0;
  }


  // Apply the FFT.


  spx_unnormalized_fft(config,fft_input,reinterpret_cast<int16_t*>(output));


}



