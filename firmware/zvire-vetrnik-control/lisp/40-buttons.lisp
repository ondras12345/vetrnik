; button handler functions

(= btn1_short (fn ()
  (ctrls "strategy" "control_shorted")
  (rels 1 nil)  ; turn off pump
))


(= btn1_long (fn ()
  (= d 0)
  (= t0 (ds18 0))
  (swen)
  (ctrls "strategy" "control_lisp")
  (pwrs "mode" pwr_start)
))


(= lb t)  ; lcd backlight, bool
(= btn2_short (fn ()
  (= lb (not lb))
  (lcdb lb)
))
