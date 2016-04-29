%===============================================================
%=== SKY-SAILOR INERTIA Version 1.00
%===
%=== This code calculate the Center of Gravity and the Inertias
%=== of Sky-Sailor. It is done in a modular way in the sense
%=== that all the components present on the airplane are listed
%=== with their:  - Weight
%===              - Dimension
%===              - Center of gravity
%=== Each part is an element of the structure "part".
%=== The position of the center of gravity for the parts is given
%=== with respect to the point where the axis of the propeller is
%=== attached to the motor.
%===
%=== The weight, dimension and geometric position are taken from
%=== the following documents:
%===     "13 - Pitot Tube Einbau Ubersicht- 1 (17.04.2005).pdf"
%===     "StarTiger III - Briging phase report - Final draft.pdf"
%===
%=== Input:         none
%=== Output:        none
%=== Graphical Out: 4 Pie plots for Masses, Ixx, Iyy, Izz repre-
%===                senting the influence of each part of the
%===                plane on these values.
%===                1 Graphical representation of the parts in
%===                3D space for visual control of parts position
%===
%=== Author: André Noth
%===         andre.noth@epfl.ch
%===         http://aero.epfl.ch
%===
%===============================================================


%===============================================================
%=== WEIGHT, DIMENSION & COG POSITION OF ALL PARTS
%===============================================================

close('all')                                                % Close all opened figure;

%=========
%=== Wings
%=========

part(1).name    = 'wing left';                              % Name of the part
part(1).m       = 0.250;                                    % Mass of the left wing [Kg]
part(1).lx      = 0.25;                                  	% Length in x direction [m]
part(1).ly      = 0.930;                                 	% Length in y direction [m]
part(1).lz      = 0.018;                                	% Length in z direction (Width) [m]
part(1).cog     = [-(0.285+0.078) ...                      	% Centre of gravity [m m m]
    (0.98+0.93)/2 ...
    (0.095/2 + 0.937/2 * sin(7/180*pi))];

part(2).name    = 'wing center';                            % Name of the part
part(2).m       = 0.306;                                  	% Mass of the left wing [Kg]
part(2).lx      = 0.25;                                   	% Length in x direction [m]
part(2).ly      = 0.980;                                  	% Length in y direction [m]
part(2).lz      = 0.018;                                  	% Length in z direction (Width) [m]
part(2).cog     = [-(0.285+0.078) ...                      	% Centre of gravity [m m m]
    0 ...
    (0.095/2 + 0.018/2)];

part(3).name    = 'wing right';                             % Name of the part
part(3).m       = 0.253;                                 	% Mass of the right wing [kg]
part(3).lx      = 0.25;                                    	% Length in x direction [m]
part(3).ly      = 0.930;                                 	% Length in y direction [m]
part(3).lz      = 0.018;                                    % Length in z direction (Width) [m]
part(3).cog     = [-(0.285+0.078) ...                     	% Centre of gravity [m m m]
    -(0.98+0.93)/2 ...
    (0.095/2 + 0.937/2 * sin(7/180*pi))];

%=========
%=== Vtail
%=========

part(4).name    = 'vtail left';                             % Name of the part
part(4).m       = 0.029;                                 	% Mass of the left Vtail [Kg]
part(4).lx      = (0.154 + 0.144) / 2 / sin(54/180*pi);    	% Length in x direction [m]
part(4).ly      = 0.68 / 2;                                	% Length in y direction [m]
part(4).lz      = 0.006;                                    % Length in z direction (Width) [m]
part(4).cog     = [-(0.820+0.900) ...                    	% Centre of gravity [m m m]
    sin(54/180*pi)*0.42/4 ...
    cos(54/180*pi)*0.42/4];

part(5).name    = 'vtail right';                            % Name of the part
part(5).m       = 0.029;                                  	% Mass of the left Vtail [Kg]
part(5).lx      = (0.154 + 0.144) / 2 / sin(54/180*pi);    	% Length in x direction [m]
part(5).ly      = 0.68 / 2;                                	% Length in y direction [m]
part(5).lz      = 0.006;                                  	% Length in z direction (Width) [m]
part(5).cog     = [-(0.820+0.900) ...                       % Centre of gravity [m m m]
    -sin(54/180*pi)*0.42/4 ...
    cos(54/180*pi)*0.42/4];

%============
%=== Winglets
%============

