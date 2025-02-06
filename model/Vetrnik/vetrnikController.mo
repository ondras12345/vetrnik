within Vetrnik;
impure function vetrnikController "vetrnik-sil based controller"
  input Modelica.Units.NonSI.AngularVelocity_rpm RPM "turbine speed";
  input Modelica.Units.SI.Voltage voltage "DC voltage";
  input Modelica.Units.SI.Current current "DC current";
  input Modelica.Units.SI.Velocity vwind "wind velocity";
  output Real duty "duty cycle 0..1";
  external "C" duty = wt_sil_controller(Modelica.Utilities.Files.loadResource("modelica://Vetrnik.vetrnikController/source/controller.lisp"), RPM, voltage, current, vwind) annotation(Include="#include \"wt_sil_modelica.h\"", IncludeDirectory="modelica://Vetrnik.vetrnikController/source", Library="vetrniksil", LibraryDirectory="modelica://Vetrnik.vetrnikController/source");
end vetrnikController;
