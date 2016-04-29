%%InitPar
%% Initialising the Parameters

% Global Constants
R =287; % gas constant [J/kg K]
kappa = 1.35; % (Cp/Cv) 
lambda = 1; % air fuel ratio (assumed constant, = 1 )
sigma0 = 14.67; % stochiometric air/fuel ratio for gasoline
T_a = 290;
T_m = 320;
p_a = 1e5;
p_e = 1.05e5;


% Throttle
A_throttle = 0.015^2*pi ; %Throttle pipe area (assume pipe diameter of 3cm) [m^2]
alpha0 = 1.5568e-6; % Found with polyfit (see IdleSpeedControl.m)
alpha1=6.1744e-6;   % found with polyfit (see IdleSpeedControl.m)

% Manifold
Vm = 0.0057;
% Engine Mass Flow

Vd = 2.48e-3; % Engine Displacement Volume, 5 cylinder 2.5L engine [m^3]
Vc = 2.48e-4; % Compression Volume

gamma0 = 0.5313; % volumetric efficiency parameters, Found with polyfit (see IdleSpeedControl.m)
gamma1 = 0.0029; %(used in Engine Air Mass Flow subsystem) Found with polyfit (see IdleSpeedControl.m)      

% Engine Torque Generation (Willans Aproach)

we_idle = 90; % engine speed [rad/s]  ~900rpm
Pf = 1.5e3; % Friction power assumed to be 2HP [W]
beta0 = Pf/we_idle; %Used in Engine torque generation (Approximation) [J/rad]

eta0 = 0.3676;
eta1 = -1.3443e-4;

Hl = 42.5e6 ; %Lower heating value of mass burnt by combustion

tau_inj_ref = 0.072751; %Used in Engine torque generation
we_tau_inj_ref = we_idle; %Used in Engine torque generation                

% Engine Speed
inertia_e = 0.23; %Rotational inertia of the engine [m^2 Kg] page 84 script

% Load Dynamics
U_bat = 14; % alternator voltage
I_gen = 15; % alternator current
eta_gen = 0.7; %Generator's efficiency (assumed to be constant)

sim_opt     = simset( 'SrcWorkspace','current',...
    'FixedStep', 1e-3); 
