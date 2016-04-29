function [element,CodeError]=ActivatedPipe(c,rho,Q,L,S,xi,frequency);

f=[frequency.minimum:frequency.resolution:frequency.maximum]';  % Frequency column vector
lasterr('');
lastwarn('');

try

% Element type: Pipe
% --------------------------------------------------------
% Given data:
%           c:      Speed of sound (scalar) [m/s].
%           rho:    Air density (scalar) [kg/m^3].
%           Q:      Volume flow (scalar) [m^3/s].
%           L:      Pipe length (scalar) [m].
%           S:      Cross section area (scalar) [m^2].
%           xi:     Friction coefficient (scalar) [1]
%           f:      Frequency vector (column vector) [Hz].
%----------------------------------------------------------
% Data to be calculated:
%           Pn:     Element Pressure drop (scalar) [Pa].
%           Tn:     Element Transfer matrix (3-dim matrix)
%                   with units: tn11 [1], tn12 [kg/(s*m^4)], tn21 [(s*m^4)/kg], tn22 [1].

% Begin your code for Pn and Tn below:

w = 2*pi*f;
k = w/c;
Z = rho*c/S;

clear Tn;
Tn(1,1,:) = cos(k*L);
Tn(1,2,:) = i*Z*sin(k*L);
Tn(2,1,:) = i*sin(k*L)/Z;
Tn(2,2,:) = cos(k*L);


D = 2*sqrt(S/pi);
U = Q/S;
Pf = xi* L/D * 1/2 * rho * U^2 ;
Pn = Pf;

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
        element.type='Pipe';
        element.c=c;
        element.rho=rho;
        element.Q=Q;
        element.L=L;
        element.S=S;
        element.xi=xi;
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
