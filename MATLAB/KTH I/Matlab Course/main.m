%% RIDE VIBRATIONS IN VEHICLES
%  Computing the behavior of a 4-wheeled vehicle in the 2D plane
%
%  by Daniel Dugas

close all; clear all; 
ans = 0; % dump variable

%% Parametric Values

% car
m1 = 800;
m2 = 50;
m3 = 50;
j = 750;
l = 2.6;
p = 0.44;
k1 = 500000;
k2 = 20000;
k3 = 20000;
c1 = 0;
c2 = 400;
c3 = 400;

% road
d = 12;
z = 0.03;

% parameters in easy to copy format: 
% m1, m2, m3, j, l, p, k1, k2, k3, c1, c2, c3, d, z 

%% Sub-Task 1

Fig1 = figure('Name','Ride Vibrations in Vehicles',...
      'NumberTitle','off',...
      'Position',[50 100 1200 500]);

[V_four, Z_four] = fourDOF(m1, m2, m3, j, l, p, k1, k2, k3, c1, c2, c3, d, z);

subplot(1,2,1);
hold on ;
plot(V_four, Z_four);
title('Four Degrees of Freedom');
xlabel('Car Speed [km/h]');
ylabel('Displacement Amplitudes [m] & [rad]');
legend('z1', 'z2', 'z3', 'Theta');

[z1, i1] = max(Z_four(:,1)); v1 = V_four(i1);
[z2, i2] = max(Z_four(:,2)); v2 = V_four(i2);
[z3, i3] = max(Z_four(:,3)); v3 = V_four(i3);
[z4, i4] = max(Z_four(:,4)); v4 = V_four(i4);

text(v1,z1,['\leftarrow  z1 max = ' num2str(z1) ' at V = ' num2str(v1) ' km/h ']);
text(v2,z2,['\leftarrow  z2 max = ' num2str(z2) ' at V = ' num2str(v2) ' km/h ']);
text(v3,z3,['\leftarrow  z3 max = ' num2str(z3) ' at V = ' num2str(v3) ' km/h ']);
text(v4,z4,['\leftarrow  Theta max = ' num2str(z4) ' at V = ' num2str(v4) ' km/h ']);


[V_two, Z_two] = twoDOF(m1, m2, m3, j, l, p, k1, k2, k3, c1, c2, c3, d, z);

Subtwo = subplot(1,2,2);
hold on ;
plot(V_two, Z_two);
title('Two Degrees of Freedom');
xlabel('Car Speed [km/h]');
ylabel('Displacement Amplitudes [m]');
legend('z', 'zt');

[ans, ii] = max(abs(Z_two(:,1))); zz  = Z_two(ii,1); vv  = V_two(ii);
[ans, it] = max(abs(Z_two(:,2))); zt = Z_two(it,2); vt = V_two(it);

text(vv ,zz ,['\leftarrow  z  Max = ' num2str(zz ) ' at V = ' num2str(vv ) ' km/h ']);
text(vt,zt,['\leftarrow  zt Max = ' num2str(zt) ' at V = ' num2str(vt) ' km/h ']);


%% Sub-Task 2

[Vc, Zc] = eigenFourDOF(m1, m2, m3, j, l, p, k1, k2, k3, c1, c2, c3, d, z);


subplot(1,2,1);
line([Vc(1) Vc(1)],ylim,'Color',[1 0 0]);
line([Vc(2) Vc(2)],ylim,'Color',[1 0 0]);
line([Vc(3) Vc(3)],ylim,'Color',[1 0 0]);
line([Vc(4) Vc(4)],ylim,'Color',[1 0 0]);
xlim([10 120]);

subplot(1,2,2);
line([Vc(1) Vc(1)],ylim,'Color',[1 0 0]);
line([Vc(2) Vc(2)],ylim,'Color',[1 0 0]);
line([Vc(3) Vc(3)],ylim,'Color',[1 0 0]);
line([Vc(4) Vc(4)],ylim,'Color',[1 0 0]);
xlim([10 120]);

values = [{['Vc = ' num2str(Vc(1)) '      Vc = ' num2str(Vc(2)) '    Vc = ' num2str(Vc(3)) '    Vc = ' num2str(Vc(4)) ]};
          {'1                       1                          1                     1'                                };
            num2str(Zc(2:4,:))                                                                                           ];
Msg = msgbox(values, 'Eigenvectors: '  );
uiwait(Msg);


%% Sub-Task 3

k2 = 10000:500:100000;
k3 = 10000:500:100000;

for n = 1:length(k3)
    [Vk(1:4,n), ans] = eigenFourDOF(m1, m2, m3, j, l, p, k1, k2(n), k3(n), c1, c2, c3, d, z);
end

Fig2 = figure('Name','Ride Vibrations in Vehicles',...
      'NumberTitle','off',...
      'Position',[650 100 600 500]);
plot(k3/1000, Vk);
hold on;
title('Critical Speeds, Four Degrees of Freedom');
xlabel('k3 Spring Constant [kN/m]');
ylabel('Critical Speed [km/h]');

%% EXTRA: VISUALIZING THE MOTION

choice = 'Cancel';
choice = questdlg('You can now select a speed at which you want to visualise the motion of the car, as a number, or on the graph directly.', ...
 'Visualisation', ...
 'Number Input','Graph Input','Cancel','Cancel');

switch choice
    case 'Number Input'
        v = inputdlg('Ground speed in km/h');
        [v, ans] = str2num(v{1});
        
        A = V_four <= v;
        B = [A(2:end);
            0];
        C = A - B;
        index = [1:length(C)]*C ;
        
        [F, dt] = visualisation(m1, m2, m3, j, l, p, k1, k2, k3(n), c1, c2, c3, d, z, V_four(index), Z_four(index,:));
        
        button = uicontrol('Position',[20 20 200 40],'String','Play',...
            'Callback',' movie(F, 5, 1/dt);');
        
    case 'Graph Input' 
        set(0,'CurrentFigure',Fig1);
        [v, ans] = ginput(1);
        
        A = V_four <= v;
        B = [A(2:end);
            0];
        C = A - B;
        index = [1:length(C)]*C ;
        
        [F, dt] = visualisation(m1, m2, m3, j, l, p, k1, k2, k3(n), c1, c2, c3, d, z, V_four(index), Z_four(index,:));
        
        button = uicontrol('Position',[20 20 200 40],'String','Play',...
            'Callback',' movie(F, 5, 1/dt);');
       
       
        
    case 'Cancel'
        
end
