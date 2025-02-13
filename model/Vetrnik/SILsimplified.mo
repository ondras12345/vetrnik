within Vetrnik;
model SILsimplified "Simplified model for software-in-the-loop simulations"
  import Modelica.Units.SI;
  WindTurbine windTurbine(R = 1.1, beta = 0.08726646259971647)  annotation(
    Placement(transformation(origin = {-38, 0}, extent = {{-10, -10}, {10, 10}})));
  Modelica.Blocks.Sources.Constant constvwind(k = 12)  annotation(
    Placement(transformation(origin = {-80, 0}, extent = {{-10, -10}, {10, 10}})));
  Modelica.Mechanics.Rotational.Components.Inertia inertia(J = 1.2)  annotation(
    Placement(transformation(extent = {{-10, -10}, {10, 10}})));
  VariableDamper load annotation(
    Placement(transformation(origin = {40, 0}, extent = {{-10, -10}, {10, 10}})));
  Modelica.Mechanics.Rotational.Components.Fixed fixed annotation(
    Placement(transformation(origin = {60, -10}, extent = {{-10, -10}, {10, 10}})));
  SI.Voltage Udc "DC voltage (at filtering capactior at rectifier output";
  SI.Current Idc "equivalent DC current";
  Real duty "duty cycle";
  SI.Resistance R "equivalent load resistance";
  Modelica.Units.NonSI.AngularVelocity_rpm rpm "turbine revolutions per minute";
  parameter SI.Resistance Rload = 2.90 "load resistance";
  parameter SI.Time Ts = 0.5 "controller sampling time";
equation
  connect(constvwind.y, windTurbine.vwind) annotation(
    Line(points = {{-68, 0}, {-48, 0}}, color = {0, 0, 127}));
  connect(windTurbine.flange, inertia.flange_a) annotation(
    Line(points = {{-28, 0}, {-10, 0}}));
  connect(inertia.flange_b, load.flange_a) annotation(
    Line(points = {{10, 0}, {30, 0}}));
  connect(fixed.flange, load.flange_b) annotation(
    Line(points = {{60, -10}, {60, 0}, {50, 0}}));
  R = Rload / max(duty, 1e-12);
  Idc = load.lossPower / Udc;
  Udc = rpm * 0.177 - 0.497;
  load.d = (Udc/max(windTurbine.omega, 1e-12))^2 / R;
  rpm = windTurbine.omega / (2*Modelica.Constants.pi) * 60;
  when sample(0, Ts) then
    // max(0, ...) is needed to prevent failing assert in C due to floating point errors.
    duty = vetrnikController(time, rpm, max(Udc, 0), max(Idc, 0), windTurbine.vwind);
  end when;
annotation(
    experiment(StartTime = 0, StopTime = 100, Tolerance = 1e-06, Interval = 0.05));
end SILsimplified;