part(6).name    = 'wglet left';                             % Name of the part
part(6).m       = 0.022;                                  	% Mass of the left Winglet [Kg]
part(6).lx      = 0.25;                                    	% Length in x direction [m]
part(6).ly      = 0.2;                                    	% Length in y direction [m]
part(6).lz      = 0.018;                                  	% Length in z direction (Width) [m]
part(6).cog     = [-(0.285+0.078) ...                       % Centre of gravity [m m m]
    (0.98/2+0.937+0.1) ...
    (0.095/2 + (0.937+0.1) * sin(7/180*pi))];

part(7).name    = 'wglet right';                            % Name of the part
part(7).m       = 0.022;                                  	% Mass of the left Winglet [Kg]
part(7).lx      = 0.25;                                    	% Length in x direction [m]
part(7).ly      = 0.2;                                     	% Length in y direction [m]
part(7).lz      = 0.018;                                   	% Length in z direction (Width) [m]
part(7).cog     = [-(0.285+0.078) ...                    	% Centre of gravity [m m m]
    -(0.98/2+0.937+0.1) ...
    (0.095/2 + (0.937+0.1) * sin(7/180*pi))];

%============
%=== Fuselage
%============

part(8).name    = 'fuselage';                               % Name of the part
part(8).m       = 0.088;                                  	% Mass of the Fuselage [Kg]
part(8).lx      = 0.820;                                   	% Length in x direction [m]
part(8).ly      = 0.05;                                    	% Length in y direction [m]
part(8).lz      = 0.095;                                 	% Length in z direction (Width) [m]
part(8).cog     = [-(0.82/2) ...                         	% Centre of gravity [m m m]
    0 ...
    0];

%=============
%=== Tail Unit
%=============

part(9).name    = 'tail unit';                              % Name of the part
part(9).m       = 0.025;                                 	% Mass of the Tail Unit [Kg]
part(9).lx      = 0.900;                                   	% Length in x direction [m]
part(9).d       = (0.027+0.017)/2;                        	% Diameter of cylinder [m]
part(9).cog   	= [-(0.82+0.9/2) ...                      	% Centre of gravity [m m m]
    0 ...
    0];

%===========
%=== Battery
%===========

part(10).name   = 'battery';                                % Name of the part
part(10).m      = 1.150;                                 	% Mass of the Battery [Kg]
part(10).lx     = 0.308;                                  	% Length in x direction [m]
part(10).ly     = 0.038;                                  	% Length in y direction [m]
part(10).lz     = 0.067;                                  	% Length in z direction (Width) [m]
part(10).cog    = [-0.334 ...                            	% Centre of gravity [m m m]
    0 ...
    -0.020];

%=========
%=== Motor
%=========

part(11).name   = 'motor';                                  % Name of the part
part(11).m      = 0.130;                                  	% Mass of the Motor [Kg]
part(11).lx     = 0.0545;                                  	% Length in x direction [m]
part(11).d      = 0.025;                                   	% Diameter of cylinder [m]
part(11).cog    = [-0.057 ...                               % Centre of gravity [m m m]
    0 ...
    0];

%===========
%=== GearBox
%===========

part(12).name   = 'gearbox';                                % Name of the part
part(12).m     	= 0.080;                                  	% Mass of the Gearbox [Kg] =================================== compléter avec les valeurs justes
part(12).lx   	= 0.036;                                  	% Length in x direction [m] ================================ compléter avec les valeurs justes
part(12).d    	= 0.032;                                  	% Diameter of cylinder [m]  ================================ compléter avec les valeurs justes
part(12).cog   	= [-0.036/2 ...                            	% Centre of gravity [m m m]
    0 ...
    0];

%=============
%=== Propeller
%=============

part(13).name   = 'propeller';                              % Name of the part
part(13).m   	= 0.050;                                  	% Mass of the Propeller [Kg]
part(13).lx   	= 0.015;                                  	% Length in x direction [m]
part(13).d   	= 0.60;                                  	% Diameter of cylinder [m]
part(13).cog  	= [0.01 ...                                 % Centre of gravity [m m m]
    0 ...
    0];

%==========
%=== Regler
%==========

part(14).name   = 'regler';                                 % Name of the part
part(14).m     	= 0.030;                                   	% Mass of the Regler [Kg] ================================ Compléter avec la valeur juste
part(14).lx   	= 0.030;                                  	% Length in x direction [m]
part(14).ly   	= 0.025;                                  	% Length in y direction [m]
part(14).lz   	= 0.016;                                  	% Length in z direction (Width) [m]
part(14).cog  	= [-0.0616 ...                            	% Centre of gravity [m m m]
    0 ...
    0.0292];

