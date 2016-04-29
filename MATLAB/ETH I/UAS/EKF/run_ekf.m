clc, clear all, close all

simulation_data = load('simulation_data.txt');

len = length(simulation_data);
sim_data = 0;

%% Init Values
dt = 0.01;

pos_real = zeros(len,3);
deg2rad = pi/180;

sigma_acc = 0.03;
sigma_acc_bias = 0.0006;
sigma_gyr = 0.00174533;
sigma_gyr_bias = 0.00261799;
sigma_pos_meas = 0.1;
sigma_pos_meas_d = sigma_pos_meas;
acc_bias_real = [0.0 0.2 0.1]';

a_real = [0.0 0 0]';
curr_pos_real = [0.0 0 0]';
curr_vel_real = [0 0 0]';

e1 = [1 0 0]';
e2 = [0 1 0]';
e3 = [0 0 1]';
I = eye(3);
O = zeros(3);

grav = 9.81*e3;

% How often do we pass an update to the filter?
update_every_n = 100;

%% Initial estimated state values.
p_init = curr_pos_real;
%r = curr_pos_real + [0 -0.5  0]';

v_init = curr_vel_real;
%v = curr_vel_real + [0.2 0.2 -0.2]';

% The initial orientation.
q_real = [1 0 0 0]';

bias_gyr_init = [0 0 0]';
bias_acc_init = [0 0 0]';

x = [p_init' v_init' bias_acc_init']';

P = eye(9)*0.0001;
P(1:3, 1:3) = eye(3) * 0.1;
P(4:6, 4:6) = eye(3) * 0.01;
P(7:9, 7:9) = eye(3) * 0.1;

Q_acc = sigma_acc^2*eye(3);
Q_acc_bias = sigma_acc_bias^2*eye(3);

%% Run
%Preallocate memory.
recording(len).x = zeros(9, 1);
recording(len).P = zeros(9, 9);
recording(len).z = zeros(4, 1);
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
    curr_pos_real = curr_pos_real + curr_vel_real*dt + a_NED_sim*(dt)^2/2;
    curr_vel_real = curr_vel_real + a_NED_sim*(dt);
    
    acc_bias_real = acc_bias_real + sigma_acc_bias * randn(3, 1);
    acc_sim = C_real * (a_NED_sim - grav);
    acc_meas = acc_sim + randn(3,1) * sigma_acc + acc_bias_real;
    u = acc_meas;
    
    recording(j).x = x;
    recording(j).P = P;
    
    % Run EKF.
    if (j==1)
        [x_pred, P_pred]=ekf_predict(x, q_real, u, P, dt, Q_acc, Q_acc_bias, grav);
    else
        [x_pred, P_pred]=ekf_predict(x, q_real, u, P, dt, Q_acc, Q_acc_bias, grav);
        
        % Do updates from time to time.
        if (mod(j, update_every_n) == 0)
            % Generate Measurements by using the true position and adding
            % noise.
            z = curr_pos_real + sigma_pos_meas * randn(3, 1);
            recording(j).z = [j; z];
            [x, P, recording(j).r] = ekf_update(x_pred, P_pred, z, sigma_pos_meas_d);
        else
            % Else our best estimate is the prediction.
            x = x_pred;
            P = P_pred;
        end
    end
    
    x_disp = x;
    P_disp = P;
    
    % Prepare data for saving.
    recording(j).curr_pos_real = curr_pos_real;
    recording(j).curr_vel_real = curr_vel_real;
    recording(j).acc_bias_real = acc_bias_real;
end

N = j;

%% save and plot
save('recording.mat','recording')
plot_data_ekf
