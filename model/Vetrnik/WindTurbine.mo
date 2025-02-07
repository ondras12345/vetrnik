within Vetrnik;
model WindTurbine "model of a wind turbine"
  extends Modelica.Mechanics.Rotational.Interfaces.PartialTorque;
  import Modelica.Units.SI;
  Modelica.Blocks.Interfaces.RealInput vwind(unit="m/s")  "wind velocity" annotation(
    Placement(transformation(origin = {-106, 2}, extent = {{-20, -20}, {20, 20}}), iconTransformation(origin = {-106, 2}, extent = {{-20, -20}, {20, 20}})));
  parameter SI.Radius R = 0.5 "rotor radius";
  parameter SI.Angle beta = 0 "rotor blade pitch angle";
  parameter SI.Density rho = 1.28 "air density";
  Real lambda "tip speed ratio";
  SI.Area S "area swept by rotor";
  SI.AngularVelocity omega "rotor angular velocity";
  SI.Velocity vtip "rotor tip velocity";
  Real Cp "power coefficient";
  Real Ct "torque coefficient";
  SI.Power Pw "ideal power of the wind";
  SI.Power P "power of the turbine";
equation
  S = Modelica.Constants.pi * R^2;
  omega = der(phi);
  vtip = omega * R;
  // max is needed to prevent division by zero
  lambda = max(vtip / max(vwind, 1e-12), 1e-12);
  Cp = windCp(lambda, beta);
  Ct = Cp/lambda;
  flange.tau = - 0.5 * Ct * rho * S * R * vwind^2;
  // diag
  Pw = 0.5*rho*S*vwind^3;
  P = -flange.tau * omega;
annotation(
    uses(Modelica(version = "4.0.0")),
  Icon(graphics = {Line(origin = {0, -7}, points = {{0, -51}, {0, 51}}, thickness = 0.5), Line(origin = {-19.94, 24.65}, points = {{-20, -20}, {20, 20}}, thickness = 0.75), Line(origin = {20.06, 24.65}, points = {{-20, 20}, {20, -20}}, thickness = 0.75), Line(origin = {0.06, 66.65}, points = {{0, -22}, {0, 22}}, thickness = 0.75), Ellipse(origin = {0, 44}, fillPattern = FillPattern.Solid, extent = {{-4, -4}, {4, 4}})}));
end WindTurbine;