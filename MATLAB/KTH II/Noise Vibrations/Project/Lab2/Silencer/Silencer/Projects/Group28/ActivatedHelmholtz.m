function [element,CodeError]=ActivatedHelmholtz(c,rho,Q,L,S,Ln,Sn,Lc,Sc,xi,r,Lr,frequency);

f=[frequency.minimum:frequency.resolution:frequency.maximum]';  % Frequency column vector
lasterr('');
lastwarn('');

try

% Element type: Helmholtz resonator
% --------------------------------------------------------
% Given data:
%           c:      Speed of sound (scalar) [m/s].
%           rho:    Air density (scalar) [kg/m^3].
%           Q:      Volume flow (scalar) [m^3/s].
%           L:      Length (scalar) [m]
%           S:      Cross section area (scalar) [m^2]
%           Ln:     Neck length (scalar) [m].
%           Sn:     Neck cross section area (scalar) [m^2].
%           Lc:     Cavity length (scalar) [m]
%           Sc:     Cavity cross section area (scalar) [m^2].
%           xi:     Friction coefficient (scalar) [1]
%           r:      Specific flow resistance (scalar) [Rayl/m]
%           Lr:     Length of the resistive material (scalar) [m]
%           f:      Frequency vector (column vector) [Hz].
%----------------------------------------------------------
% Data to be calculated:
%           Pn:     Element Pressure drop (scalar) [Pa].
%           Tn:     Element Transfer matrix (3-dim matrix)
%                   with units: tn11 [1], tn12 [kg/(s*m^4)], tn21 [(s*m^4)/kg], tn22 [1].

% Begin your code for Pn and Tn below:

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

clear Tn;
for n = 1:length(w)
    Tn(:,:,n) = T1(:,:,n)*T2(:,:,n)*T3(:,:,n);
end

D = 2*sqrt(S/pi);
U = Q/S;
Pf1 = xi* L/D * 1/2 * rho * U^2 ;
Pf2 = Pf1 ;
Pn = Pf1 + Pf2;

if not(exist('Pn'))|not(exist('Tn'))
    element=0;
    CodeError='Variable Pn and/or Tn doesn´t exist!';
else
    [Rp,Cp]=size(Pn);
    [Rt,Ct,Dt]=size(Tn);
    if (Rp~=1)|(Cp~=1)
        element=0;
        CodeError='Variable Pn must be a scalar!';
    elseif (Rt~=2)|(Ct~=2)|(Dt~=length(f))
        element=0;
        CodeError='Variable Tn must be a 3-dimensional matrix of size: [Rows,Columns,Layers]=[2,2,length(f)]';
    else
        element.type='Helmholtz';
        element.c=c;
        element.rho=rho;
        element.Q=Q;
        element.L=L;
        element.S=S;
        element.Ln=Ln;
        element.Sn=Sn;
        element.Lc=Lc;
        element.Sc=Sc;
        element.xi=xi;
        element.r=r;
        element.Lr=Lr;
        element.frequency=frequency;
        element.Pn=Pn;
        element.Tn=Tn;
        CodeError='No error';
    end %if
end %if

catch
    element=0;
    if (isempty(lasterr))&(not(isempty(lastwarn)))
        CodeError=lastwarn;
    elseif (isempty(lastwarn))&(not(isempty(lasterr)))
        CodeError=lasterr;
    elseif (not(isempty(lastwarn)))&(not(isempty(lasterr)))
        CodeError=[lasterr,' ',lastwarn];
    end
end %try-catch
