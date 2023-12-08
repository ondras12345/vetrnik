; button handler functions

(= btn1_short (fn ()
  (ctrls "strategy" "control_shorted")
))


(= btn1_long (fn ()
  (= d 0)
  (= t0 (ds18 0))
  (swen)
  (ctrls "strategy" "control_lisp")
  (pwrs "mode" pwr_start)
))


(= btn2_short (fn ()
  (lcdb (not (lcdb)))
))
