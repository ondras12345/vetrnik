within Vetrnik;
function windCp "calculate wind turbine power coefficient"
input Real lambda "tip speed ratio";
input Modelica.Units.SI.Angle beta "blade pitch angle";
output Real Cp "turbine power coefficient";
protected parameter Real c1 = 0.5176;
protected parameter Real c2 = 116;
protected parameter Real c3 = 0.4;
protected parameter Real c4 = 5;
protected parameter Real c5 = 21;
protected parameter Real c6 = 0.0068;
protected Real lambda_i;
protected Real beta_deg;
algorithm
  beta_deg := beta * 180 / Modelica.Constants.pi;
  lambda_i := 1 / ( 1/(lambda+0.08*beta_deg) - 0.035/(beta_deg^3+1) );
  Cp := c1 * (c2/lambda_i - c3*beta_deg - c4) * exp(-c5/lambda_i) + c6*lambda;
end windCp;
