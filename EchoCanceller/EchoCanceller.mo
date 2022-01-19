package Echo "Echo canceller digital twin for VHT"
  package Types
    extends Modelica.Icons.TypesPackage;
    type DB_SPL = Real(final quantity = "dbSPL", final unit = "dbSPL") "Sound intensity";

    function amplitudeFromDb "Amplitude from dB"
      import Modelica.Units.SI.PressureDifference;
    protected
      constant PressureDifference ref = 20e-6;
    public
      input DB_SPL u;
      output PressureDifference y;
    algorithm
      y := 10 ^ (u / 20.0) * ref;
    annotation(
        Documentation(info = "<html><head></head><body>This is converting a dbSPL input into a PressureDifference for the output.<div>We are using the standard formula:</div><div><br></div><div>dpSPL = 20 log10(amplitude) - log10(p0) where p0 is the value of 20 microPascal</div><div><br></div><div>As consequence:</div><div><br></div><div>amplitude = 10^(dpSPL/20)*p0&nbsp;</div><div><br></div><div><br></div></body></html>"));end amplitudeFromDb;
  annotation(
      Documentation(info = "<html><head></head><body>In the demo we are using gains expressed as dbSPL.<div><br></div><div>The idea is that the output from the VHT (in [-1,1]) are converted to Pascal in speaker models.</div><div><br></div><div>And the Pascal are converted, in the microphone models, to values in float for the VHT inputs.</div><div>VHT is saturating the value if they are outside [-1,1].</div><div><br></div><div><br></div></body></html>"));
  end Types;

  package Interfaces
    extends Modelica.Icons.InterfacesPackage;

    partial model EchoCanceller "Interface for echo canceller"
      extends Modelica.Blocks.Icons.Block;
      parameter Integer nmic=1 "Number of near microphones";
      Modelica.Blocks.Interfaces.RealInput farMic annotation(
        Placement(visible = true, transformation(origin = {-120, 40}, extent = {{-20, -20}, {20, 20}}, rotation = 0), iconTransformation(origin = {-120, 40}, extent = {{-20, -20}, {20, 20}}, rotation = 0)));
      Modelica.Blocks.Interfaces.RealOutput farSpeaker annotation(
        Placement(visible = true, transformation(origin = {-100, -40}, extent = {{-10, -10}, {10, 10}}, rotation = 0), iconTransformation(origin = {-110, -40}, extent = {{10, -10}, {-10, 10}}, rotation = 0)));
      Modelica.Blocks.Interfaces.RealInput nearMic[nmic] annotation(
        Placement(visible = true, transformation(origin = {100, 40}, extent = {{-20, -20}, {20, 20}}, rotation = 0), iconTransformation(origin = {120, 40}, extent = {{20, -20}, {-20, 20}}, rotation = 0)));
      Modelica.Blocks.Interfaces.RealOutput nearSpeaker annotation(
        Placement(visible = true, transformation(origin = {110, -42}, extent = {{-10, -10}, {10, 10}}, rotation = 0), iconTransformation(origin = {110, -42}, extent = {{-10, -10}, {10, 10}}, rotation = 0)));
  Modelica.Blocks.Interfaces.IntegerOutput tf annotation(
        Placement(visible = true, transformation(origin = {110, -84}, extent = {{-10, -10}, {10, 10}}, rotation = 0), iconTransformation(origin = {110, -84}, extent = {{-10, -10}, {10, 10}}, rotation = 0)));
      annotation(
        Icon(graphics = {Line(origin = {0, -1}, points = {{-100, 41}, {-68, 41}, {60, -41}, {100, -41}, {100, -41}}), Line(origin = {0, -2}, points = {{100, 42}, {62, 42}, {-66, -38}, {-100, -38}})}));
    end EchoCanceller;

    partial model RoomEcho "Interface for Room echo"
      extends Modelica.Blocks.Icons.Block;
  Modelica.Blocks.Interfaces.RealInput speaker annotation(
        Placement(visible = true, transformation(origin = {-120, 40}, extent = {{-20, -20}, {20, 20}}, rotation = 0), iconTransformation(origin = {-120, 50}, extent = {{-20, -20}, {20, 20}}, rotation = 0)));
  Modelica.Blocks.Interfaces.RealOutput mic1 annotation(
        Placement(visible = true, transformation(origin = {110, 40}, extent = {{-10, -10}, {10, 10}}, rotation = 0), iconTransformation(origin = {110, 40}, extent = {{-10, -10}, {10, 10}}, rotation = 0)));
  Modelica.Blocks.Interfaces.RealInput nearSource annotation(
        Placement(visible = true, transformation(origin = {-120, -40}, extent = {{-20, -20}, {20, 20}}, rotation = 0), iconTransformation(origin = {-120, -50}, extent = {{-20, -20}, {20, 20}}, rotation = 0)));
  Modelica.Blocks.Interfaces.RealOutput mic2 annotation(
        Placement(visible = true, transformation(origin = {110, -40}, extent = {{-10, -10}, {10, 10}}, rotation = 0), iconTransformation(origin = {110, -40}, extent = {{-10, -10}, {10, 10}}, rotation = 0)));
      annotation(
        Diagram,
        Icon(graphics = {Line(origin = {13.59, 10}, points = {{-13.5905, 49.9979}, {44.4095, 9.9979}, {-37.5905, -46.0021}, {-43.5905, -50.0021}}, pattern = LinePattern.Dash, arrow = {Arrow.None, Arrow.Filled}, arrowSize = 8), Line(origin = {60, 1}, points = {{0, 67}, {0, -67}}, thickness = 1.5), Polygon(origin = {-80, 51}, fillPattern = FillPattern.Solid, points = {{-8, 9}, {-8, -9}, {-2, -9}, {8, -19}, {8, 19}, {-2, 9}, {-8, 9}}), Text(origin = {91, 42}, extent = {{9, 12}, {-9, -12}}, textString = "1"), Text(origin = {91, -40}, extent = {{9, 12}, {-9, -12}}, textString = "2")}));
    end RoomEcho;

    partial model Speaker "Interface for speaker"
      extends Modelica.Blocks.Icons.Block;
      Modelica.Blocks.Interfaces.RealInput u annotation(
        Placement(visible = true, transformation(origin = {-120, 0}, extent = {{-20, -20}, {20, 20}}, rotation = 0), iconTransformation(origin = {-120, 0}, extent = {{-20, -20}, {20, 20}}, rotation = 0)));
      annotation(
        Icon(graphics = {Line(origin = {-17.13, -3.87}, points = {{44.5626, 21.6148}, {48.4206, 15.4228}, {50.6866, 8.13684}, {50.6866, 0.308842}, {50.6866, -7.64716}, {48.3586, -15.0392}, {44.3926, -21.2912}}, thickness = 1, smooth = Smooth.Bezier), Line(origin = {-17.13, -3.87}, points = {{58.4706, -35.3712}, {65.8606, -25.3992}, {70.2386, -13.0572}, {70.2386, 0.308842}, {70.2386, 13.5508}, {65.9366, 25.7848}, {58.6626, 35.7068}}, thickness = 1, smooth = Smooth.Bezier), Line(origin = {-17.13, -3.87}, points = {{71.7266, 48.7768}, {82.2606, 35.4448}, {88.5626, 18.6168}, {88.5626, 0.310842}, {88.5626, -18.1232}, {82.1786, -35.0532}, {71.5246, -48.4252}}, thickness = 1, smooth = Smooth.Bezier), Polygon(origin = {-23, 0}, fillPattern = FillPattern.Solid, points = {{27.0846, -48.9072}, {-7.22337, -19.2332}, {-39.6934, -19.2332}, {-39.6934, 18.9528}, {-7.71537, 18.9528}, {27.0846, 49.0548}, {27.0846, -48.9072}})}));
    end Speaker;

    partial model Microphone "Interface for microphone"
      extends Modelica.Blocks.Interfaces.SISO;
      
      annotation(
        Icon(graphics = {Ellipse(origin = {16, 23}, fillPattern = FillPattern.Solid, extent = {{-34, 33}, {34, -33}}), Polygon(origin = {-34, -21}, lineThickness = 1, points = {{24, 25}, {-18, -25}, {4, -41}, {44, 11}, {24, 25}})}));
    end Microphone;

    partial model EchoCancellerArchitecture "Architecture of an echo canceller"
      replaceable Echo.Interfaces.EchoCanceller echoCanceller(nmic = 2)  annotation(
        Placement(visible = true, transformation(origin = {-2, 0}, extent = {{-10, -10}, {10, 10}}, rotation = 0)));
      replaceable Echo.Interfaces.RoomEcho roomEcho annotation(
        Placement(visible = true, transformation(origin = {98, 16}, extent = {{10, 10}, {-10, -10}}, rotation = 0)));
      replaceable Echo.Interfaces.Microphone nearMic1  annotation(
        Placement(visible = true, transformation(origin = {50, 4}, extent = {{10, -10}, {-10, 10}}, rotation = 0)));
      replaceable Echo.Interfaces.Microphone farMic annotation(
        Placement(visible = true, transformation(origin = {-54, 18}, extent = {{-10, -10}, {10, 10}}, rotation = 0)));
      replaceable Echo.Interfaces.Speaker farSpeaker annotation(
        Placement(visible = true, transformation(origin = {-54, -18}, extent = {{10, -10}, {-10, 10}}, rotation = 0)));
      Echo.Implementations.PassThroughSpeaker nearSpeaker annotation(
        Placement(visible = true, transformation(origin = {50, -24}, extent = {{-10, -10}, {10, 10}}, rotation = 0)));
  replaceable Echo.Interfaces.AudioSource nearSource annotation(
        Placement(visible = true, transformation(origin = {140, 32}, extent = {{10, -10}, {-10, 10}}, rotation = 0)));
  replaceable Echo.Interfaces.AudioSource farSource annotation(
        Placement(visible = true, transformation(origin = {-102, 18}, extent = {{-10, -10}, {10, 10}}, rotation = 0)));
  replaceable Echo.Interfaces.Microphone nearMic2 annotation(
        Placement(visible = true, transformation(origin = {50, 32}, extent = {{10, -10}, {-10, 10}}, rotation = 0)));
    equation
  connect(nearMic1.y, echoCanceller.nearMic[1]) annotation(
        Line(points = {{39, 4}, {10, 4}}, color = {0, 0, 127}));
      connect(farMic.y, echoCanceller.farMic) annotation(
        Line(points = {{-43, 18}, {-26, 18}, {-26, 4}, {-14, 4}}, color = {0, 0, 127}));
      connect(echoCanceller.farSpeaker, farSpeaker.u) annotation(
        Line(points = {{-13, -4}, {-26, -4}, {-26, -18}, {-42, -18}}, color = {0, 0, 127}));
  connect(echoCanceller.nearSpeaker, nearSpeaker.u) annotation(
        Line(points = {{10, -4}, {26, -4}, {26, -24}, {38, -24}}, color = {0, 0, 127}));
  connect(farSource.y, farMic.u) annotation(
        Line(points = {{-90, 18}, {-66, 18}}, color = {255, 170, 0}, thickness = 1));
  connect(nearSpeaker.y, roomEcho.speaker) annotation(
        Line(points = {{61, -24}, {136, -24}, {136, 11}, {110, 11}}, color = {255, 170, 0}, thickness = 1));
  connect(nearSource.y, roomEcho.nearSource) annotation(
        Line(points = {{129, 32}, {118, 32}, {118, 21}, {110, 21}}, color = {255, 170, 0}, thickness = 1));
  connect(roomEcho.mic1, nearMic1.u) annotation(
        Line(points = {{87, 12}, {74.5, 12}, {74.5, 4}, {62, 4}}, color = {255, 170, 0}, thickness = 1));
  connect(roomEcho.mic2, nearMic2.u) annotation(
        Line(points = {{88, 20}, {74, 20}, {74, 32}, {62, 32}}, color = {255, 170, 0}, thickness = 1));
  connect(nearMic2.y, echoCanceller.nearMic[2]) annotation(
        Line(points = {{39, 32}, {22, 32}, {22, 4}, {10, 4}}, color = {0, 0, 127}));
    protected
      annotation(
        Diagram(graphics = {Rectangle(origin = {-75, 8}, lineColor = {0, 0, 255}, fillColor = {240, 239, 255}, pattern = LinePattern.Dash, fillPattern = FillPattern.Solid, lineThickness = 2, extent = {{-51, 42}, {51, -42}}), Rectangle(origin = {87, 6}, lineColor = {0, 170, 127}, fillColor = {234, 255, 246}, pattern = LinePattern.Dash, fillPattern = FillPattern.Solid, lineThickness = 2, extent = {{-69, 44}, {69, -44}}), Text(origin = {88, 60}, extent = {{12, -6}, {-12, 0}}, textString = "Near End"), Text(origin = {-77, 60}, extent = {{9, -6}, {-9, 0}}, textString = "Far End")}, coordinateSystem(extent = {{-140, 100}, {160, -40}})));
    end EchoCancellerArchitecture;
    
    partial model AudioSource "Interface for speaker"
      extends Modelica.Blocks.Icons.Block;
      
  Modelica.Blocks.Interfaces.RealOutput y annotation(
        Placement(visible = true, transformation(origin = {148, -2}, extent = {{-10, -10}, {10, 10}}, rotation = 0), iconTransformation(origin = {110, 0}, extent = {{-10, -10}, {10, 10}}, rotation = 0)));
      annotation(
        Icon(graphics = {Line(origin = {-61.13, 0.13}, points = {{44.5626, 21.6148}, {48.4206, 15.4228}, {50.6866, 8.13684}, {50.6866, 0.308842}, {50.6866, -7.64716}, {48.3586, -15.0392}, {44.3926, -21.2912}}, thickness = 1, smooth = Smooth.Bezier), Line(origin = {-61.13, 0.13}, points = {{58.4706, -35.3712}, {65.8606, -25.3992}, {70.2386, -13.0572}, {70.2386, 0.308842}, {70.2386, 13.5508}, {65.9366, 25.7848}, {58.6626, 35.7068}}, thickness = 1, smooth = Smooth.Bezier), Line(origin = {-61.13, 0.13}, points = {{71.7266, 48.7768}, {82.2606, 35.4448}, {88.5626, 18.6168}, {88.5626, 0.310842}, {88.5626, -18.1232}, {82.1786, -35.0532}, {71.5246, -48.4252}}, thickness = 1, smooth = Smooth.Bezier)}));
    end AudioSource;
  annotation(
      Documentation(info = "<html><head></head><body>We are using the Modelica feature allowing to specify an architecture without having to specify how the models are implemented.<div><br></div><div>For that purpose, we are defining the interfaces of all the blocks needed in the demo.</div><div><br></div><div>The EchoCancellerArchitecture model is connecting all of those interfaces.</div><div><br></div><div>We have two implementation of this architecture define in the package:</div><div><ul><li>WavEcho which is not using the VHT and is just to test the echo modeling&nbsp;</li><li>VHTEcho which is using the VHTMULTI model</li></ul></div></body></html>"));
  end Interfaces;

