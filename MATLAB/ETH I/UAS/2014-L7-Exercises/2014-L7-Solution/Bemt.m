%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% Script to calculate the thrust and drag constant using BEMT
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function [bProp,dProp] = Bemt()
%% Set the parameters
parameters
MotorSpeeds = (0:100:50000)*2*pi/60;
%ThetaProp = @(x) 13.5*pi/180;
%cPropDivR = @(x) (0.015);

%% Linear spacing of blade elements
r = linspace(rDivRStart,1,NBladeElements+1)*R;
dr = r(2)-r(1);
rMiddle = r(1:NBladeElements)+dr/2;

%% Calculate the ratio V'p/Vt
% calculate dimensionless parameters
x = rMiddle/R;
lambdaV = 0;
sigma = Nb*cPropDivR(x)/pi;
% Quadratic formula for lambda. Take the positive solution of
% a*lambda^2 + b*lambda + c
a = 1;
b = (sigma*Cla/8-lambdaV);
c = -sigma*Cla/8.*(ThetaProp(x)-alpha0).*x;
lambda = (-b+sqrt(b.^2-4*a*c))/(2*a);
inflowAngle = tan(lambda./x);
% Calculate the thrust and the drag element and sum them up for different
% motor speeds.
T = zeros(size(MotorSpeeds));
Q = zeros(size(MotorSpeeds));
for i = 1:length(MotorSpeeds)
    dT = (Nb*rho/2*Cla*(ThetaProp(x)-inflowAngle-alpha0).*cPropDivR(x)*R*dr.*(MotorSpeeds(i).*rMiddle).^2).*cos(inflowAngle);
    dQ = (Nb*rho/2*Cla*(ThetaProp(x)-inflowAngle-alpha0).*cPropDivR(x)*R*dr.*(MotorSpeeds(i).*rMiddle).^2).*sin(inflowAngle).*rMiddle;
    T(i) = sum(dT);
    Q(i) = sum(dQ);
end
Tpar = polyfit(MotorSpeeds,T,2);
display(['## The thrust constant is ', num2str(Tpar(1)), 'found with the BEMT theory']);
Qpar = polyfit(MotorSpeeds,Q,2);
display(['## The drag constant is ', num2str(Qpar(1)), 'found with the BEMT theory']);
bProp = Tpar(1); 
dProp = Qpar(1);
Tfit = Tpar(1).*MotorSpeeds.^2;
Qfit = Qpar(1).*MotorSpeeds.^2;
figure(1)
subplot(2,1,1)
plot(MotorSpeeds,T,'xk');
hold on
plot(MotorSpeeds,Tfit)
subplot(2,1,2)
plot(MotorSpeeds,Q,'xk');
hold on
plot(MotorSpeeds,Qfit)

%% Plot the induced velocity distribution and the lift distribution
mSpeed = 2500*2*pi/60;
uInd = lambda*mSpeed*R;
dT = Nb*rho/2*Cla*(ThetaProp(x)-lambda-alpha0).*cPropDivR(x)*R*dr.*(mSpeed.*rMiddle).^2;
figure(2)
subplot(2,1,1)
plot(rMiddle,uInd,'b')
xlabel('Blade radius r');
ylabel('Induced velocity');
hold on
subplot(2,1,2)
plot(rMiddle,dT,'b')
xlabel('Blade radius r');
ylabel('Thrust');
hold on

