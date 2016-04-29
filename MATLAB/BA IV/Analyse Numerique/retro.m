function[u,err,mmax] = retro(h,N)
%
% entrees : h : pas de temps
% N : nombre de pas de temps
%
% sorties : u : approximation de u au temps final (h*N)
% err : erreur entre u et son approximation au temps final
% mmax: nombre max d iterations dans la boucle de Newton
t=0;
u=1;
for n = 1:N
 t=t+h;
 x=u;
 mmax=0;
 for m = 1:10
 newton = (x-u-h*f(x,t))/(1-h*dfdx(x,t));
 x = x - newton;
 if (abs(newton) < 1.e-8)
 mmax=max(m,mmax);
 break;
 end
 end
 u=x;
end
err=abs(u-uexact(t));
end
function f=f(x,t)
 f=-x;
end
function dfdx=dfdx(x,t)
 dfdx=-1; 
end 
function uexact = uexact(t) 
 uexact=exp(-t); 
end 