%% Intake Manifold - 
% Parameters

Cp = 1003;          % J/kgK
Cv = 717;             % J/kgK
kappa = Cp/Cv; 
epsilon = 10;           %Compression ratio
Pe = 8.36 * 10^4;            %[Pa] Pressure of the exhaust gas remaining in the cylinder
R = 287;                % [J/kgK]
theta_e = 400;          % [K] temperature of the exhaust gas remaining in the cylinder
theta_in = 295 ;         % [K] temperature of the air entering the intake manifold


%% Parameters to check the model
% Vm and Vdot_ideal are initialised in simulation_error(), mdot in is found
% in the reference data. just in case:
Vm = 0.0063815;
Vdot_ideal = 0.024546;