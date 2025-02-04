model test1
  WindTurbine windturbine annotation(
    Placement(transformation(origin = {-20, 0}, extent = {{-10, -10}, {10, 10}})));
  Modelica.Blocks.Sources.Constant vwind(k = 1.2)  annotation(
    Placement(transformation(origin = {-80, 0}, extent = {{-10, -10}, {10, 10}})));
  Modelica.Mechanics.Rotational.Components.Inertia inertia(J = .001, phi(start = 0))  annotation(
    Placement(transformation(origin = {20, 0}, extent = {{-10, -10}, {10, 10}})));
  Modelica.Mechanics.Rotational.Components.Damper damper(d = .0005)  annotation(
    Placement(transformation(origin = {56, 0}, extent = {{-10, -10}, {10, 10}})));
  Modelica.Mechanics.Rotational.Components.Fixed fixed annotation(
    Placement(transformation(origin = {74, -8}, extent = {{-10, -10}, {10, 10}})));
equation
  connect(vwind.y, windturbine.vwind) annotation(
    Line(points = {{-68, 0}, {-30, 0}}, color = {0, 0, 127}));
  connect(inertia.flange_a, windturbine.flange) annotation(
    Line(points = {{10, 0}, {-10, 0}}));
  connect(inertia.flange_b, damper.flange_a) annotation(
    Line(points = {{30, 0}, {46, 0}}));
  connect(fixed.flange, damper.flange_b) annotation(
    Line(points = {{74, -8}, {74, 0}, {66, 0}}));

annotation(
    uses(Modelica(version = "4.0.0")),
  Diagram);
end test1;
