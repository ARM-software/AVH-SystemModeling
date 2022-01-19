within ARM;
package FIR
  block DigitalFIR "Digital FIR"
    extends Modelica.Blocks.Icons.Block;
    import SI = Modelica.Units.SI;
    import intf = Modelica.Blocks.Interfaces;
    import ARM.Types.Frequency_kHz;
    parameter Frequency_kHz samplingFrequency = 16 "Sampling frequency of FIR";
    intf.RealInput x "Filter input" annotation(
      Placement(transformation(extent = {{-140, -20}, {-100, 20}})));
    discrete intf.RealOutput y(start = 0) "Filtered output" annotation(
      Placement(transformation(extent = {{100, -10}, {120, 10}})));
    parameter String path = "/home/ubuntu/VHTModelicaDemos/EchoCanceller/rir.csv";
  protected
    FIRObject inFile = FIRObject(path);
  algorithm
    when sample(0, 0.001 / samplingFrequency) then
      y := filter(inFile, x);
    end when;
    annotation(
      Icon(graphics = {Rectangle(lineColor = {160, 160, 164}, fillColor = {255, 255, 255}, fillPattern = FillPattern.Backward, extent = {{-80, -78}, {22, 10}}), Polygon(lineColor = {192, 192, 192}, fillColor = {192, 192, 192}, fillPattern = FillPattern.Solid, points = {{-80, 92}, {-88, 70}, {-72, 70}, {-80, 92}}), Line(origin = {3.333, -6.667}, points = {{-83.333, 34.667}, {24.667, 34.667}, {42.667, -71.333}}, color = {0, 0, 127}, smooth = Smooth.Bezier), Line(points = {{-90, -78}, {82, -78}}, color = {192, 192, 192}), Line(points = {{-80, 80}, {-80, -88}}, color = {192, 192, 192}), Polygon(lineColor = {192, 192, 192}, fillColor = {192, 192, 192}, fillPattern = FillPattern.Solid, points = {{90, -78}, {68, -70}, {68, -86}, {90, -78}}), Text(origin = {-1, -125}, extent = {{119, -11}, {-119, 11}}, textString = "f=%samplingFrequency")}),
      Documentation(info = "<html><head></head><body>A C implementation of a digital FIR.<div>The coefficients of the FIR are saved in a .csv file.</div><div><br></div></body></html>"));
  end DigitalFIR;
  protected
  class FIRObject "FIR object"
    extends ExternalObject;
  
    protected
    function constructor "Create FIR Object"
      input String path "Coefficients in CSV file";
      output FIRObject config;
    
      external "C" config = newFIR(path) annotation(
        Library = "FIR",
        Include = "#include \"fir.h\"");
    end constructor;

    function destructor "Close FIR Object"
      input FIRObject config;
    
      external "C" closeFIR(config) annotation(
        Library = "FIR",
        Include = "#include \"fir.h\"");
    end destructor;
  end FIRObject;
  impure function filter "Filtering"
    input FIRObject fir;
    input Real x;
    output Real y;
  
    external "C" y = filterSample(fir,x) annotation(
      Library = "FIR",
      Include = "#include \"fir.h\"");
  end filter;

annotation(
    Icon(graphics = {Rectangle(lineColor = {160, 160, 164}, fillColor = {255, 255, 255}, fillPattern = FillPattern.Backward, extent = {{-80, -78}, {22, 10}}), Polygon(lineColor = {192, 192, 192}, fillColor = {192, 192, 192}, fillPattern = FillPattern.Solid, points = {{-80, 92}, {-88, 70}, {-72, 70}, {-80, 92}}), Line(origin = {3.333, -6.667}, points = {{-83.333, 34.667}, {24.667, 34.667}, {42.667, -71.333}}, color = {0, 0, 127}, smooth = Smooth.Bezier), Line(points = {{-90, -78}, {82, -78}}, color = {192, 192, 192}), Line(points = {{-80, 80}, {-80, -88}}, color = {192, 192, 192}), Polygon(lineColor = {192, 192, 192}, fillColor = {192, 192, 192}, fillPattern = FillPattern.Solid, points = {{90, -78}, {68, -70}, {68, -86}, {90, -78}})}));
end FIR;
