# README

The files in this folder are covered with the license detailed in file COPYING.

Those files are coming from the [SpeexDSP](https://gitlab.xiph.org/xiph/speexdsp) project.

Copyright for those files:

- Copyright 2002-2008 	Xiph.org Foundation
- Copyright 2002-2008 	Jean-Marc Valin
- Copyright 2005-2007	Analog Devices Inc.
- Copyright 2005-2008	Commonwealth Scientific and Industrial Research Organisation (CSIRO)
- Copyright 1993, 2002, 2006 David Rowe
- Copyright 2003 		EpicGames
- Copyright 1992-1994	Jutta Degener, Carsten Bormann

And for the [KissFFT](https://github.com/mborgerding/kissfft) used by this project:

- Copyright (c) 2003-2010 Mark Borgerding . All rights reserved.

They have been copied here to make it easier to build the project.

Some files have been modified compared to the orignal as detailed below.

# CHANGES

## fft_wrap.h

Added `#define` to be able to include it from C++ side
Added API for fft without normalization for use by [Microspeech](https://github.com/tensorflow/tflite-micro/tree/main/tensorflow/lite/micro/examples/micro_speech).

To make it easier, in the future, to replace the [KissFFT](https://github.com/mborgerding/kissfft) by another one, [Microspeech](https://github.com/tensorflow/tflite-micro/tree/main/tensorflow/lite/micro/examples/micro_speech) is using the FFT wrapper inside the speex library.

But, the speex library is exporting a different function which is a variation on the original [KissFFT](https://github.com/mborgerding/kissfft) API.

For [Microspeech](https://github.com/tensorflow/tflite-micro/tree/main/tensorflow/lite/micro/examples/micro_speech) we do not want to use this variation but only the original [KissFFT](https://github.com/mborgerding/kissfft) . So, a new API has been added to the wrapper.


## arch.h
FIXED_POINT is defined at the beginning of the file 

## mfc.c 
PLAYBACK_DELAY set to 3
We have 2 buffers of delay due to the signal processing and one additional buffer of delay
introduced by the virtual HW implemented in Python.
This delay is to prevent a deadlock due to the feedback loop connecting output of VHT to input of VHT.
We are aware of this limitation and looking at some ways to improve this.

The following code has been added:

```c
#ifdef TWO_PATH
   Dbf = 0;
#endif 
```

Compilation issue when `TWO_PATH` not defined

## os_support.h 

Forced inclusion of

```c
#include "config.h"
#include "os_support_custom.h"
```

by removing the `#ifdef` 

## config.h
Added to the project 

It is defining the size of the standard datatypes and the `#define` to use the  [KissFFT](https://github.com/mborgerding/kissfft)

## os_support_custom.h 
Added to the project and use `printf` for error handling.

`stdout` is used rather the `stderr` because in semihosting environment sometimes you only have
`stdout`.

All files not used have been removed.
