within Vetrnik;
model SIL "software-in-the-loop simulation of the whole 'vetrnik' system with lisp control"
  WindTurbine windTurbine(R = 1.1, beta = 0.08726646259971647)  annotation(
    Placement(transformation(origin = {-50, 0}, extent = {{-10, -10}, {10, 10}})));
  Modelica.Electrical.Machines.BasicMachines.SynchronousMachines.SM_PermanentMagnet smpm(p = 6, fsNominal = 100, Jr = 1.2, VsOpenCircuit = 72.3) annotation(
    Placement(transformation(origin = {-10, 0}, extent = {{10, -10}, {-10, 10}})));
  PolyphaseRectifier polyphaseRectifier(Vknee = .7, Ron = .02)  annotation(
    Placement(transformation(origin = {30, 0}, extent = {{-10, -10}, {10, 10}})));
  Modelica.Electrical.Analog.Basic.Ground groundDC annotation(
    Placement(transformation(origin = {60, -36}, extent = {{-10, -10}, {10, 10}})));
  Modelica.Electrical.Analog.Basic.Capacitor capacitor(C (displayUnit = "uF")= 9.4e-4) annotation(
    Placement(transformation(origin = {60, 0}, extent = {{-10, -10}, {10, 10}}, rotation = -90)));
  Modelica.Blocks.Sources.Constant constVwind(k = 12) annotation(
    Placement(transformation(origin = {-90, 0}, extent = {{-10, -10}, {10, 10}})));
  Modelica.Electrical.Polyphase.Basic.Star star annotation(
    Placement(transformation(origin = {10, -30}, extent = {{-10, -10}, {10, 10}}, rotation = -90)));
  Modelica.Electrical.Analog.Basic.VariableResistor Rl annotation(
    Placement(transformation(origin = {80, 0}, extent = {{-10, -10}, {10, 10}}, rotation = -90)));
  RMSFilter iFilter;
  RMSFilter iFilter2;
  RMSFilter uFilter;
  Modelica.Units.SI.Power filteredPower;
  parameter Modelica.Units.SI.Resistance Rload = 2.90 "load resistance";
  parameter Modelica.Units.SI.Time Ts = 0.5 "controller sampling time";
  Real duty "duty cycle";
equation
  connect(windTurbine.flange, smpm.flange) annotation(
    Line(points = {{-40, 0}, {-20, 0}}));
  connect(smpm.plug_sp, polyphaseRectifier.positivePlug) annotation(
    Line(points = {{-16, 10}, {-16, 20}, {12, 20}, {12, 0}, {22, 0}}, color = {0, 0, 255}));
  connect(groundDC.p, polyphaseRectifier.pin_n) annotation(
    Line(points = {{60, -26}, {60, -20}, {42, -20}, {42, -6}, {38, -6}}, color = {0, 0, 255}));
  connect(capacitor.n, groundDC.p) annotation(
    Line(points = {{60, -10}, {60, -26}}, color = {0, 0, 255}));
  connect(constVwind.y, windTurbine.vwind) annotation(
    Line(points = {{-79, 0}, {-61, 0}}, color = {0, 0, 127}));
  connect(star.plug_p, smpm.plug_sn) annotation(
    Line(points = {{10, -20}, {10, 10}, {-4, 10}}, color = {0, 0, 255}));
  connect(polyphaseRectifier.pin_p, capacitor.p) annotation(
    Line(points = {{38, 6}, {42, 6}, {42, 20}, {60, 20}, {60, 10}}, color = {0, 0, 255}));
  connect(Rl.p, capacitor.p) annotation(
    Line(points = {{80, 10}, {80, 20}, {60, 20}, {60, 10}}, color = {0, 0, 255}));
  connect(Rl.n, capacitor.n) annotation(
    Line(points = {{80, -10}, {80, -20}, {60, -20}, {60, -10}}, color = {0, 0, 255}));

  iFilter.u = -polyphaseRectifier.pin_p.i;
  iFilter2.u = Rl.i;
  uFilter.u = capacitor.v;
  filteredPower = uFilter.y * iFilter.y;
  when sample(0, Ts) then
    // max(0, ...) is needed to prevent failing assert in C due to floating point errors.
    duty = vetrnikController(time, windTurbine.omega / (2*Modelica.Constants.pi) * 60, uFilter.y, max(0, iFilter2.y), windTurbine.vwind);
  end when;
  
  Rl.R = Rload / max(duty, 1e-12);
  annotation(
    uses(Modelica(version = "4.0.0")),
    experiment(StartTime = 0, StopTime = 100, Tolerance = 1e-06, Interval = 0.05));
end SIL;