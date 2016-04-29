% Parameter file
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

rDivRStart = 0.2;
NBladeElements = 20;


omegaInit = [0;0;0];
attitudeInit = [0;0;0];
velInit = [0;0;0];
posInit = [0;0;0];
MotorSpeedInit = [300;300;300;300];