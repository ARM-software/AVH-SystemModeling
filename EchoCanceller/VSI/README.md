# VSI scripts

The Python scripts in `audio` are implementing the communication with `Modelica`.

Each script is connecting to the Modelcia server and:

- Identify itself as input or output
- Send an ID number (they should start at 0 for inputs and at 0 for outputs and increment for new IOs of the same kind)
- For output, at connection, a packet of DMA size is sent to avoid any deadlock in the VHT<->Modelica feedback loop.
- This DMA size is hardcoded and should be coherent with was is used in the code for the compute graph running on the  [VHT](https://arm-software.github.io/VHT/main/overview/html/index.html) .

