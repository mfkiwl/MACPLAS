#!/usr/bin/env gnuplot

set term pdfcairo rounded size 15cm,25cm font ',10'

set grid
set key top left Left reverse width -2

F = '5'
array T = [400, 500, 600, 700, 800, 900]

set datafile separator '\t'

set output sprintf('probes-compare-F%s.pdf', F)
set multiplot layout 4,2

set logscale y
set format y "10^{%L}"

col='N_m_0[m^-2]'
set title col noenh
p \
for[k=1:|T|] sprintf('results-F%s-T%g/probes-dislocation-3d.txt', F, T[k]) \
u 1:(abs(column(col))) w l ti sprintf('T = %g K', T[k])

unset key
col='dot_N_m_0[m^-2s^-1]'
set title col
rep

col='v_0[ms^-1]'
set title col
rep

col='stress_0_0[Pa]'
set title col
rep

col='strain_c_0_0[-]'
set title col
rep

col='dot_strain_c_0_0[s^-1]'
set title col
rep

col='tau_eff_0[Pa]'
set title col
set xlabel 't, s'
rep

col='displacement_2_0[m]'
set title col
rep

unset multiplot
