# README

The files in this folder are coming from the [Microspeech](https://github.com/tensorflow/tflite-micro/tree/main/tensorflow/lite/micro/examples/micro_speech) example.

They have been included in this project to make it easier to build it and also because some files were modified.

The files are covered by the LICENSE file include in this folder.

Copyright for the files:

- Copyright 2018 The TensorFlow Authors. All Rights Reserved.

And for the [KissFFT](https://github.com/mborgerding/kissfft) used by this project:

- Copyright (c) 2003-2010 Mark Borgerding . All rights reserved.

# CHANGES

## Microfrontend

All headers referencing TFLite have been changed so that we only reference microfrontend in the path.

microfrontend is a separate folder in this project and not more included in the TFLite hierarchy.

As consequence, most of include paths have been changed.

`fft`, `fft_utils` and `mfccNode` are using the [speex DSP library](https://gitlab.xiph.org/xiph/speexdsp) FFT wrapper

### fft.cc
Removed microfrontend [KissFFT](https://github.com/mborgerding/kissfft) header to use the [KissFFT](https://github.com/mborgerding/kissfft) from libspeex.
It looks like the [speex DSP library](https://gitlab.xiph.org/xiph/speexdsp) [KissFFT](https://github.com/mborgerding/kissfft) package has a bit more things inside and
when building with the [KissFFT](https://github.com/mborgerding/kissfft)  CMSIS pack it was not building. 

`kiss_fftr2` is in  [speex DSP library](https://gitlab.xiph.org/xiph/speexdsp) but not in official [KissFFT](https://github.com/mborgerding/kissfft) 

So, we are no more using the [KissFFT](https://github.com/mborgerding/kissfft)  CMSIS pack, and instead relying on the [speex DSP library](https://gitlab.xiph.org/xiph/speexdsp)  [KissFFT](https://github.com/mborgerding/kissfft) .

The `Fffstate` is removed and instead we directly use the `void *` from the  [speex DSP library](https://gitlab.xiph.org/xiph/speexdsp)  FFT wrapper.

This file will be modified again when we start using CMSIS-DSP FFT.

`FFtIni`t and `FFtReset` are no more used

### fft.h

Use of  [speex DSP library](https://gitlab.xiph.org/xiph/speexdsp) [KissFFT](https://github.com/mborgerding/kissfft).
Removed the `FftState` and use  `void *` from the  [speex DSP library](https://gitlab.xiph.org/xiph/speexdsp)  FFT wrapper.
`FFtInit` and `FFtReset` are no more used

### fft_util.cc 

Use of [speex DSP library](https://gitlab.xiph.org/xiph/speexdsp) [KissFFT](https://github.com/mborgerding/kissfft).
Removed the `FftState` and use  `void *` from the  [speex DSP library](https://gitlab.xiph.org/xiph/speexdsp)  FFT wrapper.

### fft_util.h 
Use of [speex DSP library](https://gitlab.xiph.org/xiph/speexdsp) [KissFFT](https://github.com/mborgerding/kissfft).
Removed the `FftState` and use  `void *` from the  [speex DSP library](https://gitlab.xiph.org/xiph/speexdsp)  FFT wrapper.

### filterbank.cc

Use of `MVE` code 

Files not used have been removed (like `frontend.c`)

## micro_speech 

### command_responder.cc 

Include `micro_model_settings.h` 
Return a command ID (that will be sent to the external world by the VHT).

Original version was using a string for the detected command.

### command_responder.h 

Return an ID in API

### main_functions.cc 

Define some `EventRecorder` functions.

Remove headers to frontend functions and other micro_speech functions which are now called as part of the CMSIS-DPS Synchronous Dataflow and are no more part of this function.

Introduce a global `g_audio_time_stamp`.


#### ml_setup

Feature provider object is no more created. Features are provided by the synchronous dataflow.

#### loop

This function has been removed. The ML task is waking up on demand by the realtime task running the
SDF.

New functions were introduced:

`copyDataToML()`

Copy data for use by the ML thread

`getDataForML()`

Get data which has been copied for use by the TFLite network (so update input of network)

`setCommand()`

Set the recognizedCommand variable.

`getCommand()`

get and return the latest recognized command 

#### mlloop

It is the ML thread. It is similar to the original loop function.

It is sleeping at entry. The SDF thread is waking it up.

First line after the sleep is to get feature for ML (`getDataForML`). We get the latest feature which has been pushed by the SDF.

Then we follow similar process to original loop:
TFLite interpreter, `Recognizer` and `RespondToCommand`.

The value from `RespondToCommand` is saved for later use by the SDF task.
If no new command is detected, by default we return 0.

### main-functions.h

The new functions defined in `main-functions.h`.
cmsis-rtos header and mutex declaration

### recognize_commands.cc 

Labels are no more coded by strings but by integers.
The integer are then returned to the SDF and communicated outside of the [VHT](https://arm-software.github.io/VHT/main/overview/html/index.html)


### recognize_commands.h

Labels are no more coded by strings but by integers.

### New files

audio_provider removed since audio is coming from the SDF
Network model is in micro_features folder 

We have new nodes for the SDF

#### clampNode.h 

It used to be in `GenerateMicroFeature`s which was calling the Micro front end and then
doing this clamping.

Following files used to be in `frontend.c` and called by `FrontendProcessSamples`

#### mfccNode.h 

Most of the work done in `FrontendProcessSamples` (`frontend.c`) except the window preprocessing


#### tfliteNode.h 
Communication with ML thread through:
`copyDataToML`, `getCommand`, `osThreadFlagsSet` (to wakeup the task)
`writeTFLiteOutput`

`writeTFLiteOutput` is using the `audioDriver.c` to make it easier because it is reusing
the functions for the virtual audio driver.

But all what is done is writing into a register of this driver and the corresponding Python
script is sending this value to Modelica.

It could be in a different file not related to audioDriver to make things cleaner.

#### windowNode.h 

Generate window coefficients and multiply samples by windows.
MVE code used.
Used to be in `window.c` in the microfrontend

