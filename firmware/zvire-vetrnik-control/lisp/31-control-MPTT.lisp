; A more realistic MPPT control function

; TODO increase step authority when it gets stuck ??
; TODO (cause of problems - low voltage resolution)

; TODO OCP / limit duty -- better do that in power board firmware


; previous values
(= pp 0)

(= s 0) ; step
(= d 0) ; duty

; configuration parameters (uppercase vars) are in 30-control-conf.lisp

(= lim (fn (x l h)
  (if
    (< x l) l
    (< h x) h
    x
  )
))

(= ctrl (fn ()
  (let v (pwrg "voltage"))
  (let i (pwrg "current"))
  (let r (pwrg "RPM"))
  (let p (* v i)) ; power
  (= s
    (if
      ; decreasing load
      (< s 0)
        (if
          (< p pp) 1 ; power is decreasing
          -1 ; power is increasing, continue decreasing
        )
      ; increasing load
      (if
        (< p pp) -1 ; power is decreasing
        1
      )
    )
  )

  ; don't get stuck at 0
  (if (is d 0) (= s 1))

  (= pp p)

  ; RPM limits
  (= d
    (if
      (< r RL) 0
      (< RH r) 255
      ; our control loop does not run extremely often,
      ; so better give step (s) more authority
      (+ d (* s SA))
    )
  )
  ; power mode
  (= d
    (if
      (is (pwrg "mode") pwr_const_duty) d
      0
    )
  )
  ; duty limits
  (= d (lim d 0 255))
  (pwrs "duty" d)
))
