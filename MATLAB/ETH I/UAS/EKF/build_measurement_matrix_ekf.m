function [ H ] = build_measurement_matrix_ekf()
O = zeros(3);
% TODO: Calculate measurement matrix
H = [eye(3), O, O];

end