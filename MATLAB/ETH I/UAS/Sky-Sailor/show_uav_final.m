
global show_uav_part vert fac;
global time_sec;

param;
wing_config;
show_uav_part = wing;

log_x       =   position(:,1);
log_y       =   position(:,2);
log_z       =   -position(:,3);
log_phi     =   angles(:,1);
log_theta   =   angles(:,2);
log_psi     =   angles(:,3);
time        =   clock;


figure(4)
set(gcf,'Position',[8 20 1120 750]);

 
subplot(4,3,[2 3 5 6]);

plot3(log_x,log_y,log_z,'--mo','LineWidth',2);                                       % Plot the value ...
hold on;
for i=1:1:length(log_x)
    plot3([log_x(i) log_x(i)],[log_y(i) log_y(i)],[log_z(i) 0],'-b','LineWidth',1);                   % ... and a line to the ground
end
axis([min(log_x)-0.1 max(log_x)+0.1 min(log_y)-0.1 max(log_y)+0.1 min(log_z)-0.1 max(log_z)+0.1])
axis equal
xlabel('X [m]'); ylabel('Y [m]'); zlabel('Minus Z [m]');                                      % Axis name

view(190,22);
grid(gca,'on');
hold on;

subplot(4,3,7);

plot (time, log_x)
hold on;
title('X position [m]')

subplot(4,3,8);

plot (time, log_y)
hold on;
title('Y position [m]')

subplot(4,3,9);

plot (time, log_z)
hold on;
title('Minus Z position [m]')

subplot(4,3,10);

plot (time, log_phi)
axis([min(time) max(time) -3.4 3.4])
hold on;
title('Phi [rad]')

subplot(4,3,11);

plot (time, log_theta)
axis([min(time) max(time) -3.4 3.4])
hold on;
title('Theta  [rad]')

subplot(4,3,12);

plot (time, log_psi)
axis([min(time) max(time) -3.4 3.4])
hold on;
title('Psi [rad]')
