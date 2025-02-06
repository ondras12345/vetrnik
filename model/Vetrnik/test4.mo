within Vetrnik;
model test4
  WindTurbine windTurbine annotation(
    Placement(transformation(origin = {-50, 0}, extent = {{-10, -10}, {10, 10}})));
  Modelica.Electrical.Machines.BasicMachines.SynchronousMachines.SM_PermanentMagnet smpm(p = 6, fsNominal = 50, Jr = .001, VsOpenCircuit = 48) annotation(
    Placement(transformation(origin = {-10, 0}, extent = {{10, -10}, {-10, 10}})));
  PolyphaseRectifier polyphaseRectifier(Vknee = .7, Ron = .05)  annotation(
    Placement(transformation(origin = {22, 0}, extent = {{-10, -10}, {10, 10}})));
  Modelica.Electrical.Analog.Basic.Ground groundDC annotation(
    Placement(transformation(origin = {50, -50}, extent = {{-10, -10}, {10, 10}})));
  Modelica.Electrical.Analog.Basic.Capacitor capacitor(C = 1e-3) annotation(
    Placement(transformation(origin = {60, 0}, extent = {{-10, -10}, {10, 10}}, rotation = -90)));
  Modelica.Blocks.Sources.Constant constVwind(k = 12) annotation(
    Placement(transformation(origin = {-90, 0}, extent = {{-10, -10}, {10, 10}})));
  Modelica.Electrical.Polyphase.Basic.Star star annotation(
    Placement(transformation(origin = {10, -20}, extent = {{-10, -10}, {10, 10}})));
  Modelica.Electrical.Analog.Basic.VariableResistor Rl annotation(
    Placement(transformation(origin = {80, 0}, extent = {{-10, -10}, {10, 10}}, rotation = -90)));
  parameter Modelica.Units.SI.Resistance Rload = 100 "load resistance";  // 2.89875 is the real-world value
  parameter Modelica.Units.SI.Time Ts = 0.5 "controller sampling time";
  Real duty "duty cycle";
equation
  connect(windTurbine.flange, smpm.flange) annotation(
    Line(points = {{-40, 0}, {-20, 0}}));
  connect(smpm.plug_sp, polyphaseRectifier.positivePlug) annotation(
    Line(points = {{-16, 10}, {-16, 20}, {12, 20}, {12, 0}, {14, 0}}, color = {0, 0, 255}));
  connect(groundDC.p, polyphaseRectifier.pin_n) annotation(
    Line(points = {{50, -40}, {50, -6}, {30, -6}}, color = {0, 0, 255}));
  connect(capacitor.n, groundDC.p) annotation(
    Line(points = {{60, -10}, {60, -20}, {50, -20}, {50, -40}}, color = {0, 0, 255}));
  connect(constVwind.y, windTurbine.vwind) annotation(
    Line(points = {{-79, 0}, {-61, 0}}, color = {0, 0, 127}));
  connect(star.plug_p, smpm.plug_sn) annotation(
    Line(points = {{0, -20}, {0, 10}, {-4, 10}}, color = {0, 0, 255}));
  connect(polyphaseRectifier.pin_p, capacitor.p) annotation(
    Line(points = {{30, 6}, {40, 6}, {40, 20}, {60, 20}, {60, 10}}, color = {0, 0, 255}));
  connect(Rl.p, capacitor.p) annotation(
    Line(points = {{80, 10}, {80, 20}, {60, 20}, {60, 10}}, color = {0, 0, 255}));
  connect(Rl.n, capacitor.n) annotation(
    Line(points = {{80, -10}, {80, -20}, {60, -20}, {60, -10}}, color = {0, 0, 255}));
  
  when sample(0, 0.5) then
    // max(0, ...) is needed to prevent failing assert in C due to floating point errors.
    duty = vetrnikController(time, windTurbine.omega / (2*Modelica.Constants.pi) * 60, capacitor.v, max(0, Rl.i), windTurbine.vwind);
  end when;
  
  Rl.R = Rload / max(duty, 1e-12);
  annotation(
    uses(Modelica(version = "4.0.0")),
    experiment(StartTime = 0, StopTime = 50, Tolerance = 1e-06, Interval = 0.01));
end test4;