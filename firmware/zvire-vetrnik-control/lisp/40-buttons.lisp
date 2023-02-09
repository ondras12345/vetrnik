; button handler functions

(= btn1_short (fn ()
  (ctrls "strategy" "control_shorted")
))


(= btn1_long (fn ()
  (= d 0)
  (ctrls "strategy" "control_lisp")
  (pwrs "mode" pwr_start)
))
