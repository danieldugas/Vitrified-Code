close all; clear all;

Gamma=0.2;
Gam2pi=Gamma/2/pi;
xv=[-1:0.05:1];
yv=[-1:0.05:1];
[xx,yy]=meshgrid(xv,yv);
uy=-Gam2pi*xx./(xx.^2+yy.^2);
ux=Gam2pi*yy./(xx.^2+yy.^2);
figure;
quiver(xx,yy,ux,uy);

%on decoupe le temps en N fenetres non chevauchantes
%de DT
dt=0.0001;
DT=5;
N=100;
%Gamma=0.2;
T=DT*[0:1:N];

%positions des tourbillons
Xr1=-1;
Yr1=0;
Xr2=1;
Yr2=0;

%positions initiales des particules
% X=[0.1 0.09 0.08]; 
% Y=[0.1 0.09 0.08];
X=[0.1 0.8 -0.8 0]; 
Y=[0.1 0 0 0.5];
% X=-2:0.25:2;
% Y=zeros(size(X));
P=length(X);

Xs=zeros(N,P);
Ys=zeros(N,P);

modeBatteur = 'contra-rotatifs';
switch(modeBatteur)
    
    case 'immobile'
        for id=1:N
            disp(T(id));
            t=[T(id):dt:T(id+1)-dt];
            for it=1:length(t)
                u=Gam2pi*((Y-Yr1)./((X-Xr1).^2+(Y-Yr1).^2)...
                    +(Y-Yr2)./((X-Xr2).^2+(Y-Yr2).^2));
                v=-Gam2pi*((X-Xr1)./((X-Xr1).^2+(Y-Yr1).^2)...
                    +(X-Xr2)./((X-Xr2).^2+(Y-Yr2).^2));
                X=X+u*dt;
                Y=Y+v*dt;
            end;
            Xs(id,:)=X;
            Ys(id,:)=Y;
        end;
        
    case 'oscillant'
        w=1.5;
        A=0.3;
        for id=1:N
            disp(T(id));
            t=[T(id):dt:T(id+1)-dt];
            Xro1= Xr1 - A*sin(w*t);
            Xro2= Xr2 - A*sin(w*t);
            for it=1:length(t)
                u=Gam2pi*((Y-Yr1)./((X-Xro1(it)).^2+(Y-Yr1).^2)...
                    +(Y-Yr2)./((X-Xro2(it)).^2+(Y-Yr2).^2));
                v=-Gam2pi*((X-Xro1(it))./((X-Xro1(it)).^2+(Y-Yr1).^2)...
                    +(X-Xro2(it))./((X-Xro2(it)).^2+(Y-Yr2).^2));
                X=X+u*dt;
                Y=Y+v*dt;
                
            end;
            Xs(id,:)=X;
            Ys(id,:)=Y;
        end;
        
    case 'clignotant'
        w=1.5;
        for id=1:N
            disp(T(id));
            t=[T(id):dt:T(id+1)-dt];
            Xro1= sign(cos(w*t));
            for it=1:length(t)
                u=Gam2pi*((Y-Yr1)./((X-Xro1(it)).^2+(Y-Yr1).^2));
                v=-Gam2pi*((X-Xro1(it))./((X-Xro1(it)).^2+(Y-Yr1).^2));
                X=X+u*dt;
                Y=Y+v*dt;            
                
            end;
            Xs(id,:)=X;
            Ys(id,:)=Y;
        end;
        
    case 'cercles'
        w=1;
        A=0.4;
        for id=1:N
            disp(T(id));
            t=[T(id):dt:T(id+1)-dt];
            Xro1=Xr1+A*cos(t*w);
            Xro2=Xr2+A*cos(t*w);
            Yro1=Yr1+A*sin(t*w);
            Yro2=Yr2+A*sin(t*w);
            for it=1:length(t)
                u=Gam2pi*((Y-Yro1(it))./((X-Xro1(it)).^2+(Y-Yro1(it)).^2)...
                    +(Y-Yro2(it))./((X-Xro2(it)).^2+(Y-Yro2(it)).^2));
                v=-Gam2pi*((X-Xro1(it))./((X-Xro1(it)).^2+(Y-Yro1(it)).^2)...
                    +(X-Xro2(it))./((X-Xro2(it)).^2+(Y-Yro2(it)).^2));
                X=X+u*dt;
                Y=Y+v*dt;            
            end;
            Xs(id,:)=X;
            Ys(id,:)=Y;
        end;
        
    case 'huit'
        w=1;
        A=0.4;
        for id=1:N
            disp(T(id));
            t=[T(id):dt:T(id+1)-dt];
            Xro1=Xr1+A*sin(t*w);
            Xro2=Xr2+A*sin(t*w);
            Yro1=Yr1+A*cos(t*w).*sin(t*w);
            Yro2=Yr2+A*cos(t*w).*sin(t*w);
            for it=1:length(t)
                u=Gam2pi*((Y-Yro1(it))./((X-Xro1(it)).^2+(Y-Yro1(it)).^2)...
                    +(Y-Yro2(it))./((X-Xro2(it)).^2+(Y-Yro2(it)).^2));
                v=-Gam2pi*((X-Xro1(it))./((X-Xro1(it)).^2+(Y-Yro1(it)).^2)...
                    +(X-Xro2(it))./((X-Xro2(it)).^2+(Y-Yro2(it)).^2));
                X=X+u*dt;
                Y=Y+v*dt;            
            end;
            Xs(id,:)=X;
            Ys(id,:)=Y;
        end;
        
    case 'contra-rotatifs'
        for id=1:N
            disp(T(id));
            t=[T(id):dt:T(id+1)-dt];
            for it=1:length(t)
                u=Gam2pi*((Y-Yr1)./((X-Xr1).^2+(Y-Yr1).^2)...
                    -(Y-Yr2)./((X-Xr2).^2+(Y-Yr2).^2));
                v=-Gam2pi*((X-Xr1)./((X-Xr1).^2+(Y-Yr1).^2)...
                    -(X-Xr2)./((X-Xr2).^2+(Y-Yr2).^2));
                X=X+u*dt;
                Y=Y+v*dt;            
            end;
            Xs(id,:)=X;
            Ys(id,:)=Y;
        end;
    case 'contra-rotatifs confines'
        %%% pour des toubillons dans un reservoir rond de rayon R
        R=2;
        R2=R^2;
        RRn1=R2/(Xr1^2+Yr1^2);
        RRn2=R2/(Xr2^2+Yr2^2);
        
        for id=1:N
            disp(T(id));
            t=[T(id):dt:T(id+1)-dt];
            for it=1:length(t)
                den1=1./(X.^2+Y.^2+RRn1.*(R2-2*(X.*Xr1+Y.*Yr1)));
                den2=1./(X.^2+Y.^2+RRn2.*(R2-2*(X.*Xr2+Y.*Yr2)));
                u=Gam2pi*((Y-Yr1)./((X-Xr1).^2+(Y-Yr1).^2)...
                    -(Y-Yr2)./((X-Xr2).^2+(Y-Yr2).^2))... % ensuite c'est les images
                    -Gam2pi*((Y-RRn1*Yr1).*den1...
                    -(Y-RRn2*Yr2).*den2);
                
                v=-Gam2pi*((X-Xr1)./((X-Xr1).^2+(Y-Yr1).^2)...
                    -(X-Xr2)./((X-Xr2).^2+(Y-Yr2).^2))... % ensuite c'est les images
                    +Gam2pi*((X-RRn1*Xr1).*den1...
                    -(X-RRn2*Xr2).*den2);
                X=X+u*dt;
                Y=Y+v*dt;            
                
            end;
            Xs(id,:)=X;
            Ys(id,:)=Y;
        end;

end; % fin du switch

figure;
for id=1:N
hold all;
for ip=1:P
    plot(Xs(1:id,ip),Ys(1:id,ip),'o');
end;
hold off;
axis([-2 2 -2 2]);
F(id)=getframe;
end;
movie(F);

%on peut faire aussi des films avec seulement la trace des particules 
%aux nmemo derniers instants

nmemo=10;
figure;
for id=1:N-nmemo
for ip=1:P
    plot(Xs(id:id+nmemo,ip),Ys(id:id+nmemo,ip),'o');
    hold all;
end;
hold off;
axis([-2 2 -2 2]);
F2(id)=getframe;
end;
movie(F2);


