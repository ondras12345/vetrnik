within Vetrnik;

model SimplifiedOpenLoop
  WindTurbine windTurbine(R = 1.1, beta = 0.08726646259971647)  annotation(
    Placement(transformation(origin = {-20, 0}, extent = {{-10, -10}, {10, 10}})));
  VariableDamper load annotation(
    Placement(transformation(origin = {60, 0}, extent = {{-10, -10}, {10, 10}})));
  Modelica.Mechanics.Rotational.Components.Inertia inertia(J = 1.2)  annotation(
    Placement(transformation(origin = {20, 0}, extent = {{-10, -10}, {10, 10}})));
  Modelica.Mechanics.Rotational.Components.Fixed fixed annotation(
    Placement(transformation(origin = {80, -10}, extent = {{-10, -10}, {10, 10}})));
  input Modelica.Blocks.Interfaces.RealInput vwind annotation(
    Placement(transformation(origin = {-80, 0}, extent = {{-20, -20}, {20, 20}}), iconTransformation(origin = {-80, -40}, extent = {{-20, -20}, {20, 20}})));
  input Modelica.Blocks.Interfaces.RealInput b annotation(
    Placement(transformation(origin = {-80, 40}, extent = {{-20, -20}, {20, 20}}), iconTransformation(origin = {-80, 60}, extent = {{-20, -20}, {20, 20}})));
  output Modelica.Blocks.Interfaces.RealOutput omega annotation(
    Placement(transformation(origin = {80, -40}, extent = {{-10, -10}, {10, 10}}), iconTransformation(origin = {80, 0}, extent = {{-10, -10}, {10, 10}})));
  Modelica.Mechanics.Rotational.Sensors.SpeedSensor speedSensor annotation(
    Placement(transformation(origin = {40, -40}, extent = {{-10, -10}, {10, 10}})));
equation
  connect(windTurbine.flange, inertia.flange_a) annotation(
    Line(points = {{-10, 0}, {10, 0}}));
  connect(inertia.flange_b, load.flange_a) annotation(
    Line(points = {{30, 0}, {50, 0}}));
  connect(load.flange_b, fixed.flange) annotation(
    Line(points = {{70, 0}, {80, 0}, {80, -10}}));
  connect(vwind, windTurbine.vwind) annotation(
    Line(points = {{-80, 0}, {-30, 0}}, color = {0, 0, 127}));
  connect(b, load.d) annotation(
    Line(points = {{-80, 40}, {40, 40}, {40, 8}, {50, 8}}, color = {0, 0, 127}));
  connect(speedSensor.w, omega) annotation(
    Line(points = {{51, -40}, {79, -40}}, color = {0, 0, 127}));
  connect(speedSensor.flange, windTurbine.flange) annotation(
    Line(points = {{30, -40}, {0, -40}, {0, 0}, {-10, 0}}));
annotation(
    Icon(graphics = {Text(origin = {0, -66},extent = {{-40, 20}, {40, -20}}, textString = "wind plant"), Line(points = {{-80, -80}, {-80, 80}, {80, 80}, {80, -80}, {-80, -80}}), Text(origin = {-52, 60}, extent = {{-8, 6}, {8, -6}}, textString = "b"), Text(origin = {-43, -40}, extent = {{-15, 6}, {15, -6}}, textString = "vwind"), Text(origin = {55, 0}, extent = {{-13, 8}, {13, -8}}, textString = "omega"), Line(origin = {0, -10}, points = {{0, -50}, {0, 50}}, thickness = 1), Ellipse(origin = {0, 40}, fillPattern = FillPattern.Solid, extent = {{-2, 2}, {2, -2}}), Line(origin = {0, 29.6464}, points = {{20, -9.64645}, {0, 10.3536}, {-20, -9.64645}}), Line(origin = {0, 52}, points = {{0, -14}, {0, 14}})}));
end SimplifiedOpenLoop;