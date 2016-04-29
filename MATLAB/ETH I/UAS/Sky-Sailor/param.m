
%================
%== Airplane data
%================

M       =   2.6;                                        % Total mass of the airplane
Ixx      =   0.6335;                                     % Inertia along x axis
Iyy      =   0.1951;                                     % Inertia along z axis
Izz      =   0.6522;                                     % Inertia along y axis


cog     =   [-0.3874 0  0.0224];  % Correct center of gravity
%cog     =   [-0.36 0  0.0224];  % Center of gravity 3cm in the front
%cog     =   [-0.42 0  0.0224];  % Center of gravity 3cm in the back

%===================
%== General constant
%===================

g       =	9.81;                                       % Gravitational acceleration
u       =   1.8e-5;                                     % Viscosity [Kg/(m*s)]

Kw      =   1.07;

L       =   3.24;

%===================
%== Environment data
%===================

rho     =   1.225;                                      % Air density [Kg/m^2]