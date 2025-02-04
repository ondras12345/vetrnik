model test2 "closed loop wind turbine control"
  WindTurbine windturbine annotation(
    Placement(transformation(origin = {-40, 0}, extent = {{-10, -10}, {10, 10}})));
  Modelica.Mechanics.Rotational.Components.Inertia inertia(J = .001, phi(start = 0))  annotation(
    Placement(transformation(extent = {{-10, -10}, {10, 10}})));
  VariableDamper variableDamper annotation(
    Placement(transformation(origin = {40, 0}, extent = {{-10, -10}, {10, 10}})));
  Modelica.Mechanics.Rotational.Components.Fixed fixed annotation(
    Placement(transformation(origin = {60, -8}, extent = {{-10, -10}, {10, 10}})));
  Modelica.Blocks.Sources.Step step(height = 5, offset = 1.2, startTime = 20)  annotation(
    Placement(transformation(origin = {-78, 0}, extent = {{-10, -10}, {10, 10}})));
equation
  connect(windturbine.flange, inertia.flange_a) annotation(
    Line(points = {{-30, 0}, {-10, 0}}));
  connect(variableDamper.flange_b, fixed.flange) annotation(
    Line(points = {{50, 0}, {60, 0}, {60, -8}}));
  connect(inertia.flange_b, variableDamper.flange_a) annotation(
    Line(points = {{10, 0}, {30, 0}}));
  variableDamper.d = max(0, 0.004 + 0.005*(windturbine.lambda - 10));
  connect(step.y, windturbine.vwind) annotation(
    Line(points = {{-66, 0}, {-50, 0}}, color = {0, 0, 127}));
  annotation(
    uses(Modelica(version = "4.0.0")));
end test2;