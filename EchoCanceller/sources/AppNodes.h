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
#ifndef _APPNODES_H_
#define _APPNODES_H_

#include <iostream>
#include <cstdio>
#include "RingBuffer.h"
#include "SchedEvents.h"
#include "StreamingSink.h"
#include "StreamingSource.h"
#include "NullSink.h"
#include "arm_math_types.h"
#include "dsp/basic_math_functions.h"
#include "dsp/support_functions.h"
#include "process.h"
#include "SchedEvents.h"
#include "AudioConfig.h"

#include "main_functions.h"



extern "C" osMutexId_t ml_mutex_Id; 
extern "C" int32_t g_latest_audio_timestamp;
extern "C" osThreadId_t gMLThreadID;



#include "nodes/windowNode.h"
#include "nodes/mfccNode.h"
#include "nodes/clampNode.h"
#include "nodes/tfliteNode.h"


/*

Audio Sink.
The custom audio sink has a counter to know how many times it has been called.
There are also 2 event recorder events as defined in SDF Pack.
They are used to trace sink execution and sink number to check that
the SDF schedule is working.

*/
template<typename IN1, int input1Size,typename IN2, int input2Size,typename IN3, int input3Size> class SpeakerSink;

template<typename IN, int inputSize>
class SpeakerSink<IN,inputSize,IN,inputSize,short,1>: public NodeBase
{
public:
    SpeakerSink(FIFOBase<IN> &src1,FIFOBase<IN> &src2,FIFOBase<short> &seq,
        ring_config_t *configFar,
        ring_config_t *configNear,int *nbSink):
    mSrc1(src1),
    mSrc2(src2),
    mSeq(seq),
    mConfigFar(configFar),
    mConfigNear(configNear),
    mNbSink(nbSink)
    {};
			
		int run()
		{
			*mNbSink = *mNbSink + 1;

#if defined(MICROSPEECH)
            g_latest_audio_timestamp += (inputSize * 1000) / AUDIO_DRV_SAMPLINGFREQUENCY_TX;
#endif 
            
			EventRecord2 (Evt_Sink, 0,0);
			EventRecord2 (Evt_SinkVal, *mNbSink,0);

            short *dummy=this->getReadBuffer3();
            (void)dummy;

            IN *b=this->getReadBuffer1();
            int err = process(b,mConfigFar);
            if (err!=0)
            {
                return(err);
            }

            b=this->getReadBuffer2();
            err = process(b,mConfigNear);
            if (err!=0)
            {
                return(err);
            }
			

			return(0);
		}
protected:
    int process(IN *b, ring_config_t * config)
    {
        
        int bufID=ringUserReserveBuffer(config);
        uint8_t *buf=ringGetBufferAddress(config,bufID);

        if (buf != NULL)
        {
           /* If a buffer is available we copy the data to the FIFO
           */
           memcpy(buf,(void*)b,inputSize*sizeof(IN));

           /* We release the buffer so than it can be used by the interrupt */
           ringUserReleaseBuffer(config);
           return(0);
        }
        else 
        {
            return(config->error);
        }
    };

	int *mNbSink; 
    ring_config_t *mConfigFar;
    ring_config_t *mConfigNear;

    IN * getReadBuffer1(){return mSrc1.getReadBuffer(inputSize);};
    IN * getReadBuffer2(){return mSrc2.getReadBuffer(inputSize);};
    short * getReadBuffer3(){return mSeq.getReadBuffer(inputSize);};


private:
    FIFOBase<IN> &mSrc1;
    FIFOBase<IN> &mSrc2;
    FIFOBase<short> &mSeq;
};


/*

Custom audio source is just adding an event recording event (as defined in SDF pack)
to trace the source execution.

*/

template<typename OUT1, int output1Size,
typename OUT2, int output2Size,
typename OUT3, int output3Size,
typename OUT4, int output4Size>
class MicrophoneSource;

