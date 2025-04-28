;;; HCS / P&O MPPT: voltage variant
;; parameters
(= SA 3) ; step authority
(= Ts 12) ; MPPT sampling period in seconds

;; previous values
(= pp 0) ; previous power
(= pv 0) ; previous voltage

; d (duty) is a global variable defined in 31-control-MPPT

(= mppt (fn ()
  (let v (pwrg "voltage"))
  (let i (pwrg "current"))
  (let p (* v i)) ; power delivered to the load

  (let s ; step direction
    (if (is (< pp p) (< pv v)) -1 1))

  (= s (if
    (is d 0) 1 ; don't get stuck at 0
    (is d 255) -1 ; don't get stuck at 255
    s))

  ; save previous values
  (= pp p)
  (= pv v)
  (* s SA)))  ; return step
