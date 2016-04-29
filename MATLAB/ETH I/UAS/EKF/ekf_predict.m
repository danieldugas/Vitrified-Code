function [x_pred, P_pred] = ekf_predict(x_prev, q_prev, u, P_prev, dt, Q_acc, Q_acc_bias, grav)
%% Propagation Step
% Predict state.
x_pred = predict_state_ekf(x_prev,q_prev,u, dt, grav);  

% Predict covariance matrix.
F = build_propagation_matrix_ekf(x_prev, q_prev, dt);  
Q = build_process_noise_matrix_ekf(q_prev, dt, Q_acc, Q_acc_bias);   

% TODO: Implement covariance prediction and return correctly predicted P
% instead of previous one.
P_pred = F*P_prev*F'+Q; 
end