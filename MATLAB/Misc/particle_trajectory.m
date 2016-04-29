clear all; close all;


%% Decoupe du temps.

Duration = 10; 
Snap = 1000; %nombre de "photos"
Tau = 1/10000;

%% Visualisation des tourbillons
X=[-2:0.2:2];
Y=[-2:0.2:2];
[XX,YY]=meshgrid(X,Y);
ax=YY./sqrt(XX.^2 + YY.^2)-20*XX.*sqrt(XX.^2 + YY.^2);
ay=-XX./sqrt(XX.^2 + YY.^2)-20*YY.*sqrt(XX.^2 + YY.^2);

A = figure('Name','Tourbillons',...
           'NumberTitle','off',...
           'Position',[50 100 600 500]);
quiver(XX,YY,ax,ay);
hold all;

%% Calculs

%initialisation
Tau2 = Tau^2;
rx = [1.0001 1]; %le vecteur position contient aussi
ry = [0.9999 1]; %les deux positions precedentes.
%==============

for n=1:Snap
    for i=1:( (Duration/Tau)/Snap )
    rx = [0 rx(1:2)];
    ry = [0 ry(1:2)];
    
    hyp = sqrt( rx(2)^2 + ry(2)^2 );
    %A=10*sin(2*pi*i*n*Tau);
    rx(1)= Tau2*( ry(2)/hyp - 20*rx(2)*hyp ) + 2*rx(2) - rx(3);
    ry(1)= Tau2*( -rx(2)/hyp - 20*ry(2)*hyp ) + 2*ry(2) - ry(3);
    end
    plot(rx(1), ry(1), 'k.', 'MarkerSize', 5);
end 