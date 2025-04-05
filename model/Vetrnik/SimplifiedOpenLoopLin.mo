within Vetrnik;

model SimplifiedOpenLoopLin "SimplifiedOpenLoop with w.start set to allow linearization."
  // set w start to make sure the turbine doesn't fail to start with large b.
  extends SimplifiedOpenLoop(inertia(w(start=100, fixed = true), phi(start=0, fixed=true)));
equation

end SimplifiedOpenLoopLin;
