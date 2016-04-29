close all; clear all;

dt = 0.001;
N = 1000;
tt = [0:N-1]'*dt;

g = 9.81;
k = 14000;
m = 10;
alpha = 265;

w = sqrt(k/m);
x0 = 0.016 ;
u = x0*sin( w*tt )';
dudt = w*x0*cos( w*tt )';

t = 0;
oldx = 0;
oldv = 0;
oldoldx = 0;

for i = 1:N
    t = t + dt;
    
    % Calculation
    v = oldv + dt/m* ( ...
     -oldx*k ...                       % Spring link between mass and origin
     -alpha* (oldv - dudt(i) ) ...     % Viscous link between mass and arm
     -m*g ...                          % Gravity
    );
    % - 1000*alpha*( oldx - u(i) ) ...  % Spring link between mass and arm
    x = oldx + dt*v;
    
    % Storage
    xx(i) = x;
    vv(i) = v;
    
    % Next iteration values
    oldx = x;
    oldv = v;
    oldoldx = oldx;
end

subplot(2,1,1)
    p = plot(tt, 100*xx, tt, 100*u, '--' );
    set(p(1), 'Color', [0 0 0])
    set(p(2), 'Color', [0.6 0.6 0.6])
    ylabel('Displacement [cm]')
    legend('Mass Position', 'Driven Position')
subplot(2,1,2)
    p = plot(tt, vv, tt, dudt, '--', tt, xx);
    set(p(1), 'Color', [1 0 0])
    set(p(2), 'Color', [0.6 0.6 0.6])
    set(p(3), 'Color', [0 0 0])
    xlabel('Time [s]')
    ylabel('Velocity [m/s]')
    legend('Mass Velocity', 'Driven Velocity', '(Mass Pos. [m])')
