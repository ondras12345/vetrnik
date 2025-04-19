; HCS / P&O MPPT: voltage variant
; This algorithm might not work too well, ADC resolution for voltage
; measurement is a bit too low.
; However, it does much better than the -duty variant under changing wind
; speeds.

; previous values
(= pp 0) ; previous power
(= pv 0) ; previous voltage

; d (duty) is a global variable defined in 31-control-MPPT

(= mppt (fn ()
  (let v (pwrg "voltage"))
  (let i (pwrg "current"))
  (let p (* v i)) ; power delivered to the load

  (let s ; step direction
    (if (is (< pp p) (< pv v)) -1 1))

  ; don't get stuck at 0
  (if (is d 0) (= s 1))
  ; don't get stuck at 255
  (if (is d 255) (= s -1))

  ; save previous values
  (= pp p)
  (= pv v)
  s  ; return step
))
