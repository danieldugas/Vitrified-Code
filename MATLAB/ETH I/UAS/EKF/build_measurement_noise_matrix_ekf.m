function [ R ] = build_measurement_noise_matrix_ekf(sigma_pos_meas)
%BUILD_MEASUREMENT_NOISE_MATRIX Construct the measurement noise matrix R.
% TODO: Construct the measurement noise matrix R.
R = eye(3)*sigma_pos_meas;
end

