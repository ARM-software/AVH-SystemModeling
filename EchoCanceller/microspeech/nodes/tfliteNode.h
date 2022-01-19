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
#ifndef _TFLITE_NODE_H_ 
#define _TFLITE_NODE_H_ 

#define NBCLASSES 3
#define UNKNOWN 0x0000 
#define YES 0x1000 
#define NO 0x2000

static const int classes[NBCLASSES]={UNKNOWN,YES,NO};

extern "C" void writeTFLiteOutput(int val);

template<typename IN, int inputSize> class TFLite;

template<int inputSize>
class TFLite<q7_t,inputSize>: public GenericSink<q7_t, inputSize>
{
public:
    TFLite(FIFOBase<q7_t> &src):
    GenericSink<q7_t,inputSize>(src){};

    int run()
    {
        int result=0;
        q7_t *b=this->getReadBuffer();

        // Copy new data for recognition
        copyDataToML(b);

        // Read latest recognized command
        result = getCommand();


        // Unknown and silence mapped to unknown
        if ((result==0) || (result==1))
        {
              result=0;
        }
        else
        {
              // Map YES/NO to 1/2
            result --;
        }

        if ((result<0) || (result>=NBCLASSES))
        {
              result=0;
        }

        writeTFLiteOutput(classes[result]);

        
        // Wakeup ML thread for new recognition
        osThreadFlagsSet((osThreadId_t)gMLThreadID,1);


        return(0);
    }

};

#endif