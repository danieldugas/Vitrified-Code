function [u2]=newmark(N,M,tau)
%
%  Schema de Newmark pour l'equation des ondes
%
%  parametres
%
%  N        : nombre de points interieurs dans l'intervalle [0,1]
%  h        : pas d'espace
%  M        : nombre de pas de temps
%  tau      : pas de temps
%  t        : temps courant
%  u0     : N-vecteur, u0(i) est une approximation de u(x_i,t_n-1)
%  u1     : N-vecteur, u1(i) est une approximation de u(x_i,t_n)
%  u2     : N-vecteur, u2(i) est une approximation de u(x_i,t_n+1)
%
h=1./(N+1);
lambda=tau^2/h^2;

Frames = 100; 
hold off; A = figure();
%
% condition initiale u0 et u1
%
for i=1:N
  u0(i)=w( i*h );
end
u1(1)=u0(1)+0.5*lambda*( 0-2*u0(1)+u0(2) );
for i=2:N-1
  u1(i)=u0(i)+0.5*lambda*( u0(i-1)-2*u0(i)+u0(i+1) );
end
u1(N)=u0(N)+0.5*lambda*( u0(N-1)-2*u0(N)+0 );
%
% schema de Newmark
%
t=tau;
for n=2:M
  t=t+tau;
  u2(1)= 2*u1(1) - u0(1) + lambda*( 0-2*u1(1)+u1(2) );
  for i=2:N-1
    u2(i)= 2*u1(i) - u0(i) + lambda*( u1(i-1)-2*u1(i)+u1(i+1) );
  end
  u2(N)= 2*u1(N) - u0(N) + lambda*( u1(N-1)-2*u1(N)+0 );
%
% reactualiser la solution et imprimer la norme euclidienne de u2
%
  norm2=0;
  for i=1:N
    u0(i)=u1(i);
    u1(i)=u2(i);
    norm2=norm2+u2(i)*u2(i);
  end
  fprintf(' pas de temps %d temps %f norm2 %e \n',n,t,norm2)
  if ( mod(n, M/Frames) == 0 )
    figure(A);
    set(A, 'Name',['Calcul ', num2str(100*t/M/tau), '%']);
    plot([h:h:1-h], u2);
    F(n*Frames/M)=getframe;
  end
end
%
% imprimer l'erreur maximum au temps final
%
err = 0;
for i=1:N
    erri = abs(u2(i)-uex(i*h,t));
    if (erri>err) 
        err = erri;
    end
end
fprintf(' erreur maximum au temps final  %e \n',err)

movie(F,[1 1 2]);
pause;
set(A, 'Name','Film');
movie(F);

pause;
x=(0:0.0001:1);
plot(x, w(x));
hold all;
plot(x, uex(x,t));
 
%
% definition de la condition initiale w(x)
%
function init = w(x)
init = exp(-1000*(x - 0.5).^2);    
%
% solution exacte (utilise pour le calcul de l'erreur)
%
function u_exact = uex(x,t)
u_exact =  0.5*(omega(x-t)+omega(x+t));
%
% definition de la condition initiale prolongee 
%          par imparite et 2-periodicite
%
function init = omega(x)
y = mod(x,2);

  init = w(y).*(y<=1) - w(y-1).*(y>1);
