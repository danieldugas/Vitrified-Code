close all; clear all; % on ferme les figures et on elimine les variables precedentes
V0     = 1;               
ax     = [-2 2 0 2];    % plot axis (et volume de control)
nx = 31;                % nb de points dans la direction x
ny = 21;                % nb de points dans la direction y

% on definit les vecteurs et matrices des coordonees
x = linspace(ax(1),ax(2),nx);
y = linspace(ax(3),ax(4),ny);

[xx,yy] = meshgrid(x,y);

%% Question 2: champ de vitesse
profil ='b(t)';
switch(profil)
    case 'a'
        v=-V0*yy;
        u=V0*xx; % mettre ici la bonne vitesse horizontal !!!!!
        
        quiver(xx,yy,u,v,0.8,'k');
    case 'b'
        L=1; 
        v=-V0*yy./cosh(xx/L).^2;
        u=V0*L*tanh(xx/L);
        
        quiver(xx,yy,u,v,0.8,'k');
    case 'b(t)'
        t = linspace(0, 10, 100);
        [xxx, yyy, ttt] = meshgrid(x, y, t);
        
        LLL = 1 + sin(2*ttt)/2;
        v=-V0*yyy./cosh(xxx./LLL).^2;
        u=V0*LLL.*tanh(xxx./LLL);
        z=ttt*0;
        
        %quiver3(xxx,yyy,ttt, u,v, z, 0.8,'k');
end


figure
hold on;

axis(ax), xlabel x, ylabel y, title(['profil (' profil ')'])

%% Question 3: calcul des flux
dx=x(2)-x(1); dy=y(2)-y(1);

topFlux  = trapz(v(end, :))*dx;
bottomFlux = trapz(v(1, :))*dx;


rightFlux  = trapz(u(:, end))*dy;
leftFlux  = trapz(u(:, 1))*-dy;
sideFlux =rightFlux + leftFlux;

totalFlux = sideFlux + topFlux + bottomFlux;

%% Question 4: lignes de courant


 
 switch(profil)
     case 'a'
         ylc=abs(k./xlc);
     case 'b'
         
         k = linspace(0, 3, 32);
         
         
         % resolution par y(x), impossible de modeliser la ligne de courant centrale
         % xlc = linspace(ax(1),ax(2),100);
         %[kk, xxlc] = meshgrid(k, xlc);
         %yylc = kk./(V0*L*tanh(abs(xxlc)/L));
         %plot(xxlc, yylc);
         %surf(xxlc, yylc, kk);
         
         % resolution par x(y), le resultat comprend des parties superflues
         ylc = linspace(ax(3),ax(4),1000);
         [kk, yylc] = meshgrid(k, ylc);
         xxlc = L*atanh(kk./(V0*L*yylc));
         xxlc( yylc < kk/(V0*L) ) = 999; % on enlève les parties superflues
         plot(xxlc, yylc);
         plot(-xxlc, yylc); % symmetrie sur l'axe y
         
     case 'b(t)'
         nk = 4
         k = linspace(0.2, 1, nk);
         xlc = linspace(ax(1),ax(2),100);
         [tt, xxlc] = meshgrid(t, xlc);
         LL = 1 + sin(2*tt)/2;
         
         
         
         for i = 1:nk
             yylc = k(i)./(V0*LL.*tanh(abs(xxlc)./LL));
             surf(xxlc, yylc, tt);
         end
         %plot3(xxxlc(10, :, :), yyylc(10, :, :), ttt(10, :, :));
         %surf(xxxlc, yyylc, ttt);
         
 end
 


%% Question 5: trajectoire d'une particule
% x0 = 0.11;   % position du depart en x
% y0 = 1.8;   % position du depart en y
% 
% dt=0.01; tf=3;
% t=0:dt:tf;
% nt=length(t);
% 
% switch(profil)
%     case 'a'
%         vf = @(x,y,t) -V0*y;
%         uf = @(x,y,t) V0*x;
% %     case 'b'
% % 
% %     case 'b(t)'
%         
% end
% 
% xt=zeros(nt,1);
% yt=zeros(nt,1);
% xt(1) = x0;
% yt(1) = y0;
% 
% % for ii=1:nt-1
% %     xt(ii+1)=xt(ii)+dt*uf(xt(ii),yt(ii),t(ii));
% %     yt(ii+1)=yt(ii)+dt*vf(xt(ii),yt(ii),t(ii));
% % end
% 
% plot(xt,yt,'r');
