/*

Generated with CMSIS-DSP SDF Scripts.
The generated code is not covered by CMSIS-DSP license.

The support classes and code is covered by CMSIS-DSP license.

*/


#include "arm_math.h"
#include "custom.h"
#include "GenericNodes.h"
#include "AppNodes.h"
#include "scheduler.h"

/* List of nodes */
static NodeBase *nodeArray[14]={0};

/*

Description of the scheduling. It is a list of nodes to call.
The values are indexes in the previous array.

*/
static unsigned int schedule[244]=
{ 
1,5,7,0,3,9,8,2,1,5,7,0,3,9,8,2,1,5,7,0,3,9,8,4,13,10,6,2,1,5,7,0,3,9,8,2,1,5,7,0,
3,9,8,4,13,10,6,2,1,5,7,0,3,9,8,2,1,5,7,0,3,9,8,2,1,5,7,0,3,9,8,4,13,10,6,2,1,5,7,0,
3,9,8,2,1,5,7,0,3,9,8,4,13,10,6,2,1,5,7,0,3,9,8,2,1,5,7,0,3,9,8,2,1,5,7,0,3,9,8,4,
13,10,6,11,12,2,1,5,7,0,3,9,8,2,1,5,7,0,3,9,8,4,13,10,6,2,1,5,7,0,3,9,8,2,1,5,7,0,3,9,
8,2,1,5,7,0,3,9,8,4,13,10,6,2,1,5,7,0,3,9,8,2,1,5,7,0,3,9,8,4,13,10,6,2,1,5,7,0,3,9,
8,2,1,5,7,0,3,9,8,2,1,5,7,0,3,9,8,4,13,10,6,2,1,5,7,0,3,9,8,2,1,5,7,0,3,9,8,4,13,10,
6,11,12,2,
};

/***********

FIFO buffers

************/
#define FIFOSIZE0 128
#define FIFOSIZE1 1
#define FIFOSIZE2 128
#define FIFOSIZE3 128
#define FIFOSIZE4 134
#define FIFOSIZE5 128
#define FIFOSIZE6 128
#define FIFOSIZE7 128
#define FIFOSIZE8 128
#define FIFOSIZE9 128
#define FIFOSIZE10 128
#define FIFOSIZE11 384
#define FIFOSIZE12 480
#define FIFOSIZE13 480
#define FIFOSIZE14 1
#define FIFOSIZE15 40
#define FIFOSIZE16 200
#define FIFOSIZE17 1960

#define BUFFERSIZE0 128
int16_t buf0[BUFFERSIZE0]={0};

#define BUFFERSIZE1 1
int16_t buf1[BUFFERSIZE1]={0};

#define BUFFERSIZE2 128
int16_t buf2[BUFFERSIZE2]={0};

#define BUFFERSIZE3 128
int16_t buf3[BUFFERSIZE3]={0};

#define BUFFERSIZE4 134
int16_t buf4[BUFFERSIZE4]={0};

#define BUFFERSIZE5 128
int16_t buf5[BUFFERSIZE5]={0};

#define BUFFERSIZE6 128
int16_t buf6[BUFFERSIZE6]={0};

#define BUFFERSIZE7 128
int16_t buf7[BUFFERSIZE7]={0};

#define BUFFERSIZE8 128
int16_t buf8[BUFFERSIZE8]={0};

#define BUFFERSIZE9 128
int16_t buf9[BUFFERSIZE9]={0};

#define BUFFERSIZE10 128
int16_t buf10[BUFFERSIZE10]={0};

#define BUFFERSIZE11 384
int16_t buf11[BUFFERSIZE11]={0};

#define BUFFERSIZE12 480
int16_t buf12[BUFFERSIZE12]={0};

#define BUFFERSIZE13 480
int16_t buf13[BUFFERSIZE13]={0};

#define BUFFERSIZE14 1
int16_t buf14[BUFFERSIZE14]={0};

#define BUFFERSIZE15 40
int16_t buf15[BUFFERSIZE15]={0};

#define BUFFERSIZE16 200
int8_t buf16[BUFFERSIZE16]={0};

#define BUFFERSIZE17 1960
int8_t buf17[BUFFERSIZE17]={0};


/**************
 
 Classes created for pure function calls (like some CMSIS-DSP functions)

***************/


