clear all% close all
deg2rad = pi/180;
load('recording.mat');

t = linspace(0,length(recording)/100,length(recording));

% Pre-allocate.
pos_real = zeros(length(recording), 3);
pos_est = zeros(length(recording), 3);
vel_real = zeros(length(recording), 3);
vel_est = zeros(length(recording), 3);
bias_acc = zeros(length(recording), 3);
measurement = zeros(length(recording), 4);

for i = 1:length(recording)
    pos_real(i,:) = recording(i).curr_pos_real';
    pos_est(i,1:3) = recording(i).x(1:3)';
    
    vel_real(i,:) = recording(i).curr_vel_real;
    vel_est(i,:) = recording(i).x(4:6);
 
    bias_acc(i,1:3) = recording(i).x(7:9);
    bias_acc(i,4:6) = recording(i).acc_bias_real;
    
    if recording(i).z
      measurement(i,:)  = recording(i).z;
    end
end

%% Position Plot
figure
valid_meas = measurement(:, 1) > 0;

subplot(3,1,1)
plot(t, pos_real(:,1),'r', t, pos_est(:,1),'b')
hold on;
plot(t(measurement(valid_meas, 1)), measurement(valid_meas, 2),'gx')
legend('Ground Truth','Filter Estimate', 'Measurements');
axis tight
ylabel('Position x [m]');

subplot(3,1,2)
plot(t, pos_real(:,2),'r', t, pos_est(:,2),'b')
hold on;
plot(t(measurement(valid_meas, 1)), measurement(valid_meas, 3),'gx')
axis tight
ylabel('Position y [m]');

subplot(3,1,3)
plot(t, pos_real(:,3),'r', t, pos_est(:,3),'b')
hold on;
plot(t(measurement(valid_meas, 1)), measurement(valid_meas, 4),'gx')
axis tight
ylabel('Position z [m]');

%% Velocity Plot
figure
subplot(3,1,1)
plot(t, vel_real(:,1),'r');
hold on
plot(t,vel_est(:,1),'b')

legend('Ground Truth','Filter Estimate');

axis tight
ylim([-2 2]) 
ylabel('Velocity x [m/s]');

subplot(3,1,2)
plot(t, vel_real(:,2),'r');
hold on

plot(t,vel_est(:,2),'b')
axis tight
ylabel('Velocity y [m/s]');
ylim([-2 2]) 

subplot(3,1,3)
plot(t, vel_real(:,3),'r');
hold on

plot(t,vel_est(:,3),'b')
axis tight
ylim([-2 2]) 
xlabel('Time [s]');
ylabel('Velocity z [m/s]');


%% Bias Acc Plot
figure
subplot(3,1,1)
plot(t,bias_acc(:,4),'r')
hold on
plot(t,bias_acc(:,1),'b')

legend('Ground Truth', 'Filter Estimate');

axis tight
% ylim([-0.0 0.05]) 
ylabel('Bias x [m/s^2]');

subplot(3,1,2)
plot(t,bias_acc(:,5),'r')
hold on
plot(t,bias_acc(:,2),'b')
axis tight
ylabel('Bias y [m/s^2]');
% ylim([-0.0 0.05]) 

subplot(3,1,3)

plot(t,bias_acc(:,6),'r')
hold on
plot(t,bias_acc(:,3),'b')
axis tight
% ylim([-0.05 -0.0]) 
xlabel('Time [s]');
ylabel('Bias z [m/s^2]');
