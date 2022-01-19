within ARM;
package Sound
  block WaveInput "Wave Input"
    extends Modelica.Blocks.Icons.Block;
    import SI = Modelica.Units.SI;
    import intf = Modelica.Blocks.Interfaces;
    import ARM.Types.Frequency_kHz;

    parameter Frequency_kHz samplingFrequency = 16 "Sampling frequency of Wave file";
    discrete intf.RealOutput y(start = 0) "Output signal connector" annotation(
      Placement(transformation(extent = {{100, -10}, {120, 10}})));
    parameter String path = "/home/ubuntu/VHTModelicaDemos/EchoCanceller/sounds/sine.wav";
  protected
    WaveInFile inFile = WaveInFile(path);
  algorithm
    when sample(0, 0.001 / samplingFrequency) then
      y := waveInput(inFile);
    end when;
    annotation(
      Diagram,
      Icon(graphics = {Text(origin = {-5, 48}, extent = {{81, -24}, {-81, 24}}, textString = "Wave"), Text(origin = {-1, -125}, extent = {{119, -11}, {-119, 11}}, textString = "f=%samplingFrequency"), Line(origin = {-6.49161, -21.4035}, points = {{-63, 28.5317}, {-56, 17.6336}, {-49, -17.6336}, {-42, -28.5317}, {-35, 0}, {-28, 28.5317}, {-21, 17.6336}, {-14, -17.6336}, {-7, -28.5317}, {0, 0}, {7, 28.5317}, {14, 17.6336}, {21, -17.6336}, {28, -28.5317}, {35, 0}, {42, 28.5317}, {49, 17.6336}, {56, -17.6336}, {63, -28.5317}, {70, 0}}, smooth = Smooth.Bezier)}));
  end WaveInput;

  block WaveOutput "Wave Output"
    extends Modelica.Blocks.Icons.Block;
    import SI = Modelica.Units.SI;
    import intf = Modelica.Blocks.Interfaces;
    import ARM.Types.Frequency_kHz;
    parameter Frequency_kHz samplingFrequency = 16 "Sampling frequency of Wave file";
    intf.RealInput x(start = 0) "Input signal connector" annotation(
      Placement(transformation(extent = {{-140, -20}, {-100, 20}})));
    parameter String path = "/home/ubuntu/VHTModelicaDemos/EchoCanceller/sounds/output.wav";
  protected
    WaveOutFile outFile = WaveOutFile(path, integer(samplingFrequency * 1000));
  algorithm
    when sample(0, 0.001 / samplingFrequency) then
      waveOutput(outFile, x);
    end when;
    annotation(
      Icon(graphics = {Text(origin = {-5, 48}, extent = {{81, -24}, {-81, 24}}, textString = "Wave"), Line(origin = {-6.49161, -21.4035}, points = {{-63, 28.5317}, {-56, 17.6336}, {-49, -17.6336}, {-42, -28.5317}, {-35, 0}, {-28, 28.5317}, {-21, 17.6336}, {-14, -17.6336}, {-7, -28.5317}, {0, 0}, {7, 28.5317}, {14, 17.6336}, {21, -17.6336}, {28, -28.5317}, {35, 0}, {42, 28.5317}, {49, 17.6336}, {56, -17.6336}, {63, -28.5317}, {70, 0}}, smooth = Smooth.Bezier), Text(origin = {-1, -125}, extent = {{119, -11}, {-119, 11}}, textString = "f=%samplingFrequency")}));
  end WaveOutput;

  protected
  class WaveInFile "Wave input object"
    extends ExternalObject;

    protected
    function constructor "Open Wave for reading"
      input String path;
      output WaveInFile config;
    
      external "C" config = openWaveForReading(path) annotation(
        Library = "WAVE",
        Include = "#include \"wave.h\"");
    end constructor;

    function destructor "Close wave"
      input WaveInFile config;
    
      external "C" closeWave(config) annotation(
        Library = "WAVE",
        Include = "#include \"wave.h\"");
    end destructor;
  end WaveInFile;
  class WaveOutFile "Wave output object"
    extends ExternalObject;
  
    protected
    function constructor "Open Wave for writing"
      input String path;
      input Integer samplingFrequency;
      output WaveOutFile config;
    
      external "C" config = openWaveForWriting(path, samplingFrequency) annotation(
        Library = "WAVE",
        Include = "#include \"wave.h\"");
    end constructor;

    function destructor "Close wave"
      input WaveOutFile config;
    
      external "C" closeWave(config) annotation(
        Library = "WAVE",
        Include = "#include \"wave.h\"");
    end destructor;
  end WaveOutFile;
  impure function waveInput "Wave input"
    input WaveInFile wave;
    output Real y;
  
    external "C" y = readWavSampleF32(wave) annotation(
      Library = "WAVE",
      Include = "#include \"wave.h\"");
  end waveInput;
  impure function waveOutput "Wave output"
    input WaveOutFile wave;
    input Real y;
  
    external "C" writeWavSampleF32(wave, y) annotation(
      Library = "WAVE",
      Include = "#include \"wave.h\"");
  end waveOutput;
  annotation(
    Icon(graphics = {Ellipse(origin = {1, 1}, lineThickness = 1, extent = {{-81, 83}, {81, -83}}, startAngle = -60, endAngle = 60, closure = EllipseClosure.None), Ellipse(origin = {-22, -2}, lineThickness = 1, extent = {{-86, 68}, {86, -68}}, startAngle = -50, endAngle = 50, closure = EllipseClosure.None), Ellipse(origin = {-75, -1}, lineThickness = 1, extent = {{-117, 55}, {117, -55}}, startAngle = -30, endAngle = 30, closure = EllipseClosure.None), Polygon(origin = {-24, 0}, fillPattern = FillPattern.Solid, points = {{-20, 20}, {-20, -20}, {0, -20}, {20, -60}, {20, 60}, {0, 20}, {-20, 20}})}),
    uses(Modelica(version = "4.0.0")),
  Documentation(info = "<html><head></head><body>Utility functions to read write to wav files.<div>It is a very limited implementation of the wav standard.</div><div><br></div><div>The files needs to be mono and 16 bits per sample.</div><div>The only sampling rate tested so far is 16 kHz but the models may work with different sampling rates.</div></body></html>"));
end Sound;
