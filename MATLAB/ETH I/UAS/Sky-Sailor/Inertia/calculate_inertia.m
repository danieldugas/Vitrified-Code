%===============================================================
%=== CALCULATE INERTIA Version 1.00
%===
%=== This function calculates the Ixx, Iyy & Izz inertias of
%=== a parallelepiped align with x,y,z axis or a cylinder
%=== aligned along x axis (sufficient for the first application, 
%=== could be extended...).
%===
%=== Input:     part        structure containing:
%===            part.cog    center of gravity of the part
%===            part.lx     length (cyl & paral)
%===            part.ly     width (paral only)
%===            part.lz     height (paral only)
%===            part.d      diameter (cyl only)
%===            cog         Total center of gravity of airplane
%===
%=== Output:    Ixx,Iyy,Izz Inertias
%===
%=== Author: André Noth
%===         andre.noth@epfl.ch
%===         http://aero.epfl.ch
%===
%===============================================================


function [Ixx,Iyy,Izz]=calculate_inertia(part,cog)

if(isempty(part.d))                                         % Its not a cylinder... -> parallelepiped
    Ixx     = 1/12 * part.m * (part.ly^2 + part.lz^2);     	% Ixx inertia of the part w.r.t its center of gravity
    Iyy     = 1/12 * part.m * (part.lx^2 + part.lz^2);    	% Iyy inertia of the part w.r.t its center of gravity
    Izz     = 1/12 * part.m * (part.lx^2 + part.ly^2);      % Izz inertia of the part w.r.t its center of gravity
else                                                        % It must be a parallelepiped
    Ixx     = 1/2  * part.m * (part.d/2)^2;                 % Ixx inertia of the part w.r.t its center of gravity
    Iyy     = 1/12 * part.m * (3*(part.d/2)^2 + part.lx^2); % Iyy inertia of the part w.r.t its center of gravity
    Izz     = Iyy;                                          % Izz inertia of the part w.r.t its center of gravity
end

Ixx     =   Ixx + part.m * ( (part.cog(2)-cog(2))^2 + (part.cog(3)-cog(3))^2 );     % We complete the inertia according the...
Iyy     =   Iyy + part.m * ( (part.cog(1)-cog(1))^2 + (part.cog(3)-cog(3))^2 );     % ...cog of the airplane
Izz     =   Izz + part.m * ( (part.cog(2)-cog(2))^2 + (part.cog(3)-cog(3))^2 );

end