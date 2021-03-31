#!/usr/bin/env gnuplot

rho = 500.
c_p = 1000.
lambda = 200.
a = lambda / (rho*c_p)
# BC2
q = 3e5
# BC3
h = 2000.
T_ref = 1000.

T_1d_1(x, t) = 1000 * (1 - erf(x/sqrt(4*a*t)))
T_1d_2(x, t) = 2*q/lambda*sqrt(a*t/pi)*exp(-x**2/(4*a*t)) - q*x/lambda*(1 - erf(x/sqrt(4*a*t)))
T_1d_3(x, t) = T_ref * (1 - erf(x/sqrt(4*a*t)) - exp(h*x/lambda+h**2*a*t/lambda**2)*(1 - erf(x/sqrt(4*a*t)+h*sqrt(a*t)/lambda)))

f1 = 'probes-temperature-1d-BC1.txt'
f2 = 'probes-temperature-1d-BC2.txt'
f3 = 'probes-temperature-1d-BC3.txt'

set terminal pdfcairo rounded
set grid
set key bottom right opaque box reverse Left width 1
set samples 1001


set output 'result-probes-1d-BC1.pdf'
p \
for [i=0:5] f1 u 1:'T_'.i.'[K]' w l lw 2 ti 'T_'.i,\
for [i=0:5] T_1d_1(0.1*i, x) w l lt -1 dt 2 noti

set output 'result-probes-1d-BC2.pdf'
p \
for [i=0:5] f2 u 1:'T_'.i.'[K]' w l lw 2 ti 'T_'.i,\
for [i=0:5] T_1d_2(0.1*i, x) w l lt -1 dt 2 noti

set output 'result-probes-1d-BC3.pdf'
p \
for [i=0:5] f3 u 1:'T_'.i.'[K]' w l lw 2 ti 'T_'.i,\
for [i=0:5] T_1d_3(0.1*i, x) w l lt -1 dt 2 noti


set output 'result-probes-2d.pdf'
set key top left
T_2d(r) = 500 + (1000-500) * log(r/0.5) / log(1/0.5)

p \
'result-2d-order2-x.dat' u 1:3 smooth unique w l lw 2 ti 'T',\
T_2d(x) w l lt -1 dt 2 noti
