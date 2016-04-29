%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Hybrid and Embedded control systems
% Homework 1
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Initialization
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

close all;
init_tanks;
g = 9.82;
Tau = 1/alpha1*sqrt(2*tank_h10/g);
k = 60*beta*Tau;
Gamma = alpha1^2/alpha2^2;
chi=0.5;
omega0=[0.1,0.2,0.2];
zeta=[0.7,0.7,0.8];
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Continuous Control design
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
num1=k;
den1=[Tau,1];
num2=Gamma;
den2=[Gamma*Tau,1];
uppertank=tf(num1,den1); % Transfer function for upper tank
lowertank=tf(num2,den2); % Transfer function for upper tank
G=uppertank*lowertank; % Transfer function from input to lower tank level


%% CalculatePID paramaeters
for i=1:3
[K,Ti,Td,N]=polePlacePID(chi, omega0(i), zeta(i),Tau,Gamma,k);
numPID=K*[Ti+Td*N*Ti, Ti*N+1, N];
denPID=[Ti, Ti*N, 0];
[K, Ti, Td, N]; %Debug
F = tf(numPID,denPID); % Transfer function for the controller
sim('tanks');
sinf = stepinfo(simout.Data, simout.Time); 
M = (sinf.SettlingMax-50)*10;
Tss = sinf.SettlingTime-100;
Tr = sinf.RiseTime;
[Tr M Tss]; %Debug
end

margin(F*G);
[~,~,~,Wpm] = margin(F*G);



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Digital Control design
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% Question 7 
% change to Continuous Controller with ZOH before proceeding
for (j = 1:50) 
Ts = j*0.1;
sim('tanks');

sinf = stepinfo(simout.Data, simout.Time); 
M(j) = (sinf.SettlingMax-50)*10;
Tss(j) = sinf.SettlingTime-100;
Tr(j) = sinf.RiseTime;
end
x = 0.1:0.1:5;
plot(x, [M; Tss; Tr]);
xlabel('Ts');
legend('M','Setting Time','Tr');
title('Continuous Controller with ZOH');

%% Question 8
% Change to discrete controller before proceeding
for (j = 1:50) 
Ts = j*0.1; %sampling time
% Discretize the continous controller, save it in state space form
Fd =c2d(F,Ts);
[num,den] = tfdata(Fd,'v');
[Aa,Ba,Ca,Da] = tf2ss(num,den); %saving to space state form
sim('tanks');

sinf = stepinfo(simout.Data, simout.Time); 
M(j) = (sinf.SettlingMax-50)*10;
Tss(j) = sinf.SettlingTime-100;
Tr(j) = sinf.RiseTime;
end
x = 0.1:0.1:5;
plot(x, [M; Tss; Tr]);
xlabel('Ts');
legend('M','Setting Time','Tr');
title('Discrete Controller');
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Discrete Control design
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% Question 12
Ts = 4;
Gd = c2d(G,Ts); % Sampled system model
Fd =c2d(F,Ts); 

%% Question 14
[numFG,~] = tfdata((1+F*G),'v'); 

%% Question 16
a1 = 0.09162;
a2 = 0.07393;
b1 = -1.45;
b2 = 0.5254;

A = [1      a1      0       0 ;
     b1-1   a2      a1      0 ;
     b2-b1  0       a2      a1;
     -b2    0       0       a2 ];
 
 d = numFG / numFG(1);
 d0 = d(2);
 d1 = d(3);
 d2 = d(4);
 d3 = d(5);
 
 y = [d0 - b1 + 1 ;
      d1 - b2 + b1;
      d2 + b2     ;
      d3           ];

 x = A^(-1)*y;
 
 r =  x(1);
 c0 = x(2);
 c1 = x(3);
 c2 = x(4);

 Fd = tf([c0 c1 c2], [1 r-1 -r], Ts); % Transfer function for discrete designed controller
 [numFdGd,~] = tfdata((1+Fd*Gd),'v');
 roots(numFdGd)
