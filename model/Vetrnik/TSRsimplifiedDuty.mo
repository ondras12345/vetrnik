within Vetrnik;
model TSRsimplifiedDuty "TSRsimplified with equivalent duty cycle calculation"
  extends TSRsimplified(plant(inertia(w(start=0, fixed=true), phi(start=0, fixed=true))));
  Real duty "equivalent duty cycle";
  Modelica.Units.NonSI.AngularVelocity_rpm rpm "turbine revolutions per minute";
  Modelica.Units.SI.Voltage Udc "equivalent DC voltage (at filtering capactior at rectifier output)";
  parameter Modelica.Units.SI.Resistance Rload = 2.90 "load resistance";
equation
  rpm = plant.omega/(2*Modelica.Constants.pi)*60;
  Udc = max(rpm*0.177 - 0.497, 0);
  duty = plant.b * Rload * (plant.omega / max(Udc, 1e-12))^2;
end TSRsimplifiedDuty;
