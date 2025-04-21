within Vetrnik;

model TSRsimplified "TSR MPPT algorithm testing with SimplifiedOpenLoop"
  SimplifiedOpenLoop plant annotation(
    Placement(transformation(origin = {80, 0}, extent = {{-20, -20}, {20, 20}})));
  Modelica.Blocks.Interfaces.RealInput vwind annotation(
    Placement(transformation(origin = {-184, -8}, extent = {{-12, -12}, {12, 12}}), iconTransformation(origin = {-28, -2}, extent = {{-20, -20}, {20, 20}})));
  Modelica.Blocks.Math.Gain gain(k = lambdaopt/plant.windTurbine.R)  annotation(
    Placement(transformation(origin = {-64, 20}, extent = {{-10, -10}, {10, 10}})));
  parameter Real lambdaopt = 9.25 "optimal tip speed ratio for this wind turbine";
  parameter Modelica.Units.SI.Time Ts = 0.5 "controller sampling period";
  parameter Modelica.Units.SI.Time vwindTs = 0.5 "wind speed sensor sampling period";
  parameter Real kd = -0.006208 "gain of discrete time PI controller";
  parameter Real Td = 22.251 "time constant of discrete time PI controller";
  Modelica.Blocks.Math.Feedback feedback annotation(
    Placement(transformation(origin = {-40, 20}, extent = {{-10, -10}, {10, 10}})));
  Modelica.Clocked.RealSignals.Sampler.Hold hold1 annotation(
    Placement(transformation(origin = {40, 20}, extent = {{-6, -6}, {6, 6}})));
  Modelica.Clocked.ClockSignals.Clocks.PeriodicRealClock periodicClock1(period = Ts)  annotation(
    Placement(transformation(origin = {-180, -60}, extent = {{-6, -6}, {6, 6}})));
  Modelica.Clocked.RealSignals.Sampler.SampleClocked sample2 annotation(
    Placement(transformation(origin = {-140, 20}, extent = {{-6, -6}, {6, 6}})));
  Modelica.Clocked.RealSignals.Sampler.SampleClocked sample1 annotation(
    Placement(transformation(origin = {-20, 20}, extent = {{-6, -6}, {6, 6}})));
  Modelica.Clocked.RealSignals.Sampler.Hold hold2 annotation(
    Placement(transformation(origin = {-100, 20}, extent = {{-6, -6}, {6, 6}})));
  Modelica.Clocked.ClockSignals.Clocks.PeriodicRealClock periodicClock2(period = vwindTs)  annotation(
    Placement(transformation(origin = {-180, -40}, extent = {{-6, -6}, {6, 6}})));
  DiscreteLimPI discreteLimPI(kd = kd, Td = Td, ymax = 0.9851, ymin = 0, Kaw = 50) annotation(
    Placement(transformation(origin = {10, 20}, extent = {{-10, -10}, {10, 10}})));
equation
  connect(vwind, plant.vwind) annotation(
    Line(points = {{-184, -8}, {64, -8}}, color = {0, 0, 127}));
  connect(gain.y, feedback.u1) annotation(
    Line(points = {{-53, 20}, {-48, 20}}, color = {0, 0, 127}));
  connect(sample2.u, vwind) annotation(
    Line(points = {{-148, 20}, {-160, 20}, {-160, -8}, {-184, -8}}, color = {0, 0, 127}));
  connect(feedback.y, sample1.u) annotation(
    Line(points = {{-30, 20}, {-28, 20}}, color = {0, 0, 127}));
  connect(plant.omega, feedback.u2) annotation(
    Line(points = {{96, 0}, {100, 0}, {100, -40}, {-40, -40}, {-40, 12}}, color = {0, 0, 127}));
  connect(sample1.clock, periodicClock1.y) annotation(
    Line(points = {{-20, 12}, {-20, -60}, {-174, -60}}, color = {175, 175, 175}));
  connect(hold2.y, gain.u) annotation(
    Line(points = {{-94, 20}, {-76, 20}}, color = {0, 0, 127}));
  connect(sample2.y, hold2.u) annotation(
    Line(points = {{-134, 20}, {-108, 20}}, color = {0, 0, 127}));
  connect(periodicClock2.y, sample2.clock) annotation(
    Line(points = {{-174, -40}, {-140, -40}, {-140, 12}}, color = {175, 175, 175}));
  connect(discreteLimPI.y, hold1.u) annotation(
    Line(points = {{21, 20}, {33, 20}}, color = {0, 0, 127}));
  connect(hold1.y, plant.b) annotation(
    Line(points = {{46, 20}, {50, 20}, {50, 12}, {64, 12}}, color = {0, 0, 127}));
  connect(sample1.y, discreteLimPI.u) annotation(
    Line(points = {{-14, 20}, {-2, 20}}, color = {0, 0, 127}));
  annotation(
    Diagram(coordinateSystem(extent = {{-200, -100}, {120, 100}})),
    Icon(coordinateSystem(extent = {{-200, -100}, {120, 100}})),
  experiment(StartTime = 0, StopTime = 400, Tolerance = 1e-06, Interval = 0.1),
  __OpenModelica_simulationFlags(csvInput = "vwind-ramps.csv", lv = "LOG_STDOUT,LOG_ASSERT,LOG_STATS", s = "dassl", variableFilter = ".*"));
end TSRsimplified;