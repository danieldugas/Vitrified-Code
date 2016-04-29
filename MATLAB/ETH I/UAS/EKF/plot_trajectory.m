% Plot the simulation data.
close all; clear all; clc;
simulation_data = load('simulation_data.txt');

len = length(simulation_data);
position = zeros(len, 3);
attitude = zeros(len, 3);

dt = 0.01;
deg2rad = pi/180;

% Initial conditions.
q_real = [1 0 0 0]';
pos_real = [0 0 0]';
vel_real = [0 0 0]';
t = linspace(0,len * 0.01,len);

for j=1:len
    fprintf('Step %i \n', j)
    
    % Import data.
    data = simulation_data(j,:)';
    a_NED_sim = data(1:3);
    gyr_sim = data(7:9);
    
    % Prediction Attitude.
    q_real = propagateQuat(gyr_sim,q_real,dt);
    C_real = quat2rot(q_real)';
    
    % Prediction Position.
    pos_real = pos_real + vel_real*dt + a_NED_sim*(dt)^2/2;
    position(j, :) = pos_real;
    
    rpy = quat2rpy(q_real);
    roll = rpy(1);
    pitch = rpy(2);
    yaw = rpy(3);    
    attitude(j, :) = [roll pitch yaw]/deg2rad;
end

plot3(position(:, 1), position(:, 2), position(:, 3))
title('Position')
xlabel('x [m]');
ylabel('y [m]');
zlabel('z [m]');
axis equal;

%% Attitude Plot
figure
subplot(3,1,1)
title('Attitude')
plot(t, attitude(:,1),'b');

legend('Ground Truth');

axis tight
ylabel('Roll Angle [deg]');
subplot(3,1,2)
plot(t, attitude(:,2),'r');
hold on
plot(t,attitude(:,2),'b')
hold on
axis tight
ylabel('Pitch Angle [deg]');
subplot(3,1,3)
plot(t, attitude(:,3),'r');
hold on
plot(t,attitude(:,3),'b')
hold on
axis tight
xlabel('Time [s]');
ylabel('Yaw Angle [deg]');