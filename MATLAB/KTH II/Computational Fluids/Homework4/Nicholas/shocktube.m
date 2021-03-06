%        Barotropic shock tube solved by McCormack scheme
%        ------------------------------------------------
% At t = 0:
%  	Rho |
%       +---------
%       |         |
%       |         |
%       |          --------
%       +---------+--------+--> X
%       0        L/2       L
% Solve the Riemann problem. At both ends, the tube is closed
% so u = 0. Equations:
%     d(rho)/dt + d(rho*u)dx = 0
%     d(rho*u)/dt + d(rho*u*u + p)/dx = 0
% 
% ==============================================================
% !!!!!!!!!!  READ THIS BEFORE YOU START !!!!!!!!!!!!!!!!!!!!!!!
%  
% 
% U in the program is the matrix of conservative varibles: rho and
% rho*u stored as columns in the matrix U
%     
%       rho_1     (rho*u)_1
%       rho_2     (rho*u)_2
% U =    .           .
%        .           .
%       rho_n     (rho*u)_n 
% 
% 
% rho = U(:,1)  and  u=U(:,2)./rho
%
% rho: Density
% u  : Velocity
% p  : Pressure
% Kentr = K (in the homework)

global X DX
global GAMMA R P0 RHO0 P1 RHO1 Kentr;
global C2 C0;
% Constants
GAMMA  = 1.4;  
R      = 288.7;   % 8.314 J/0K.mol = 8.314 *1000/(0.8*28 + 0.2*32) =
                  % 288.7 Joule/kg/oK

% Compute initial state:
% Left conditions:

P0      = 1e5;                   % Pascal
T0      = 300;                   % oK
RHO0    = P0/T0/R;               % km/m^3
Kentr   = P0/RHO0^GAMMA;         % Isentropic relation

% Right conditions:

P1      = 1e4;                   % Atmosphere 
RHO1    = (P1/Kentr)^(1/GAMMA);  % Isentropic relation

% Tube geometry:

L  = 3;

% Grid:

n  = 81;
X  = linspace(0,L,n);
DX = X(2)-X(1);

% Initial conditions:

U      = zeros(n,2);             % allocate U, u zero 
U(:,1) = RHO0*ones(n,1);         % density left
I      = find(X > L/2);          % right half of tube
U(I,1) = RHO1*ones(length(I),1); % density right
T(1) = 0; %starttime

% Artificial viscosity, try other parameter values

C2 = 0.9; C0 = 0.2; 

%  Courant number, try with different
CN = 0.74;

nstp   = 50;                     % number of timesteps
 
for k = 1:nstp,

  
  %%% !!!Here you have to add something to determine the timestep!!!! 
  
  % 0) Actualise the variables
      rho = U(:,1);
      u   = U(:,2)./rho;
  
  % 1) Find the speed of sound
      c = sqrt(Kentr*GAMMA*rho.^(GAMMA-1));
  
  % 2) Compute vmax, maximal, absolute value of the characteristic speeds
      vmax = max(max(abs(u+c)),max(abs(u-c)));
 
  % 3) Determine the time step using the CFL condition 
  %    and the Courant number, CN
      dt = CN*DX./vmax;
      if k>1
        T(k)=T(k-1)+dt;
      end
  
  
  
  
  [U,SP,Shock_velocity] = mac_cormack(U,dt);      % Use MacCormack to update the solution
  
  % Decode the variables:
  rho  = U(:,1);
  u    = U(:,2)./rho; 
  p    = Kentr*rho.^GAMMA;
  Shock_pos(k)=SP;
  Shock_speed_analytical(k)=Shock_velocity;
  
  
  % Plot 
  if k == 1           % plot initial condition
    figure(2)
    subplot(311)
    plot(X,rho,'r'), title('Density')
    subplot(312),plot(X,u,'b'), title('Velocity')
    subplot(313),plot(X,p,'m'), title('Pressure') 
  end;
  % Look at the density, 
  % pressure and velocity
   if rem(k,4) == 0,           % plot every fourth time step 
    figure(5)
    subplot(311)
    plot(X,rho,'r'), title('Density')
    subplot(312),plot(X,u,'b'), title('Velocity')
    subplot(313),plot(X,p,'m'), title('Pressure') 
    drawnow;
  end;
end;
for i=2:nstp;
    Shock_speed(i)=(Shock_pos(i)-Shock_pos(i-1))/(T(i)-T(i-1));
end
Average_shock_speed_numm=mean(Shock_speed)
Average_shock_speed_ana=mean(Shock_speed_analytical)
% plot the solution at the end time
if ~exist('old_Shock_pos','var')
    old_Shock_pos = Shock_pos;
    old_Shock_speed = Shock_speed;
    old_Shock_speed_analytical = Shock_speed_analytical;
end
figure(10)
subplot(311), plot(X,rho(:,1),'-+'); , hold on, grid on, title('Density')
subplot(312), plot(X,u,'-+');hold on, grid on, title('Velocity')
subplot(313),plot(X,p,'-+'),grid on, title('Pressure') 
figure(11)
CNs = num2str(CN), C0s = num2str(C0), C2s = num2str(C2);
subplot(131), plot(T,[old_Shock_pos' Shock_pos']), legend('previous', 'current'),title({['Shock position - CN = ' CNs ', C0 = ' C0s ', C2 = ' C2s]}), xlim([0 T(nstp)]), xlabel('Time [s]'), ylabel('Distance [m]')
subplot(132),plot(T,[old_Shock_speed' Shock_speed']), legend('previous', 'current'),title('Shock speed'), ylim([-1000 1000]), xlim([0 T(nstp)]), xlabel('Time [s]'), ylabel('Speed [m/s]')
subplot(133),plot(T,[old_Shock_speed_analytical' Shock_speed_analytical']), legend('previous', 'current'),title('Shock speed analytical'), ylim([-1000 1000]), xlim([0 T(nstp)]), xlabel('Time [s]')

old_Shock_pos = Shock_pos;
old_Shock_speed = Shock_speed;
old_Shock_speed_analytical = Shock_speed_analytical;