function [u,err] = sysdiff(M,h,N)
%
% entrees  :    M   : nombre de composantes de u
%               h   : pas de temps
%               N   : nombre de pas de temps
%
% sortie   :    u   : approximation de u au temps final
%               err : erreur entre u et son approximation au temps final
% condition initiale
for i=1:M
	u(i)=sin(i*pi/(M+1));
end
% decomposition de cholesky de la matrice I+hA : I+hA = LL^T
diag  = 1+2*h*(M+1)*(M+1);
sdiag = -h*(M+1)*(M+1);
a(1)=sqrt(diag);
for i=1:M-1
  c(i) = sdiag/a(i);
  a(i+1) = sqrt(diag-c(i)*c(i));

end
% schema d Euler retrograde : (I+hA) u^{n+1} = u^{n}
for n=1:N
% resolution du systeme lineaire Ly = u^{n}
  u(1)=u(1)/a(1);
  for i=1:M-1
    u(i+1) = (u(i+1)-c(i)*u(i))/a(i+1);
  end
% resolution du systeme lineaire L^T u^{n+1} = y 
  u(M)=u(M)/a(M);
  for i=M-1:-1:1
    u(i) = (u(i)-c(i)*u(i+1))/a(i);
  end
end
% calcul de l erreur au temps final
  for i=1:M
err=0.;
coeff = exp(-(2-2*cos(pi/(M+1)))*(M+1)*(M+1)*n*h);
for i=1:M
	    err=err+(sin(i*pi/(M+1))*coeff-u(i))^2;
end
err=sqrt(err);
end