model WavEcho
  extends Echo.Interfaces.EchoCancellerArchitecture(
  redeclare replaceable Implementations.EmptyCanceller echoCanceller, 
  redeclare replaceable Implementations.WavSource farSource(gain=55,path = "/home/ubuntu/VHTModelicaDemos/EchoCanceller/sounds/background.wav"), 
  redeclare replaceable Implementations.Micro farMic(), 
  redeclare replaceable Implementations.WavSource nearSource(gain=55,path = "/home/ubuntu/VHTModelicaDemos/EchoCanceller/sounds/yesno.wav"),
  redeclare replaceable Implementations.Micro nearMic1(gain=75), 
  redeclare replaceable Implementations.Micro nearMic2(gain=75),
  redeclare replaceable Implementations.WavSpeaker farSpeaker(path = "/home/ubuntu/VHTModelicaDemos/EchoCanceller/BuildC/signalWithEcho.wav"), 
  redeclare replaceable Implementations.PyRoomAcoustics roomEcho(
  speakerToMic1RIR = "/home/ubuntu/VHTModelicaDemos/EchoCanceller/rir_speaker_mic1.csv",
  nearSourceToMic1RIR="/home/ubuntu/VHTModelicaDemos/EchoCanceller/rir_nearsource_mic1.csv",
  speakerToMic2RIR = "/home/ubuntu/VHTModelicaDemos/EchoCanceller/rir_speaker_mic2.csv",
  nearSourceToMic2RIR="/home/ubuntu/VHTModelicaDemos/EchoCanceller/rir_nearsource_mic2.csv"), nearSpeaker.gain = 94);
