within Vetrnik;
model PolyphaseRectifier "m-phase rectifier"
  parameter Integer m(min=1) = 3 "Number of phases" annotation(Evaluate=true);
  parameter Modelica.Units.SI.Resistance Ron(final min=0, start=1e-5) "Closed diode resistance";
  parameter Modelica.Units.SI.Conductance Goff(final min=0, start=1e-5) "Opened diode conductance";
  parameter Modelica.Units.SI.Voltage Vknee(final min=0, start=0) "Threshold voltage";
  Modelica.Electrical.Polyphase.Basic.Star star1(m=m) annotation(
    Placement(transformation(origin = {0, 50}, extent = {{-10, -10}, {10, 10}}, rotation = 90)));
  Modelica.Electrical.Polyphase.Basic.Star star2(m=m) annotation(
    Placement(transformation(origin = {0, -50}, extent = {{-10, -10}, {10, 10}}, rotation = -90)));
  Modelica.Electrical.Polyphase.Ideal.IdealDiode diode1(
    final m=m,
    final Ron=fill(Ron, m),
    final Goff=fill(Goff, m),
    final Vknee=fill(Vknee, m)
  ) annotation(
    Placement(transformation(origin = {0, 20}, extent = {{-10, -10}, {10, 10}}, rotation = 90)));
  Modelica.Electrical.Polyphase.Ideal.IdealDiode diode2(
    final m=m,
    final Ron=fill(Ron, m),
    final Goff=fill(Goff, m),
    final Vknee=fill(Vknee, m)
  ) annotation(
    Placement(transformation(origin = {0, -20}, extent = {{-10, -10}, {10, 10}}, rotation = 90)));
  Modelica.Electrical.Polyphase.Interfaces.PositivePlug positivePlug annotation(
    Placement(transformation(origin = {-80, 0}, extent = {{-10, -10}, {10, 10}}), iconTransformation(origin = {-80, 0}, extent = {{-10, -10}, {10, 10}})));
  Modelica.Electrical.Analog.Interfaces.PositivePin pin_p annotation(
    Placement(transformation(origin = {80, 60}, extent = {{-10, -10}, {10, 10}}), iconTransformation(origin = {80, 60}, extent = {{-10, -10}, {10, 10}})));
  Modelica.Electrical.Analog.Interfaces.NegativePin pin_n annotation(
    Placement(transformation(origin = {80, -60}, extent = {{-10, -10}, {10, 10}}), iconTransformation(origin = {80, -60}, extent = {{-10, -10}, {10, 10}})));
equation
  connect(diode2.plug_n, diode1.plug_p) annotation(
    Line(points = {{0, -10}, {0, 10}}, color = {0, 0, 255}));
  connect(diode1.plug_n, star1.plug_p) annotation(
    Line(points = {{0, 30}, {0, 40}}, color = {0, 0, 255}));
  connect(star2.plug_p, diode2.plug_p) annotation(
    Line(points = {{0, -40}, {0, -30}}, color = {0, 0, 255}));
  connect(positivePlug, diode2.plug_n) annotation(
    Line(points = {{-80, 0}, {0, 0}, {0, -10}}, color = {0, 0, 255}));
  connect(pin_n, star2.pin_n) annotation(
    Line(points = {{80, -60}, {0, -60}}, color = {0, 0, 255}));
  connect(pin_p, star1.pin_n) annotation(
    Line(points = {{80, 60}, {0, 60}}, color = {0, 0, 255}));

annotation(
    uses(Modelica(version = "4.0.0")),
  Icon(graphics = {Line(origin = {-19.0036, 0.214286}, points = {{-29.8536, 0}, {10.1464, 0}, {10.1464, -20}, {30.1464, 0}, {10.1464, 20}, {10.1464, 0}, {10.1464, 0}}), Line(origin = {11.1429, 0.214286}, points = {{0, -20}, {0, 20}}), Line(origin = {31.1429, 0.214286}, points = {{-20, 0}, {20, 0}}), Rectangle(extent = {{-80, 80}, {80, -80}})}));
end PolyphaseRectifier;
