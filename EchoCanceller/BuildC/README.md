# BuildC Content

The `buildAndRun.sh` script is building the Modelica simulator and launching the demo.

`run.sh` is just re-launching a demo without rebuilding everything. It is useful when you are just changing some simulation parameters or when you have changed the `.axf` running on the [VHT](https://arm-software.github.io/VHT/main/overview/html/index.html).

And there is a `clean.sh` to clean the generated files.

Let's details the steps in `buildAndRun.sh`

`omc $script`

It is compiling the Modelica model and generating C sources, a makefile and an `.xml` file containing the parameters of the model. Those parameters are read by the model when it is launched.

Script is a variable to select `VHTEcho` or `WavEcho` model inside the Modelica `.mo` file.

`make -f "$name.makefile" `

It is building the C sources using the generated Makefile.

`python3.8 params.py -m $name`

It is a script we have written to more easily update the parameters of the model with the right paths.

It is also possible to directly edit the `.xml` file.

This script is generating a copy of the modified `.xml` file and the original is not changed.

`./$name -f $name\_init_updated.xml`

Run the Modelica simulator using the updated `.xml` file with the new parameters.

In case the `VHTEcho` model is run:

`python3.8 analyze.py -m $name`

Analyze the output of the simulation and generate `YesNoResult.png` with the results.

The output of the simulation is `Echo.WavEcho_res.mat` which can plotted using `OMPlot`.

All the variables of the model are contained in this file.

The file can also be opened with Matlab.



