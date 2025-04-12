within Vetrnik;
model CompensatedPower "TODO testing"
input Modelica.Units.SI.AngularVelocity omega;
input Modelica.Units.SI.Power electricalPower;
parameter Modelica.Units.SI.MomentOfInertia J;
parameter Modelica.Units.SI.Time Ts;
output Modelica.Units.SI.Power compensatedPower;
Modelica.Units.SI.AngularVelocity prevOmega;
algorithm
  when sample(-1e-3, Ts) then
    compensatedPower := electricalPower + J * omega * (omega - prevOmega) / Ts;
    prevOmega := omega;
  end when;
end CompensatedPower;