# --------------------------------------------------------------------------
# Copyright (c) 2020-2022 Arm Limited (or its affiliates). All rights reserved.
# Copyright (c) 2014-2017 EPFL-LCAV
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

# Lots of inspiration from 
# https://notebook.community/LCAV/pyroomacoustics/notebooks/pyroomacoustics_demo
# which has been used as a learning resource.
#
# So we kept the Copyright (c) 2014-2017 EPFL-LCAV
# because some part of this script are directly coming from above tutorial.

import pyroomacoustics as pra
import matplotlib.pyplot as plt
import numpy as np

SAVERIR = False
PLOT    = False
PLOT3D  = False

# The desired reverberation time and dimensions of the room
rt60 = 0.1  # seconds
room_dim = [4, 3, 3]  # meters


e_absorption, max_order = pra.inverse_sabine(rt60, room_dim)

samplingFrequency = 16000

# Create the room
room = pra.ShoeBox(
    room_dim, fs=samplingFrequency, materials=pra.Material(e_absorption), max_order=max_order
)

micDist = 0.2
mic1 = [2.0 - micDist/2.0, 1.5, 1]
mic2 = [2.0 + micDist/2.0, 1.5, 1]
speaker =[2, 1.7, 1]
person=[1.0,0.5,1.7]

micToPerson=np.array(person)-0.5*(np.array(mic1)+np.array(mic2))
micToPerson=micToPerson / np.linalg.norm(micToPerson)
vertical=np.array([0,-1,0])
angleOfArrival=np.arccos(np.dot(micToPerson,vertical))


soundSpeed =343.0
d1 = np.linalg.norm(np.array(person) - np.array(mic1))
d2 = np.linalg.norm(np.array(person) - np.array(mic2))
d = np.abs(d1 - d2)
delaySamples = d / soundSpeed * samplingFrequency

print("delay sample %f" % delaySamples)
if (d1 > d2):
    print("Delay must be applied to mic2 in SDF graph\n")
else:
    print("Delay must be applied to mic1 in SDF graph\n")


angle=90

mic_locs = np.c_[
    mic1,  # mic 1
    mic2,
]

mic1ID = 0
mic2ID = 1

speakerID = 0 
personID = 1


from pyroomacoustics.directivities import (
    DirectivityPattern,
    DirectionVector,
    CardioidFamily,
)

micPattern = DirectivityPattern.HYPERCARDIOID
speakerPattern = DirectivityPattern.CARDIOID 
personPattern = DirectivityPattern.SUBCARDIOID 

dirMic1 = CardioidFamily(
    orientation=DirectionVector(azimuth=angle-180, colatitude=90, degrees=True),
    pattern_enum=micPattern,
)


dirSpeaker = CardioidFamily(
    orientation=DirectionVector(azimuth=angle, colatitude=90, degrees=True),
    pattern_enum=speakerPattern,
)

dirPerson = CardioidFamily(
    orientation=DirectionVector(azimuth=angle-45, colatitude=90, degrees=True),
    pattern_enum=personPattern,
)

from scipy.io import wavfile
_, audio = wavfile.read('yesno.wav')

# finally place the array in the room
room.add_microphone_array(mic_locs, directivity=dirMic1)

room.add_source(position=speaker, directivity=dirSpeaker)
room.add_source(position=person, directivity=dirPerson)

if PLOT3D:
   fig, ax = room.plot()
   ax.set_xlim([0, 4])
   ax.set_ylim([0, 3])
   ax.set_zlim([0, 3]);
   plt.show()

if PLOT:
    room_2d  = pra.ShoeBox(p=room_dim[:2])
    dirMic1 = CardioidFamily(
        orientation=DirectionVector(azimuth=angle-180, degrees=True), 
        pattern_enum=micPattern
    )
    dirSpeaker = CardioidFamily(
        orientation=DirectionVector(azimuth=angle, degrees=True), 
        pattern_enum=speakerPattern
    )
    dirPerson = CardioidFamily(
        orientation=DirectionVector(azimuth=angle-45, degrees=True), 
        pattern_enum=personPattern
    )
    room_2d.add_source(speaker[:2], directivity=dirSpeaker)
    room_2d.add_source(person[:2],  directivity=dirPerson)
    room_2d.add_microphone(mic_locs[:2], directivity=dirMic1)

    room_2d.plot()
    plt.show()


room.compute_rir()

#print(room.rir[mic1ID][speakerID])

#global_delay = pra.constants.get("frac_delay_length") // 2
#print(global_delay)



# plot the RIR between mic 1 and source 0
if PLOT:
   #plt.plot(room.rir[mic1ID][speakerID])
   #plt.show()

   #plt.plot(room.rir[mic1ID][personID])
   #plt.show()

   room.plot_rir()
   plt.show()



rt60 = room.measure_rt60()

#print(rt60)

t60 = pra.experimental.measure_rt60(room.rir[mic1ID][speakerID], fs=room.fs, plot=PLOT)
print(f"The RT60 for speaker to mic1 is {t60 * 1000:.0f} ms")

t60 = pra.experimental.measure_rt60(room.rir[mic1ID][personID], fs=room.fs, plot=PLOT)
print(f"The RT60 for person to mic1 is {t60 * 1000:.0f} ms")

t60 = pra.experimental.measure_rt60(room.rir[mic2ID][speakerID], fs=room.fs, plot=PLOT)
print(f"The RT60 for speaker to mic2 is {t60 * 1000:.0f} ms")

t60 = pra.experimental.measure_rt60(room.rir[mic2ID][personID], fs=room.fs, plot=PLOT)
print(f"The RT60 for person to mic2 is {t60 * 1000:.0f} ms")

if SAVERIR:
   np.savetxt("rir_speaker_mic1.csv", room.rir[mic1ID][speakerID], delimiter=",")
   np.savetxt("rir_nearsource_mic1.csv", room.rir[mic1ID][personID], delimiter=",")
   np.savetxt("rir_speaker_mic2.csv", room.rir[mic2ID][speakerID], delimiter=",")
   np.savetxt("rir_nearsource_mic2.csv", room.rir[mic2ID][personID], delimiter=",")


#room.simulate(reference_mic=0, snr=30)

#print(room.mic_array.signals[-1,:].shape)

#room.mic_array.to_wav(
#    "output.wav",
#    norm=True,
#    bitdepth=np.int16,
#)#