equation

  annotation(
    Diagram(coordinateSystem(extent = {{-200, -200}, {200, 200}})),
      Documentation(info = "<html><head></head><body>Implementation of the EchoCancellerArchitecture using a model of the echo canceller which is doing nothing.<div>It is just to test the echo modeling and the environment to be used by the VHT.</div></body></html>"));

end WavEcho;

  package Implementations
    extends Modelica.Icons.InternalPackage;

    model EmptyCanceller
      extends Echo.Interfaces.EchoCanceller;
  Modelica.Blocks.Math.Sum sum1(nin=nmic) annotation(
        Placement(visible = true, transformation(origin = {40, 40}, extent = {{10, -10}, {-10, 10}}, rotation = 0)));
  Modelica.Blocks.Math.Gain gain(k = 0.5)  annotation(
        Placement(visible = true, transformation(origin = {2, 40}, extent = {{10, -10}, {-10, 10}}, rotation = 0)));
  Modelica.Blocks.Sources.IntegerConstant integerConstant(k = 0)  annotation(
        Placement(visible = true, transformation(origin = {26, -84}, extent = {{-10, -10}, {10, 10}}, rotation = 0)));
    equation
      connect(farMic, nearSpeaker) annotation(
        Line(points = {{-120, 40}, {2, 40}, {2, -42}, {122, -42}}, color = {0, 0, 127}, smooth = Smooth.Bezier));
  connect(sum1.y, gain.u) annotation(
        Line(points = {{30, 40}, {14, 40}}, color = {0, 0, 127}));
  connect(gain.y, farSpeaker) annotation(
        Line(points = {{-8, 40}, {-48, 40}, {-48, -40}, {-100, -40}}, color = {0, 0, 127}, smooth = Smooth.Bezier));
  connect(nearMic, sum1.u) annotation(
        Line(points = {{100, 40}, {52, 40}}, color = {0, 0, 127}, thickness = 0.5));
  connect(integerConstant.y, tf) annotation(
        Line(points = {{37, -84}, {110, -84}}, color = {255, 127, 0}));
      annotation(
        Diagram(coordinateSystem(extent = {{-200, -200}, {200, 200}})),
        Documentation(info = "<html><head></head><body>Implementation of the EchoCanceller node used in the WavEcho implementation.<div><br><div>This node is doing nothing and just connection the farSource to the nearSpeaker and the nearSources to the farSpeaker.</div><div>The two near sources (micros) are averaged.</div></div><div><br></div></body></html>"));
    end EmptyCanceller;

    model Micro
      extends Echo.Interfaces.Microphone;
      import ARM.Types.Frequency_kHz;
      import Echo.Types.DB_SPL;
      parameter DB_SPL gain = 70 "Gain of microphone";
  FromPressure fromPressure(gain=gain) annotation(
        Placement(visible = true, transformation(origin = {-6, 0}, extent = {{-10, -10}, {10, 10}}, rotation = 0)));
    equation
      connect(fromPressure.y, y) annotation(
        Line(points = {{6, 0}, {110, 0}}, color = {0, 0, 127}));
  connect(u, fromPressure.u) annotation(
        Line(points = {{-120, 0}, {-18, 0}}, color = {0, 0, 127}));
      annotation(
        Diagram(coordinateSystem(extent = {{-200, -200}, {200, 200}})),
        Icon(graphics = {Text(origin = {-1, -125}, extent = {{119, -11}, {-119, 11}}, textString = "f=%samplingFrequency")}),
  Documentation(info = "<html><head></head><body>The micro model is converting a PressureDifference to a real value to be used as input of the VHT.<div>The conversion is specified as a gain in dbSPL. This gain is describing the pressure difference which is mapped to -1 or 1.</div><div><br></div><div><b>So a smaller gain means real signal in output will be stronger !</b></div><div><b><br></b></div><div><b><br></b></div><div><br></div></body></html>"));
    end Micro;

    model ToPressure
      extends Modelica.Blocks.Interfaces.SISO;
      import Echo.Types.DB_SPL;
      parameter DB_SPL gain = 70;
    equation
      y = Echo.Types.amplitudeFromDb(gain) * u;
      annotation(
        Diagram(coordinateSystem(extent = {{-200, -200}, {200, 200}})),
        Icon(graphics = {Text(origin = {-5, 4}, extent = {{-81, 34}, {81, -34}}, textString = ">P")}),
  Documentation(info = "<html><head></head><body>The function is converting a real value to a pressure difference using a gain expressed in dbSPL.<div><br></div><div>The gain is the pressure difference corresponding to a value of 1.</div></body></html>"));
    end ToPressure;

    model WavSpeaker
      extends Echo.Interfaces.Speaker;
      import ARM.Types.Frequency_kHz;
      import Echo.Types.DB_SPL;
      parameter Frequency_kHz samplingFrequency = 16 "Sampling Frequency";
      parameter String path = "Path to output wav";
      ARM.Sound.WaveOutput waveOutput(path = path, samplingFrequency = samplingFrequency) annotation(
        Placement(visible = true, transformation(origin = {-18, 0}, extent = {{-10, -10}, {10, 10}}, rotation = 0)));
    equation
      connect(u, waveOutput.x) annotation(
        Line(points = {{-120, 0}, {-30, 0}}, color = {0, 0, 127}));
      annotation(
        Diagram(coordinateSystem(extent = {{-200, -200}, {200, 200}})),
        Icon(graphics = {Text(origin = {-1, -125}, extent = {{119, -11}, {-119, 11}}, textString = "f=%samplingFrequency")}),
  Documentation(info = "<html><head></head><body>Save a real signal into a wav file.<div>The sampling frequency to use for generating the wav must be specified as a parameter.</div><div>The path to the output file must also be specified.</div><div><br></div></body></html>"));
    end WavSpeaker;

    model FromPressure
      extends Modelica.Blocks.Interfaces.SISO;
      import Echo.Types.DB_SPL;
      parameter DB_SPL gain;
    protected
      parameter Real scaling = Echo.Types.amplitudeFromDb(gain);
    equation
      y = u / scaling;
      annotation(
        Diagram(coordinateSystem(extent = {{-200, -200}, {200, 200}})),
        Icon(graphics = {Text(origin = {5, 4}, extent = {{-81, 34}, {81, -34}}, textString = "P>")}),
  Documentation(info = "<html><head></head><body>Convert a pressure difference to a value between [-1,1].<div><br></div><div>The conversion factor is specified with a gain in dbSPL. It is corresponding to the pressure mapped to 1.</div><div><b>So a smaller gain means a higher signal !</b></div></body></html>"));
    end FromPressure;

    model PyRoomAcoustics
      extends Echo.Interfaces.RoomEcho;
      import ARM.Types.Frequency_kHz;
      parameter Frequency_kHz samplingFrequency = 16;
      parameter String speakerToMic1RIR "Speaker to Mic1 RIR";
      parameter String nearSourceToMic1RIR "Near Source to Mic1 RIR";
      parameter String speakerToMic2RIR "Speaker to Mic2 RIR";
      parameter String nearSourceToMic2RIR "Near Source to Mic2 RIR";
      ARM.FIR.DigitalFIR speakerToMic1(samplingFrequency = samplingFrequency, path = speakerToMic1RIR) annotation(
        Placement(visible = true, transformation(origin = {-54, 40}, extent = {{-10, -10}, {10, 10}}, rotation = 0)));
      Modelica.Blocks.Nonlinear.FixedDelay fixedDelay(delayTime = 1.0e-3 / samplingFrequency) annotation(
        Placement(visible = true, transformation(origin = {58, 40}, extent = {{-10, -10}, {10, 10}}, rotation = 0)));
  ARM.FIR.DigitalFIR nearSourceToMic1(path = nearSourceToMic1RIR, samplingFrequency = samplingFrequency) annotation(
        Placement(visible = true, transformation(origin = {-54, 2}, extent = {{-10, -10}, {10, 10}}, rotation = 0)));
  Modelica.Blocks.Math.Add add annotation(
        Placement(visible = true, transformation(origin = {6, 40}, extent = {{-10, -10}, {10, 10}}, rotation = 0)));
  ARM.FIR.DigitalFIR speakerToMic2(path = speakerToMic2RIR, samplingFrequency = samplingFrequency) annotation(
        Placement(visible = true, transformation(origin = {-54, -40}, extent = {{-10, -10}, {10, 10}}, rotation = 0)));
  ARM.FIR.DigitalFIR nearSourceToMic2(path = nearSourceToMic2RIR, samplingFrequency = samplingFrequency) annotation(
        Placement(visible = true, transformation(origin = {-54, -76}, extent = {{-10, -10}, {10, 10}}, rotation = 0)));
  Modelica.Blocks.Math.Add add1 annotation(
        Placement(visible = true, transformation(origin = {4, -40}, extent = {{-10, -10}, {10, 10}}, rotation = 0)));
  Modelica.Blocks.Nonlinear.FixedDelay fixedDelay1(delayTime = 1.0e-3 / samplingFrequency) annotation(
        Placement(visible = true, transformation(origin = {64, -40}, extent = {{-10, -10}, {10, 10}}, rotation = 0)));
    equation
  connect(speaker, speakerToMic1.x) annotation(
        Line(points = {{-120, 40}, {-66, 40}}, color = {0, 0, 127}));
  connect(nearSource, nearSourceToMic1.x) annotation(
        Line(points = {{-120, -40}, {-93, -40}, {-93, 2}, {-66, 2}}, color = {0, 0, 127}));
  connect(add.y, fixedDelay.u) annotation(
        Line(points = {{17, 40}, {46, 40}}, color = {0, 0, 127}));
  connect(speakerToMic1.y, add.u1) annotation(
        Line(points = {{-43, 40}, {-24.5, 40}, {-24.5, 46}, {-6, 46}}, color = {0, 0, 127}));
  connect(nearSourceToMic1.y, add.u2) annotation(
        Line(points = {{-43, 2}, {-26, 2}, {-26, 34}, {-6, 34}}, color = {0, 0, 127}));
  connect(fixedDelay.y, mic1) annotation(
        Line(points = {{70, 40}, {110, 40}}, color = {0, 0, 127}));
  connect(speaker, speakerToMic2.x) annotation(
        Line(points = {{-120, 40}, {-82, 40}, {-82, -40}, {-66, -40}}, color = {0, 0, 127}));
  connect(nearSource, nearSourceToMic2.x) annotation(
        Line(points = {{-120, -40}, {-92, -40}, {-92, -76}, {-66, -76}}, color = {0, 0, 127}));
  connect(add1.y, fixedDelay1.u) annotation(
        Line(points = {{16, -40}, {52, -40}}, color = {0, 0, 127}));
  connect(fixedDelay1.y, mic2) annotation(
        Line(points = {{76, -40}, {110, -40}}, color = {0, 0, 127}));
  connect(speakerToMic2.y, add1.u1) annotation(
        Line(points = {{-42, -40}, {-26, -40}, {-26, -34}, {-8, -34}}, color = {0, 0, 127}));
  connect(nearSourceToMic2.y, add1.u2) annotation(
        Line(points = {{-42, -76}, {-20, -76}, {-20, -46}, {-8, -46}}, color = {0, 0, 127}));
      annotation(
        Diagram(coordinateSystem(extent = {{-200, -200}, {200, 200}})),
        Icon(graphics = {Text(origin = {-1, -125}, extent = {{119, -11}, {-119, 11}}, textString = "f=%samplingFrequency")}),
  Documentation(info = "<html><head></head><body>Implementation of the room acoustic.<div>The parameters are the paths to the 4 Room Impulse Responses encoding the 4 different paths:</div><div><ul><li>Near speaker -&gt; Mic 1</li><li>Near speaker -&gt; Mic 2</li><li>Near source -&gt; Mic 1</li><li>Near source -&gt; Mic 2</li></ul><div>The sampling frequency must be specified.</div></div></body></html>"));
    end PyRoomAcoustics;

    model PassThroughSpeaker
      extends Echo.Interfaces.Speaker;
      import Echo.Types.DB_SPL;
      parameter DB_SPL gain = 70 "Gain of speaker";
      Modelica.Blocks.Interfaces.RealOutput y annotation(
        Placement(visible = true, transformation(origin = {124, 0}, extent = {{-10, -10}, {10, 10}}, rotation = 0), iconTransformation(origin = {110, 0}, extent = {{-10, -10}, {10, 10}}, rotation = 0)));
  ToPressure toPressure(gain=gain) annotation(
        Placement(visible = true, transformation(origin = {-20, -2}, extent = {{-10, -10}, {10, 10}}, rotation = 0)));
    equation
      connect(u, toPressure.u) annotation(
        Line(points = {{-120, 0}, {-32, 0}, {-32, -2}}, color = {0, 0, 127}));
  connect(toPressure.y, y) annotation(
        Line(points = {{-8, -2}, {124, -2}, {124, 0}}, color = {0, 0, 127}));
      annotation(
        Diagram(coordinateSystem(extent = {{-200, -200}, {200, 200}})),
        Documentation(info = "<html><head></head><body>Model of a pass-through speaker which is just converting the input real signal to a pressure difference without reading ay wav file.<div><br></div><div>The conversion is specifid using a gain expressed in dbSPL.</div><div><br></div></body></html>"));
    end PassThroughSpeaker;

    model VHTCanceller
      extends Echo.Interfaces.EchoCanceller;
      ARM.Models.VHTMULTI vhtmulti(hasTF = true,launchVHT = true, samplingFrequency = 16,
      tfSamplingFrequency=10, vhtCommandLine = "-V /home/ubuntu/VHTModelicaDemos/EchoCanceller/VSI/audio/python -f /home/ubuntu/VHTModelicaDemos/EchoCanceller/fvp_config.txt -a /home/ubuntu/VHTModelicaDemos/EchoCanceller/Objects/EchoCanceller.axf") annotation(
        Placement(visible = true, transformation(origin = {6, 10}, extent = {{-10, -10}, {10, 10}}, rotation = 0)));
    equation
      connect(vhtmulti.ya, nearSpeaker) annotation(
        Line(points = {{18, 4}, {90, 4}, {90, -42}, {122, -42}}, color = {0, 0, 127}));
      connect(farMic, vhtmulti.xb) annotation(
        Line(points = {{-120, 40}, {-50, 40}, {-50, 16}, {-6, 16}}, color = {0, 0, 127}));
      connect(vhtmulti.yb, farSpeaker) annotation(
        Line(points = {{-4, 4}, {-50, 4}, {-50, -40}, {-120, -40}}, color = {0, 0, 127}));
  connect(nearMic[1], vhtmulti.xa) annotation(
        Line(points = {{100, 40}, {60, 40}, {60, 16}, {18, 16}}, color = {0, 0, 127}));
  connect(nearMic[2], vhtmulti.xc) annotation(
        Line(points = {{100, 40}, {60, 40}, {60, 10}, {18, 10}}, color = {0, 0, 127}));
  connect(vhtmulti.tf, tf) annotation(
        Line(points = {{18, 2}, {68, 2}, {68, -84}, {110, -84}}, color = {255, 127, 0}));
      annotation(
        Diagram(coordinateSystem(extent = {{-200, -200}, {200, 200}})),
        Documentation(info = "<html><head></head><body>Implementation of the EchoCanceller using the VHTMULTI block from the ARM package.<div>Two sampling rates must be defined:</div><div><ul><li>For the TFLite output</li><li>For the other IOs&nbsp;</li></ul><div>In addition to that, parameters to launch the VHT must be specified. See the documentation for the VHTMULTI block.</div></div><div><br></div></body></html>"));
    end VHTCanceller;

    model SimpleEcho
      extends Modelica.Blocks.Interfaces.SISO;
  Modelica.Blocks.Nonlinear.FixedDelay fixedDelay(delayTime = 0.3)  annotation(
        Placement(visible = true, transformation(origin = {-50, 0}, extent = {{-10, -10}, {10, 10}}, rotation = 0)));
  Modelica.Blocks.Math.Gain gain(k = 0.1)  annotation(
        Placement(visible = true, transformation(origin = {30, 0}, extent = {{-10, -10}, {10, 10}}, rotation = 0)));
    equation
      connect(u, fixedDelay.u) annotation(
        Line(points = {{-120, 0}, {-62, 0}}, color = {0, 0, 127}));
  connect(fixedDelay.y, gain.u) annotation(
        Line(points = {{-38, 0}, {18, 0}}, color = {0, 0, 127}));
  connect(gain.y, y) annotation(
        Line(points = {{42, 0}, {110, 0}}, color = {0, 0, 127}));
      annotation(
        Diagram(coordinateSystem(extent = {{-200, -200}, {200, 200}})));
    end SimpleEcho;

    model WavSource
      extends Echo.Interfaces.AudioSource;
      import Echo.Types.DB_SPL;
      parameter DB_SPL gain = 70 "Gain of audio source";
      import ARM.Types.Frequency_kHz;
      parameter Frequency_kHz samplingFrequency = 16 "Sampling Frequency";
      parameter String path = "Path to output wav";
  ARM.Sound.WaveInput waveInput(path = path, samplingFrequency = samplingFrequency) annotation(
        Placement(visible = true, transformation(origin = {24, -2}, extent = {{-10, -10}, {10, 10}}, rotation = 0)));
  Echo.Implementations.ToPressure wavVolumeGain(gain=gain) annotation(
        Placement(visible = true, transformation(origin = {84, -2}, extent = {{-10, -10}, {10, 10}}, rotation = 0)));
    equation
  connect(waveInput.y, wavVolumeGain.u) annotation(
        Line(points = {{36, -2}, {95, -2}}, color = {0, 0, 127}));
  connect(wavVolumeGain.y, y) annotation(
        Line(points = {{95, -2}, {148, -2}}, color = {0, 0, 127}));
      annotation(
        Diagram(coordinateSystem(extent = {{-200, -200}, {200, 200}})),
        Documentation(info = "<html><head></head><body>Read a wav file and convert the real values to a PressureDifference.<div><br></div><div>The conversion is using a gain expressed in dbSPL.</div></body></html>"));
    end WavSource;
    annotation(
      Diagram(coordinateSystem(extent = {{-200, -200}, {200, 200}})),
      Documentation(info = "<html><head></head><body>Implementations of the interfaces used in th EchoCancellerArchitecture.<div><br></div><div>Note that the parameters for those implementations are not visible in the architure diagram (since they re not part of the interface). So they have to be modified in the source code instantiating the architecture.</div><div><br></div></body></html>"));
  end Implementations;

