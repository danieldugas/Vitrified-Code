% Element type: Quarter wave resonator
% --------------------------------------------------------
% Given data:
%           c:      Speed of sound (scalar) [m/s].
%           rho:    Air density (scalar) [kg/m^3].
%           Q:      Volume flow (scalar) [m^3/s].
%           L:      Length (scalar) [m].
%           S:      Cross section area (scalar) [m^2].
%           Ls:     Side branch length (scalar) [m]
%           Ss:     Side branch cross section area (scalar) [m^2].
%           xi:     Friction coefficient (scalar) [1]
%           eeta:   Loss factor (scalar) [1]
%           f:      Frequency vector (column vector) [Hz].
%----------------------------------------------------------
% Data to be calculated:
%           Pn:     Element Pressure drop (scalar) [Pa].
%           Tn:     Element Transfer matrix (3-dim matrix) 
%                   with units: tn11 [1], tn12 [kg/(s*m^4)], tn21 [(s*m^4)/kg], tn22 [1].

% Begin your code for Pn and Tn below:

w = 2*pi*f;
c_ = c*(1 + i*eeta/2);
k  = w/c;
k_ = w/c_;
Z  = rho*c/S;
Zs = -i*rho*c_ *cot(k_ *Ls);

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

clear Tn;
for n = 1:length(w)
    Tn(:,:,n) = T1(:,:,n)*T2(:,:,n)*T3(:,:,n);
end

D = 2*sqrt(S/pi);
U = Q/S;
Pf1 = xi* L/D * 1/2 * rho * U^2 ;
Pf2 = Pf1 ;
Pn = Pf1 + Pf2;