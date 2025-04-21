within Vetrnik;
block DiscreteLimPI
  extends Modelica.Clocked.RealSignals.Interfaces.PartialClockedSISO;
  parameter Real kd "Gain of discrete PI controller";
  parameter Real Td(min=Modelica.Constants.small) "Time constant of discrete PI controller";
  parameter Real Tt(min=Modelica.Constants.small) "anti-windup gain coefficient";
  parameter Real ymax "upper limit of output";
  parameter Real ymin "lower limit of output";
  Modelica.Blocks.Nonlinear.Limiter limiter(uMax = ymax, uMin = ymin)  annotation(
    Placement(transformation(origin = {60, 0}, extent = {{-10, -10}, {10, 10}})));
  Modelica.Blocks.Math.Add add(k2 = -1)  annotation(
    Placement(transformation(origin = {60, -40}, extent = {{-10, -10}, {10, 10}}, rotation = -90)));
  Modelica.Blocks.Math.Add3 add3 annotation(
    Placement(transformation(origin = {-40, 0}, extent = {{-10, -10}, {10, 10}})));
  Modelica.Blocks.Math.Gain gain(k = kd)  annotation(
    Placement(transformation(origin = {30, 0}, extent = {{-10, -10}, {10, 10}})));
  Modelica.Clocked.RealSignals.NonPeriodic.UnitDelay unitDelay1(final y_start = 0)  annotation(
    Placement(transformation(origin = {-40, 40}, extent = {{10, -10}, {-10, 10}}, rotation = -0)));
  Modelica.Blocks.Math.Gain gain1(k = Tt)  annotation(
    Placement(transformation(origin = {20, -60}, extent = {{10, -10}, {-10, 10}})));
  Modelica.Blocks.Math.Gain gain2(k = 1/Td)  annotation(
    Placement(transformation(origin = {-80, 0}, extent = {{-10, -10}, {10, 10}})));
  Modelica.Blocks.Math.Add add1 annotation(
    Placement(transformation(origin = {2, 0}, extent = {{-10, -10}, {10, 10}})));
  Modelica.Blocks.Math.Gain gain3(k = sign(kd))  annotation(
    Placement(transformation(origin = {-22, -60}, extent = {{10, -10}, {-10, 10}}, rotation = -0)));
equation
  connect(limiter.y, y) annotation(
    Line(points = {{71, 0}, {110, 0}}, color = {0, 0, 127}));
  connect(add.u1, limiter.y) annotation(
    Line(points = {{66, -28}, {66, -20}, {80, -20}, {80, 0}, {71, 0}}, color = {0, 0, 127}));
  connect(add.u2, limiter.u) annotation(
    Line(points = {{54, -28}, {54, -20}, {44, -20}, {44, 0}, {48, 0}}, color = {0, 0, 127}));
  connect(gain1.u, add.y) annotation(
    Line(points = {{32, -60}, {60, -60}, {60, -50}}, color = {0, 0, 127}));
  connect(unitDelay1.u, add3.y) annotation(
    Line(points = {{-28, 40}, {-20, 40}, {-20, 0}, {-28, 0}}, color = {0, 0, 127}));
  connect(add3.u1, unitDelay1.y) annotation(
    Line(points = {{-52, 8}, {-60, 8}, {-60, 40}, {-50, 40}}, color = {0, 0, 127}));
  connect(gain.y, limiter.u) annotation(
    Line(points = {{41, 0}, {48, 0}}, color = {0, 0, 127}));
  connect(u, gain2.u) annotation(
    Line(points = {{-120, 0}, {-92, 0}}, color = {0, 0, 127}));
  connect(gain2.y, add3.u2) annotation(
    Line(points = {{-68, 0}, {-52, 0}}, color = {0, 0, 127}));
  connect(add1.y, gain.u) annotation(
    Line(points = {{13, 0}, {18, 0}}, color = {0, 0, 127}));
  connect(add1.u1, add3.y) annotation(
    Line(points = {{-10, 6}, {-16, 6}, {-16, 0}, {-28, 0}}, color = {0, 0, 127}));
  connect(add1.u2, u) annotation(
    Line(points = {{-10, -6}, {-20, -6}, {-20, -20}, {-96, -20}, {-96, 0}, {-120, 0}}, color = {0, 0, 127}));
  connect(gain1.y, gain3.u) annotation(
    Line(points = {{10, -60}, {-10, -60}}, color = {0, 0, 127}));
  connect(gain3.y, add3.u3) annotation(
    Line(points = {{-32, -60}, {-60, -60}, {-60, -8}, {-52, -8}}, color = {0, 0, 127}));
  annotation (defaultComponentName="PI1",
       Icon(graphics={
        Polygon(
          points={{90,-82},{68,-74},{68,-90},{90,-82}},
          lineColor={192,192,192},
          fillColor={192,192,192},
          fillPattern=FillPattern.Solid),
        Line(points={{-90,-82},{82,-82}}, color={192,192,192}),
        Line(points={{-80,76},{-80,-92}}, color={192,192,192}),
        Polygon(
          points={{-80,90},{-88,68},{-72,68},{-80,90}},
          lineColor={192,192,192},
          fillColor={192,192,192},
          fillPattern=FillPattern.Solid),
        Line(
          points={{-80,-82},{-80,-10},{-32,-10},{-32,18},{16,18},{16,46},{64,46},
              {64,80}},
          color={0,0,127},
          pattern=LinePattern.Dot),
        Text(
          extent={{-30,-4},{82,-58}},
          textColor={192,192,192},
          textString="PI"),
        Text(
          extent={{-150,-150},{150,-110}},
          textString="Td=%Td"),
        Ellipse(
          extent={{-87,-3},{-75,-15}},
          lineColor={0,0,127},
          fillColor={255,255,255},
          fillPattern=FillPattern.Solid),
        Ellipse(
          extent={{-37,25},{-25,13}},
          lineColor={0,0,127},
          fillColor={255,255,255},
          fillPattern=FillPattern.Solid),
        Ellipse(
          extent={{9,52},{21,40}},
          lineColor={0,0,127},
          fillColor={255,255,255},
          fillPattern=FillPattern.Solid),
        Ellipse(
          extent={{58,87},{70,75}},
          lineColor={0,0,127},
          fillColor={255,255,255},
          fillPattern=FillPattern.Solid)}));
end DiscreteLimPI;