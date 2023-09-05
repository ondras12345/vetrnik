# coding: utf-8
# https://www.irf.com/technical-info/designtp/dt98-2.pdf
Vcc = 12  # driver supply voltage
Vf = 1.1  # bootstrap diode forward voltage (1n4007, should use different D - too slow)
Vgsmin = 10  # minimum required Vgs
Vdson = 50 * 0.012
Vdson
dVbsmax = Vcc - Vgsmin - Vdson
dVbsmax
Qg = 78e-9
Iqbs = 55e-6 # Quiescent V BS supply current
f = 10e3
Cmin = (2*(2*Qg + Iqbs / f))/(Vcc - Vf - Vdson - Vgsmin)
Cmin
f = 50
Cmin
