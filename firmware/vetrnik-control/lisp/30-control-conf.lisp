; configuration
(= RLC 100) ; minimum RPM to switch contactor
(= RL 100) ; RPM low: leave the turbine unloaded until this RPM
(= RH 2000) ; RPM high: do not let the turbine get past this
(= T0M 85) ; max t0
(= T0S 75) ; t0 to restart at after shutdown due to T0M
(= PS 60) ; only start the pump if t0 is higher than this
