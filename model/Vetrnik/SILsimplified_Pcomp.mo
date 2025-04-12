within Vetrnik;
model SILsimplified_Pcomp "Simplified model for software-in-the-loop simulations"
  import Modelica.Units.SI;
  Modelica.Blocks.Sources.Constant constvwind(k = 12)  annotation(
    Placement(transformation(origin = {-80, 0}, extent = {{-10, -10}, {10, 10}})));
  SI.Voltage Udc "DC voltage (at filtering capactior at rectifier output";
  SI.Current Idc "equivalent DC current";
  Real duty "duty cycle";
  SI.Resistance R "equivalent load resistance";
  Modelica.Units.NonSI.AngularVelocity_rpm rpm "turbine revolutions per minute";
  SI.Power Pcomp "compensated power";
  parameter SI.Resistance Rload = 2.90 "load resistance";
  parameter SI.Time Ts = 0.5 "controller sampling time";
  CompensatedPower compensatedPower(J = simplifiedOpenLoop.inertia.J, Ts=Ts);
  SimplifiedOpenLoop simplifiedOpenLoop annotation(
    Placement(transformation(extent = {{-20, -20}, {20, 20}})));
equation
  R = Rload/max(duty, 1e-12);
  Idc = simplifiedOpenLoop.load.lossPower/Udc;
  Udc = rpm*0.177 - 0.497;
  simplifiedOpenLoop.b = (Udc/max(simplifiedOpenLoop.omega, 1e-12))^2/R;
  rpm = simplifiedOpenLoop.omega/(2*Modelica.Constants.pi)*60;
//Pcomp = Udc * Idc + inertia.J * simplifiedOpenLoop.omega * der(simplifiedOpenLoop.omega);  // TODO testing
  compensatedPower.omega = simplifiedOpenLoop.omega;
  compensatedPower.electricalPower = Udc*Idc;
  when sample(0, Ts) then
// max(0, ...) is needed to prevent failing assert in C due to floating point errors.
    duty = vetrnikController(time, rpm, max(Udc, 0), max(Idc, 0), simplifiedOpenLoop.vwind);
    Pcomp = compensatedPower.compensatedPower;
  end when;
  connect(simplifiedOpenLoop.vwind, constvwind.y) annotation(
    Line(points = {{-16, -8}, {-40, -8}, {-40, 0}, {-68, 0}}, color = {0, 0, 127}));
  annotation(
    experiment(StartTime = 0, StopTime = 100, Tolerance = 1e-06, Interval = 0.05));
end SILsimplified_Pcomp;