template<typename OUT, int outputSize>
class MicrophoneSource<OUT,outputSize,OUT,outputSize,OUT,outputSize,short,1> : public NodeBase
{
public:
    MicrophoneSource(FIFOBase<OUT> &dst1,FIFOBase<OUT> &dst2,FIFOBase<OUT> &dst3,FIFOBase<short> &seq,
        ring_config_t *configFarMic,
        ring_config_t *configNearMic1,
        ring_config_t *configNearMic2):
        mDst1(dst1),
        mDst2(dst2),
        mDst3(dst3),
        mSeq(seq),
        mConfigFarMic(configFarMic),
        mConfigNearMic1(configNearMic1),
        mConfigNearMic2(configNearMic2)
    {};
 
		int run()
		{
			EventRecord2 (Evt_Source, 0,0);

            short *dummy=this->getWriteBuffer4();
            (void)dummy;
			
            OUT *b=this->getWriteBuffer1();
            int err = process(b,mConfigFarMic);
            if (err!=0)
            {
                return(err);
            }

            b=this->getWriteBuffer2();
            err = process(b,mConfigNearMic1);
            if (err!=0)
            {
                return(err);
            }

            b=this->getWriteBuffer3();
            err = process(b,mConfigNearMic2);
            if (err!=0)
            {
                return(err);
            }

			return(0);
		}

protected:
    int process(OUT *b,ring_config_t *config)
    {
        /* 
           Try to reserve a buffer. If no buffer is available, the task running
           this node will sleep.

           If there is a timeout (configured when the ring buffer was initialized)
           the function will return a NULL pointer.

        */
        int bufID=ringUserReserveBuffer(config);
        uint8_t *buf=ringGetBufferAddress(config,bufID);


        if (buf != NULL)
        {
           /* If a buffer is available we copy the data to the FIFO
           */
           memcpy((void*)b,buf,outputSize*sizeof(OUT));

           /* We release the buffer so than it can be used by the interrupt */
           ringUserReleaseBuffer(config);
           return(0);
        }
        else 
        {
            return(config->error);
        }
    }

    OUT * getWriteBuffer1(){return mDst1.getWriteBuffer(outputSize);};
    OUT * getWriteBuffer2(){return mDst2.getWriteBuffer(outputSize);};
    OUT * getWriteBuffer3(){return mDst3.getWriteBuffer(outputSize);};
    short * getWriteBuffer4(){return mSeq.getWriteBuffer(outputSize);};

    ring_config_t *mConfigFarMic;
    ring_config_t *mConfigNearMic1;
    ring_config_t *mConfigNearMic2;

private:
    FIFOBase<OUT> &mDst1;
    FIFOBase<OUT> &mDst2;
    FIFOBase<OUT> &mDst3;
    FIFOBase<short> &mSeq;


};

/*

Process node.

All functions are defined in process.c

*/


template<typename IN1, int input1Size,
         typename IN2, int input2Size,
         typename OUT1,int output1Size>
class AEC;

template<int inputSize>
class AEC<q15_t,inputSize,q15_t,inputSize,q15_t,inputSize>: 
    public GenericNode21<q15_t,inputSize,q15_t,inputSize,q15_t,inputSize>
{
public:
    AEC(FIFOBase<q15_t> &src1,FIFOBase<q15_t> &src2,FIFOBase<q15_t> &dst1):
    GenericNode21<q15_t,inputSize,q15_t,inputSize,q15_t,inputSize>(src1,src2,dst1){
    };

    int run(){
        q15_t *farMic=this->getReadBuffer1();
        q15_t *nearMic=this->getReadBuffer2();

        q15_t *farSpeaker=this->getWriteBuffer();
        aec(nearMic,farMic,farSpeaker,inputSize);
        return(0);
    };

};

template<typename IN, int inputSize,typename OUT,int outputSize> class ANR;

