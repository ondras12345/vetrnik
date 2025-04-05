within Vetrnik;

model SimplifiedOpenLoopTest "model for manual testing of SimplifiedOpenLoop"
  // set w start to make sure the turbine doesn't fail to start with large constB.
  SimplifiedOpenLoop simplifiedOpenLoop(inertia(w(start=100, fixed = true))) annotation(
    Placement(transformation(extent = {{-10, -10}, {10, 10}})));
  Modelica.Blocks.Sources.Constant constVwind(k = 15)  annotation(
    Placement(transformation(origin = {-80, -20}, extent = {{-10, -10}, {10, 10}})));
  Modelica.Blocks.Sources.Constant constB(k = .300)  annotation(
    Placement(transformation(origin = {-80, 20}, extent = {{-10, -10}, {10, 10}})));
equation
  connect(simplifiedOpenLoop.b, constB.y) annotation(
    Line(points = {{-8, 6}, {-20, 6}, {-20, 20}, {-69, 20}}, color = {0, 0, 127}));
  connect(constVwind.y, simplifiedOpenLoop.vwind) annotation(
    Line(points = {{-69, -20}, {-20, -20}, {-20, -4}, {-8, -4}}, color = {0, 0, 127}));
annotation(
    experiment(StartTime = 0, StopTime = 100, Tolerance = 1e-06, Interval = 0.05));
end SimplifiedOpenLoopTest;