; Adaptive HCS based on doi 10.1109/TIE.2010.2044732

; parameters
(= be 0.08) ; beta: angular velocity error gain
(= d0 2)    ; initial step size for mode 0
(= ep 30)   ; epsilon: bound for wind change detection
(= Ts 8)  ; MPPT sampling period in seconds

; previous values
(= pp 0) ; previous power
(= ppp 0) ; pre-previous power
(= pv 0) ; previous voltage
(= pr 0) ; previous rpm
(= ps d0) ; previous step
(= pdv nil) ; previous delta vwind

; state
(= m 0) ; mode
; mode 0: HCS with peak detection
; mode 1: no perturbation (constant duty)
; mode 2: adaptive hill climbing using kopt
(= ko 0) ; kopt
(= ro 0) ; omega opt
(= po 0) ; Popt

; d (duty) is a global variable defined in 31-control-MPPT

(= abs (fn (x) (if (< x 0) (* -1 x) x)))
; isclose with absolute tolerance
(= isc (fn (x y tol) (< (abs (- x y)) tol)))
; sign
(= sgn (fn (x) (if (< x 0) -1 (< 0 x) 1 0)))

(= mppt (fn ()
  (let v (pwrg "voltage"))
  (let i (pwrg "current"))
  (let r (pwrg "RPM"))
  (let p (* v i)) ; power delivered to the load

  (let dr (- r pr))

  ; true if wind velocity changed
  (let dv (or
    (< ep (abs dr)) ; omega changed too much
    (is (< ps 0) (< dr 0)) ; omega does not follow duty inversely
    (and (< p pp) (< pp ppp)) ; power has gone down two times in a row
    ))

  (let s ; step direction
    (if (is m 0)
          ; mode 0
          (if (and dv (not (is ko 0))) (do
              (= m 2)
              (let rs (pow (/ p ko) 0.3333333333))  ; omega*
              (* be (- r rs)) ; step value
            )
            (if (and (< p pp) (not dv) (not pdv)) (do
                (= m 1)
                (= ko (/ pp (pow pr 3)))
                (= ro pr)
                (= po pp) ; Popt
                (- 0 ps) ; step value
              )
              (* ps (if (< p pp) -1 1)) ; step value
            )
          )
        (is m 1)
          ; mode 1
          (if (and (isc r ro 20) (isc p po 50)) 0 ; step value = 0
              ; else
              (do
                (= m 2)
                (let rs (pow (/ p ko) 0.3333333333))  ; omega*
                (* be (- r rs)) ; step value
              )
          )
        ; else: mode 2
        (do
          (let rs (pow (/ p ko) 0.3333333333))  ; omega*
          ; the last arg of isc is an absolute tolerance value for the RPM being "close enough"
          (if
            (and (not dv) (isc r rs 15)) (do
                (= m 0)
                (* d0 (sgn ps) (sgn (- p pp))) ; step value
              )
            (* be (- r rs)) ; step value
          ))))

  ; save previous values
  (= ppp pp)
  (= pp p)
  (= pv v)
  (= pr r)
  (= ps s)
  (= pdv dv)
  s  ; return step
))
