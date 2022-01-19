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
from xml.etree import ElementTree as et
import os.path 
import os
from pathlib import Path
import argparse

parser = argparse.ArgumentParser(description='Parse test description')

parser.add_argument('-notf', action='store_false', help="No TFLite")
parser.add_argument('-t', nargs='?',type = str, default="12", help="Simulation duration")
parser.add_argument('-m', nargs='?',type = str, default="Echo.VHTEcho", help="Model name")

args = parser.parse_args()



srcParamsPath="%s_init.xml" % args.m
dstParamsPath="%s_init_updated.xml" % args.m

demoDir=Path(os.getcwd()).parent
currentDir=Path(os.getcwd())

soundFolder=os.path.join(demoDir,"sounds")
vsiDir=os.path.join(demoDir,"VSI/audio/python")
fvpConfig=os.path.join(demoDir,"fvp_config.txt")
elf=os.path.join(demoDir,"Objects/EchoCanceller.axf")

vhtCommandLineOptions="-V %s -f %s -a %s" % (vsiDir, fvpConfig,elf)

pathToVHT="/opt/arm_vsi/VHT-Corstone-300"

farEndWav=os.path.join(soundFolder,"background.wav")
nearEndWav=os.path.join(soundFolder,"yesno.wav")

if args.m=="Echo.VHTEcho":
   outputWav=os.path.join(currentDir,"cleanedSignal.wav")
else:
   outputWav=os.path.join(currentDir,"signalWithEcho.wav")

nearSourceToMic1CSV=os.path.join(demoDir,"rir_nearsource_mic1.csv")
nearSourceToMic2CSV=os.path.join(demoDir,"rir_nearsource_mic2.csv")
speakerToMic1CSV=os.path.join(demoDir,"rir_speaker_mic1.csv")
speakerToMic2CSV=os.path.join(demoDir,"rir_speaker_mic2.csv")


tree = et.parse(srcParamsPath)

# Set simulation stop time to 12 s
simuParams=tree.find('DefaultExperiment')
simuParams.set("stopTime",args.t)
print(simuParams.attrib)

def setString(nodeName,path):
    vhtNode=tree.find(".//*[@name='%s']/String" % nodeName)
    vhtNode.set("start",path)
    return(vhtNode)

def setBoolean(nodeName,path):
    vhtNode=tree.find(".//*[@name='%s']/Boolean" % nodeName)
    vhtNode.set("start",path)
    return(vhtNode)

#set TFLite mode
if args.m=="Echo.VHTEcho":
   if args.notf:
      vhtNode=setBoolean('echoCanceller.vhtmulti.hasTF',"true")
   else:
      vhtNode=setBoolean('echoCanceller.vhtmulti.hasTF',"false")
   print(vhtNode.attrib)

   # Set path to VHT
   vhtNode=setString('echoCanceller.vhtmulti.vhtCommand',pathToVHT)
   print(vhtNode.attrib)
   
   # Set VHT command line
   vhtNode=setString('echoCanceller.vhtmulti.vhtCommandLine',vhtCommandLineOptions)
   print(vhtNode.attrib)

# Set wav files
vhtNode=setString('farSource.path',farEndWav)
print(vhtNode.attrib)

vhtNode=setString('nearSource.path',nearEndWav)
print(vhtNode.attrib)

vhtNode=setString('farSpeaker.path',outputWav)
print(vhtNode.attrib)

# Set room echo files
vhtNode=setString('roomEcho.nearSourceToMic1RIR',nearSourceToMic1CSV)
print(vhtNode.attrib)

vhtNode=setString('roomEcho.nearSourceToMic2RIR',nearSourceToMic2CSV)
print(vhtNode.attrib)

vhtNode=setString('roomEcho.speakerToMic1RIR',speakerToMic1CSV)
print(vhtNode.attrib)

vhtNode=setString('roomEcho.speakerToMic2RIR',speakerToMic2CSV)
print(vhtNode.attrib)

tree.write(dstParamsPath)

# Launch simulator with ./Echo.VHTEcho -f Echo.VHTEcho_init_updated.xml
