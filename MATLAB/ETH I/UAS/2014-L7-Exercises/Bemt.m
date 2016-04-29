%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% Script to calculate the thrust and drag constant using BEMT
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function [bProp,dProp] = Bemt()
%% Set the parameters
m = 0.58;
I = diag([0.0086,0.0086,0.0172]);
L = 0.225;
h = 0.007;
R = 0.08;
ThetaProp = @(x) (25-25.*(x-0.2))*pi/180;
cPropDivR = @(x) (-0.6641*(x).^2+0.6969.*x+0.0172);
Cla = 5.7;
Cd = 0;
alpha0 = -2*pi/180;
Nb = 2;
Iprop = 12*10^-6;
Rmot = 0.128;
Kmot = 1/(1900/60*2*pi);
g = 9.81;
rho = 1.2;
Vp = 0;

rDivRStart = 0.2;
NBladeElements = 20;

MotorSpeeds = (100:100:5000)*2*pi/60;


%% Linear spacing of blade elements
r = linspace(rDivRStart,1,NBladeElements+1)*R;
dr = r(2)-r(1);
rMiddle = r(1:NBladeElements)+dr/2;

%% Calculate the ratio V'p/Vt
% calculate dimensionless parameters
x = r/R; lambdav = Vp./MotorSpeeds/R; sigma = NBladeElements*cPropDivR(x)/pi ;

% Quadratic formula for lambda = V'p/Vt.
for i = 1:length(x)
    syms z;
    z = solve(z^2 + z * (sigma(i)*Cla/8 - lambdav(i) ) - sigma(i)*Cla/8 .* (ThetaProp(x(i)) - alpha0) .* x(i) , z);
    lambda(i) = double( abs(z(1)) ); %Keep positive or negative root?
    Vpprime(i,:) = lambda(i) * MotorSpeeds * R;
    phi(i,:) = Vpprime(i,:)./MotorSpeeds/r(i);
    alpha(i,:) = ThetaProp(x(i))*ones(size(MotorSpeeds)) - phi(i,:) ;
    c(i,:) = cPropDivR(x(i))*ones(size(MotorSpeeds));
    rmesh(i,:) = r(i)*ones(size(MotorSpeeds));
    
end




% Calculate the thrust and the drag element and sum them up for different
% motor speeds.

dL = rho/2 * Cla *(alpha-alpha0) .* c * dr .* (Vpprime ./ phi).^2 ;
dD = rho/2 * Cd * c * dr .* (Vpprime ./ phi).^2 ;

dT = Nb*(dL.*cos(phi)-dD.*sin(phi));
dQ = Nb*(dL.*sin(phi)-dD.*cos(phi)).*rmesh;

T = sum(dT,1);
Q = sum(dQ,1);
% Calculate the thrust and drag constant
pT = polyfit(MotorSpeeds,T,2);
pQ = polyfit(MotorSpeeds,Q,2);
bProp = pT(3) ;
dProp = pQ(3) ;

