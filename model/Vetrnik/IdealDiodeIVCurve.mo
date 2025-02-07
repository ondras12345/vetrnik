within Vetrnik;
model IdealDiodeIVCurve "obtain the IV curve of Modelica.Electrical.Analog.Ideal.IdealDiode"
  Modelica.Electrical.Analog.Ideal.IdealDiode diode(Ron = 1, Vknee = 0.7)  annotation(
    Placement(transformation(origin = {40, 0}, extent = {{-10, -10}, {10, 10}}, rotation = -90)));
  Modelica.Electrical.Analog.Sources.RampVoltage rampVoltage(V = 6, duration = 6, offset = -3)  annotation(
    Placement(transformation(origin = {-20, 0}, extent = {{-10, -10}, {10, 10}}, rotation = -90)));
  Modelica.Electrical.Analog.Basic.Ground ground annotation(
    Placement(transformation(origin = {-20, -36}, extent = {{-10, -10}, {10, 10}})));
  Modelica.Electrical.Analog.Basic.Resistor resistor(R (displayUnit = "Ohm")= 10)  annotation(
    Placement(transformation(origin = {10, 20}, extent = {{-10, -10}, {10, 10}})));
equation
  connect(rampVoltage.n, diode.n) annotation(
    Line(points = {{-20, -10}, {-20, -20}, {40, -20}, {40, -10}}, color = {0, 0, 255}));
  connect(ground.p, rampVoltage.n) annotation(
    Line(points = {{-20, -26}, {-20, -10}}, color = {0, 0, 255}));
  connect(rampVoltage.p, resistor.p) annotation(
    Line(points = {{-20, 10}, {-20, 20}, {0, 20}}, color = {0, 0, 255}));
  connect(resistor.n, diode.p) annotation(
    Line(points = {{20, 20}, {40, 20}, {40, 10}}, color = {0, 0, 255}));
  annotation(experiment(StartTime = 0, StopTime = 6, Tolerance = 1e-06, Interval = 0.06));
end IdealDiodeIVCurve;