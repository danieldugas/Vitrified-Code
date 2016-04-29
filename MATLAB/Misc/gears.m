clear all; close all;

%	PARAMETERS
max_RPM = 10000; % max RPM tolerated by the engine
max_ground_speed = 180; % max operational speed of vehicle
peak_RPM = 6500; % RPM at which engine produces max power

%	AXES
ground_speed = linspace(0,sqrt(max_ground_speed),1000).^2; % speed in km/h
RPM = []; % RPM in relation to ground speed for each gear.

%	GEAR SETUP
% each gear is characterised by its r, 
% the ratio between wheel axis angular speed and engine angular speed
% For gear 'i', RPM = r(i)*ground_speed
N_gears = 6; 
% for now we manually define r for each gear. later, introduce a function calculating r for maximum torque
% r = f(N_gears, torque_curve, etc)
r = [1280 640 320 160 80 40];

% ratios for each gear
RPM = ground_speed'*r;
extremas_RPM = [0 max_RPM];
extremas_ground_speed = extremas_RPM'*(1./r);
power = 10000-0.001*(RPM-peak_RPM).^2;
power = max(power, 0); RPM(RPM>max_RPM) = nan; 
plot3(extremas_ground_speed,extremas_RPM,zeros(size(extremas_ground_speed)));
hold on;
plot3(ground_speed,RPM,power);
xlim([0 max_ground_speed]);
zlim([0 3*max(max(power))]);
xlabel('Ground Speed');
ylabel('RPM');
zlabel('Power');
