; button handler functions

(= btn1_short (fn ()
  (ctrls "strategy" "control_shorted")
))


(= btn1_long (fn ()
  ; check power mode to prevent mode3 error on power board
  (if
    (is (pwrg "mode") pwr_shorted) (do
      (= d 0)
      (= t0 (ds18 0))
      (swen)
      (ctrls "strategy" "control_lisp")
      (pwrs "mode" pwr_start)
    )
    ; else: do nothing
  )
))


(= btn2_short (fn ()
  (lcdb (not (lcdb)))
))


(= btn2_long (fn ()
  (ethrst)
))
