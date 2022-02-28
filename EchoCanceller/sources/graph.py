# --------------------------------------------------------------------------
# Copyright (c) 2020-2022 Arm Limited (or its affiliates). All rights reserved.
# 
# SPDX-License-Identifier: Apache-2.0
# 
# Licensed under the Apache License, Version 2.0 (the License); you may
# not use this file except in compliance with the License.
# You may obtain a copy of the License at
# 
# www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an AS IS BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# --------------------------------------------------------------------------
from cmsisdsp.sdf.scheduler import *

MICROSPEECH=True

### Define new types of Nodes 

# The seq arc on micros and speaker is a dummy fifo
# used only to enforce a sequence of speaker and mic
# We want that for each call to the mic node, there is a call
# to the speaker node
# Otherwise the feedback loop cannot work.

# Custom AudioSink to use event recorder and a counter
class SpeakerSink(GenericSink):
    def __init__(self,name,theType,inLength):
        GenericSink.__init__(self,name)
        self.addInput("farSpeaker",theType,inLength)
        self.addInput("nearSpeaker",theType,inLength)
        self.addInput("seq",theType,1)

    @property
    def typeName(self):
        return "SpeakerSink"

# Custom AudioSource to use event recorder
# Contain all microphones because in scheduler
# we want to ensure they all run at "same" time
class MicrophoneSource(GenericSource):
    def __init__(self,name,theType,inLength):
        GenericSource.__init__(self,name)
        self.addOutput("farMic",theType,inLength)
        self.addOutput("nearMic1",theType,inLength)
        self.addOutput("nearMic2",theType,inLength)
        self.addOutput("seq",theType,1)

    @property
    def typeName(self):
        return "MicrophoneSource"

class ANR(GenericNode):
    def __init__(self,name,theType,inLength):
        GenericNode.__init__(self,name)
        self.addInput("i",theType,inLength)
        self.addOutput("o",theType,inLength)

    @property
    def typeName(self):
        return "ANR"

class PassThrough(GenericNode):
    def __init__(self,name,theType,inLength):
        GenericNode.__init__(self,name)
        self.addInput("i",theType,inLength)
        self.addOutput("o",theType,inLength)

    @property
    def typeName(self):
        return "PassThrough"

class Gain(GenericNode):
    def __init__(self,name,theType,inLength):
        GenericNode.__init__(self,name)
        self.addInput("i",theType,inLength)
        self.addOutput("o",theType,inLength)

    @property
    def typeName(self):
        return "Gain"

class AEC(GenericNode):
    def __init__(self,name,theType,inLength):
        GenericNode.__init__(self,name)
        # Inputs will be ordereed in alphabetical order.
        # So, input 1 is farMic
        self.addInput("nearMic",theType,inLength)
        self.addInput("farMic",theType,inLength)
        self.addOutput("farSpeaker",theType,inLength)

    @property
    def typeName(self):
        return "AEC"

class Duplicate(GenericNode):
    def __init__(self,name,theType,inLength):
        GenericNode.__init__(self,name)
        self.addInput("i",theType,inLength)
        self.addOutput("oa",theType,inLength)
        self.addOutput("ob",theType,inLength)

    @property
    def typeName(self):
        return "Duplicate"

class BeamFormer(GenericNode):
    def __init__(self,name,theType,inLength):
        GenericNode.__init__(self,name)
        self.addInput("ia",theType,inLength)
        self.addInput("ib",theType,inLength)
        self.addOutput("o",theType,inLength)

    @property
    def typeName(self):
        return "BeamFormer"


##### Microspeech nodes

if MICROSPEECH:
   class Window(GenericNode):
       def __init__(self,name,inLength):
           GenericNode.__init__(self,name)
           self.addInput("i",CType(SINT16),inLength)
           self.addOutput("oa",CType(SINT16),inLength)
           self.addOutput("ob",CType(SINT16),1)
   
       @property
       def typeName(self):
           return "Window"
   
   class MFCC(GenericNode):
       def __init__(self,name,inLength,outLength):
           GenericNode.__init__(self,name)
           self.addInput("ia",CType(SINT16),inLength)
           self.addInput("ib",CType(SINT16),1)
           self.addOutput("o",CType(SINT16),outLength)
   
       @property
       def typeName(self):
           return "MFCC"

   # Sending a message to the TFLite thread to recognize current sound
   # and use latest recognition to try to make a decision about current
   # word
   class TFLite(GenericSink):
       def __init__(self,name,theType,inLength):
           GenericSink.__init__(self,name)
           self.addInput("i",theType,inLength)
   
       @property
       def typeName(self):
           return "TFLite"


   class Clamp(GenericNode):
       def __init__(self,name,inLength):
           GenericNode.__init__(self,name)
           self.addInput("i",CType(SINT16),inLength)
           self.addOutput("o",CType(SINT8),inLength)
   
       @property
       def typeName(self):
           return "Clamp"

### Define nodes

# Size of DMA buffer (to be defined in RingConfig.h in SDF pack)
# Tradeoff between latency and number of interrupts
DMABUF = 128

