# Echo Canceller and Keyword spotting on Virtual Hardware

This demo is integrating an echo canceller and keyword spotter on the [ARM Virtual Hardware](https://arm-software.github.io/VHT/main/overview/html/index.html) which is interacting with a model of the room acoustic implemented with [OpenModelica](https://www.openmodelica.org/)

- [How to build and run the demo](EchoCanceller/Documentation/BUILDANDRUN.md)
- [Understanding what is inside the demo](EchoCanceller/Documentation/DEMO.md)

## Quick introduction

For more details, see the above links.

The simulation is containing two simulators:

- The [ARM Virtual Hardware](https://arm-software.github.io/VHT/main/overview/html/index.html) (VHT) where the M55 application is run
- The [OpenModelica](https://www.openmodelica.org/) model as a digital twin and implementing a model of the world

The [OpenModelica](https://www.openmodelica.org/) simulator is launching the [VHT](https://arm-software.github.io/VHT/main/overview/html/index.html).

The [VHT](https://arm-software.github.io/VHT/main/overview/html/index.html) is connecting to the [Modelica](https://www.openmodelica.org/) simulator using sockets.

![architecture](EchoCanceller/Documentation/architecture.png)

The [Modelica](https://www.openmodelica.org/) simulator is implementing the acoustic of a room as described on the Modelica architecture diagram.

![vhtechocanceller](EchoCanceller/Documentation/vhtechocanceller.png)

A `farSource` (background voices) is played through the `nearSpeaker` and captured by the near microphones (either through direct pass from the speaker to the micros or through the room echo - both are modeled with the `roomEcho` block).

The `nearSource` is playing a `yes/no` signal.

We want to detect the `yes/no` keywords and remove the background noise.

The echo canceller and noise reduction (and simplified beamformer) are cleaning the signal before recognition by the Tensor Flow Lite network.

The cleaned signal is also sent back to the far end.
