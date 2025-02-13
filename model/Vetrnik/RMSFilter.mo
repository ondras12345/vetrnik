within Vetrnik;
model RMSFilter "discrete time RMS filter utilized by vetrnik-power"
  parameter Integer N = 512;
  Real sum_squares(start=0);
  Real rms(start=1);  // TODO 1e-3?
  Modelica.Blocks.Interfaces.RealInput u annotation(
    Placement(transformation(origin = {-106, 0}, extent = {{-20, -20}, {20, 20}}), iconTransformation(origin = {-60, 0}, extent = {{-20, -20}, {20, 20}})));
  Modelica.Blocks.Interfaces.RealOutput y annotation(
    Placement(transformation(origin = {100, 0}, extent = {{-10, -10}, {10, 10}}), iconTransformation(origin = {60, 0}, extent = {{-10, -10}, {10, 10}})));
algorithm
  when sample(0, 104e-6) then
    sum_squares := sum_squares - sum_squares / N;
    sum_squares := sum_squares + u*u;
    rms := (rms + sum_squares / N / rms) / 2;
    y := rms;
  end when;
annotation(
    Diagram,
  Icon(graphics = {Rectangle(extent = {{-60, 60}, {60, -60}}), Text(origin = {1, -1}, extent = {{-31, 21}, {31, -21}}, textString = "RMS")}));
end RMSFilter;