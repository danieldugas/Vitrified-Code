f = 0:1000; %Frequency range
%% ----- PIPE ------

rho = 1;
c = 1;
S = 1;
L = 1;

w = 2*pi*f;
k = w/c;
Z = rho*c/S;

clear T;
T(1,1,:) = cos(k*L);
T(1,2,:) = i*Z*sin(k*L);
T(2,1,:) = i*sin(k*L)/Z;
T(2,2,:) = cos(k*L);

D = 2*sqrt(S/pi);
U = Q/S;
Pf = xi* L/D * 1/2 * rho * U^2 ;
Pn = Pf;

%% ----- QUARTER-WAVE ------

rho = 1;
eeta = 1;
c = 1;
S = 1;
L = 1;
Ss = 1;
Ls = 1;

w = 2*pi*f;
c_ = c*(1 + i*eeta/2);
k  = w/c;
k_ = w/c_;
Z  = rho*c/S;
Zs = -i*rho*c_*cot(k_*Ls);

clear T1;
T1(1,1,:) = cos(k*L);
T1(1,2,:) = i*Z*sin(k*L);
T1(2,1,:) = i*sin(k*L)/Z;
T1(2,2,:) = cos(k*L);

clear T2;
T2(1,1,:) = w*0 + 1;
T2(1,2,:) = w*0;
T2(2,1,:) = Ss./Zs; %COULD BE ACTUAL INVERSE VECTOR
T2(2,2,:) = w*0 + 1;

clear T3;
T3 = T1;

clear T;
for n = 1:length(w)
    Tn(:,:,n) = T1(:,:,n)*T2(:,:,n)*T3(:,:,n);
end

D = 2*sqrt(S/pi);
U = Q/S;
Pf1 = xi* L/D * 1/2 * rho * U^2 ;
Pf2 = Pf1 ;
Pn = Pf1 + Pf2;

%% ----- HELMHOLTZRESONATOR ------

rho = 1;
c = 1;
S = 1;
L = 1;
Sn = 1;
Ln = 1;
Sc = 1;
Lc = 1;
Lr = 1;
r  = 1;


w = 2*pi*f;
k  = w/c;
R = r*Lr;
Z  = rho*c/S;
Zs = R + i*w*rho*Ln + rho*c*c*Sn./(i*w*Lc*Sc);

clear T1;
T1(1,1,:) = cos(k*L);
T1(1,2,:) = i*Z*sin(k*L);
T1(2,1,:) = i*sin(k*L)/Z;
T1(2,2,:) = cos(k*L);

clear T2;
T2(1,1,:) = w*0 + 1;
T2(1,2,:) = w*0;
T2(2,1,:) = Sn./Zs; %COULD BE ACTUAL INVERSE VECTOR
T2(2,2,:) = w*0 + 1;

clear T3;
T3 = T1;

clear T;
for n = 1:length(w)
    Tn(:,:,n) = T1(:,:,n)*T2(:,:,n)*T3(:,:,n);
end
D = 2*sqrt(S/pi);
U = Q/S;
Pf1 = xi* L/D * 1/2 * rho * U^2 ;
Pf2 = Pf1 ;
Pn = Pf1 + Pf2;

%% ----- EXPANSION CHAMBER ------

rho = 1;
c = 1;
S = 1;
L = 1;
Sc = 1;
Lc = 1;

w = 2*pi*f;
k = w/c;
Z = rho*c/S;
Zc = rho*c/Sc;

clear T1;
T1(1,1,:) = cos(k*L);
T1(1,2,:) = i*Z*sin(k*L);
T1(2,1,:) = i*sin(k*L)/Z;
T1(2,2,:) = cos(k*L);

clear T2;
T2(1,1,:) = cos(k*Lc);
T2(1,2,:) = i*Zc*sin(k*Lc);
T2(2,1,:) = i*sin(k*Lc)/Zc;
T2(2,2,:) = cos(k*Lc);

clear T3;
T3 = T1;

clear T;
for n = 1:length(w)
    Tn(:,:,n) = T1(:,:,n)*T2(:,:,n)*T3(:,:,n);
end

D = 2*sqrt(S/pi);
Dc = 2*sqrt(Sc/pi);
U = Q/S;
Uc = Q/Sc;
Pf1 = xi* L/D * 1/2 * rho * U^2 ;
Pf2 = xi* Lc/Dc * 1/2 * rho * Uc^2 ;
Pf3 = Pf1;

xis = ( 1 - S/Sc )^2 ;
Ps = xis * 1/2 * rho * U^2 ;
Pn = Pf1 + Pf2 + Pf3 + Ps;
