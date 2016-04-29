function exo10()

close all; clear all;


%% Decoupe du temps. DT est l'ecart entre deux affichages
%  dt l'ecart de temps entre deux calculs differentiels
dt=0.0001;
DT=0.5;
N=1000;
T=DT*[0:1:N];


%% positions initiales des particules
%X=[0.1 0.09 0.08 0.8 -0.8 0];
%Y=[0.1 0.09 0.08 0 0 0.5];
%X=[0.1 0.8 -0.8 0];
%Y=[0.1 0 0 0.5];
warp = 10;
X=0+ 0.2* tand(linspace(-atand(warp),atand(warp),5)) / warp;
Y=-0.2+zeros(1,length(X));
P=length(Y);

Xs=zeros(N,P);
Ys=zeros(N,P);

%% pré-initialisation des tourbillons
modeBatteur = 'huit';

Gamma=0.3;
Gam2pi1=Gamma/(2*pi);
Gam2pi2=-Gamma/(2*pi);

Xr1 = -1;
Yr1 = 0;
Xr2 = 1;
Yr2 = 0;

%parametres des cas oscillants
A=1;
w=0.2;

%parametres du cas contra-rotatif (^ pour l'activer, modifier Gam2pi2 ^)

R = 1.5;
R2 = R^2;
imGam2pi1 = 0;
imGam2pi2 = 0;
orient1 = sign(Gam2pi1);
orient2 = sign(Gam2pi2);
uimg = 0;
vimg = 0;
rotationBatteur = 'default'; %pas une variable controle
if (Gam2pi1*Gam2pi2)<0
    rotationBatteur = 'contra-rotation';
    imGam2pi1 = Gam2pi1;
    imGam2pi2 = Gam2pi2;
end

%% visualisation des tourbillons
xv=[-2:0.2:2];
yv=[-2:0.2:2];
[xx,yy]=meshgrid(xv,yv);
ux=Gam2pi1*(yy-Yr1)./((xx-Xr1).^2+(yy-Yr1).^2)...
  +Gam2pi2*(yy-Yr2)./((xx-Xr2).^2+(yy-Yr2).^2)...
  -imGam2pi1*( yy-Yr1 .* (R^2./(Xr1^2 + Yr1^2)) )./...
   ( (xx).^2+ (yy).^2+ (R^2./(Xr1^2 + Yr1^2)).*(R^2 - 2*(xx*Xr1 + yy*Yr1)) )...
  -imGam2pi2*(yy-Yr2.*(R^2 ./ (Xr2^2 + Yr2^2) ))./...
   ( (xx).^2+(yy).^2+ (R^2./(Xr2^2 + Yr2^2)).*(R^2 - 2*(xx*Xr2 + yy*Yr2)) );
uy=-Gam2pi1*(xx-Xr1)./((xx-Xr1).^2+(yy-Yr1).^2)...
   -Gam2pi2*(xx-Xr2)./((xx-Xr2).^2+(yy-Yr2).^2)...
   +imGam2pi1*(xx-Xr1.*(R^2 ./ (Xr1^2 + Yr1^2) ))./...
    ( (xx).^2+(yy).^2+ (R^2./(Xr1^2 + Yr1^2))*(R^2 - 2*(xx*Xr1 + yy*Yr1)) )...
   +imGam2pi2*(xx-Xr2*(R^2 ./ (Xr2^2 + Yr2^2) ))./...
    ( (xx).^2+(yy).^2+ (R^2/(Xr2^2 + Yr2^2))*(R^2 - 2*(xx*Xr2 + yy*Yr2)) );

A = figure('Name','Tourbillons',...
           'NumberTitle','off',...
           'Position',[50 100 600 500]);
quiver(xx,yy,ux,uy);

B = figure('Name','Trajectoires',...
           'NumberTitle','off',...
           'Position',[700 100 600 500]);

%% Calculs
for id=1:N
    %disp(T(id));
    Rr1 = (R2/(Xr1^2 + Yr1^2));
    Rr2 = (R2/(Xr2^2 + Yr2^2));
    t=[T(id):dt:T(id+1)-dt];
    for it=1:length(t)
        
        switch(modeBatteur)
            
            case 'immobile'
                
            case 'x oscillation'
                
                Xr1 = -1 +A*sin(w*t(it));
                Yr1 = 0                 ;
                Xr2 = 1  +A*sin(w*t(it));
                Yr2 = 0                 ;
                
            case 'alternant'
                
                Gam2pi1 = ( 1+cos(w*t(it)) )*Gamma/(2*pi)*orient1;
                Gam2pi2 = ( 1-cos(w*t(it)) )*Gamma/(2*pi)*orient2;
                
            case 'circulaire'
                
                Xr1 = -1 +A*sin(w*t(it));
                Yr1 = 0  +A*cos(w*t(it));
                Xr2 = 1  +A*sin(w*t(it));
                Yr2 = 0  +A*cos(w*t(it));
                
            case 'huit'
                
                Xr1 = -1 +A*sin(w*t(it));
                Yr1 = 0  +A*cos(w*t(it))*sin(w*t(it));
                Xr2 = 1  +A*sin(w*t(it));
                Yr2 = 0  +A*cos(w*t(it))*sin(w*t(it));
            
        end;
            
        switch rotationBatteur
            
            case 'contra-rotation'
                
                uimg = Gam2pi1*( Y-Yr1 * Rr1 )./...
                        ( X.^2+ Y.^2+ Rr1*(R2 - 2*(X*Xr1 + Y*Yr1)) )...
                      +Gam2pi2*( Y-Yr2*Rr2 )./...
                        ( X.^2+ Y.^2+ Rr2*(R2 - 2*(X*Xr2 + Y*Yr2)) );
                vimg = Gam2pi1*( X-Xr1*Rr1 )./...
                        ( X.^2+ Y.^2+ Rr1*(R2 - 2*(X*Xr1 + Y*Yr1)) )...
                      +Gam2pi2*( X-Xr2*Rr2 )./...
                        ( X.^2+ Y.^2+ Rr2*(R2 - 2*(X*Xr2 + Y*Yr2)) );
        
        end
        
        u=Gam2pi1*(Y-Yr1)./((X-Xr1).^2+(Y-Yr1).^2)...
         +Gam2pi2*(Y-Yr2)./((X-Xr2).^2+(Y-Yr2).^2)...
         -uimg;

        v=-Gam2pi1*(X-Xr1)./((X-Xr1).^2+(Y-Yr1).^2)...
          -Gam2pi2*(X-Xr2)./((X-Xr2).^2+(Y-Yr2).^2)...
          +vimg;

        X=X+u*dt;
        Y=Y+v*dt;
    end;
    Xs(id,:)=X;
    Ys(id,:)=Y;
    
    plotframe(id);
    
end;

%% Visualisation du film
close(B);
C = figure('Name','Film (appuyer sur espace)',...
           'NumberTitle','off',...
           'Position',[700 100 600 500]);
movie(F,[1 1 2]);
pause;
set(C, 'Name','Film');
movie(F, 1, 100/DT) %DT représente des centièmes de secondes (abritraire)

%% Affichage des particules et de leurs [mempos] dernières positions
    function plotframe(id)
        figure(B);
        set(B, 'Name',['Trajectoires ', num2str(100*id/N), '%']);
        mempos = 50/DT;
        backid = ( (id<=mempos)*1+(id>mempos)*(id-mempos) ):id ;
        for ip=1:P
            plot(Xs(backid,ip),Ys(backid,ip));
            hold all;
        end;
        plot(Xr1, Yr1, 'x', 'MarkerSize', abs(200*Gam2pi1));
        plot(Xr2, Yr2, 'x', 'MarkerSize', abs(200*Gam2pi2));
        hold off;
        axis([-2 2 -2 2]);
        F(id)=getframe;
    end


end


