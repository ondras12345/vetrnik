within Vetrnik;
model SILsimplified "Simplified model for software-in-the-loop simulations"
  import Modelica.Units.SI;
  Modelica.Blocks.Sources.Constant constvwind(k = 12)  annotation(
    Placement(transformation(origin = {10, -20}, extent = {{-10, -10}, {10, 10}})));
  SI.Voltage Udc "DC voltage (at filtering capactior at rectifier output)";
  SI.Current Idc "equivalent DC current";
  Real duty "duty cycle";
  SI.Resistance R "equivalent load resistance";
  Modelica.Units.NonSI.AngularVelocity_rpm rpm "turbine revolutions per minute";
  parameter SI.Resistance Rload = 2.90 "load resistance";
  parameter SI.Time Ts = 0.5 "controller sampling time";
  SimplifiedOpenLoop simplifiedOpenLoop annotation(
    Placement(transformation(origin = {60, 0}, extent = {{-20, -20}, {20, 20}})));
  Modelica.Blocks.Routing.RealPassThrough omega annotation(
    Placement(transformation(origin = {-44, 20}, extent = {{-6, -6}, {6, 6}})));
  Modelica.Blocks.Routing.RealPassThrough b annotation(
    Placement(transformation(origin = {4, 20}, extent = {{-6, -6}, {6, 6}})));
equation
  R = Rload/max(duty, 1e-12);
  Idc = simplifiedOpenLoop.load.lossPower/Udc;
  Udc = rpm*0.177 - 0.497;
  b.y = (Udc/max(omega.u, 1e-12))^2/R;
  rpm = omega.u/(2*Modelica.Constants.pi)*60;
  when sample(0, Ts) then
// max(0, ...) is needed to prevent failing assert in C due to floating point errors.
    duty = vetrnikController(time, rpm, max(Udc, 0), max(Idc, 0), simplifiedOpenLoop.vwind);
  end when;
  connect(simplifiedOpenLoop.vwind, constvwind.y) annotation(
    Line(points = {{44, -8}, {35.5, -8}, {35.5, -20}, {21, -20}}, color = {0, 0, 127}));
  connect(b.y, simplifiedOpenLoop.b) annotation(
    Line(points = {{10, 20}, {20, 20}, {20, 12}, {44, 12}}, color = {0, 0, 127}));
  connect(simplifiedOpenLoop.omega, omega.u) annotation(
    Line(points = {{76, 0}, {80, 0}, {80, -40}, {-60, -40}, {-60, 20}, {-52, 20}}, color = {0, 0, 127}));
  annotation(
    experiment(StartTime = 0, StopTime = 100, Tolerance = 1e-06, Interval = 0.05),
  Diagram(graphics = {Rectangle(origin = {-20, 20}, extent = {{-20, 20}, {20, -20}}), Text(origin = {-20, 20}, extent = {{-14, 8}, {14, -8}}, textString = "LISP
controller")}));
end SILsimplified;