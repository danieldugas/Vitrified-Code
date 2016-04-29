function [ Q_k ] = build_process_noise_matrix_ekf(q, dt, Q_acc, Q_acc_bias)
%BUILD_PROCESS_NOISE_COVARIANCE Construct the discrete time process noise 
%covariance matrix Q.

C = quat2rot(q);

%% Building Q_k
Q_k = zeros(9);
% First row
Q_k(1:3, 1:3) = (dt^2*Q_acc + dt^4/4*Q_acc_bias);
Q_k(1:3, 4:6) = Q_acc*dt + Q_acc_bias*dt^3/2;
Q_k(1:3, 7:9) = -C'*Q_acc_bias*dt^2/2;

% Second row
Q_k(4:6, 1:3) = Q_acc*dt + Q_acc_bias*dt^3/2;
Q_k(4:6, 4:6) = Q_acc + Q_acc_bias*dt^2;
Q_k(4:6, 7:9) = -C'*dt*Q_acc_bias;

% Third row
Q_k(7:9, 1:3) = -C*Q_acc_bias*dt^2/2;
Q_k(7:9, 4:6) = -C*Q_acc_bias*dt;
Q_k(7:9, 7:9) = Q_acc_bias;

Q_k = Q_k * dt;
end

