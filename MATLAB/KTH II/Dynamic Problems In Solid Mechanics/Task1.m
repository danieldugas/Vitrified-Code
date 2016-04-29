close all;
load('signal.mat')
dt = time(2) - time(1);
fs = 1/dt; % sampling frequency
speed = intg(acceleration)*dt ; %trapezoidal integration
distance = intg(speed)*dt ; 

figure(1)
  subplot(3,2,1)
    plot(time, acceleration)
    ylabel('acceleration [m/s^2]')
    title('Signal')

  subplot(3,2,3)
    plot(time, speed)
    ylabel('speed [m/s]')

  subplot(3,2,5)
    plot(time, distance)
    xlabel('time [s]')
    ylabel('distance [m]')

N = 3752/2;
frequency = linspace(0,fs/2,N)';
aspectrum = abs(fft(acceleration))/N;
sspectrum = abs(fft(speed))/N;
dspectrum = abs(fft(distance))/N;

  subplot(3,2,2)
    plot(frequency, aspectrum(1:N))
    title('Spectrum')
    

  subplot(3,2,4)
    plot(frequency, sspectrum(1:N))
    
  subplot(3,2,6)
    plot(frequency, dspectrum(1:N))
    xlabel('frequency [Hz]')

