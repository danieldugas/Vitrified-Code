function [ F_d ] = build_propagation_matrix_ekf(x, q, dt)
    C = quat2rot(q);
    O = zeros(3);
    
    % TODO: Calculate continuous time Jacobian.
    F_c = [O, eye(3), O;...
           O, O,      C;...
           O, O,      O];
    
    % TODO: Calculate discrete time Jacobian.
    F_d = expm(F_c*dt) ; 
end