uint32_t scheduler(int *error,sched_config *config)
{
    int sdfError=0;
    uint32_t nbSchedule=0;

    /*
    Create FIFOs objects
    */
    FIFO<int16_t,FIFOSIZE0,1> fifo0(buf0);
    FIFO<int16_t,FIFOSIZE1,1> fifo1(buf1);
    FIFO<int16_t,FIFOSIZE2,1> fifo2(buf2);
    FIFO<int16_t,FIFOSIZE3,1> fifo3(buf3);
    FIFO<int16_t,FIFOSIZE4,0> fifo4(buf4,6);
    FIFO<int16_t,FIFOSIZE5,1> fifo5(buf5);
    FIFO<int16_t,FIFOSIZE6,1> fifo6(buf6);
    FIFO<int16_t,FIFOSIZE7,1> fifo7(buf7);
    FIFO<int16_t,FIFOSIZE8,1> fifo8(buf8);
    FIFO<int16_t,FIFOSIZE9,1> fifo9(buf9);
    FIFO<int16_t,FIFOSIZE10,1> fifo10(buf10);
    FIFO<int16_t,FIFOSIZE11,0> fifo11(buf11);
    FIFO<int16_t,FIFOSIZE12,1> fifo12(buf12);
    FIFO<int16_t,FIFOSIZE13,1> fifo13(buf13);
    FIFO<int16_t,FIFOSIZE14,1> fifo14(buf14);
    FIFO<int16_t,FIFOSIZE15,1> fifo15(buf15);
    FIFO<int8_t,FIFOSIZE16,0> fifo16(buf16);
    FIFO<int8_t,FIFOSIZE17,1> fifo17(buf17);

    /* 
    Create node objects 
    */

    AEC<int16_t,128,int16_t,128,int16_t,128> aec(fifo3,fifo6,fifo7);
    nodeArray[0]=(NodeBase*)&aec;

    MicrophoneSource<int16_t,128,int16_t,128,int16_t,128,int16_t,1> allMics(fifo0,fifo4,fifo5,fifo1,config->ringConfigFARMIC,config->ringConfigNEARMIC1,config->ringConfigNEARMIC2);
    nodeArray[1]=(NodeBase*)&allMics;

    SpeakerSink<int16_t,128,int16_t,128,int16_t,1> allSpeakers(fifo10,fifo2,fifo1,config->ringConfigFARSPEAKER,config->ringConfigNEARSPEAKER,&(config->nbSink));
    nodeArray[2]=(NodeBase*)&allSpeakers;

    ANR<int16_t,128,int16_t,128> anr(fifo7,fifo8);
    nodeArray[3]=(NodeBase*)&anr;

    SlidingBuffer<int16_t,480,160> audioWin(fifo11,fifo12);
    nodeArray[4]=(NodeBase*)&audioWin;

    BeamFormer<int16_t,128,int16_t,128,int16_t,128> beamFormer(fifo4,fifo5,fifo6);
    nodeArray[5]=(NodeBase*)&beamFormer;

    Clamp<int16_t,40,int8_t,40> clamp(fifo15,fifo16);
    nodeArray[6]=(NodeBase*)&clamp;

    Duplicate<int16_t,128,int16_t,128,int16_t,128> dup(fifo0,fifo2,fifo3);
    nodeArray[7]=(NodeBase*)&dup;

    Duplicate<int16_t,128,int16_t,128,int16_t,128> dup2(fifo9,fifo10,fifo11);
    nodeArray[8]=(NodeBase*)&dup2;

    Gain<int16_t,128,int16_t,128> gain(fifo8,fifo9,config->gain);
    nodeArray[9]=(NodeBase*)&gain;

    MFCC<int16_t,480,int16_t,1,int16_t,40> mfcc(fifo13,fifo14,fifo15,16000,512);
    nodeArray[10]=(NodeBase*)&mfcc;

    SlidingBuffer<int8_t,1960,1760> mfccWin(fifo16,fifo17);
    nodeArray[11]=(NodeBase*)&mfccWin;

    TFLite<int8_t,1960> tflite(fifo17);
    nodeArray[12]=(NodeBase*)&tflite;

    Window<int16_t,480,int16_t,480,int16_t,1> window(fifo12,fifo13,fifo14);
    nodeArray[13]=(NodeBase*)&window;

    /* Run several schedule iterations */
    while(sdfError==0)
    {
        /* Run a schedule iteration */
        for(unsigned long id=0 ; id < 244; id++)
        {
            unsigned int nodeId = schedule[id];
            sdfError = nodeArray[nodeId]->run();
            CHECKERROR;
        }
       nbSchedule++;
    }

    *error=sdfError;
    return(nbSchedule);
}
