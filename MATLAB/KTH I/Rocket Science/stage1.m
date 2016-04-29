function stage1
%(mfuel0, m0)
mfuel0 = 371069;
m0 = 446817;
close all;
%% TO DO:

%% Time set-up

Duration = 600; %in seconds
snapshots = 100; %amount of recorded points
Tau = 1/100;
steps = Duration/Tau/snapshots; %amount of steps in between snapshots

%% Plot Set-up

rxsnap(1:snapshots) = NaN;
rysnap(1:snapshots) = NaN;
vsnap(1:snapshots) = NaN;
asnap(1:snapshots) = NaN;
gsnap(1:snapshots) = NaN;
Fdsnap(1:snapshots) = NaN;
tsnap(1:snapshots) = NaN;

x = (0:snapshots-1)*steps*Tau;

set(gcf, 'Name','Simulation',...
    'NumberTitle','off',...
    'Position',[50 100 600 500]);

hold all;
titles = [num2str(mfuel0) 'kg fuel, ' num2str(m0) 'kg total (' num2str(100*mfuel0/m0) '% fuel)'];
p = plot(x, rysnap, '.', 'MarkerSize', 5, 'DisplayName', titles);
legend('-DynamicLegend');
xlabel('Time [s]')
ylabel('h [km]')

%% Calculations
%formula: m(t)*r'' = -0.5*rho*Cx*A*r'2 - m(t)*g + veff*m(t)'

%useful constants
re = 6371000; %Earth's mean radius (Wikipedia)

%initialisation
m = m0;
mfuel = mfuel0;
missionheight = 182.78; % km
missionspeed = 1982; % m/s
% thrust = dm*Isp*g = dm*ve
dm= 9*235; %kg/s
thrust = 9*650000; %Newtons
Isp = 282; %s

Cd = 0.75;
A=9;
Tau2 = Tau^2;

%% variable declarations

rx = [0 0 0]; %the position vector. It also contains
ry = [0 0 0]; %the previous and next position.
%position index: [next current previous]

j=0; %total step counter
jthrust = 0; %absolute total of steps where engines were on

hmax = 0; %stores the maximum height
t_hmax = NaN; % instant of maximum height
hobj = 0; % is 1 once maximum height has been reached
t_hobj = 0;
thruston = 1; %Engines are on
t_eoff = 0; % instant of engine shut off
K=0;
dy = 0;
d2y = 0;
g = 9.81;

for n=1:snapshots
    
    rxsnap(n) = rx(2);
    rysnap(n) = ry(2);
    vsnap(n) = dy/Tau;
    asnap(n) = ( ry(1) - 2*ry(2) + ry(3) )/ Tau2;
    Fdsnap(n) = - (K/m) *d2y / Tau2;
    gsnap(n) = - g;
    tsnap(n) = thruston*thrust/m;
    
    set(gcf, 'Name',['t+' num2str(j*Tau) 's  Simulation  - ' num2str(100*mfuel/mfuel0, 3) '% Fuel Left']);
    for i=1:steps
        
        rx = [0 rx(1:2)];
        ry = [0 ry(1:2)];
        
        %precalculations
        dx = rx(2) - rx(3);
        dy = ry(2) - ry(3);
        d2x = dx*sqrt(dx*dx + dy*dy);
        d2y = dy*sqrt(dx*dx + dy*dy);
        g = 9.81*(re/(re+ry(2)))*(re/(re+ry(2))); %gravity(altitude)
        K = 0.5 * density(ry(2)) * Cd * A; %drag(altitude)
        
        % fuel availability check
        if thruston == 1
            jthrust=jthrust+1;
            m=m0-dm*jthrust*Tau;
            mfuel = mfuel0-dm*jthrust*Tau;
            if mfuel <= dm*Tau
                thruston = 0
                t_eoff = j*Tau;
                text(t_eoff,ry(2)/1000,['\leftarrow  t+' num2str(t_eoff) 's,  Engine Off'],'HorizontalAlignment','left');
            end
        else
            if mod(j, 1/Tau) == 0
                %activated every second after thrust sequence
                %useful for debugging
            end
        end
        
        % minimum height check
        if ry(2)<0
            set(gcf, 'Name',['t+' num2str(j*Tau) 's - Touchdown - Over']);
            ry(2) = 0;
            break
        end
        % maximum height check
        if dy*(hmax==0) < 0
            hmax = ry(3)
            t_hmax = j*Tau;
            text(t_hmax,hmax/1000,['t+' num2str(t_hmax) 's,  maximum height: ' num2str(hmax/1000) 'km     \rightarrow  '],'HorizontalAlignment','right');
        end
        
        % mission height check
        if (dy>=0)*(ry(2)>=230000)*(ry(2)<=234000)*(hobj==0)
            hobj = 1;
            t_hobj = j*Tau;
            text(t_hobj,ry(2)/1000,['\leftarrow  t+' num2str(t_hobj) 's,  Mission Altitude Reached - Speed is ' num2str(dy/Tau)],'HorizontalAlignment','left');
        end
        
        % Computing next value for position
        rx(1)= - (K/m) *d2x + 2*rx(2) - rx(3);
        ry(1)= - Tau2*g + Tau2*thruston*thrust/m - (K/m) *d2y + 2*ry(2) - ry(3);
    
        j=j+1;    
    end
    
    
    
    set(p,'YData', rysnap/1000);
    drawnow;
end

datafigure = figure('Name','Mission Data',...
    'NumberTitle','off',...
    'Position',[700 30 600 650]);
subplot(2,1,1);
hold all;
plot(x, vsnap, 'DisplayName', 'Speed');
%line([t_eoff t_eoff], [min(vsnap) max(vsnap)],'Color','k');
legend('show')
subplot(2,1,2);
hold all;
area(x, asnap, 'DisplayName', 'Acceleration', 'FaceColor', [0.8 0.8 0.8]);
plot(x, Fdsnap, 'DisplayName', 'Drag Force/m');
plot(x, gsnap, 'DisplayName', 'gravity');
plot(x, tsnap, 'DisplayName', 'Thrust/m');
legend('show')
%ylim([-100 100]);

end

function out = density(h)
rho = [1.347 1.225 1.112 1.007 0.909 0.819 0.736 0.660 0.590 0.525 0.467 0.413 0.194 0.088 0.04 0.018 0.003 0.001 0.0003 0.00008 0];
alt = [-1 0 1 2 3 4 5 6 7 8 9 10 15 20 25 30 40 50 60 70 100];
A = (h/1000) >= alt;
B = [A(2:end) 0];
C = A - B;
out = C*rho';
end
