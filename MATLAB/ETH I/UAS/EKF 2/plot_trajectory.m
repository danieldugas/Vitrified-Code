% Plot the simulation data.
close all; clear all; clc;
load('realdata.mat');
idx_gyr = 2;	% Set the data entry point for inputs.
idx_cam = 1;	% Set the data entry point for updates.
t_start = t_q_CW_m(idx_cam);  % Read the start time.
t_end = t_q_CW_m(800);
groundtruth_start = find(t_q_WI_gt > t_start, 1);
groundtruth_end = find(t_q_WI_gt > t_end, 1);

%% Plotting
title('Attitude EKF')
subplot(2, 1, 1); 
plot(t_q_WI_gt(groundtruth_start:groundtruth_end), ...
    q_WI_gt(groundtruth_start:groundtruth_end, :), '-', 'LineWidth', 1);
hold off;
leg = legend('$q_{WI_gt-w}$', '$q_{WI_gt-x}$', '$q_{WI_gt-y}$', '$q_{WI_gt-z}$');
set(leg,'Interpreter','Latex');

xlabel('seconds')
ylabel('Ground truth orientation')

gyro_start = find(t_omega_m > t_start, 1);
gyro_end = find(t_omega_m > t_end, 1);

subplot(2, 1, 2); plot(t_omega_m(gyro_start:gyro_end, 1), ...
    omega_m(gyro_start:gyro_end, :),'LineWidth', 1);
xlabel('seconds')
ylabel('Gyroscope measurements [rad/sec]')
leg = legend('omega_x', 'omega_y', 'omega_z');
