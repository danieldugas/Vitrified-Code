clear all; close all;

pref = 2*10^(-5) ;
pref2 = pref^2 ;

load('Calibration ThirdOctave Spectrum.mat')
Lp = 93;
Gss = Gxxn(24);

K = pref2*10^(Lp/10)/Gss ;

load('Exhaust ThirdOctave Spectrum')
Lpun = 10*log10(K*Gxxn/pref2) ;

load('Exhaust Narrowband Spectrum')
Lpu = 10*log10(K*Gxx/pref2) ;

plot(f, Lpu)
hold all;
stairs(fn, Lpun)
xlabel('Frequency [HZ]')
ylabel('Lp [dB]')
legend('Narrowband', 'Third-Octave')

save('Exhaust Lpu.mat', 'f', 'Lpu')

A = [-16.1 -13.4 -10.9 -8.6 -6.6 -4.8 -3.2 -1.9 -0.8]' ;
fnt = fn(fn>=125&fn<=800) ;
Lpunt = Lpun(fn>=125&fn<=800) ;
Launt = Lpunt + A ;