function [ x_curr ] = predict_state_ekf(x_prev,q_prev,u, dt, grav)
%PROPAGATE_STATE_EKF Propagate state.

% Initialization.
p_est = x_prev(1:3);
v_est = x_prev(4:6);
q_est = q_prev;
b_a_est = x_prev(7:9);
a_m = u(1:3);

C_est = quat2rot(q_est)';

% Apply prediction model.
p_est_new = p_est + dt*v_est + dt^2/2*(C_est'*(a_m - b_a_est) + grav);
v_est_new = v_est + dt*(C_est'*(a_m - b_a_est) + grav);
b_a_est_new = b_a_est;

x_curr = zeros(9,1);
x_curr(1:3) = p_est_new;
x_curr(4:6) = v_est_new;
x_curr(7:9) = b_a_est_new;
end

