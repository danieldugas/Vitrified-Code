close all; clear all;

dt = 0.01;
N = 1000;
tt = [0:N-1]'*dt;

g = 9.81;
k1 = 14000;
k2 = 4000;
m = 10;
M = 100;
c = 265;

for j = 1:20

speed = 0.1*(j-1);
l0 = 1.6 ;
h0 = 0.1 ;
h = h0/2*(1 - cos( 2*pi*speed*tt/l0 ))';
dhdt = h0/2*2*pi*speed/l0*sin( 2*pi*speed*tt/l0 )';

t = 0;
oldx = 0;
oldv = 0;
oldoldx = 0;
oldX = 0;
oldV = 0;
oldoldX = 0;

for i = 1:N
    t = t + dt;
    
    % Calculation
    v = oldv + dt/m* ( ...
     - k1*(oldx - h(i) ) ...
     - k2*(oldx - oldX ) ...
     - c *(oldv - oldV ) ...
     - g*m ...
    );
    V = oldV + dt/M* ( ...
     - k2*(oldX - oldx ) ...
     - c *(oldV - oldv ) ...
     - g*M ...
    );
    % - 1000*alpha*( oldx - h(i) ) ...  % Spring link between mass and arm
    x = oldx + dt*v;
    X = oldX + dt*V;
    
    % Storage
    xx(i) = x;
    vv(i) = v;
    XX(i) = X;
    VV(i) = V;
    xx2(i,j) = x;
    XX2(i,j) = X;
    
    % Next iteration values
    oldx = x;
    oldv = v;
    oldoldx = oldx;
    oldX = X;
    oldV = V;
    oldoldX = oldX;
end

end

figure(1)
subplot(2,1,1)
    p = plot(tt, xx+1, tt, XX+2, tt, h, '--' );
    set(p(1), 'Color', [0 0 0])
    set(p(2), 'Color', [0 0 0])
    set(p(3), 'Color', [0.6 0.6 0.6])
    ylabel('Displacement [m]')
    legend('m Position', 'M Position','Road Position')
subplot(2,1,2)
    p = plot(tt, vv, tt, VV, tt, dhdt, '--', tt, xx);
    set(p(1), 'Color', [1 0 0])
    set(p(2), 'Color', [0 0.5 0])
    set(p(3), 'Color', [0.6 0.6 0.6])
    set(p(4), 'Color', [0 0 0])
    xlabel('Time [s]')
    ylabel('Velocity [m/s]')
    legend('m Velocity', 'M Velocity', 'Road Y-Velocity', '(Mass Pos. [m])')

figure(2)
    h = surf(XX2)
    set(h,'LineStyle','none')
