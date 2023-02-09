(= d_v 0) ; display view (A / B)

; line 0 A
(= d_l0a (fn ()
  ; 0
  (lcdn (pwrg "voltage") 3 1)
  (lcds "V ")
  ; 7
  (lcdn (pwrg "current") 2 3)
  (lcds "A ")
  ; 15
  (lcdn (pwrg "mode") 1 0)
  ; 16
  (lcdw 0)
))

; line 0 B
(= d_l0b (fn ()
  ; 0
  (lcdn (pwrg "duty") 3 0)
  (lcds "d ")
  ; 5
  (lcdn (pwrg "RPM") 3 0)
  (lcds "RPM ")
  ; 12
  (lcdn (pwrg "temperature_heatsink") 2 0)
  ; 14
  (lcds "'C")
  ; 16
  (lcdw 0)
))

; line 1 A
(= d_l1a (fn ()
  ; 0
  (lcdn (stats "energy") 3 2)
  (lcds "kWh")
  ; 9
  (lcdw 1)
))

; line 1 B
(= d_l1b (fn ()
  ; 0
  (lcds (ctrlg "strategy"))
  (lcdw 1)
))

; main display function
(= disp (fn ()
  (if
    (<= d_v 2) (do (d_l0a) (d_l1a))
    (<= d_v 5) (do (d_l0b) (d_l1b))
  )
  (= d_v (if (<= 5 d_v) 0 (+ d_v 1)))
))
