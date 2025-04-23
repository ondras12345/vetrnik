; Basic infrastructure for MPPT control. The actual MPPT function is in a
; separate file.
; configuration parameters (uppercase vars) are in 30-control-conf.lisp

; TODO don't ignore OCP Cxxx

(= ti 0) ; time
(= d 0) ; duty


(= pump (fn ()
  (out "PUMP"
    (if
      (pwrg "last5m") (if
          (not t0) t  ; t0 is nil - broken sensor
          (and
            (not (out "PUMP"))  ; do not switch relay off if temperature falls
            (< t0 PS)
          ) nil
          t
        )
      nil  ; turbine in not generating power
    )
  )
))

(= swen (fn ()
  (pwrs "sw_enable"
    (if
      (not t0) nil  ; always off if the temperature sensor doesn't work
      (if
        (pwrg "sw_enable") (< t0 T0M)  ; enabled - check MAX
        (< t0 T0S)  ; disabled - check start
      )
    )
  )
))

(= ctrl (fn ()
  ; ctrl is called each time a new state object is received.
  ; If everything is working correctly, that should be every 0.5 seconds.
  (= ti (+ ti 0.5))

  (= t0 (ds18 0))
  (swen)
  (pump)

  (if (<= Ts ti)
    (do
      (= ti (- ti Ts)) ; prevent loss of precision in float numbers
      (let s (mppt))  ; step
      ; RPM limits
      (let r (pwrg "RPM"))
      (ctrls "contactor" (< RLC r))
      (= d
        (if
          (< r RL) 0
          (< RH r) 255
          (+ d s)
        )
      )
      ; check power mode & apply duty limits
      (= d (if
          (is (pwrg "mode") pwr_const_duty) (lim d 0 255)
          0
          ))
      (pwrs "duty" d)
      ))
))
