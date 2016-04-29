load('identification_data.mat')

InitPar; %initialises constants for the simulink

X = fminsearch(@(x) simulation_error(x, Data),[0.0062195,0.025964]);

Vm = X(1) %we find 0.00094472
Vdot_ideal = X(2)  %we find 0.025351


% Better: use least-squares to find the best nominal mdot_in
mdot_in_nominal = 0.01;

%simulate and get nominal states
[t,x,y] = sim('manifold',[0 20],[],[num2str(mdot_in_nominal),'+0*t']);
x_nominal = x(end,:) ;

%feed nominal values to linmod, linearize the model
linear_manifold = linmod('manifold',x_nominal, mdot_in_nominal) ;

T_x = diag(x_nominal) ;
T_y = diag(x_nominal(1)) ;
T_u = diag(mdot_in_nominal) ;

A_n = T_x^-1 * linear_manifold.a * T_x ;
B_n = T_x^-1 * linear_manifold.b * T_u ;
C_n = T_y^-1 * linear_manifold.c * T_x ;
D_n = T_y^-1 * linear_manifold.d * T_u ;

sim('linearized_manifold')

plot(linearized_comparison)
legend('non linear model','linear model')
title('P')