model Test
ARM.FIR.DigitalFIR digitalFIR(path = "/home/ubuntu/VHTModelicaDemos/EchoCanceller/rir_nearsource_mic1.csv")  annotation(
    Placement(visible = true, transformation(origin = {-74, 4}, extent = {{-10, -10}, {10, 10}}, rotation = 0)));
ARM.Sound.WaveOutput waveOutput(path = "/home/ubuntu/VHTModelicaDemos/EchoCanceller/sounds/echodebug.wav")  annotation(
    Placement(visible = true, transformation(origin = {-14, 4}, extent = {{-10, -10}, {10, 10}}, rotation = 0)));
    Modelica.Blocks.Sources.SineVariableFrequencyAndAmplitude sine annotation(
      Placement(visible = true, transformation(origin = {-130, 4}, extent = {{-10, -10}, {10, 10}}, rotation = 0)));
  Modelica.Blocks.Sources.Constant const(k = 1.0)  annotation(
      Placement(visible = true, transformation(origin = {-178, 10}, extent = {{-10, -10}, {10, 10}}, rotation = 0)));
  Modelica.Blocks.Sources.Ramp ramp(duration = 1, height = 1000)  annotation(
      Placement(visible = true, transformation(origin = {-178, -30}, extent = {{-10, -10}, {10, 10}}, rotation = 0)));
  equation
    connect(digitalFIR.y, waveOutput.x) annotation(
      Line(points = {{-63, 4}, {-26, 4}}, color = {0, 0, 127}));
  connect(sine.y, digitalFIR.x) annotation(
      Line(points = {{-118, 4}, {-86, 4}}, color = {0, 0, 127}));
  connect(const.y, sine.amplitude) annotation(
      Line(points = {{-166, 10}, {-142, 10}}, color = {0, 0, 127}));
  connect(ramp.y, sine.f) annotation(
      Line(points = {{-166, -30}, {-154, -30}, {-154, -2}, {-142, -2}}, color = {0, 0, 127}));
    annotation(
    Diagram(coordinateSystem(extent = {{-200, -200}, {200, 200}})),
      Documentation(info = "<html><head></head><body>Test of one of the room impulse response to check there is an effect.<div><br></div></body></html>"));

