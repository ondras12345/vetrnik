; TSR MPPT with discrete time PI controller

; parameters

; calculate kd correction factor
; (= pi 3.14159265)
; (= o2r (/ 30 pi)) ; omega to RPM
; (= r2o (/ 1 o2r))
; (= Rload 2.90)  ; load resistance
; (= sqr (fn (x) (* x x)))
; (= r2v 0.177)  ; RPM to Udc
; (= b2duty (* 256 Rload (sqr (/ r2o r2v))))
(= b2duty 259.8656)  ; convert damper.b to duty 0..255
(= r2o 0.1047198)  ; RPM to omega

(= kd (* -0.006208 b2duty))  ; discrete PI controller gain
(= Td 22.251)   ; time constant of discrete time PI controller
(= ka (/ -50 b2duty)) ; anti windup gain, must have the same sign as kd
(= Ts 0.5) ; sampling period
(= lo 9.25) ; optimal TSR (lambda)
(= R 1.1)  ; turbine rotor radius

; calculated params
(= g (/ lo R)) ; vwind to omega_opt gain
(= dn 0)  ; non-saturated controller output

(= x 0)    ; controller state

(= mppt (fn ()
  (let o (* (pwrg "RPM") r2o)) ; omega
  (let vw (ctrlg "vwind"))
  (let e (- (* g vw) o))
  (= x (+
    x
    (* (/ 1 Td) e)  ; I
    (* ka (- d dn)) ; anti-windup (ka * (d(k-1) - d_unsat(k-1)))
  ))
  (= dn (round (* (+ x e) kd))) ; non-saturated controller output
  (- dn d)  ; return requested change of duty
))
