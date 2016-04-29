
%==========================================================================
%=== CONFIGURATION OF THE SKY-SAILOR WING
%=== André Noth 29.04.2006
%==========================================================================

param;


rot_inc_wing  = [cos(deg2rad(2.8))      0                   sin(deg2rad(2.8));          % Rotation of the incidence of 2.8° of the wing
                 0                      1                   0;                          % with respect to the tail-motor reference line
                 -sin(deg2rad(2.8))     0                   cos(deg2rad(2.8))];
             
rot_inc_vtail = [cos(deg2rad(1))        0                   sin(deg2rad(1));            % Rotation of the incidence of 1° of the wing
                 0                      1                   0;                          % with respect to the tail-motor reference line  
                 -sin(deg2rad(1))       0                   cos(deg2rad(1))];
             
rot_died_wleft = [1                      0                  0;                          % Rotation due to the Diedral angle of the wing
                 0                      cos(deg2rad(7))     -sin(deg2rad(7));
                 0                      sin(deg2rad(7))     cos(deg2rad(7))];
             
rot_died_wright= [1                      0                  0;                          % Rotation due to the Diedral angle of the wing
                 0                      cos(deg2rad(-7))    -sin(deg2rad(-7));
                 0                      sin(deg2rad(-7))    cos(deg2rad(-7))];
             
rot_died_vleft= [1                      0                   0;                          % Rotation due to the Diedral angle of the vtail
                 0                      cos(deg2rad(36))     -sin(deg2rad(36));
                 0                      sin(deg2rad(36))     cos(deg2rad(36))];
             
rot_died_vright= [1                      0                  0;                          % Rotation due to the Diedral angle of the vtail
                 0                      cos(deg2rad(-36))   -sin(deg2rad(-36));
                 0                      sin(deg2rad(-36))   cos(deg2rad(-36))];

            
wing(1).name    = 'wing center';                                % Name of the wing
wing(1).airfoil = 'we';                                         % Name of the airfoil
wing(1).p2      = [-0.285, 0.98/2, -0.018];                     % Coordinate of the 1st point of attack border
wing(1).p1      = [-0.285,-0.98/2, -0.018];                     % Coordinate of the 2nd point of attack border
wing(1).c1      = 0.25;                                         % Corde at to the   1st point of attack border
wing(1).c2      = 0.25;                                         % Corde at to the   2nd point of attack border
wing(1).alpha   = 2.8;                                          % Angle of incidence of the wing
wing(1).ul      = (rot_inc_wing * [0;0;-1])';                   % Unity vector perpendicular to the cord-border plane   (for the lift)
wing(1).ud      = (rot_inc_wing * [-1;0;0])';                   % Unity vector in the rear direction of the wing        (for the drag)
wing(1).um      = (rot_inc_wing * [0;1;0])';                   % Unity vector containing all the 1/4 point of the wing (for the moment)
wing(1).ailratio= 0;
wing(1).ailangle= 0;

wing(2).name    = 'wing left';                                  % Name of the wing
wing(2).airfoil = 'we';                                         % Name of the airfoil
wing(2).p1      = [-0.285, -0.98/2, -0.018];                    % Coordinate of the 1st point of attack border
wing(2).p2      = [-0.285, -(0.98/2 + 0.9925*(0.257)), -0.018 - 0.1219*(0.257)];      % Coordinate of the 2nd point of attack border
wing(2).c1      = 0.25;                                         % Corde at to the   1st point of attack border
wing(2).c2      = 0.25;                                         % Corde at to the   2nd point of attack border
wing(2).alpha   = 2.8;                                          % Angle of incidence of the wing
wing(2).ul      = (rot_died_wleft * rot_inc_wing * [0;0;-1])'; 	% Unity vector perpendicular to the cord-border plane   (for the lift)
wing(2).ud      = (rot_died_wleft * rot_inc_wing * [-1;0;0])'; 	% Unity vector in the rear direction of the wing        (for the drag)
wing(2).um      = (rot_died_wleft * rot_inc_wing * [0;1;0])'; 	% Unity vector containing all the 1/4 point of the wing (for the moment)
wing(2).ailratio= 0;
wing(2).ailangle= 0;