%========
%=== MPPT
%========

part(15).name   = 'mppt';                                   % Name of the part
part(15).m     	= 0.024;                                   	% Mass of the Regler [Kg]
part(15).lx     = 0.066;                                   	% Length in x direction [m]
part(15).ly     = 0.010;                                   	% Length in y direction [m]
part(15).lz    	= 0.0365;                                 	% Length in z direction (Width) [m]
part(15).cog  	= [-0.125 ...                               % Centre of gravity [m m m]
    0 ...
    0];

%=======
%=== BEC
%=======

part(16).name   = 'bec';                                    % Name of the part
part(16).m     	= 0.017;                                   	% Mass of the BEC [Kg]
part(16).lx  	= 0.066;                                   	% Length in x direction [m]
part(16).ly   	= 0.036;                                   	% Length in y direction [m]
part(16).lz    	= 0.010;                                 	% Length in z direction (Width) [m]
part(16).cog  	= [-0.2 ...                                 % Centre of gravity [m m m]
    0 ...
    0.0216];

%==============
%=== ServoBoard
%==============

part(17).name   = 'servo board';                            % Name of the part
part(17).m  	= 0.008;                                    % Mass of the ServoBoard [Kg]
part(17).lx 	= 0.023;                                    % Length in x direction [m]
part(17).ly 	= 0.048;                                    % Length in y direction [m]
part(17).lz 	= 0.014;                                    % Length in z direction (Width) [m]
part(17).cog	= [-0.48 ...                                % Centre of gravity [m m m]
    0 ...
    0.0216];

%==============
%=== RCreceiver
%==============

part(18).name   = 'rc receiver';                            % Name of the part
part(18).m      = 0.015;                                    % Mass of the RC receiver [Kg] ================================ Compléter avec la valeur juste
part(18).lx 	= 0.042;                                    % Length in x direction [m]
part(18).ly 	= 0.013;                                    % Length in y direction [m]
part(18).lz 	= 0.022;                                    % Length in z direction (Width) [m]
part(18).cog    = [-0.573 ...                               % Centre of gravity [m m m]
    0 ...
    0.0064];

%==========
%=== MP2028
%==========

part(19).name   = 'mp2028';                                 % Name of the part
part(19).m    	= 0.033;                                    % Mass of the MP 2028 Autopilot [Kg]
part(19).lx  	= 0.102;                                    % Length in x direction [m]
part(19).ly    	= 0.038;                                    % Length in y direction [m]
part(19).lz 	= 0.018;                                    % Length in z direction (Width) [m]
part(19).cog  	= [-0.573 ...                               % Centre of gravity [m m m]
    0 ...
    -0.016];

%==============
%=== Radiomodem
%==============

part(20).name   = 'radio modem';                            % Name of the part
part(20).m      = 0.030;                                    % Mass of the RadioModem [Kg]
part(20).lx 	= 0.077;                                    % Length in x direction [m]
part(20).ly     = 0.041;                                    % Length in y direction [m]
part(20).lz 	= 0.013;                                    % Length in z direction (Width) [m]
part(20).cog    = [-0.557 ...                               % Centre of gravity [m m m]
    0 ...
    -0.04];

%===============
%=== GPS Antenna
%===============

part(21).name    = 'gps ant';                               % Name of the part
part(21).m       = 0.010;                                   % Mass of the GPS Antenna [Kg]
part(21).lx      = 0.027;                                   % Length in x direction [m]
part(21).ly      = 0.020;                                   % Length in y direction [m]
part(21).lz      = 0.007;                                   % Length in z direction (Width) [m]
part(21).cog     = [-0.632 ...                              % Centre of gravity [m m m]
    0 ...
    0.022];

% Note: the radio modem antenna is not taken into account due to it extremely low weight

%===============================================================
%=== CALCULATION OF THE TOTAL MASS AND CENTER OF GRAVITY
%===============================================================

masses = 0;                                                 % The array containing masses is initialized to zero;
for i=1:1:length(part)                                      % For all the parts...
    masses(i,1)            	= part(i).m;                    % ... the mass is stored in the array
    masses(i,2)          	= i;                            % ... the rank is also stored for future sorting
