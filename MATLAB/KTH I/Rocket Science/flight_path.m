function flight_path(angles, masses)


%% Time set-up

Duration = 10;
Snapshots = 200; %nombre de "photos"
Tau = 1/10000;

%% Plot Set-up

set(gcf, 'Name','Flight Path',...
    'NumberTitle','off',...
    'Position',[50 100 600 500]);

hold all;

%% Calculations
%formula: m*r'' = -0.5*rho*Cx*A*r'2 - m*g

tests = length(angles)*length(masses);
titles = '';

rxsnap = zeros(Snapshots, tests);
rysnap = zeros(Snapshots, tests);

for testnumber = 1:tests
    
    %initialisation
    m0 = masses( 1+mod(testnumber-1,length(masses)) );
    a0 = angles( ceil(testnumber/length(masses)) );
    titles = [titles; {[num2str(a0) 'deg, ' num2str(m0) 'kg']}];
    
    E0 = 150; %total energy imparted by thrust sequence
    v0 = sqrt(2*E0 / m0);
    K = 0.5 * 1.2 * 0.75 * 0.0078; %approx of 0.5*rho*Cx*A rho=1.2 Cx=0.75 A=0.0078
    g = 9.81;
    
    Tau2 = Tau^2;
    
    rx = [v0*cosd(a0)*Tau 0]; %the position vector also contains
    ry = [v0*sind(a0)*Tau 0]; %the two previous positions.
    %==============
    
    
    for n=1:Snapshots
        if ry(1) >= 0
            rxsnap(n, testnumber) = rx(1);
            rysnap(n, testnumber) = ry(1);
            for i=1:( (Duration/Tau)/Snapshots )
                rx = [0 rx(1:2)];
                ry = [0 ry(1:2)];
                
                dx = rx(2) - rx(3);
                dy = ry(2) - ry(3);
                d2x = dx*sqrt(dx*dx + dy*dy);
                d2y = dy*sqrt(dx*dx + dy*dy);
                rx(1)= - (K/m0) *d2x + 2*rx(2) - rx(3);
                ry(1)= - Tau2*g - (K/m0) *d2y + 2*ry(2) - ry(3);
            end
        else
            break
        end
    end
    
end

plot(rxsnap, rysnap, '.', 'MarkerSize', 5, 'DisplayName', titles);
legend('-DynamicLegend');

end