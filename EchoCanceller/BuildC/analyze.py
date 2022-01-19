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

# This script is reading the output of the simulation and diplaying a plot
# with the cleaned signal and the yes/no recognition signal.
# It is faster than doing it through the OpenModelica plotting.
import os.path
import DyMat

import matplotlib as mpl
import matplotlib.pyplot as plt
import numpy as np
import matplotlib.ticker as ticker

import argparse

parser = argparse.ArgumentParser(description='Parse test description')

parser.add_argument('-m', nargs='?',type = str, default="Echo.VHTEcho", help="Model name")

args = parser.parse_args()


path="."
data="%s_res.mat" % args.m

filePath=os.path.join(path,data)

d = DyMat.DyMatFile(filePath)

#print(d.names())

DISPSAMPLING=1000

# We may have several samples for the same time in the Modelica output
# because the same function may be reevaluated several time for the same time instant
# by the solver.
# We only get some subsamples for plotting and ensure we have only one sample
# per time instant.
def getSignal(block):
    farSignal=d.data(block)
    times=d.abscissa(block)
    maxTime=np.max(times[0])
    dt = 1.0/DISPSAMPLING
    r=[]
    current=0
    for t,v in zip(times[0],farSignal):
        if t>= current:
            r.append(v)
            current += dt 
    return(np.array(r),maxTime)



farSignal,maxTime=getSignal("echoCanceller.farSpeaker")

minSignal = np.min(farSignal)
maxSignal = np.max(farSignal)

if args.m=="Echo.VHTEcho":
   tf,_=getSignal("echoCanceller.tf")
   tfScale = 4.0
   tf = tf / tfScale


ticks = ticker.FuncFormatter(lambda x, pos: '{0:g}'.format(x/1000))


fig, ax = plt.subplots()  # Create a figure containing a single axes.
if args.m=="Echo.VHTEcho":
   ax.set_title('YES/NO Recognition')
else:
   ax.set_title('Signal with echo')
ax.xaxis.set_major_formatter(ticks)
ax.set_xlabel("s")
if args.m=="Echo.VHTEcho":
   plt.ylim([minSignal,2.1/tfScale])
else:
   plt.ylim([minSignal,maxSignal])
ax.plot(farSignal,linewidth=0.1)  # Plot some data on the axes.

if args.m=="Echo.VHTEcho":
   ax.set_yticks([1/tfScale,2/tfScale])
   ax.set_yticklabels(['Yes', 'No'])  # horizontal colorbar
   ax.plot(tf,linewidth=0.5)  # Plot some data on the axes.
   ax.hlines([1/tfScale, 2/tfScale],0,maxTime*1000,colors='r',linestyles=(0,(5,5)),linewidth=0.5)

if args.m=="Echo.VHTEcho":
   plt.savefig("YesNoResult.png")
else:
   plt.savefig("SignalWithEcho.png")




