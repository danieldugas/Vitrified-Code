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