template<int inputSize>
class ANR<q15_t,inputSize,q15_t,inputSize>: public GenericNode<q15_t,inputSize,q15_t,inputSize>
{
public:
    ANR(FIFOBase<q15_t> &src,FIFOBase<q15_t> &dst):GenericNode<q15_t,inputSize,q15_t,inputSize>(src,dst){};

    int run(){
        q15_t *a=this->getReadBuffer();
        q15_t *b=this->getWriteBuffer();
        anr(a,b,inputSize);
        return(0);
    };

};

template<typename IN, int inputSize,
         typename OUT1, int output1Size,
         typename OUT2,int output2Size>
class Duplicate;

template<int inputSize>
class Duplicate<q15_t,inputSize,q15_t,inputSize,q15_t,inputSize>: 
    public GenericNode12<q15_t,inputSize,q15_t,inputSize,q15_t,inputSize>
{
public:
    Duplicate(FIFOBase<q15_t> &src,FIFOBase<q15_t> &dst1,FIFOBase<q15_t> &dst2):
    GenericNode12<q15_t,inputSize,q15_t,inputSize,q15_t,inputSize>(src,dst1,dst2){
    };

    int run(){
        q15_t *src=this->getReadBuffer();

        q15_t *b1=this->getWriteBuffer1();
        q15_t *b2=this->getWriteBuffer2();
        

        memcpy(b1,src,sizeof(q15_t)*inputSize);
        memcpy(b2,src,sizeof(q15_t)*inputSize);
        return(0);
    };

};

template<typename IN1, int input1Size,
         typename OUT1, int output1Size,
         typename OUT2, int output2Size>
class BeamFormer;

template<int inputSize>
class BeamFormer<q15_t,inputSize,q15_t,inputSize,q15_t,inputSize>: 
    public GenericNode21<q15_t,inputSize,q15_t,inputSize,q15_t,inputSize>
{
public:
    BeamFormer(FIFOBase<q15_t> &src1,FIFOBase<q15_t> &src2,FIFOBase<q15_t> &dst1):
    GenericNode21<q15_t,inputSize,q15_t,inputSize,q15_t,inputSize>(src1,src2,dst1){
    };

    int run(){
        q15_t *src1=this->getReadBuffer1();
        q15_t *src2=this->getReadBuffer2();

        q15_t *b=this->getWriteBuffer();
        

        beamFormer(src1,src2,b,inputSize);
        return(0);
    };

};


template<typename IN, int inputSize,typename OUT,int outputSize> class PassThrough;

template<typename IN, int inputSize>
class PassThrough<IN,inputSize,IN,inputSize>: public GenericNode<IN,inputSize,IN,inputSize>
{
public:
    PassThrough(FIFOBase<IN> &src,FIFOBase<IN> &dst):GenericNode<IN,inputSize,IN,inputSize>(src,dst){};

    int run(){
        IN *a=this->getReadBuffer();
        IN *b=this->getWriteBuffer();
        memcpy(b,a,sizeof(IN)*inputSize);
        return(0);
    };

};

template<typename IN, int inputSize,typename OUT,int outputSize> class Gain;

template<typename IN, int inputSize>
class Gain<IN,inputSize,IN,inputSize>: public GenericNode<IN,inputSize,IN,inputSize>
{
public:
    Gain(FIFOBase<IN> &src,FIFOBase<IN> &dst,float gainf):GenericNode<IN,inputSize,IN,inputSize>(src,dst)
    {
        shift=0;
        while(fabs(gainf>1.0))
        {
            gainf=gainf/2.0; 
            shift++;
        }
        arm_float_to_q15(&gainf,&gain,1);
    };

    int run(){
        IN *a=this->getReadBuffer();
        IN *b=this->getWriteBuffer();
        arm_scale_q15(a,gain,shift,b,inputSize);
        return(0);
    };

protected:
    q15_t gain;
    int shift;

};


#endif