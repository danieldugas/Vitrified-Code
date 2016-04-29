function show_uavs(in)

global show_uav_part vert fac;
global time_sec;
global show_total_force_e;
global show_total_momentum_e;
global show_momentum_aero;
global mov;
global wind_angle;
global relspeed;
global coeff;
global init;
global wing;
global cog;
global first_execution;
global misc;

disp_forces = 1;
disp_misc   = 1;
take_movie  = 0;

phi     = in(1);
theta   = in(2);
psi     = in(3);
finesse = in(4);

if (isempty(first_execution))
    first_execution = 1;
    eval('global wing; global cog; global misc; wing_config;');
    eval('global show_total_momentum_e; global show_total_force_e;')
    eval('global ail_angle');
else

    if (isempty(get(0,'CurrentFigure')))
        figure(1)
        set(gcf,'Position',[8 20 1024+8 768+20]);
        cla
        whitebg([.0 .0 0.0]);
        set(gcf,'Color','k');
        plot3(0,0,0);
        hold on
        view(190,22)
        axis equal
        axis([-1.7 1.7 -1.7 1.7 -1.7 1.7]);                                                      % Axis limits
        xlabel('X [m]'); ylabel('Y [m]'); zlabel('Z [m]');
    else
        cla
    end

    for i=1:1:length(wing)

        h(i)=   patch('Vertices',wing(i).vert, ...                             % List of point coordinates
            'Faces',wing(i).conn , ...                                         % List of interconnection between points
            'FaceColor',[1 1 0], ...
            'FaceAlpha',0.5);                                                  % Transparency set to 0.2 (high)
        
        ail(i)=   patch('Vertices',wing(i).ail, ...                             % List of point coordinates
            'Faces',wing(i).conn , ...                                         % List of interconnection between points
            'FaceColor',[1 0 0], ...
            'FaceAlpha',0.5);                                                  % Transparency set to 0.2 (high)

        rotate(ail(i),wing(i).dir(1,:)-wing(i).dir(2,:),rad2deg(-wing(i).ailangle)*2,wing(i).dir(1,:));
        
        if (disp_forces)
            fx_start = wing(i).p_0_25(1)-cog(1);
            fx_end   = wing(i).p_0_25(1)-cog(1) + show_total_force_e(1,i)/5;
            fy_start = wing(i).p_0_25(2)-cog(2);
            fy_end   = wing(i).p_0_25(2)-cog(2) + show_total_force_e(2,i)/5;
            fz_start = wing(i).p_0_25(3)-cog(3);
            fz_end   = wing(i).p_0_25(3)-cog(3) + show_total_force_e(3,i)/5;

            forces_X = [fx_start,   fx_end,     fx_end,     fx_end];
            forces_Y = [fy_start,   fy_start,   fy_end,     fy_end];
            forces_Z = [fz_start,   fz_start,   fz_start,   fz_end];
            force_X  = [fx_start,   fx_end];
            force_Y  = [fy_start,   fy_end];
            force_Z  = [fz_start,   fz_end];

            mx_start = wing(i).p_0_25(1)-cog(1);
            mx_end   = wing(i).p_0_25(1)-cog(1) + show_total_momentum_e(1,i)/5;
            my_start = wing(i).p_0_25(2)-cog(2);
            my_end   = wing(i).p_0_25(2)-cog(2) + show_total_momentum_e(2,i)/5;
            mz_start = wing(i).p_0_25(3)-cog(3);
            mz_end   = wing(i).p_0_25(3)-cog(3) + show_total_momentum_e(3,i)/5;

            moments_X = [mx_start,  mx_end,     mx_end,     mx_end];
            moments_Y = [my_start,  my_start,   my_end,     my_end];
            moments_Z = [mz_start,  mz_start,   mz_start,   mz_end];
            moment_X  = [mx_start,	mx_end];
            moment_Y  = [my_start,	my_end];
            moment_Z  = [mz_start,	mz_end];

            fs(i)=plot3(forces_X,forces_Y,forces_Z,'LineWidth',1,'Color',[0 1 0]);
            ms(i)=plot3(moments_X,moments_Y,moments_Z,'LineWidth',1,'Color',[1 0 0]);
            f(i)=plot3(force_X,force_Y,force_Z,'LineWidth',2,'Color',[0 1 0]);
            m(i)=plot3(moment_X,moment_Y,moment_Z,'LineWidth',2,'Color',[1 0 0]);
        end
    end
    
    
    spd = plot3([0,relspeed(1)]/5,[0,relspeed(2)]/5,[0,relspeed(3)]/5,'LineWidth',2,'Color',[0 0 1]);
    if (disp_forces)
        hi  = text(relspeed(1)/5,relspeed(2)/5,relspeed(3)/5,{strcat(num2str(norm(relspeed)),' m/s'),strcat(num2str(rad2deg(theta)),'°'),strcat(num2str(rad2deg(wing(1).wind_angle)),'°'),strcat(num2str(finesse))});
    end
    t = hgtransform('Parent',gca);

    set(h,'Parent',t);
    set(ail,'Parent',t);
    
    if disp_misc
    for i=1:1:length(misc)
        misch(i) = plot_parallelepiped(misc(i).cog,misc(i).lx,misc(i).ly,misc(i).lz,[0,0,1]);
    end
    set(misch,'Parent',t);
    end

    if (disp_forces)
        set(fs,'Parent',t);
        set(ms,'Parent',t);
        set(f,'Parent',t);
        set(m,'Parent',t);
        set(hi,'Parent',t);
        
    end
    set(spd,'Parent',t);

    Rx = makehgtform('xrotate',phi);
    Ry = makehgtform('yrotate',theta);
    Rz = makehgtform('zrotate',psi);
    Rlast= makehgtform('yrotate',pi);
    a = Rlast*Rz*Ry*Rx;
    set(t,'Matrix',a);
    set(gcf,'Renderer','opengl')

    axe = plot3([0,1,0,0,0,0],[0,0,0,1,0,0],[0,0,0,0,0,1],'LineWidth',2,'Color',[0.5 0.5 0.5]);
    rotate(axe, [0 1 0],    180,              [0 0 0]);

    if (first_execution == 1)
        annotation1 = annotation(gcf,'textbox',...
            'Position',[0.7 0.82 0.07 0.022],...
            'Color',[1 0 0],...
            'FitHeightToText','off',...
            'String',{'Moment'});

        annotation2 = annotation(gcf,'textbox',...
            'Position',[0.7 0.79 0.07 0.022],...
            'Color',[0 1 0],...
            'FitHeightToText','off',...
            'String',{'Forces'});

        annotation2 = annotation(gcf,'textbox',...
            'Position',[0.7 0.76 0.07 0.022],...
            'Color',[0 0 1],...
            'FitHeightToText','off',...
            'String',{'UAV Speed'});
        init = 1;
        disp('initialisation');
    end

    drawnow
    first_execution = 0;
      if (take_movie)
            F = getframe(gcf);
            mov = addframe(mov,F);
      end
end
end