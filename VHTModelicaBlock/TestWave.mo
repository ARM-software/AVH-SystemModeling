model TestWave
  ARM.Sound.WaveInput waveInput annotation(
    Placement(visible = true, transformation(origin = {-118, 2}, extent = {{-10, -10}, {10, 10}}, rotation = 0)));
  ARM.Sound.WaveOutput waveOutput annotation(
    Placement(visible = true, transformation(origin = {-26, 4}, extent = {{-10, -10}, {10, 10}}, rotation = 0)));
equation
  connect(waveInput.y, waveOutput.x) annotation(
    Line(points = {{-106, 2}, {-38, 2}, {-38, 4}}, color = {0, 0, 127}));
protected
  annotation(
    Diagram(coordinateSystem(extent = {{-200, -200}, {200, 200}})),
    uses(Modelica(version = "4.0.0")));
end TestWave;