wing(3).name    = 'wing left_ail';                              % Name of the wing
wing(3).airfoil = 'we';                                         % Name of the airfoil
wing(3).p1      = [-0.285, -(0.98/2 + 0.9925*(0.257)), -0.018 - 0.1219*(0.257)]; 	% Coordinate of the 1st point of attack border
wing(3).p2      = [-0.285, -(0.98/2 + 0.9925*(0.937)), -0.018 - 0.1219*(0.937)];  	% Coordinate of the 2nd point of attack border
wing(3).c1      = 0.25;                                         % Corde at to the   1st point of attack border
wing(3).c2      = 0.25;                                         % Corde at to the   2nd point of attack border
wing(3).alpha   = 2.8;                                          % Angle of incidence of the wing
wing(3).ul      = (rot_died_wleft * rot_inc_wing * [0;0;-1])';	% Unity vector perpendicular to the cord-border plane   (for the lift)
wing(3).ud      = (rot_died_wleft * rot_inc_wing * [-1;0;0])'; 	% Unity vector in the rear direction of the wing        (for the drag)
wing(3).um      = (rot_died_wleft * rot_inc_wing * [0;1;0])';  % Unity vector containing all the 1/4 point of the wing (for the moment)
wing(3).ailratio= 0.276;
wing(3).ailangle= 0;

wing(4).name    = 'wing right';                                 % Name of the wing
wing(4).airfoil = 'we';                                         % Name of the airfoil
wing(4).p2      = [-0.285, 0.98/2, -0.018];                     % Coordinate of the 1st point of attack border
wing(4).p1      = [-0.285, (0.98/2 + 0.9925*(0.257)), -0.018 - 0.1219*(0.257)];      % Coordinate of the 2nd point of attack border
wing(4).c1      = 0.25;                                         % Corde at to the   1st point of attack border
wing(4).c2      = 0.25;                                         % Corde at to the   2nd point of attack border
wing(4).alpha   = 2.8;                                          % Angle of incidence of the wing
wing(4).ul      = (rot_died_wright * rot_inc_wing * [0;0;-1])';	% Unity vector perpendicular to the cord-border plane   (for the lift)
wing(4).ud      = (rot_died_wright * rot_inc_wing * [-1;0;0])';	% Unity vector in the rear direction of the wing        (for the drag)
wing(4).um      = (rot_died_wright * rot_inc_wing * [0;1;0])';	% Unity vector containing all the 1/4 point of the wing (for the moment)
wing(4).ailratio= 0;
wing(4).ailangle= 0;

wing(5).name    = 'wing right_ail';                             % Name of the wing
wing(5).airfoil = 'we';                                         % Name of the airfoil
wing(5).p2      = [-0.285, (0.98/2 + 0.9925*(0.257)), -0.018 - 0.1219*(0.257)];      % Coordinate of the 1st point of attack border
wing(5).p1      = [-0.285, (0.98/2 + 0.9925*(0.937)), -0.018 - 0.1219*(0.937)];      % Coordinate of the 2nd point of attack border
wing(5).c1      = 0.25;                                         % Corde at to the   1st point of attack border
wing(5).c2      = 0.25;                                         % Corde at to the   2nd point of attack border
wing(5).alpha   = 2.8;                                          % Angle of incidence of the wing
wing(5).ul      = (rot_died_wright * rot_inc_wing * [0;0;-1])';	% Unity vector perpendicular to the cord-border plane   (for the lift)
wing(5).ud      = (rot_died_wright * rot_inc_wing * [-1;0;0])';	% Unity vector in the rear direction of the wing        (for the drag)
wing(5).um      = (rot_died_wright * rot_inc_wing * [0;1;0])';	% Unity vector containing all the 1/4 point of the wing (for the moment)
wing(5).ailratio= 0.276;
wing(5).ailangle= 0;

wing(6).name    = 'vtail left';                                 % Name of the wing
wing(6).airfoil = 'naca0005';                                   % Name of the airfoil
wing(6).p1      = [-1.60, 0,        0];                         % Coordinate of the 1st point of attack border
wing(6).p2      = [-1.62, -0.81*.42, -0.59*.42];                % Coordinate of the 2nd point of attack border
wing(6).c1      = 0.12;                                         % Corde at to the   1st point of attack border
wing(6).c2      = 0.10;                                         % Corde at to the   2nd point of attack border
wing(6).alpha   = 0;                                            % Angle of incidence of the wing
wing(6).ul      = (rot_died_vleft * rot_inc_vtail * [0;0;-1])';	% Unity vector perpendicular to the cord-border plane   (for the lift)
wing(6).ud      = (rot_died_vleft * rot_inc_vtail * [-1;0;0])';	% Unity vector in the rear direction of the wing        (for the drag)
wing(6).um      = (rot_died_vleft * rot_inc_vtail * [0;1;0])'; % Unity vector containing all the 1/4 point of the wing (for the moment)
wing(6).ailratio= 0.33;
wing(6).ailangle= 0;