end
masses=sortrows(masses,-1);                                 % The masses are sorted in descending order
names_sort_by_mass = [];                                    % The array containing names sorted by masses is initialized
for i=1:1:length(part)                                      % For all the parts...
    names_sort_by_mass                = [names_sort_by_mass cellstr(part(masses(i,2)).name)];   % The list of name is completed according to the masses list
end
names_sort_by_mass = names_sort_by_mass';                   % Transposition
masses(:,2)=[];                                             % We don't need the second column anymore (for sorting only)

total_mass      = sum(masses')                              % The total mass of the airplane is computed

cog = [0,0,0];                                              % The center of gravity is initialized
for i=1:1:length(part)                                      % For all the part...
    cog = cog + part(i).m * part(i).cog;                    % ... the cog of the part is ponderated with the mass
end
cog = cog / total_mass                                      % The total center of gravity is obtained


%===============================================================
%=== CALCULATION OF THE TOTAL MASS AND CENTER OF GRAVITY
%===============================================================

Ixxs    =   0;                                              % The table of Ixx inertias for all parts is initialized
Iyys    =   0;                                              % The table of Iyy inertias for all parts is initialized
Izzs    =   0;                                              % The table of Izz inertias for all parts is initialized
for i=1:1:length(part)                                      % For all parts...
    [part(i).Ixx part(i).Iyy part(i).Izz]           =   calculate_inertia(part(i),cog);     % The inertias are calculated
    Ixxs(i,1)                                       =   part(i).Ixx;                        % Ixx is stored unsorted
    Ixxs(i,2)                                       =   i;                                  % Rank is stored for future sorting
    Iyys(i,1)                                       =   part(i).Iyy;                        % Iyy is stored unsorted
    Iyys(i,2)                                       =   i;                                  % Rank is stored for future sorting
    Izzs(i,1)                                       =   part(i).Izz;                        % Izz is stored unsorted
    Izzs(i,2)                                       =   i;                                  % Rank is stored for future sorting
end

Ixxs    =   sortrows(Ixxs,-1);                              % The Inertias are sorted in descending order
Iyys    =   sortrows(Iyys,-1);
Izzs    =   sortrows(Izzs,-1);

names_sort_by_Ixx = [];                                     % The arrays containing names sorted by Inertias are initialized
names_sort_by_Iyy = [];
names_sort_by_Izz = [];

for i=1:1:length(part)                                      % For all the parts...
    names_sort_by_Ixx                = [names_sort_by_Ixx cellstr(part(Ixxs(i,2)).name)];   % The list of name is completed according to the Inertias list
    names_sort_by_Iyy                = [names_sort_by_Iyy cellstr(part(Iyys(i,2)).name)];
    names_sort_by_Izz                = [names_sort_by_Izz cellstr(part(Izzs(i,2)).name)];
end

names_sort_by_Ixx                = names_sort_by_Ixx';      % Transposition
names_sort_by_Iyy                = names_sort_by_Iyy';
names_sort_by_Izz                = names_sort_by_Izz';

Ixxs(:,2)=[];                                               % We don't need the second column anymore (for sorting only)
Iyys(:,2)=[];
Izzs(:,2)=[];

total_Ixx      = sum(Ixxs')                                 % The total Inertias of the airplane are computed
total_Iyy      = sum(Iyys')
total_Izz      = sum(Izzs')



%===============================================================
%=== GRAPHICAL REPRESENTATION
%===============================================================

if (1)
    Screen_Size = get(0,'ScreenSize')
    win_width   = Screen_Size(3)/2.5;
    win_height  = Screen_Size(4)/2.5;
    win_deltax  = 10;
    win_deltay  = 30;

    %===================
    %=== Graph of Masses
    %===================

    figure('Position',[1 35+win_height+win_deltay win_width win_height])
    explode = ones(size(masses));
    h = pie(masses/total_mass,explode); colormap jet
    textObjs = findobj(h,'Type','text');
    oldStr = get(textObjs,{'String'});
    newStr = strcat(strcat(names_sort_by_mass,'--'),oldStr);
    legend(newStr,'Position',[0.01706 0.06481 0.1918 0.8815])
    for i=1:1:length(newStr)
        if findstr(char(oldStr(i,:)),'1%')>0
            newStr(i,:)=cellstr('');
        end
    end
    view(90,45)
    set(textObjs,{'String'},newStr)
    set(gcf, 'MenuBar', 'none')

    title('MASSES DISTRIBUTION');


    %===================
    %=== Graph of Ixx
    %===================

    figure('Position',[1+win_width+win_deltax 35+win_height+win_deltay win_width win_height])
    explode = ones(size(Ixxs));
    h = pie(Ixxs/total_Ixx,explode); colormap jet
    textObjs = findobj(h,'Type','text');
    oldStr = get(textObjs,{'String'});
    newStr = strcat(strcat(names_sort_by_Ixx,'--'),oldStr);
    legend(newStr,'Position',[0.01706 0.06481 0.1918 0.8815])
    for i=1:1:length(newStr)
        if findstr(char(oldStr(i,:)),'1%')>0
            newStr(i,:)=cellstr('');
        end
    end
    view(90,45)
    set(textObjs,{'String'},newStr)
    set(gcf, 'MenuBar', 'none')
    title('Ixx DISTRIBUTION');


    %===================
    %=== Graph of Iyy
    %===================

    figure('Position',[1 35 win_width win_height])
    explode = ones(size(Iyys));
    h = pie(Iyys/total_Iyy,explode); colormap jet
    textObjs = findobj(h,'Type','text');
    oldStr = get(textObjs,{'String'});
    newStr = strcat(strcat(names_sort_by_Iyy,'--'),oldStr);
    legend(newStr,'Position',[0.01706 0.06481 0.1918 0.8815])
    for i=1:1:length(newStr)
        if findstr(char(oldStr(i,:)),'1%')>0
            newStr(i,:)=cellstr('');
        end
    end
    view(90,45)
    set(textObjs,{'String'},newStr)
    set(gcf, 'MenuBar', 'none')
    title('Iyy DISTRIBUTION');

    %===================
    %=== Graph of Izz
    %===================

    figure('Position',[1+win_width+win_deltax 35 win_width win_height])
    explode = ones(size(Izzs));
    h = pie(Izzs/total_Izz,explode); colormap jet
    textObjs = findobj(h,'Type','text');
    oldStr = get(textObjs,{'String'});
    newStr = strcat(strcat(names_sort_by_Izz,'--'),oldStr);
    legend(newStr,'Position',[0.01706 0.06481 0.1918 0.8815])
    for i=1:1:length(newStr)
        if findstr(char(oldStr(i,:)),'1%')>0
            newStr(i,:)=cellstr('');
        end
    end
    view(90,45)
    set(textObjs,{'String'},newStr)
    set(gcf, 'MenuBar', 'none')
    title('Izz DISTRIBUTION');

    %===========================
    %=== Graph of Airplane Parts
    %===========================

    fn = figure('Position',[1+win_width+win_deltax 35 win_width win_height-45]);
    xmin =   10000;
    xmax = - 10000;
    ymin =   10000;
    ymax = - 10000;
    zmin =   10000;
    zmax = - 10000;

    color_map = lines(length(part));

    for i=1:1:length(part)
        if(isempty(part(i).d))
            plot_parallelepiped(part(i).cog,part(i).lx,part(i).ly,part(i).lz,color_map(i,:));
            xmin = min(xmin,part(i).cog(1)-part(i).lx/2);
            xmax = max(xmax,part(i).cog(1)+part(i).lx/2);
            ymin = min(ymin,part(i).cog(2)-part(i).ly/2);
            ymax = max(ymax,part(i).cog(2)+part(i).ly/2);
            zmin = min(zmin,part(i).cog(3)-part(i).lz/2);
            zmax = max(zmax,part(i).cog(3)+part(i).lz/2);
            hold on;
        else
            plot_cylinder(part(i).cog,part(i).lx,part(i).d,color_map(i,:))
            xmin = min(xmin,part(i).cog(1)-part(i).lx/2);
            xmax = max(xmax,part(i).cog(1)+part(i).lx/2);
            ymin = min(ymin,part(i).cog(2)-part(i).d/2);
            ymax = max(ymax,part(i).cog(2)+part(i).d/2);
            zmin = min(zmin,part(i).cog(3)-part(i).d/2);
            zmax = max(zmax,part(i).cog(3)+part(i).d/2);
        end
    end

    view(3)

    for az=45:1:800
        if ~ishandle(fn)
            break
        end
        view(az,22)
        set(gca,'DataAspectRatio',[1 1 1])
        axis([xmin xmax ymin ymax zmin zmax])
        drawnow
        %pause(.001)
    end
end
