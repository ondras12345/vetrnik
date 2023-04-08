#!/usr/bin/env octave
%% datasheet values
P = 3  % W
S = 7.7  % VA
f = 50  % Hz
U = 230  % V

%% impedance calculation
I = S / U
cos_phi = P / S
%phi = acos(cos_phi)
Z = U / I
R = Z * cos_phi
%R = P / I^2  % stejne
Xl = sqrt(Z^2 - R^2)
%Xl = Z * sin(phi)
L = Xl / (2*pi*f)


%% measured values for comparison
m_R = 566.7
m_I = 55e-3