def getPowerOfTwo(x):
    s = 1 
    while(s<x):
        s=s<<1 
    return(s)

if MICROSPEECH:
   sampleRate=16000
   WINDOWSIZE = 30 # ms 
   WINDOWSTEP = 20 #  ms
   MFCCBANDS = 40
   NBMFCCFORTFLITE = 49
   
   WINDOWLEN = int(np.floor(WINDOWSIZE*1.0e-3 * sampleRate))
   FFTLEN = getPowerOfTwo(WINDOWLEN)
   WINDOWOVERLAPLEN = int(np.floor((WINDOWSIZE-WINDOWSTEP)*1.0e-3 * sampleRate))


   # It will be roughly 1 sec
   FEATURELEN = NBMFCCFORTFLITE*MFCCBANDS

   # We slide by 6 MFCC feature so 100 ms since MFCC
   # are computed every 20 ms
   # It should correspond to 10 features in the recognition buffer
   # Original demo (at 32 MHz) is using only around 10 features
   # More averaging with more features is not working.
   #
   # This also defines how often the recognitino is done
   # and the TF sampling rate in the Modelica model must
   # be coherent with this.
   # tfSamplingFrequency in modelica is set to 10 Hz (100 ms)
   FEATUREOVERLAPLEN = FEATURELEN - 5 * MFCCBANDS

   

# Data types
intType=CType(SINT16)
int8Type=CType(SINT8)

# Audio Source and the ring buffer used for RX (FIFO of DMA buffers)

allMics=MicrophoneSource("allMics",intType,DMABUF)
allMics.addVariableArg("config->ringConfigFARMIC")
allMics.addVariableArg("config->ringConfigNEARMIC1")
allMics.addVariableArg("config->ringConfigNEARMIC2")


# Audio sink and the ring buffer for TX
# There is also a counter to count how many times this sink
# is executed. It may be useful in event recorder trace for debug purpose

allSpeakers=SpeakerSink("allSpeakers",intType,DMABUF)

allSpeakers.addVariableArg("config->ringConfigFARSPEAKER")
allSpeakers.addVariableArg("config->ringConfigNEARSPEAKER")
allSpeakers.addVariableArg("&(config->nbSink)")


dup=Duplicate("dup",intType,DMABUF)
beamFormer=BeamFormer("beamFormer",intType,DMABUF)

aec=AEC("aec",intType,DMABUF)
anr=ANR("anr",intType,DMABUF)

gain=Gain("gain",intType,DMABUF)
gain.addVariableArg("config->gain")

#### Microspeech nodes

if MICROSPEECH:
   window=Window("window",WINDOWLEN)
   mfcc=MFCC("mfcc",WINDOWLEN,MFCCBANDS)
   mfcc.addLiteralArg(sampleRate)
   mfcc.addLiteralArg(FFTLEN)
   
   audioWin=SlidingBuffer("audioWin",intType,WINDOWLEN,WINDOWOVERLAPLEN)
   dup2=Duplicate("dup2",intType,DMABUF)

   
   mfccWin=SlidingBuffer("mfccWin",int8Type,FEATURELEN,FEATUREOVERLAPLEN)
   tflite=TFLite("tflite",int8Type,FEATURELEN)
   clamp=Clamp("clamp",MFCCBANDS)




g = Graph()

g.connect(allMics.farMic,dup.i)

g.connect(allMics.seq, allSpeakers.seq)

g.connect(dup.oa,allSpeakers.nearSpeaker)

g.connect(dup.ob,aec.farMic)

# The delay is for the very simplified beamformer
# This delay is computed by roomTransferFunction.py
# The script is also computing on which microphone the delay
# must be applied
g.connectWithDelay(allMics.nearMic1,beamFormer.ia,6)
g.connect(allMics.nearMic2,beamFormer.ib)
g.connect(beamFormer.o,aec.nearMic)

g.connect(aec.farSpeaker,anr.i)
g.connect(anr.o,gain.i)

if MICROSPEECH:

   g.connect(gain.o,dup2.i)
   g.connect(dup2.oa,allSpeakers.farSpeaker)


   g.connect(dup2.ob,audioWin.i)
   g.connect(audioWin.o, window.i)

   g.connect(window.oa,mfcc.ia)
   g.connect(window.ob,mfcc.ib)

   g.connect(mfcc.o,clamp.i)
   g.connect(clamp.o,mfccWin.i)
   g.connect(mfccWin.o,tflite.i)
else:
   g.connect(gain.o,allSpeakers.farSpeaker)


print("Generate graphviz and code")

conf=Configuration()
# C interface for scheduler function
conf.cOptionalArgs="sched_config *config"
#conf.memoryOptimization=True

# Because the schedule is very long
# It is disabled when debugging but it will give
# a very long sequence of function calls in the generated schedule.
conf.codeArray=True

# vertical graphviz layout
conf.horizontal=False


sched = g.computeSchedule()

print("Schedule length = %d" % sched.scheduleLength)
print("Memory usage %d bytes" % sched.memory)


# Generate code
sched.ccode(".",config=conf)

# Generate graphviz
with open("test.dot","w") as f:
    sched.graphviz(f,config=conf)

