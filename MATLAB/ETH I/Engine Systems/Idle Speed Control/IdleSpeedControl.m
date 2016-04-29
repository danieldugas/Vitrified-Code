%% Idle Speed Control System
% By: Daniel Dugas, Gaspard Lhermitte, Mattis Koh

clear all
close all

%% Initialising the Parameters running InitPar.m 
InitPar
load dynamic2_extracted.mat

%% Linearization
% Reference values for linearization
omega_e_ref = 127;  
u_alpha_ref = 2.5;
u2_ref = 3.11e-5;
p_m_ref = 2.11e4;   

[A1,B1,C1,D1] = linmod('nonlinear_plant',[omega_e_ref, p_m_ref],[u_alpha_ref,u2_ref]); % Might have to also input temperature

%Pade delay with constant engine speed as input from linearization.
T_ref = 1/omega_e_ref*tau_inj_ref*we_tau_inj_ref;
[num,den] = pade(T_ref,4);
sys2 = tf(num,den);

[A,B,C,D] = ssdata(minreal(lft(tf(ss(A1,B1,C1,D1)),sys2)));

% Normalization
Tu = u_alpha_ref;
Ty = omega_e_ref; 
% Tx = diag([reference_states]);
Tx = eye(6);

An = Tx^-1*A*Tx;
Bn = Tx^-1*B*Tu;
Cn = Ty^-1*C*Tx;
Dn = Ty^-1*D*Tu;


%% Controller

Q = C'*C ;
R = 0.01 ;
K = lqr(A,B,Q,R);

Ac = A - B*K ;
Bc = B ;
Cc = C ;
Dc = D ;

states = {'one' 'two' 'three' 'four' 'five' 'six'};
inputs = {'u'};
outputs = {'y'};

sys_cl = ss(Ac,Bc,Cc,Dc,'statename',states,'inputname',inputs,'outputname',outputs);

% t = 0:0.01:5;
% u =0.2*ones(size(t));
% [y,t,x]=lsim(sys_cl,u,t);
% % hold all;
% % % plot(t,y);
% % 
% % title('Step Response with LQR Control')

% Observer
ob = obsv(sys_cl);
observability = rank(ob); % = 6 -> Observable

slowest_pole = min(abs(real(eig(Ac))));

P = [-100 -101 -102 -103 -104 -105];
L = place(A',C',P)' ;

% Controller With Integrator
gamma = 0.8;
I     = 1;
At = [A, [0;0;0;0;0;0]; -C, 0];
Bt = [B; 0];
Ct = [C, 0; [0,0,0,0,0,0], gamma*I];

Qt = Ct'*Ct;
Kt = lqr(At,Bt,Qt,R);
K  =  Kt(1:end-1);
Ki = -Kt(end);
K_tilde = [K, -Ki];

% Controller Matrices

LLDR_Ac = [A-B*K-L*C, B*Ki ; zeros(1,7)];
LLDR_Bc = [-L ; 1 ];
LLDR_Cc = [-K_tilde];
LLDR_Dc = [0];

% Discretization, Tunstin emulation
sys_c = ss(LLDR_Ac,LLDR_Bc,LLDR_Cc,LLDR_Dc);
sys_d = c2d(sys_c,0.001,'tustin');

LLDR_Ad = sys_d.A;
LLDR_Bd = sys_d.B;
LLDR_Cd = sys_d.C;
LLDR_Dd = sys_d.D;
LLDR_Ty = Ty;
LLDR_Tu = Tu;
LLDR_Tx = Tx;
LLDR_u_alphanom =  u_alpha_ref;
save('controller.mat', 'LLDR_Ad','LLDR_Bd','LLDR_Cd','LLDR_Dd',...
     'LLDR_Tu','LLDR_Tx','LLDR_Ty','LLDR_u_alphanom');
InitPar
sim('plant_controller')