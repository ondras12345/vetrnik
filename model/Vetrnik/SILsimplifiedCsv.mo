within Vetrnik;
model SILsimplifiedCsv "Simplified model for software-in-the-loop simulations with input for wind velocity (to be used with csvInput)"
  // Unfortunately, I wasn't able to figure out how to make csvInput work with OMEdit.
  // It seems to be looking for the file in inputPath even if I specify an absolute pat for csvInput.
  // Workaround: wait for it to generate the inpuPath in /tmp and copy the csv file there manually.
  import Modelica.Units.SI;
  SI.Voltage Udc "DC voltage (at filtering capactior at rectifier output)";
  SI.Current Idc "equivalent DC current";
  Real duty "duty cycle";
  SI.Resistance R "equivalent load resistance";
  Modelica.Units.NonSI.AngularVelocity_rpm rpm "turbine revolutions per minute";
  parameter SI.Resistance Rload = 2.90 "load resistance";
  parameter SI.Time Ts = 0.5 "controller sampling time";
  SimplifiedOpenLoop simplifiedOpenLoop(inertia(w(start=0, fixed = true), phi(start=0, fixed=true))) annotation(
    Placement(transformation(origin = {60, 0}, extent = {{-20, -20}, {20, 20}})));
  Modelica.Blocks.Routing.RealPassThrough omega annotation(
    Placement(transformation(origin = {-44, 20}, extent = {{-6, -6}, {6, 6}})));
  Modelica.Blocks.Routing.RealPassThrough b annotation(
    Placement(transformation(origin = {4, 20}, extent = {{-6, -6}, {6, 6}})));
  Modelica.Blocks.Interfaces.RealInput vwind annotation(
    Placement(transformation(origin = {0, -20}, extent = {{-20, -20}, {20, 20}}), iconTransformation(origin = {0, -20}, extent = {{-20, -20}, {20, 20}})));
initial equation
  duty = 0;
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
  connect(b.y, simplifiedOpenLoop.b) annotation(
    Line(points = {{10, 20}, {20, 20}, {20, 12}, {44, 12}}, color = {0, 0, 127}));
  connect(simplifiedOpenLoop.omega, omega.u) annotation(
    Line(points = {{76, 0}, {80, 0}, {80, -40}, {-60, -40}, {-60, 20}, {-52, 20}}, color = {0, 0, 127}));
  connect(vwind, simplifiedOpenLoop.vwind) annotation(
    Line(points = {{0, -20}, {26, -20}, {26, -8}, {44, -8}}, color = {0, 0, 127}));
  annotation(
  Diagram(graphics = {Rectangle(origin = {-20, 20}, extent = {{-20, 20}, {20, -20}}), Text(origin = {-20, 20}, extent = {{-14, 8}, {14, -8}}, textString = "LISP
controller")}),
  experiment(StartTime = 0, StopTime = 400, Tolerance = 1e-06, Interval = 0.1),
  __OpenModelica_simulationFlags(csvInput = "vwind-ramps.csv", lv = "LOG_STDOUT,LOG_ASSERT,LOG_STATS", s = "dassl", variableFilter = ".*"));
end SILsimplifiedCsv;