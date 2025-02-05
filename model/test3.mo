model test3
  WindTurbine windTurbine annotation(
    Placement(transformation(origin = {-60, 0}, extent = {{-10, -10}, {10, 10}})));
  Modelica.Electrical.Machines.BasicMachines.SynchronousMachines.SM_PermanentMagnet smpm(p = 6, fsNominal = 50, Jr = .001, VsOpenCircuit = 48)  annotation(
    Placement(transformation(origin = {-20, 0},extent = {{10, -10}, {-10, 10}}, rotation = -0)));
  PolyphaseRectifier polyphaseRectifier annotation(
    Placement(transformation(origin = {20, 0}, extent = {{-10, -10}, {10, 10}})));
  Modelica.Electrical.Analog.Basic.Ground groundDC annotation(
    Placement(transformation(origin = {40, -50}, extent = {{-10, -10}, {10, 10}})));
  Modelica.Electrical.Analog.Basic.Capacitor capacitor(C = 1e-6)  annotation(
    Placement(transformation(origin = {50, 0}, extent = {{-10, -10}, {10, 10}}, rotation = -90)));
  Modelica.Blocks.Sources.Constant constVwind(k = 12)  annotation(
    Placement(transformation(origin = {-100, 0}, extent = {{-10, -10}, {10, 10}})));
  Modelica.Electrical.Analog.Basic.Resistor resistor(R = 1000)  annotation(
    Placement(transformation(origin = {70, 0}, extent = {{-10, -10}, {10, 10}}, rotation = -90)));
  Modelica.Electrical.Polyphase.Basic.Star star annotation(
    Placement(transformation(origin = {0, -20}, extent = {{-10, -10}, {10, 10}})));
equation
  connect(windTurbine.flange, smpm.flange) annotation(
    Line(points = {{-50, 0}, {-30, 0}}));
  connect(smpm.plug_sp, polyphaseRectifier.positivePlug) annotation(
    Line(points = {{-26, 10}, {-26, 20}, {2, 20}, {2, 0}, {12, 0}}, color = {0, 0, 255}));
  connect(groundDC.p, polyphaseRectifier.pin_n) annotation(
    Line(points = {{40, -40}, {40, -6}, {28, -6}}, color = {0, 0, 255}));
  connect(capacitor.n, groundDC.p) annotation(
    Line(points = {{50, -10}, {50, -20}, {40, -20}, {40, -40}}, color = {0, 0, 255}));
  connect(polyphaseRectifier.pin_p, capacitor.p) annotation(
    Line(points = {{28, 6}, {40, 6}, {40, 20}, {50, 20}, {50, 10}}, color = {0, 0, 255}));
  connect(constVwind.y, windTurbine.vwind) annotation(
    Line(points = {{-88, 0}, {-70, 0}}, color = {0, 0, 127}));
  connect(resistor.p, capacitor.p) annotation(
    Line(points = {{70, 10}, {70, 20}, {50, 20}, {50, 10}}, color = {0, 0, 255}));
  connect(resistor.n, capacitor.n) annotation(
    Line(points = {{70, -10}, {70, -20}, {50, -20}, {50, -10}}, color = {0, 0, 255}));
  connect(star.plug_p, smpm.plug_sn) annotation(
    Line(points = {{-10, -20}, {-10, 10}, {-14, 10}}, color = {0, 0, 255}));
  annotation(
    uses(Modelica(version = "4.0.0")));
end test3;