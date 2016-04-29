%===============================================================
%=== SKY-SAILOR MODEL Version 1.00
%===
%=== This code establish the mathematical representation of the
%=== aerodynamic model of Sky-Sailor. Using the Lagrange-Euler
%=== formalism, all the equations describing the behaviour of
%=== of the airplane are established using the SYMBOLIC TOOLBOX.
%=== These equations are then stored in the file "Model.m" and
%=== will be evaluated in "Dynamica.m" using a this point real
%=== values for speed, angular rates, .etc that were in "Model.m"
%=== still in symbolic form.
%=== 
%=== Input:         none
%=== Output:        none
%=== Graphical Out: 4 Pie plots for Masses, Ixx, Iyy, Izz repre-
%===                senting the influence of each part of the
%===                plane on these values.
%===                1 Graphical representation of the parts in
%===                3D space for visual control of parts position
%===
%=== Based on:      Smartfish_model.m created by Alex Vuillemin
%===
%=== Author: André Noth
%===         andre.noth@epfl.ch
%===         http://aero.epfl.ch
%===
%===============================================================

function out = calculate_force_and_moment(in)

global wing;
global we;
global naca0005;
global show_total_force_e;
global show_total_momentum_e;
global show_momentum_aero;
global wind_angle
global relspeed
global coeff
global cog


if ( exist('we') == 0 | isempty(we) )
    cd 'Airfoils';extract_polars;cd ..
end

if ( exist('wing') == 0 | isempty(wing) )
    wing_config;
end

relspeed_x  = 	in(1);
relspeed_y  = 	in(2);
relspeed_z  =   in(3);
relspeed    =   [relspeed_x relspeed_y relspeed_z];

omega_phi   =   in(4);
omega_theta =   in(5);
omega_psi   =   in(6);
omega       =   [omega_phi omega_theta omega_psi];

u_ail_l     =   in(7);
u_ail_r     =   in(8);
u_vtail_l   =   in(9);
u_vtail_r   =   in(10);
u_motor     =   in(11);

wing(1).ailangle =   0;
wing(2).ailangle =   0;
wing(3).ailangle =   u_ail_l;
wing(4).ailangle =   0;
wing(5).ailangle =   u_ail_r;
wing(6).ailangle =   u_vtail_l;
wing(7).ailangle =   u_vtail_r;

param;
%==============================================================
%===            EXTRACTION OF CL,CD,CM                     ====
%==============================================================


for i=1:1:length(wing)
    speed_rotation      =   cross(omega,wing(i).p_0_25-cog);                            % Wing speed due to the rotation of the airplane is calculated...
    relspeed_adjusted   =	relspeed + speed_rotation;                                  % ... and added to the airplane translation speed
    
    comp_d              =   (wing(i).ud * -relspeed_adjusted');
    comp_l              =   (wing(i).ul * -relspeed_adjusted');
    wind_angle	        =   atan2(comp_l,comp_d);
    wind_speed          =   norm(comp_d*wing(i).ud+comp_l*wing(i).ul);
    
    wing(i).wind_angle = wind_angle;
    wing(i).wind_speed = wind_speed;
    
    re(i)                  =   rho * wing(i).c * wind_speed / u;                      % The reynolds number is calculated...

    %  re(i)           =   100000;
    [CL CD CM]          =   get_CL_CD_CM(wing(i).ailangle,re(i),wind_angle,wing(i).airfoil);	% ... and the lift, drag & moment coefficients are retrieved

    eval(strcat('CL',num2str(i),' = CL;'));
    eval(strcat('CD',num2str(i),' = CD;'));
    eval(strcat('CM',num2str(i),' = CM;'));
    
end


log_total_force_e       =   [];
log_total_momentum_e    =   [];
log_momentum_aero       =   [];

%==============================================================
%===     CALCULATION OF THE AERODYNAMIC FORCES & MOMENT    ====
%==============================================================

for i=1:1:length(wing)
    
    speed_rotation          =   cross(omega,wing(i).p_0_25-cog);                        % The wind due to rotation is computed (opposite of speed -> '-')
    relspeed_adjusted       =   relspeed + speed_rotation;                              % Addition of relative wind and wind due to rotation 
         
    wind_pow2       =   wing(i).wind_speed^2;              	% Will be used for calculation of the forces

    Aspect_ratio=L/wing(i).c;   
    Ind_Drag=Kw/(pi*Aspect_ratio);  
    
    CL(i) = eval(strcat('CL',num2str(i)));
    CD(i) = eval(strcat('CD',num2str(i)))+ Ind_Drag*CL(i)^2;
    CM(i) = eval(strcat('CM',num2str(i)));
    
    lift_force  = 1/2 * rho * CL(i) * wing(i).area * wind_pow2;                             % ONLY norm of the lift force
    drag_force  = 1/2 * rho * CD(i)* wing(i).area * wind_pow2;                             % ONLY norm of the drag force


    momentum    = 1/2 * rho * CM(i) * wing(i).area * wind_pow2 * wing(i).c;     % ONLY norm of the momentum
  
    wind_angle = wing(i).wind_angle;
    
    total_force_e       =   (cos(wind_angle) * drag_force - sin(wind_angle) * lift_force) * wing(i).ud + ...
                            (sin(wind_angle) * drag_force + cos(wind_angle) * lift_force) * wing(i).ul;                        
                        
    total_momentum_e    =   momentum * wing(i).um + cross(wing(i).p_0_25-cog,total_force_e);
    

    log_total_force_e       =   [log_total_force_e,     total_force_e'];
    log_total_momentum_e    =   [log_total_momentum_e,  total_momentum_e'];
    log_momentum_aero       =   [log_momentum_aero, (momentum * wing(i).um)'];
end

%pause;
show_total_force_e      =  log_total_force_e;
show_total_momentum_e   =  log_total_momentum_e;
show_momentum_aero      =  log_momentum_aero;


out = sum([log_total_force_e;log_total_momentum_e]') + [u_motor,0,0,0,0,0];

out(7)=CL(1)/CD(1);

end