end Test;

  model VHTEcho
    extends Echo.Interfaces.EchoCancellerArchitecture(redeclare replaceable Implementations.VHTCanceller echoCanceller, 
    redeclare replaceable Implementations.WavSource farSource(gain = 55, path = "/home/ubuntu/VHTModelicaDemos/EchoCanceller/sounds/background.wav"), 
    redeclare replaceable Implementations.Micro farMic(), 
    redeclare replaceable Implementations.WavSource nearSource(gain = 55, path = "/home/ubuntu/VHTModelicaDemos/EchoCanceller/sounds/yesno.wav"),
    redeclare replaceable Implementations.Micro nearMic1(gain=65), 
    redeclare replaceable Implementations.Micro nearMic2(gain=65),
    redeclare replaceable Implementations.WavSpeaker farSpeaker(path = "/home/ubuntu/VHTModelicaDemos/EchoCanceller/BuildC/cleanedSignal.wav"), 
    redeclare replaceable Implementations.PyRoomAcoustics roomEcho(
    speakerToMic1RIR = "/home/ubuntu/VHTModelicaDemos/EchoCanceller/rir_speaker_mic1.csv",
    nearSourceToMic1RIR="/home/ubuntu/VHTModelicaDemos/EchoCanceller/rir_nearsource_mic1.csv",
    speakerToMic2RIR = "/home/ubuntu/VHTModelicaDemos/EchoCanceller/rir_speaker_mic2.csv",
    nearSourceToMic2RIR="/home/ubuntu/VHTModelicaDemos/EchoCanceller/rir_nearsource_mic2.csv")
  , nearSpeaker.gain = 94);
  equation

    annotation(
      Diagram(coordinateSystem(extent = {{-200, -200}, {200, 200}})),
      Documentation(info = "<html><head></head><body>Implementation of the EchoCancellerArchitecture using the VHTMULTI.<div><br></div><div>If you need to change parameters, you has to do it in the source code of this Model since the parameters</div><div>are not visible in the interface (in the UI).</div><div><br></div><div>The parameters can also be changed in the Variable windows once a simulation has been executed.</div><div><br></div><div><br></div><div>The parameters are:</div><div><ul><li>Sampling frequencies</li><li>How to launch the VHT</li><li>VHT Launch mode</li><li>hasTF (cf description of VHTMULTI in ARM package)</li><li>csv file for the room echo</li><li>path to the wav files</li><li>gain to converte to PressureDifference and from PressureDifference.</li></ul></div></body></html>"));
  end VHTEcho;
  annotation(version="0.1.0",
versionDate="2022-01-12",
    uses(Modelica(version = "4.0.0")),
    Icon(graphics = {Line(origin = {60, 1}, points = {{0, 67}, {0, -67}}, thickness = 1.5), Line(origin = {13.59, 10}, points = {{-13.5905, 49.9979}, {44.4095, 9.9979}, {-37.5905, -46.0021}, {-43.5905, -50.0021}}, pattern = LinePattern.Dash, arrow = {Arrow.None, Arrow.Filled}, arrowSize = 8), Line(origin = {34.99, -21.01}, points = {{25.0067, 41.0067}, {-20.9933, -34.9933}, {-24.9933, -40.9933}}, pattern = LinePattern.Dash, arrow = {Arrow.None, Arrow.Filled}, arrowSize = 8)}),
  Documentation(info = "<html><head></head><body><div style=\"font-family: 'MS Shell Dlg 2';\"><font size=\"4\"><b>All files in this package are covered with the following license:</b></font></div><div style=\"font-family: 'MS Shell Dlg 2'; font-size: 12px;\"><br></div><div style=\"font-family: 'MS Shell Dlg 2'; font-size: 12px;\">Copyright (C) 2010-2022 ARM Limited or its affiliates. All rights reserved.</div><div style=\"font-family: 'MS Shell Dlg 2'; font-size: 12px;\">&nbsp;</div><div style=\"font-family: 'MS Shell Dlg 2'; font-size: 12px;\">SPDX-License-Identifier: Apache-2.0</div><div style=\"font-family: 'MS Shell Dlg 2'; font-size: 12px;\">&nbsp;</div><div style=\"font-family: 'MS Shell Dlg 2'; font-size: 12px;\">Licensed under the Apache License, Version 2.0 (the License); you may</div><div style=\"font-family: 'MS Shell Dlg 2'; font-size: 12px;\">not use this file except in compliance with the License.</div><div style=\"font-family: 'MS Shell Dlg 2'; font-size: 12px;\">You may obtain a copy of the License at</div><div style=\"font-family: 'MS Shell Dlg 2'; font-size: 12px;\"><br></div><div style=\"font-family: 'MS Shell Dlg 2'; font-size: 12px;\">&nbsp;www.apache.org/licenses/LICENSE-2.0</div><div style=\"font-family: 'MS Shell Dlg 2'; font-size: 12px;\">&nbsp;</div><div style=\"font-family: 'MS Shell Dlg 2'; font-size: 12px;\">Unless required by applicable law or agreed to in writing, software</div><div style=\"font-family: 'MS Shell Dlg 2'; font-size: 12px;\">distributed under the License is distributed on an AS IS BASIS, WITHOUT</div><div style=\"font-family: 'MS Shell Dlg 2'; font-size: 12px;\">WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.</div><div style=\"font-family: 'MS Shell Dlg 2'; font-size: 12px;\">See the License for the specific language governing permissions and</div><div style=\"font-family: 'MS Shell Dlg 2'; font-size: 12px;\">limitations under the License.</div><div><br></div></body></html>"));
end Echo;
