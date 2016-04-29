function [x_upd, P_upd, r]=ekf_update(x_pred, P_pred, z, sigma_pos_meas)

%% TODO Remove this once the update is implemented.
x_upd = x_pred;
P_upd = P_pred;
r = 0;

%% Measurement Step

% Compute measurement matrix.
H = build_measurement_matrix_ekf();

% Compute residual.
z_pred = meas_eq_ekf(x_pred);
%% TODO: Implement the residual calculation.
% r = ...; 

% Compute covariance of residual.
R = build_measurement_noise_matrix_ekf(sigma_pos_meas);
%% TODO: Implement the residual covariance calculation.
% S = ...;

% Compute Kalman gain.
%% TODO: Implement the Kalman gain calculation.
% K = ...;

% Compute correction.
%% TODO: Implement the Kalman gain calculation.
% dx_corr = ...;

% Update state vector.
%% TODO: Update the state vector.
% x_upd = ...; 

% Update covariance matrix.
%% TODO: Update the covariance.
% P_upd = ...;
end