wing(7).name    = 'vtail right';                                % Name of the wing
wing(7).airfoil = 'naca0005';                                   % Name of the airfoil
wing(7).p2      = [-1.60, 0,        0];                         % Coordinate of the 1st point of attack border
wing(7).p1      = [-1.62, 0.81*.42, -0.59*.42];	                % Coordinate of the 2nd point of attack border
wing(7).c1      = 0.12;                                         % Corde at to the   1st point of attack border
wing(7).c2      = 0.10;                                         % Corde at to the   2nd point of attack border
wing(7).alpha   = 0;                                            % Angle of incidence of the wing
wing(7).ul      = (rot_died_vright * rot_inc_vtail * [0;0;-1])';% Unity vector perpendicular to the cord-border plane   (for the lift)
wing(7).ud      = (rot_died_vright * rot_inc_vtail * [-1;0;0])';% Unity vector in the rear direction of the wing        (for the drag)
wing(7).um      = (rot_died_vright * rot_inc_vtail * [0;1;0])';% Unity vector containing all the 1/4 point of the wing (for the moment)
wing(7).ailratio= 0.33;
wing(7).ailangle= 0;

% BE CAREFUL OF THE ORDER OF P1 AND P1 (FOR VISUALIZATION)

%============
%=== Fuselage
%============

misc(1).name    = 'fuselage';                               % Name of the part
misc(1).lx      = 0.820;                                   	% Length in x direction [m]
misc(1).ly      = 0.05;                                    	% Length in y direction [m]
misc(1).lz      = 0.095;                                 	% Length in z direction (Width) [m]
misc(1).cog     = [-(0.82/2) ...                         	% Centre of gravity [m m m]
                    0 ...
                    +0.03]-cog;
                
%=============
%=== Tail Unit
%=============

misc(2).name    = 'tail unit';                              % Name of the part
misc(2).ly      = (0.027+0.017)/2;                          % Mass of the Tail Unit [Kg]
misc(2).lx      = 0.900;                                   	% Length in x direction [m]
misc(2).lz       = (0.027+0.017)/2;                        	% Diameter of cylinder [m]
misc(2).cog   	= [-(0.82+0.9/2) ...                      	% Centre of gravity [m m m]
                    0 ...
                    0]-cog;                


for i=1:1:length(wing)
    p1              = wing(i).p1;                                       % Coordinate of the 1st point of attack border
    p2              = wing(i).p2;                                       % Coordinate of the 2nd point of attack border
    c1              = wing(i).c1;                                       % Corde at to the   1st point of attack border    
    c2              = wing(i).c2;                                       % Corde at to the   2st point of attack border
    ailratio        = wing(i).ailratio;
    wing(i).area    = abs(p2(2)-p1(2))*(c1+c2)/2;                       % Total area of the wing
    error           = 10000;                                            
    for j=0:0.001:1                                                     % We will calculate the COG of the wing...
        p           = p1 * j + p2 * (1-j);                              % ... by finding the point p for which...
        c           = c1 * j + c2 * (1-j);                              % ... the related corde divide exactly...
        area        = abs((p(2)-p1(2))*(c1 + c)/2);                     % ... the wing in two parts of equal...
        if (abs(wing(i).area-2*area)) < error                           % ... surface.
            wing(i).p     = p;                                          % Having this point, we store it in the structure...
            wing(i).c     = c;                                          % ... and also the corde at this point
            error = abs(wing(i).area-2*area);  
        end
    end
    wing(i).p_0_25  = wing(i).p - 1/4 * [wing(i).c 0 0];        % 1/4 of the wing where forces and momentum are considered to be
    wing(i).vert = [p1;p2;p2-c2*[1-ailratio 0 0];p1-c1*[1-ailratio 0 0]] - [cog;cog;cog;cog];
    wing(i).ail  = [p1-c1*[1-ailratio 0 0];p2-c2*[1-ailratio 0 0];p2-c2*[1 0 0];p1-c1*[1 0 0]] - [cog;cog;cog;cog];
    wing(i).conn = [1, 2, 3, 4];
    wing(i).dir  = [p2-c2*[1-ailratio 0 0];p1-c1*[1-ailratio 0 0]] - [cog;cog];
end



