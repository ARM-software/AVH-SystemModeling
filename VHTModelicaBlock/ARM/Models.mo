within ARM;
package Models 
  partial block VHTGeneric "VHT Generic Model"
    extends Modelica.Blocks.Icons.Block;
    import SI=Modelica.Units.SI;
    import intf=Modelica.Blocks.Interfaces;
    import ARM.Types.Frequency_kHz;
    
    parameter Frequency_kHz samplingFrequency()=1 "Sampling frequency of VHT IOs";
    parameter SI.Time computeTime=0.0 "Computation time for the processing";


    parameter Boolean launchVHT = true;
    parameter String vhtCommand="/opt/arm_vsi/VHT-Corstone-300" ;
    parameter String vhtCommandLine="-V /home/ubuntu/VHTModelicaDemos/EchoCanceller/VSI/audio/python -f /home/ubuntu/VHTModelicaDemos/EchoCanceller/fvp_config.txt -a /home/ubuntu/VHTModelicaDemos/EchoCanceller/Objects/OrtaModelica.axf";
    
  
  annotation (Documentation(info = "<html><head>
</head>
<body>
<h1>VHT Model</h1>
<p>A Model allowing to start/stop the ARM Virtual hardware and communicate with it.</p>
<p>The model is assuming that the input/output are in [-1,1]. The input will be saturated 
if it is outside of this interval.</p>
<p>The IOs are sampled. The sampling frequency is a parameter of the model.</p>
<p>The output can be delayed compared to the input of the model thanks to another parameter of the model.</p>
<p>The model can launch the VHT and some parameters are available to define the paths.
But if you want to connect the MDK to the model, uncheck the box controlling the
VHT launch. You'll need to run the MDK simulation after the simulation has started and is
displaying 0% in OMEdit.</p>
<p>If the VHT launch is automatic, the simulation should start roughly 4 secs after displaying 0% in OMEdit.
If it is taking much longer, cancel the simulation and look at the output messages to see
the errors. Generally it is the paths given to the VHT which are not right.</p><p>If you are not using OMEdit, but running the simulation in command line, then you can just wait the the \"Start VHT Server\" to appear in the console before manually launching the VHT.</p><p>Or, you can let the simulator launch the VHT automatically.</p>

</body></html>"),
      Icon(coordinateSystem(preserveAspectRatio = true, extent = {{-100, -100}, {100, 100}}), graphics = {Text(origin = {0, -28},lineColor = {0, 145, 189}, extent = {{-80, 80}, {80, -20}}, textString = "VHT\nCorstone"), Text(origin = {-1, -125}, extent = {{119, -11}, {-119, 11}}, textString = "f=%samplingFrequency")}),
      Diagram);
  end VHTGeneric;

  block VHT "VHT"
    extends VHTGeneric;

    Modelica.Blocks.Interfaces.RealInput x(start=0.0) "Input signal connector" annotation (Placement(
        transformation(extent={{-140,-20},{-100,20}})));
    discrete Modelica.Blocks.Interfaces.RealOutput y(start=0.0) "Output signal connector" annotation (Placement(
        transformation(extent={{100,-10},{120,10}})));

    protected
       VHTCommunication config = VHTCommunication(launchVHT,vhtCommand,vhtCommandLine,1,1);

    algorithm
        when sample(0, 0.001/samplingFrequency) then
           vhtInput(config,0,x);
        end when;
    
        when sample(computeTime, 0.001/samplingFrequency) then
           y := vhtOutput(config,0);
        end when;

  annotation(
      Documentation(info = "<html><head></head><body>A simple VHT with one input and one output.<div><br></div><div>Both IOs are sampled at same frequency.</div></body></html>"));end VHT;

  block VHTMULTI "VHT MULTI"
    extends VHTGeneric;
  parameter Boolean hasTF=false;
  import Modelica.Units.SI.Frequency;
    
    parameter Frequency tfSamplingFrequency=8.3333 "Sampling frequency of VHT TFLite IO";
  Modelica.Blocks.Interfaces.RealInput xa annotation(
      Placement(visible = true, transformation(origin = {100, 56}, extent = {{-20, -20}, {20, 20}}, rotation = 0), iconTransformation(origin = {120, 58}, extent = {{20, -20}, {-20, 20}}, rotation = 0)));
  Modelica.Blocks.Interfaces.RealInput xb annotation(
      Placement(visible = true, transformation(origin = {-120, 56}, extent = {{-20, -20}, {20, 20}}, rotation = 0), iconTransformation(origin = {-120, 58}, extent = {{-20, -20}, {20, 20}}, rotation = 0)));
  discrete Modelica.Blocks.Interfaces.RealOutput ya annotation(
      Placement(visible = true, transformation(origin = {110, -58}, extent = {{-10, -10}, {10, 10}}, rotation = 0), iconTransformation(origin = {110, -60}, extent = {{-10, -10}, {10, 10}}, rotation = 0)));
  discrete Modelica.Blocks.Interfaces.RealOutput yb annotation(
      Placement(visible = true, transformation(origin = {-100, -60}, extent = {{-10, -10}, {10, 10}}, rotation = 0), iconTransformation(origin = {-110, -60}, extent = {{10, -10}, {-10, 10}}, rotation = 0)));
    Modelica.Blocks.Interfaces.RealInput xc annotation(
      Placement(visible = true, transformation(origin = {100, 16}, extent = {{-20, -20}, {20, 20}}, rotation = 0), iconTransformation(origin = {120, 0}, extent = {{20, -20}, {-20, 20}}, rotation = 0)));
    Modelica.Blocks.Interfaces.IntegerOutput tf annotation(
      Placement(visible = true, transformation(origin = {110, -88}, extent = {{-10, -10}, {10, 10}}, rotation = 0), iconTransformation(origin = {110, -88}, extent = {{-10, -10}, {10, 10}}, rotation = 0)));
  protected
    parameter VHTCommunication config = VHTCommunication(launchVHT,vhtCommand,vhtCommandLine,3,3); 
  algorithm
        when sample(0, 0.001/samplingFrequency) then
           vhtInput(config,0,xa);
           vhtInput(config,1,xb);
           vhtInput(config,2,xc);
        end when;
    
        when sample(computeTime, 0.001/samplingFrequency) then
           ya := vhtOutput(config,0);
           yb := vhtOutput(config,1);
        end when;
  
        when sample(0.009, 1.0/tfSamplingFrequency) then
        if hasTF then
           tf := integer(8*vhtOutput(config,2));
        else 
           tf := -1;
        end if;
        end when;
  

  annotation(
      Documentation(info = "<html><head></head><body>This model is made to work with the echo canceller demo which is includign a TFLite (TF) model.<div>So there is an output for the TFLite keyword recognition.</div><div><br></div><div>The value of this output is an integer.</div><div><br></div><div>If the hasTF parameter is true, this output will gets its value from the VHT. If hasTF is false, this output will be forced to -1.</div><div><br></div><div>The meaning of the values for the TF output are:</div><div><ul><li>TF not used : -1</li><li>Unknown or silnce : 0</li><li>Yes : 1</li><li>No : 2</li></ul><div>The sampling rate for the TF output is different from the sampling rate for the other IOs.</div></div><div>This rate is not arbitrary. It has to be chosen according to the real-time dataflow running on the VHT and which should call the TF recognition periodically. The TF rate is derived from the audio sampling frequency and the size of the buffers used in the signal processing front end.</div><div><br></div><div><br></div></body></html>"));end VHTMULTI;

  protected
  class VHTCommunication "VHTCommunication object"
    extends ExternalObject;
     protected
     function constructor "Connect to VHT"
        input Boolean launchVHT;
        input String vhtCommand;
        input String vhtCommandLine;
        input Integer nbInputs; 
        input Integer nbOutputs;
        output VHTCommunication config;
        external "C" config=createConfig(launchVHT,vhtCommand,vhtCommandLine,nbInputs,nbOutputs)
          annotation(Library = "VHT",
                  Include="#include \"model.h\"");
  
      end constructor;
     function destructor "Close connections to VHT"
        input VHTCommunication config;
        external "C" destroyConfig(config)
        annotation(Library = "VHT",
                  Include="#include \"model.h\"");
      end destructor;
   end VHTCommunication;
   impure function vhtOutput "VHT Output - read from VHT"
      input VHTCommunication server;
      input Integer ioID;
      output Real y;
      external "C" y = getSample(server,ioID)
        annotation(Library = "VHT",
                   Include="#include \"model.h\"");
 
   end vhtOutput;
   impure function vhtInput "VHT Input - write to VHT"
      input VHTCommunication server;
      input Integer ioID;
      input Real x;
      external "C" writeSample(server,ioID,x)
        annotation(Library = "VHT",
                   Include="#include \"model.h\"");

  end vhtInput;

annotation(Documentation(info = "<html><head>
</head>
<body>
<h1>ARM Modelica Package</h1>
<p>Experimental package to connect the ARM Virtual Hardware (VHT) to Modelica.</p><p>It contains a generic model which can be inherited to create different models with different numbers of input / outputs.</p><p>It is containing two models for demos:</p><p></p><ul><li>A model with one input and one output</li><li>A model with 3 inputs, 3 outputs but with 1 output being integer and with different sampling frequency</li></ul><div><br></div><div>The sampling frequency must be specified for the IOs.</div><div><br></div><div>It is also possible to specifify if Modelica should launch automatically the VHT process.</div><div>If yes, then the path to the VHT and the command line options but be specified as parameter of the model.</div><div><br></div><div>VHT and Modelica are using sockets to communicate. It is not yet possible to specify the socket port to use.</div><div>It is currently hardcoded in the C code provided by this package and by the Python scripts used on the VHT side.</div><div><br></div><p></p>

</body></html>"),
Icon(
    coordinateSystem(preserveAspectRatio = true, extent = {{-100, -100}, {100, 100}}), graphics = {Rectangle(lineThickness = 1.25, extent = {{-98, 98}, {98, -98}}), Bitmap(origin = {-1, 0}, extent = {{-93, 94}, {93, -94}}, imageSource = "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")}));
end Models;
