; configuration
(= SA 8) ; step authority
(= RLC 100) ; minimum RPM to switch contactor
(= RL 50) ; RPM low
(= RH 2000) ; RPM high, TODO
(= T0M 85) ; max t0
(= T0S 75) ; t0 to start after T0M
(= PS 60) ; only start the pump if t0 is higher than this
(= Ts 8.0) ; MPPT sampling period in seconds
;(= J (* 1.2 0.0109662)) ; moment of inertia, adjusted for rpm instead of rad/s
