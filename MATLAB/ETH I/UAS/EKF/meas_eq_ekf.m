function [ z_est ] = meas_eq_ekf(x_pred)
%MEAS_EQ Compute the measurement prediction.
%TODO: Compute the measurement prediction.
z_est = x_pred(1:3) % + vk ??;
end

